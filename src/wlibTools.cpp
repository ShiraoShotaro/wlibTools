#include "wlibCarCameraRig.hpp"

#include <vector>
#include <string>
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>
#include "exception/MStatusException.hpp"
#include <maya/MDrawRegistry.h>

//*** INCLUDE HEADERS ***

#include "command/SetKeyframesCommand.hpp"

#include "node/CarJibCraneNode.hpp"
#include "node/CameraRecordNode.hpp"
#include "node/GamepadNode.hpp"
#include "node/CameraHelperHUD.hpp"
#include "node/FocusControllerNode.hpp"
#include "node/CameraMultiTrackRecorderNode.hpp"
#include "node/CameraRigNode.hpp"
#include "node/CameraStabilizerNode.hpp"
#include "node/CameraGimbalNode.hpp"
#include "node/MotionMixerNode.hpp"
#include "node/MotionRecorderNode.hpp"
#include "node/DroneNode.hpp"
#include "node/PadToRotateNode.hpp"
#include "node/RotateNode.hpp"
#include "node/ScalarRecorderNode.hpp"
#include "node/AngleRecorderNode.hpp"
#include "translator/ExaTranslator.hpp"
//***********************




namespace {
class CommandPair {
public:
	const MString command;
	void * (*creator)();
	CommandPair(const MString & command, void * (*creator)()) : command(command), creator(creator) {}
	virtual ~CommandPair() {}
private:
	CommandPair() = delete;
};

class NodePair {
public:
	const MString node;
	const MTypeId id;
	void * (*creator)();   
	MStatus (*initialize)();
	const MPxNode::Type type;
	NodePair(const MString & node, const MTypeId id, void * (*creator)(), MStatus (*initialize)(), const MPxNode::Type & type = MPxNode::Type::kDependNode) : node(node), id(id), creator(creator), initialize(initialize), type(type) {}
	virtual ~NodePair() {}
private:
	NodePair() = delete;
};

//*** �V�����R�}���h��ǉ�����ꏊ ***
std::vector<CommandPair> getCommands(void) {
	//�R�}���h�������ɒǉ�
	//CommandPair("�R�}���h��", [�N���G�C�^�[�֐��̃|�C���^]),
	return std::vector<CommandPair> {
		CommandPair("wlibSetKeyframes", wlib::SetKeyframesCommand::creator),
//		CommandPair("wlib_sortVertexID", wlib::SortVertexID::creator),
		//CommandPair("Hogehoge", wlib::Hogehoge::creator),
	};
}

//*** �V�����m�[�h��ǉ�����ꏊ ***
std::vector<NodePair> getNodes(void) {
	//�m�[�h�������ɒǉ�
	//NodePair("�m�[�h��", [Node ID], [�N���G�C�^�[�֐��̃|�C���^], [initialize�֐��̃|�C���^]),
	return std::vector<NodePair> {
		NodePair("wlibCarJibCrane", 0x71000, wlib::CarJibCraneNode::creator, wlib::CarJibCraneNode::initialize),
			NodePair("wlibGamepad", 0x71001, wlib::GamepadNode::creator, wlib::GamepadNode::initialize),
			NodePair("wlibCameraRecord", 0x71002, wlib::CameraRecordNode::creator, wlib::CameraRecordNode::initialize),
			NodePair("wlibFocusController", 0x71003, wlib::FocusControllerNode::creator, wlib::FocusControllerNode::initialize),
			NodePair("wlibCameraMultiTrackRecorder", 0x71004, wlib::CameraMultiTrackRecorderNode::creator, wlib::CameraMultiTrackRecorderNode::initialize),
			NodePair("wlibCameraStabilizer", 0x71005, wlib::CameraStabilizerNode::creator, wlib::CameraStabilizerNode::initialize),
			NodePair("wlibCameraGimbal", 0x71006, wlib::CameraGimbalNode::creator, wlib::CameraGimbalNode::initialize),
			NodePair("wlibMotionMixer", 0x71007, wlib::MotionMixerNode::creator, wlib::MotionMixerNode::initialize),
			NodePair("wlibMotionRecorder", 0x71008, wlib::MotionRecorderNode::creator, wlib::MotionRecorderNode::initialize),
			NodePair("wlibDrone", 0x71009, wlib::DroneNode::creator, wlib::DroneNode::initialize),
			NodePair("wlibPadToRotate", 0x71010, wlib::PadToRotateNode::creator, wlib::PadToRotateNode::initialize),
			NodePair("wlibRotate", 0x71011, wlib::RotateNode::creator, wlib::RotateNode::initialize),
			NodePair("wlibScalarRecorder", 0x71012, wlib::ScalarRecorderNode::creator, wlib::ScalarRecorderNode::initialize),
			NodePair("wlibAngleRecorder", 0x71013, wlib::AngleRecorderNode::creator, wlib::AngleRecorderNode::initialize)
	};
}


/** [hidden]addCommands
* �R�}���h��ǉ�����B�R�}���h�̈ꗗ��getCommands�֐����ɋL�q����
* @param plugin �v���O�C���C���X�^���X
* @throws MStatusException �R�}���h���o�^�ł��Ȃ���
*/
void addCommands(MFnPlugin & plugin) {
	std::vector<CommandPair> cmd_pairs = getCommands();
	for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.registerCommand(p->command, p->creator), "�R�}���h�̓o�^���ɃG���[���������܂��� : �R�}���h��" + p->command, "addCommands");
	}
}


/** [hidden]addNodes
* �m�[�h��ǉ�����B�m�[�h�̈ꗗ��getNodes�֐����ɋL�q����
* @param plugin �v���O�C���C���X�^���X
* @throws MStatusException �m�[�h���o�^�ł��Ȃ���
*/
void addNodes(MFnPlugin & plugin) {
	std::vector<NodePair> node_pairs = getNodes();
	for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.registerNode(p->node, p->id, p->creator, p->initialize, p->type), "�m�[�h�̓o�^���ɃG���[���������܂��� : �m�[�h��" + p->node, "addNode");
	}
}


