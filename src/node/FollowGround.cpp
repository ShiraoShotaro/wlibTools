#include "FollowGround.hpp"

#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnNumericData.h>

#include "exception/MStatusException.hpp"

const MTypeId cma::FollowGround::id(cma::FollowGround::kFollowGroundID);
MObject cma::FollowGround::inMesh;
MObject cma::FollowGround::rayVector;
MObject cma::FollowGround::rayPoint;
MObject cma::FollowGround::outPoint;
MObject cma::FollowGround::maxDistance;
MObject cma::FollowGround::distance;

////////////////////////////////////////
// FollowGround
////////////////////////////////////////
cma::FollowGround::FollowGround()
	: distance_value(50000)
{ }


////////////////////////////////////////
// ~FollowGround
////////////////////////////////////////
cma::FollowGround::~FollowGround() { }


////////////////////////////////////////
// creator
////////////////////////////////////////
void * cma::FollowGround::creator(void) {
	return new FollowGround;
}


////////////////////////////////////////
// initialize
////////////////////////////////////////
MStatus cma::FollowGround::initialize(void) {

	//�A�g���r���[�g�̒ǉ�
	try {
		_addAttr_outPoint();
		_addAttr_inMesh();
		_addAttr_rayVector();
		_addAttr_rayPoint();
		_addAttr_distance();
	}
	catch (const MStatusException & e) {
		std::cerr << e.toString() << std::endl;
		return e.stat;
	}
	
	return MStatus::kSuccess;
}

bool cma::FollowGround::isPassiveOutput(const MPlug & plug) const {
	if (plug == inMesh ||
		plug == outPoint ||
		plug == rayPoint) return true;
	return false;
}


////////////////////////////////////////
// compute
////////////////////////////////////////
MStatus cma::FollowGround::compute(const MPlug & plug, MDataBlock & data) {
	MStatus ret;
	try {
		if (plug == this->outPoint) {
			//���̓A�g���r���[�g�l�擾
			MDataHandle ground, ray_from, ray_vector, max_distance;
			_getInputValue(data, &ground, &ray_from, &ray_vector, &max_distance);

			VectorF ray_from_v(ray_from.asFloat3());
			VectorF ray_vector_v(ray_vector.asFloat3());
			double max_distance_v = max_distance.asDouble();

			bool is_cross = false;
			this->distance_value = _getLengthToCrossPoint(ground.asMesh(), ray_from_v, ray_vector_v, &is_cross, max_distance_v);
			
			VectorF ray_cross(ray_vector_v * this->distance_value + ray_from_v);

			_setOutputValue(data, ray_cross.x, ray_cross.y, ray_cross.z);
			data.setClean(plug);
		}
		else if (plug == this->distance) {
			MDataHandle outputHandle = data.outputValue(this->distance, &ret);
			MStatusException::throwIfError(ret, "distance�̎擾�Ɏ��s", "cma::FollowGround::compute");	
			outputHandle.setDouble(this->distance_value);	
			data.setClean(plug);
		}
	}
	catch (MStatusException e) {
		std::cerr << e.toString() << std::endl;
	}
	return MStatus::kSuccess;
}

void cma::FollowGround::_getInputValue(MDataBlock & data, MDataHandle * ground, MDataHandle * ray_from, MDataHandle * ray_vector, MDataHandle * max_distance) {
	MStatus ret;
	*ground = data.inputValue(inMesh, &ret);
	MStatusException::throwIfError(ret, "ground���b�V���̎擾�Ɏ��s", "cma::FollowGround::_getInputValue");
	*ray_from = data.inputValue(rayPoint, &ret);
	MStatusException::throwIfError(ret, "���C�n�_�̎擾�Ɏ��s", "cma::FollowGround::_getInputValue");
	*ray_vector = data.inputValue(rayVector, &ret);
	MStatusException::throwIfError(ret, "���C�x�N�g���̎擾�Ɏ��s", "cma::FollowGround::_getInputValue");
	*max_distance = data.inputValue(maxDistance, &ret);
	MStatusException::throwIfError(ret, "�ő咷�̎擾�Ɏ��s", "cma::FollowGround::_getInputValue");
}

void cma::FollowGround::_setOutputValue(MDataBlock & data, const float x, const float y, const float z) {
	MStatus ret;
	MDataHandle outputHandle = data.outputValue(outPoint, &ret);
	MStatusException::throwIfError(ret, "outPoint�̎擾�Ɏ��s", "cma::FollowGround::_setOutputValue");	
	outputHandle.set(x,y,z);
}

double cma::FollowGround::_getLengthToCrossPoint(const MObject & ground, const VectorF ray_point, const VectorF ray_vector, bool * is_cross, const double max_distance) {
	MStatus ret;

	MItMeshPolygon face_iter(ground, &ret);
	MStatusException::throwIfError(ret, "�|���S���t�F�[�X�C�e���[�^�̎擾�Ɏ��s", "cma::FollowGround::_getLengthToCrossPoint");

	MFnMesh ground_mesh(ground, &ret);
	MStatusException::throwIfError(ret, "�|���S���t�F�[�X�̎擾�Ɏ��s", "cma::FollowGround::_getLengthToCrossPoint");

	MPointArray points;
	MIntArray vertexes;
	int triangle_num;
	bool is_cross_current;
	double length = max_distance;
	
	for (; !face_iter.isDone(); face_iter.next()) {
		//�O�p�ʂ̐����擾 
		ret = face_iter.numTriangles(triangle_num);
		MStatusException::throwIfError(ret, "�O�p�ʐ��̎擾�Ɏ��s", "cma::FollowGround::_getLengthToCrossPoint");

		for (int idx = 0; idx < triangle_num; idx++) {
			//�O�p�ʂ��擾
			face_iter.getTriangle(idx, points, vertexes);
			MStatusException::throwIfError(ret, "�O�p�t�F�[�X�f�[�^�̎擾�Ɏ��s", "cma::FollowGround::_getLengthToCrossPoint");
			
			//�`�F�b�N�֐�
			double length_tmp = _checkHitPolygon(points, ray_point, ray_vector, &is_cross_current, max_distance);

			if (is_cross_current) {
				std::cout << "CROSS!! distance => " << length << std::endl;
				if (length_tmp < length) length = length_tmp;
				*is_cross = true;
			}
		}
	}

	return length;
}

double cma::FollowGround::_checkHitPolygon(const MPointArray & points, const VectorF ray_point, const VectorF ray_vector, bool * is_cross, const double max_distance) {

	//���_����3�Œ�
	if (points.length() != 3) throw MStatusException(MStatus::kInvalidParameter, "�O�p�ʂɂȂ��Ă��Ȃ�", "cma::FollowGround::_checkHitPolygon");

	double ret = max_distance;
	if (is_cross != nullptr) *is_cross = false;

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
					if (is_cross != nullptr) *is_cross = true;
				}
			}
		}
	}

	return ret;
}
