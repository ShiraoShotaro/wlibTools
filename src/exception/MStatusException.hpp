/// @file MStatusException.hpp
/// @brief MStatusException�N���X�w�b�_�t�@�C��
/// @author Shirao Shotaro (pdi)
/// @date 2017.12

#pragma once
#ifndef CMA_MSTATUSEXCEPTION_HPP_
#define CMA_MSTATUSEXCEPTION_HPP_

#include <iostream>
#include <maya/MString.h>
#include <maya/MStatus.h>

namespace wlib {

/// @brief MStatus���O�����Ƃ��ď������₷������N���X
/// @version 2018.02
/// @sa MStatus
///
/// OpenMaya C++ API��MStatus�^�ɁA��O���������������́BMStatus�^�̃G���[���O�Ƃ��ď������A�ώG��if���������Ȃ��čςށB
/// �܂��A���̊֐����g�p����O�𔭐�������ꍇ�́A�K���L���b�`���A���̃G���[���ʂ��G���[�o�͂֕\�����邱�ƁB
class MStatusException{
public:

	const MStatus stat;		///< MStatus�{��
	const MString message;	///< �G���[���b�Z�[�W
	const MString place;	///< �G���[�̔����ꏊ�B�������Ɍ��܂�͂Ȃ����A�����̓G���[�������������Ɏ��s���̊֐����B
	
	/// @brief �R���X�g���N�^
	/// 
	/// �G���[�̃X�e�[�^�X�ƃ��b�Z�[�W��o�^�ł���B�܂��Aplace�͔C�ӂɎw��ł��A�i�K���w�肷�邱�Ƃ��D�܂������j�w�肵�Ȃ��ꍇ��unlogged�ƕ\�������B
	/// �܂��Aunlogged�̏ꍇ�AtoString�֐��ɂ����ď㏑�����\�ł���B
	/// @param stat �G���[�̃X�e�[�^�X
	/// @param message ���b�Z�[�W
	/// @param place �����������ł��镶����
	MStatusException(const MStatus & stat, const MString & message, const MString & place = "<unlogged>");

	/// �f�X�g���N�^
	virtual ~MStatusException();

	/// @brief �w��t�H�[�}�b�g�ɐ��`���ꂽ�G���[���b�Z�[�W���擾����B
	/// 
	/// @return �G���[���b�Z�[�W
	MString toString(void) const;

	/// @brief �w��t�H�[�}�b�g�ɐ��`���ꂽ�G���[���b�Z�[�W���擾����B
	///
	/// �����ӏ����I�[�o�[���C�h���邱�Ƃ��ł��܂��B�������A�I�[�o�[���C�h����ɂ�place��unlogged�ɂ��Ă����K�v������܂��B
	/// @param place_override �I�[�o�[���C�h����place���ʕ�����
	/// @return �G���[���b�Z�[�W
	MString toString(const MString & place_override) const;

	/// @brief MStatus�^�ւ̃L���X�g�I�[�o�[���[�h
	/// @return stat�̒l
	operator MStatus() const;

	/// @brief MString�^�ւ̃L���X�g�I�[�o�[���[�h
	/// @return toString�֐��̖߂�l
	operator MString() const;

	/// @brief �G���[�敪�����������`�F�b�N����
	/// @param comp ��ׂ����G���[�敪
	/// @return stat��comp�����������true
	bool operator==(const MStatus & comp) const;

	/// @brief �X�g���[���^�̏o�͂ɑ΂���I�[�o�[���[�h
	///
	/// ���̊֐����g�p�����ꍇ�́Aplace�̃I�[�o�[���C�h�͂ł��܂���B
	/// @param os �X�g���[��
	/// @param e ��O
	/// @return �o�͌�̃X�g���[��
	friend std::ostream & operator<<(ostream & os, const MStatusException & e);

	/// @brief if�����O�X���[���[�e�B���e�B�[�֐�
	///
	/// �������X�e�[�^�X��kSuccess�ȊO�̎��ɁAMStatusException���X���[���郆�[�e�B���e�B�[�֐�
	/// @param stat �G���[�̃X�e�[�^�X
	/// @param message ���b�Z�[�W
	/// @param place �����������ł��镶����
	/// @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�������ꍇ
	static void throwIf(const MStatus & stat, const MString & message, const MString & place = "<unlogged>");

protected:

private:
	MStatusException() = delete;

};

};

#endif //end of include guard
