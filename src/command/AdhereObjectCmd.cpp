#include "AdhereObjectCmd.hpp"

#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MString.h>
#include <maya/MGlobal.h>

#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>

#include "exception/MStatusException.hpp"

void * wlib::AdhereObjectCmd::creator()
{
	return new AdhereObjectCmd;
}

//[x] [y] [z] [object/world]
MStatus wlib::AdhereObjectCmd::doIt(const MArgList & args)
{
	try {
		if (args.length() != 5) {
			MStatusException::throwIf(MStatus::kInvalidParameter, "usage: AdhereObject [x] [y] [z] [object/world] [maxdistance]", "AdhereObject");
		}

		MVector ray_vector_master;
		MString space;
		double max_distance;
		args.get(0, ray_vector_master.x);
		args.get(1, ray_vector_master.y);
		args.get(2, ray_vector_master.z);
		args.get(3, space);
		args.get(4, max_distance);
		if (space != "object" && space != "world") {
			MStatusException::throwIf(MStatus::kInvalidParameter, "��4������object��world�Ŏw�肵�Ă�������", "AdhereObject");
		}
		
		MSelectionList select;
		MStatusException::throwIf(MGlobal::getActiveSelectionList(select), "�I�𒆂̃I�u�W�F�N�g���擾�ł��܂���ł���", "AdhereObjectCmd");

		if (select.length() < 2) {
			MStatusException::throwIf(MStatus::kInvalidParameter, "���Ȃ��Ƃ�2�ȏ�̃I�u�W�F�N�g���I������Ă���K�v������܂��B", "AdhereObjectCmd");
		}

		//�ڒ���
		MStatus ret;
		MDagPath ground;
		select.getDagPath(select.length() - 1, ground);
		MFnTransform ground_transform(ground);

		//MFnMesh ground_mesh(ground, &ret);
		//MStatusException::throwIfError(ret, "�|���S���t�F�[�X�̎擾�Ɏ��s", "wlib::FollowGround::_getLengthToCrossPoint");

		MDagPath dag;
		MPointArray points;
		MIntArray vertexes;
		int triangle_num;
		VectorF normal;
		this->histories_.clear();
		for (unsigned int i = 0; i < select.length() - 1; ++i) {
			//�ڒ���̃t�F�[�X�C�e���[�^
			MItMeshPolygon face_iter(ground, MObject::kNullObj, &ret);
			MStatusException::throwIf(ret, "�|���S���t�F�[�X�C�e���[�^�̎擾�Ɏ��s", "AdhereObjectCmd");

			//�ڒ����̃g�����X�t�H�[��
			select.getDagPath(i, dag);
			MFnTransform transform(dag);

			//��΍��W
			VectorF point = transform.rotatePivot(MSpace::kWorld);
			MQuaternion rot;
			transform.getRotation(rot);

			//�x�N�g���̉�]
			MVector ray_vector = ray_vector_master;
			if (space == "object") {
				MQuaternion ray_quat(ray_vector.x, ray_vector.y, ray_vector.z, 0);
				ray_quat = (rot.conjugate() * ray_quat) * rot;
				ray_vector.x = ray_quat.x;
				ray_vector.y = ray_quat.y;
				ray_vector.z = ray_quat.z;
			}
			std::cerr << "====================" << std::endl;
			std::cerr << "POINT:" << point.toString() << std::endl;
			std::cerr << "RAY:" << ray_vector.x << "," << ray_vector.y << "," << ray_vector.z << std::endl;

			bool is_cross_current = false;
			for (; !face_iter.isDone() && !is_cross_current; face_iter.next()) {
				//�O�p�ʂ̐����擾 
				ret = face_iter.numTriangles(triangle_num);
				MStatusException::throwIf(ret, "�O�p�ʐ��̎擾�Ɏ��s", "AdhereObjectCmd");

				for (int idx = 0; idx < triangle_num; idx++) {
					//�O�p�ʂ��擾
					face_iter.getTriangle(idx, points, vertexes, MSpace::kWorld);
					MStatusException::throwIf(ret, "�O�p�t�F�[�X�f�[�^�̎擾�Ɏ��s", "AdhereObjectCmd");

					//�`�F�b�N�֐�
					double length_tmp = _checkHitPolygon(/*ground_transform.getTranslation(MSpace::kWorld)*/ MVector(), points, point, ray_vector, is_cross_current, normal, max_distance);

					if (is_cross_current) {

						MVector world_up = MGlobal::upAxis();
						MQuaternion rotation(world_up, normal);
						this->histories_.emplace_back(std::move(dag), ray_vector.normal() * length_tmp, rot, rotation);
						break;
					}
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
	for (auto p = this->histories_.begin(); p != this->histories_.end(); ++p) p->undoIt();
	return MStatus();
}

MStatus wlib::AdhereObjectCmd::redoIt(void)
{
	for (auto p = this->histories_.begin(); p != this->histories_.end(); ++p) p->doIt();
	return MStatus();
}

bool wlib::AdhereObjectCmd::isUndoable(void) const
{
	return true;
}

double wlib::AdhereObjectCmd::_checkHitPolygon(const MVector & abs_position, const MPointArray & _points, const VectorF & ray_point, const VectorF & ray_vector, bool & is_cross, VectorF & normal, const double max_distance)
{
	//���_����3�Œ�
	if (_points.length() != 3) throw MStatusException(MStatus::kInvalidParameter, "�O�p�ʂɂȂ��Ă��Ȃ�", "AdhereObject");

	double ret = max_distance;
	is_cross = false;

	MPointArray points;
	points.append(_points[0] + abs_position);
	points.append(_points[1] + abs_position);
	points.append(_points[2] + abs_position);

	std::cerr << "TRIANGLE[0]:" << points[0].x << "," << points[0].y << "," << points[0].z << std::endl;
	std::cerr << "TRIANGLE[1]:" << points[1].x << "," << points[1].y << "," << points[1].z << std::endl;
	std::cerr << "TRIANGLE[2]:" << points[2].x << "," << points[2].y << "," << points[2].z << std::endl;

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
		//���C�����ʂƕ��s�ł͂Ȃ�
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

wlib::AdhereObjectCmd::_History::_History(MDagPath && _dag, const MVector & _move, const MQuaternion & _before_rotate, const MQuaternion & _after_rotate)
	: dag(std::move(_dag)), move(_move), before_rotate(_before_rotate), after_rotate(_after_rotate)
{
}

MStatus wlib::AdhereObjectCmd::_History::doIt() const
{
	MStatus ret;
	MFnTransform transform(this->dag, &ret);
	if (ret != MStatus::kSuccess) {
		AdhereObjectCmd::displayError("�g�����X�t�H�[���m�[�h�̎擾�Ɏ��s���܂���");
		return ret;
	}
	transform.translateBy(this->move, MSpace::kWorld);
	//transform.setTranslation(hitpoint, MSpace::kWorld);
	transform.setRotation(this->after_rotate, MSpace::kWorld);
	return ret;
}

MStatus wlib::AdhereObjectCmd::_History::undoIt() const
{
	MStatus ret;
	MFnTransform transform(this->dag, &ret);
	if (ret != MStatus::kSuccess) {
		AdhereObjectCmd::displayError("�g�����X�t�H�[���m�[�h�̎擾�Ɏ��s���܂���");
		return ret;
	}
	transform.translateBy(-this->move, MSpace::kWorld);
	//transform.setTranslation(hitpoint, MSpace::kWorld);
	transform.setRotation(this->before_rotate, MSpace::kWorld);
	return ret;
}