/** [hidden]removeCommands
* �R�}���h��o�^��������B�R�}���h�̈ꗗ��getCommands�֐����ɋL�q����
* @param plugin �v���O�C���C���X�^���X
* @throws MStatusException �R�}���h���o�^�����ł��Ȃ���
*/
void removeCommands(MFnPlugin & plugin) {
	std::vector<CommandPair> cmd_pairs = getCommands();
	for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.deregisterCommand(p->command), "�R�}���h�̓o�^�������ɃG���[���������܂��� : �R�}���h��" + p->command, "addCommands");
	}
}


/** [hidden]addNodes
* �m�[�h��o�^��������B�m�[�h�̈ꗗ��getNodes�֐����ɋL�q����
* @param plugin �v���O�C���C���X�^���X
* @throws MStatusException �m�[�h���o�^�����ł��Ȃ���
*/
void removeNodes(MFnPlugin & plugin) {
	std::vector<NodePair> node_pairs = getNodes();
	for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.deregisterNode(p->id), "�m�[�h�̓o�^���ɃG���[���������܂��� : �m�[�h��" + p->node, "addNode");
	}
}

};


/** initializePlugin
 * �v���O�C���̏������������s���Bmaya������R�[�������֐��B
 */
MStatus wlib::initializePlugin(MObject _obj) {
	MFnPlugin plugin(_obj, "Autodesk", "2015 update2");
	std::cerr << "White Library - Car Camera Rig Plug-in" << std::endl;
	std::cerr << "version 0.1" << std::endl;
	MStatus stat = MStatus::kSuccess;
	try {
		addCommands(plugin);
		addNodes(plugin);

		//Translator
		MStatusException::throwIf(plugin.registerFileTranslator("exaTranslator", "", &ExaTranslator::creator, "", "", true), "exaExport�̒ǉ��Ɏ��s");

		//CameraHelperHUD�֌W
		MStatusException::throwIf(plugin.registerNode("wlibCameraHelperHUD", 0x72000, &CameraHelperHUD::creator, &CameraHelperHUD::initialize, MPxNode::kLocatorNode, &CameraHelperHUD::drawDbClassification), "CameraHelperHUD�̓o�^�Ɏ��s");
		MStatusException::throwIf(MHWRender::MDrawRegistry::registerDrawOverrideCreator(CameraHelperHUD::drawDbClassification, CameraHelperHUD::drawRegistrantId, CameraHelperHUDDrawOverride::Creator), "CameraHelperHUDDrawOverride�̓o�^�Ɏ��s");

		//CameraRigNode�֌W
		MStatusException::throwIf(plugin.registerNode("wlibCameraRig", 0x72001, &CameraRigNode::creator, &CameraRigNode::initialize, MPxNode::kLocatorNode, &CameraRigNode::drawDbClassification), "CameraRigNode�̓o�^�Ɏ��s");
		MStatusException::throwIf(MHWRender::MDrawRegistry::registerDrawOverrideCreator(CameraRigNode::drawDbClassification, CameraRigNode::drawRegistrantId, CameraRigNodeDrawOverride::Creator), "CameraRigNodeDrawOverride�̓o�^�Ɏ��s");

	}
	catch (MStatusException e) {
		std::cerr << e.toString("wlib::initializePlugin") << std::endl;
		stat = e.stat;
	}


	//���j���[�̒ǉ�
	MGlobal::executeCommand("wlibCameraTools()", true);


	if (stat == MStatus::kSuccess) {
		//�ǂݍ��݊���
		std::cerr << "Finished to load CarMotionAnimator plug-in." << std::endl;
		std::cerr << "(c) 2017 S.Shirao" << std::endl;
#ifdef _DEBUG
		//�f�o�b�O���̂ݕW���o�͂��G���[�o�͂֐؂�ւ�
		std::cout.rdbuf(std::cerr.rdbuf());
		std::cout << "CarMotionAnimator is debug mode." << std::endl;
#endif
	}
	else {
		//�ǂݍ��ݎ��s
		std::cerr << "Failed to initialize plugin." << std::endl;
	}

	return stat;
}


/** uninitializePlugin
 * �v���O�C���̃A�����[�h�������s���Bmaya������R�[�������֐��B
 */
MStatus wlib::uninitializePlugin(MObject _obj) {
	MFnPlugin plugin(_obj);
	MStatus stat = MStatus::kSuccess;
	try {
		removeCommands(plugin);
		removeNodes(plugin);

		MStatusException::throwIf(plugin.deregisterFileTranslator("exaTranslator"), "exaExport�̉����Ɏ��s");

		MStatusException::throwIf(MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(CameraHelperHUD::drawDbClassification, CameraHelperHUD::drawRegistrantId), "CameraHUDDrawOverride�̉����Ɏ��s���܂���");
		MStatusException::throwIf(plugin.deregisterNode(0x72000), "CameraHUD�̉����Ɏ��s");

		MStatusException::throwIf(MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(CameraRigNode::drawDbClassification, CameraRigNode::drawRegistrantId), "CameraRigNodeDrawOverride�̉����Ɏ��s���܂���");
		MStatusException::throwIf(plugin.deregisterNode(0x72001), "CameraRigNode�̉����Ɏ��s");
	}
	catch (MStatusException e) {
		std::cerr << e.toString("wlib::uninitializePlugin") << std::endl;
		stat = e.stat;
	}

	//���j���[�̉���
	//MGlobal::executeCommand("wlib_DeleteUI");

	return stat;
}