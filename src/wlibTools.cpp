#include "wlibTools.hpp"

#include <vector>
#include <string>
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>
#include "exception/MStatusException.hpp"
#include <maya/MDrawRegistry.h>

//*** INCLUDE HEADERS ***
#include "node/FollowGround.hpp"
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
//		CommandPair("wlibSetKeyframes", wlib::SetKeyframesCommand::creator),
//		CommandPair("wlib_sortVertexID", wlib::SortVertexID::creator),
		//CommandPair("Hogehoge", wlib::Hogehoge::creator),
	};
}

//*** �V�����m�[�h��ǉ�����ꏊ ***
std::vector<NodePair> getNodes(void) {
	//�m�[�h�������ɒǉ�
	//NodePair("�m�[�h��", [Node ID], [�N���G�C�^�[�֐��̃|�C���^], [initialize�֐��̃|�C���^]),
	return std::vector<NodePair> {
		NodePair("wlibAdhereObject", 0x73000, wlib::CarJibCraneNode::creator, wlib::CarJibCraneNode::initialize),
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