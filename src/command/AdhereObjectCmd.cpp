#include "AdhereObjectCmd.hpp"

#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MQuaternion.h>
#include <maya/MVector.h>
#include <maya/MString.h>
#include <maya/MGlobal.h>

#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>

#include "exception/MStatusException.hpp"

//[x] [y] [z] [object/world]
MStatus wlib::AdhereObjectCmd::doIt(const MArgList & args)
{
	try {
		if (args.length() != 5) {
			MStatusException::throwIf(MStatus::kInvalidParameter, "usage: AdhereObject [x] [y] [z] [object/world] [maxdistance]", "AdhereObject");
		}

		MVector ray_vector;
		MString space;
		double max_distance;
		args.get(0, ray_vector.x);
		args.get(1, ray_vector.y);
		args.get(2, ray_vector.z);
		args.get(3, space);
		args.get(4, max_distance);
		if (space != "object" && space != "world") {
			MStatusException::throwIf(MStatus::kInvalidParameter, "第4引数はobjectかworldで指定してください", "AdhereObject");
		}
		
		MSelectionList select;
		MStatusException::throwIf(MGlobal::getActiveSelectionList(select), "選択中のオブジェクトを取得できませんでした", "AdhereObjectCmd");

		if (select.length() < 2) {
			MStatusException::throwIf(MStatus::kInvalidParameter, "少なくとも2つ以上のオブジェクトが選択されている必要があります。", "AdhereObjectCmd");
		}

		//接着先
		MStatus ret;
		MDagPath ground;
		select.getDagPath(select.length() - 1, ground);

		//MFnMesh ground_mesh(ground, &ret);
		//MStatusException::throwIfError(ret, "ポリゴンフェースの取得に失敗", "wlib::FollowGround::_getLengthToCrossPoint");

		MDagPath dag;
		MPointArray points;
		MIntArray vertexes;
		int triangle_num;
		VectorF normal;
		for (unsigned int i = 0; i < select.length() - 1; ++i) {
			//接着先のフェースイテレータ
			MItMeshPolygon face_iter(ground, MObject::kNullObj, &ret);
			MStatusException::throwIf(ret, "ポリゴンフェースイテレータの取得に失敗", "AdhereObjectCmd");

			//接着物のトランスフォーム
			select.getDagPath(i, dag);
			MFnTransform transform(dag);

			//絶対座標
			VectorF point;
			MVector vec = transform.getTranslation(MSpace::kWorld);
			point.x = vec.x;
			point.y = vec.y;
			point.z = vec.z;
			MQuaternion rot;
			transform.getRotation(rot);

			//ベクトルの回転
			if (space == "object") {
				MQuaternion ray_quat(ray_vector.x, ray_vector.y, ray_vector.z, 0);
				ray_quat = rot.conjugateIt() * ray_quat * rot;
				ray_vector.x = ray_quat.x;
				ray_vector.y = ray_quat.y;
				ray_vector.z = ray_quat.z;
			}

			for (; !face_iter.isDone(); face_iter.next()) {
				//三角面の数を取得 
				ret = face_iter.numTriangles(triangle_num);
				MStatusException::throwIf(ret, "三角面数の取得に失敗", "AdhereObjectCmd");

				bool is_cross_current;
				for (int idx = 0; idx < triangle_num; idx++) {
					//三角面を取得
					face_iter.getTriangle(idx, points, vertexes);
					MStatusException::throwIf(ret, "三角フェースデータの取得に失敗", "AdhereObjectCmd");

					//チェック関数
					double length_tmp = _checkHitPolygon(points, point, ray_vector, is_cross_current, normal, max_distance);

					if (is_cross_current) {

						//pointからray_vector方向へmax_distance動かし、normalの方向へ回転する
						VectorF hitpoint = point + ray_vector.normal() * max_distance;




						break;
					}
				}
			}

		}

		MString ac_obj, pa_obj;
		args.get(0, ac_obj);
		args.get(1, pa_obj);

		//座標取得
		MDoubleArray abs_pos_array;
		MGlobal::executeCommand("xform -q -ws -rp " + ac_obj, abs_pos_array);
		double px = abs_pos_array[0];
		double py = abs_pos_array[1];
		double pz = abs_pos_array[2];




		MPointArray points;
		MIntArray vertexes;
		int triangle_num;
		bool is_cross_current;
		double length = max_distance;

		for (; !face_iter.isDone(); face_iter.next()) {
			//三角面の数を取得 
			ret = face_iter.numTriangles(triangle_num);
			MStatusException::throwIfError(ret, "三角面数の取得に失敗", "wlib::FollowGround::_getLengthToCrossPoint");

			for (int idx = 0; idx < triangle_num; idx++) {
				//三角面を取得
				face_iter.getTriangle(idx, points, vertexes);
				MStatusException::throwIfError(ret, "三角フェースデータの取得に失敗", "wlib::FollowGround::_getLengthToCrossPoint");

				//チェック関数
				double length_tmp = _checkHitPolygon(points, ray_point, ray_vector, &is_cross_current, max_distance);

				if (is_cross_current) {
					std::cout << "CROSS!! distance => " << length << std::endl;
					if (length_tmp < length) length = length_tmp;
					*is_cross = true;
				}
			}
		}

	}
	catch (MStatusException e) {
		this->displayError(e.toString());
		return e;
	}

	return this->redoIt();
}

MStatus wlib::AdhereObjectCmd::undoIt(void)
{
	return MStatus();
}

MStatus wlib::AdhereObjectCmd::redoIt(void)
{

	return MStatus();
}

bool wlib::AdhereObjectCmd::isUndoable(void) const
{
	return false;
}

double wlib::AdhereObjectCmd::_checkHitPolygon(const MPointArray & points, const VectorF & ray_point, const VectorF & ray_vector, bool & is_cross, VectorF & normal, const double max_distance)
{
	//頂点数が3固定
	if (points.length() != 3) throw MStatusException(MStatus::kInvalidParameter, "三角面になっていない", "AdhereObject");

	double ret = max_distance;
	is_cross = false;

	VectorD edge1(points[1] - points[0]);
	VectorD edge2(points[2] - points[0]);

	constexpr double epsilon = 0.00001;
	auto isEqual = [epsilon](double a, double b) -> bool {
		return (epsilon > (a - b) && -epsilon < (a - b));
	};

	VectorD ray(ray_vector);
	VectorD inv_ray(ray_vector.inv());

	double denominator = VectorD::det(edge1, edge2, inv_ray);

	if (!isEqual(denominator, 0.0)) {
		//レイが平面と平行ではない
		VectorD d = ray_point - VectorD(points[0]);
		double u = VectorD::det(d, edge2, inv_ray) / denominator;

		if ((0 <= u) && (u <= 1)) {
			double v = VectorD::det(edge1, d, inv_ray) / denominator;
			if ((0 <= v) && (u + v <= 1.0)) {
				double t = VectorD::det(edge1, edge2, d) / denominator;
				if (t >= 0 && t <= max_distance) {
					ret = t;
					is_cross = true;
					normal = edge1 * edge2;
				}
			}
		}
	}

	return ret;
}
