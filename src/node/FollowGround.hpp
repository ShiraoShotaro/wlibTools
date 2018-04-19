#pragma comment(user, "CarMotionAnimator Plug-in Header")
/////////////////////////////////////////////////////
//FollowGround.hpp
//
//Copyright (c) 2017 Shotaro Shirao
//
//This software is released under the MIT License.
//http://opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////

#pragma once
#ifndef CMA_FOLLOWGROUND_HPP_
#define CMA_FOLLOWGROUND_HPP_

#include <maya/MString.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MPointArray.h>
#include "utility/Vector.hpp"


namespace cma {

/**
 * FollowGround�m�[�h�N���X
 * �n�_����̃x�N�g�����C�ƁA�n�ʂƂ̌�_���W���Z�o����m�[�h
 * @version 1.0
 * @see MPxNode http://download.autodesk.com/us/maya/2011help/api/class_m_px_node.html
 */
class FollowGround : public MPxNode{
public:
	//////////////////////////////
	// PUBLIC MEMBER CONSTANT
	//////////////////////////////
	/**
	 * �m�[�hID�萔
	 */
	enum { kFollowGroundID = 0x70000 };

	//////////////////////////////
	// PUBLIC MEMBER VALIABLES
	//////////////////////////////

	/** ���̓��b�V��
	 * �n�ʂƂ��Ďg�p�����B
	 */
	static MObject inMesh;

	/** �o�̓|�C���g
	 * �n�ʂɐڂ���_�̍��W
	 */
	static MObject outPoint;

	/** ���̓x�N�g��
	 * �n�_����̃x�N�g�����C
	 */
	static MObject rayVector;
	
	/** ���C�n�_
	 */
	static MObject rayPoint;

	/** ����
	 */
	static MObject distance;

	/** �ő勗��
	 */
	static MObject maxDistance;
	
	/** �m�[�hID
	 */
	static const MTypeId id;
	
	//////////////////////////////
	// PUBLIC MEMBER FUNCTION
	//////////////////////////////

	/** �R���X�g���N�^
	 */	
	FollowGround();

	/** �f�X�g���N�^
	 */
	virtual ~FollowGround();

	/**
	 * �m�[�h�̌v�Z��������
	 * �����𑪂��ĕ��̂��ړ�������
	 * @param plug �v�Z���̃v���O
	 * @param data �ҏW�\�ȓ����f�[�^
	 * @return �v�Z�ɐ��������ꍇkSuccess��Ԃ�
	 * @see MPxNode::compute http://download.autodesk.com/us/maya/2011help/api/class_m_px_node.html#3dd5454e3fa92598836018ae9ab220c3
	 */
	virtual MStatus compute(const MPlug & plug,	MDataBlock & data) override;

	/**
	 * �V�����C���X�^���X�̐����֐�
	 * @return �V�����C���X�^���X��new�|�C���^
	 */
	static void * creator(void);

	/**
	 * �m�[�h�o�^���ɌĂ΂��֐�
	 * @return �������ɐ��������ꍇkSuccess��Ԃ�
	 */
	static MStatus initialize(void);

	/**
	 *
	 */
	virtual bool isPassiveOutput(const MPlug & plug) const override;

protected:

private:
	double distance_value;
	
	/**
	 * inMesh�̃A�g���r���[�g�ǉ�
	 * @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�̎��ɁA�ڍׂ��܂߂ė�O�𓊂���
	 * @see cma::MStatusException
	 */
	static void _addAttr_inMesh(void);
	
	/**
	 * inVector�̃A�g���r���[�g�ǉ�
	 * @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�̎��ɁA�ڍׂ��܂߂ė�O�𓊂���
	 * @see cma::MStatusException
	 */
	static void _addAttr_rayVector(void);
	
	/**
	 * outPoint�̃A�g���r���[�g�ǉ�
	 * @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�̎��ɁA�ڍׂ��܂߂ė�O�𓊂���
	 * @see cma::MStatusException
	 */
	static void _addAttr_outPoint(void);

	/**
	 * rayPoint�̃A�g���r���[�g�ǉ�
	 * @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�̎��ɁA�ڍׂ��܂߂ė�O�𓊂���
	 * @see cma::MStatusException
	 */
	static void _addAttr_rayPoint(void);

	/**
	 * distance,maxDistance�̃A�g���r���[�g�ǉ�
	 * @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�̎��ɁA�ڍׂ��܂߂ė�O�𓊂���
	 * @see cma::MStatusException
	 */
	static void _addAttr_distance(void);

	/**
	 * �Čv�Z(compute)���Ɏg�p����A�g���r���[�g�l�̎擾
	 * @param data compute�֐��̈����ł���data�̎Q��
	 * @param ground �O���E���h
	 * @param ray_from ���C�n�_
	 * @param ray_vector ���C�x�N�g��
	 * @param max_distance �ő勗��
	 * @throws MStatusException ���ꂼ��̎擾���ʂ�kSuccess�ȊO�������ꍇ
	 * @see cma::MStatusException
	 */
	static void _getInputValue(MDataBlock & data, MDataHandle * ground, MDataHandle * ray_from, MDataHandle * ray_vector, MDataHandle * max_distance);

	/**
	 * �Čv�Z���ꂽ�l���A�g���r���[�g�֔��f������
	 * @param data compute�֐��̈����ł���data�̎Q��
	 * @param result �i�[���錋�ʂ̒l
	 * @throws MStatusException �i�[���̌��ʂ�kSuccess�ȊO�������ꍇ
	 * @see cma::MStatusException
	 */
	static void _setOutputValue(MDataBlock & data, const float x, const float y, const float z);
	
	/** 
	 * �t�F�[�X���擾���A���ꂼ��̃t�F�[�X�Ƃ̋��������߂�B
	 * @param ground �O���E���h�̃I�u�W�F�N�g
	 * @param ray_point ���C�n�_�̃|�C���g
	 * @param ray_vector ���C�̃x�N�g��
	 * @param is_cross ����������ǂ������i�[����
	 * @param max_distance �ő勗��
	 * @throws MStatusException ���s����kSuccess�ȊO�̃G���[�����������ꍇ
	 * @return ����
	 */
	static double _getLengthToCrossPoint(const MObject & ground, const VectorF ray_point, const VectorF ray_vector, bool * is_cross = nullptr, const double max_distance = 100000.0);

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
	static double _checkHitPolygon(const MPointArray & points, const VectorF ray_point, const VectorF ray_vector, bool * is_cross = nullptr, const double max_distance = 100000.0);

};

};

#endif //end of include guard
