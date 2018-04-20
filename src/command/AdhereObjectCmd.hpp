#pragma once
#ifndef WLIB_ADHEREOBJECTCMD_HPP_
#define WLIB_ADHEREOBJECTCMD_HPP_


#include <vector>
#include <maya/MPxCommand.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>
#include <maya/MPointArray.h>
#include <maya/MStatus.h>
#include <maya/MDagPath.h>
#include "Vector.hpp"

namespace wlib {

class AdhereObjectCmd : public MPxCommand {
public:

	AdhereObjectCmd() = default;
	virtual ~AdhereObjectCmd() = default;

	static void * creator();

	virtual MStatus doIt(const MArgList & args) override;
	virtual MStatus undoIt(void) override;
	virtual MStatus redoIt(void) override;

	virtual bool isUndoable(void) const;

private:

	struct _History {
		MDagPath dag;
		MVector move;
		MQuaternion after_rotate;
		MQuaternion before_rotate;
	
		_History(MDagPath && _dag, const MVector & _move, const MQuaternion & _before_rotate, const MQuaternion & _after_rotate);

		MStatus doIt() const;
		MStatus undoIt() const;
	};

	std::vector<_History> histories_;

	/**
	* �O�p�ʂƃx�N�g���̌�_�܂ł̋��������߂�
	* @param points �O�p�ʂ̒��_
	* @param ray_point ���C�̎n�_
	* @param ray_vector ���C�x�N�g��
	* @param is_cross ����������ǂ���
	* @param max_distance �ő勗��
	* @return ����
	* @throws MStatusException �v�Z��G���[�����������ꍇ
	*/
	static double _checkHitPolygon(const MVector & abs_position, const MPointArray & points, const VectorF & ray_point, const VectorF & ray_vector, bool & is_cross, VectorF & normal, const double max_distance = 100000.0);


};

}

#endif
