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

//*** 新しいコマンドを追加する場所 ***
std::vector<CommandPair> getCommands(void) {
	//コマンドをここに追加
	//CommandPair("コマンド名", [クリエイター関数のポインタ]),
	return std::vector<CommandPair> {
		CommandPair("wlibSetKeyframes", wlib::SetKeyframesCommand::creator),
//		CommandPair("wlib_sortVertexID", wlib::SortVertexID::creator),
		//CommandPair("Hogehoge", wlib::Hogehoge::creator),
	};
}

//*** 新しいノードを追加する場所 ***
std::vector<NodePair> getNodes(void) {
	//ノードをここに追加
	//NodePair("ノード名", [Node ID], [クリエイター関数のポインタ], [initialize関数のポインタ]),
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
* コマンドを追加する。コマンドの一覧はgetCommands関数内に記述する
* @param plugin プラグインインスタンス
* @throws MStatusException コマンドが登録できない時
*/
void addCommands(MFnPlugin & plugin) {
	std::vector<CommandPair> cmd_pairs = getCommands();
	for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.registerCommand(p->command, p->creator), "コマンドの登録中にエラーが発生しました : コマンド名" + p->command, "addCommands");
	}
}


/** [hidden]addNodes
* ノードを追加する。ノードの一覧はgetNodes関数内に記述する
* @param plugin プラグインインスタンス
* @throws MStatusException ノードが登録できない時
*/
void addNodes(MFnPlugin & plugin) {
	std::vector<NodePair> node_pairs = getNodes();
	for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.registerNode(p->node, p->id, p->creator, p->initialize, p->type), "ノードの登録中にエラーが発生しました : ノード名" + p->node, "addNode");
	}
}


/** [hidden]removeCommands
* コマンドを登録解除する。コマンドの一覧はgetCommands関数内に記述する
* @param plugin プラグインインスタンス
* @throws MStatusException コマンドが登録解除できない時
*/
void removeCommands(MFnPlugin & plugin) {
	std::vector<CommandPair> cmd_pairs = getCommands();
	for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.deregisterCommand(p->command), "コマンドの登録解除中にエラーが発生しました : コマンド名" + p->command, "addCommands");
	}
}


/** [hidden]addNodes
* ノードを登録解除する。ノードの一覧はgetNodes関数内に記述する
* @param plugin プラグインインスタンス
* @throws MStatusException ノードが登録解除できない時
*/
void removeNodes(MFnPlugin & plugin) {
	std::vector<NodePair> node_pairs = getNodes();
	for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
		wlib::MStatusException::throwIf(plugin.deregisterNode(p->id), "ノードの登録中にエラーが発生しました : ノード名" + p->node, "addNode");
	}
}

};


/** initializePlugin
 * プラグインの初期化処理を行う。maya側からコールされる関数。
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
		MStatusException::throwIf(plugin.registerFileTranslator("exaTranslator", "", &ExaTranslator::creator, "", "", true), "exaExportの追加に失敗");

		//CameraHelperHUD関係
		MStatusException::throwIf(plugin.registerNode("wlibCameraHelperHUD", 0x72000, &CameraHelperHUD::creator, &CameraHelperHUD::initialize, MPxNode::kLocatorNode, &CameraHelperHUD::drawDbClassification), "CameraHelperHUDの登録に失敗");
		MStatusException::throwIf(MHWRender::MDrawRegistry::registerDrawOverrideCreator(CameraHelperHUD::drawDbClassification, CameraHelperHUD::drawRegistrantId, CameraHelperHUDDrawOverride::Creator), "CameraHelperHUDDrawOverrideの登録に失敗");

		//CameraRigNode関係
		MStatusException::throwIf(plugin.registerNode("wlibCameraRig", 0x72001, &CameraRigNode::creator, &CameraRigNode::initialize, MPxNode::kLocatorNode, &CameraRigNode::drawDbClassification), "CameraRigNodeの登録に失敗");
		MStatusException::throwIf(MHWRender::MDrawRegistry::registerDrawOverrideCreator(CameraRigNode::drawDbClassification, CameraRigNode::drawRegistrantId, CameraRigNodeDrawOverride::Creator), "CameraRigNodeDrawOverrideの登録に失敗");

	}
	catch (MStatusException e) {
		std::cerr << e.toString("wlib::initializePlugin") << std::endl;
		stat = e.stat;
	}


	//メニューの追加
	MGlobal::executeCommand("wlibCameraTools()", true);


	if (stat == MStatus::kSuccess) {
		//読み込み完了
		std::cerr << "Finished to load CarMotionAnimator plug-in." << std::endl;
		std::cerr << "(c) 2017 S.Shirao" << std::endl;
#ifdef _DEBUG
		//デバッグ時のみ標準出力をエラー出力へ切り替え
		std::cout.rdbuf(std::cerr.rdbuf());
		std::cout << "CarMotionAnimator is debug mode." << std::endl;
#endif
	}
	else {
		//読み込み失敗
		std::cerr << "Failed to initialize plugin." << std::endl;
	}

	return stat;
}


/** uninitializePlugin
 * プラグインのアンロード処理を行う。maya側からコールされる関数。
 */
MStatus wlib::uninitializePlugin(MObject _obj) {
	MFnPlugin plugin(_obj);
	MStatus stat = MStatus::kSuccess;
	try {
		removeCommands(plugin);
		removeNodes(plugin);

		MStatusException::throwIf(plugin.deregisterFileTranslator("exaTranslator"), "exaExportの解除に失敗");

		MStatusException::throwIf(MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(CameraHelperHUD::drawDbClassification, CameraHelperHUD::drawRegistrantId), "CameraHUDDrawOverrideの解除に失敗しました");
		MStatusException::throwIf(plugin.deregisterNode(0x72000), "CameraHUDの解除に失敗");

		MStatusException::throwIf(MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(CameraRigNode::drawDbClassification, CameraRigNode::drawRegistrantId), "CameraRigNodeDrawOverrideの解除に失敗しました");
		MStatusException::throwIf(plugin.deregisterNode(0x72001), "CameraRigNodeの解除に失敗");
	}
	catch (MStatusException e) {
		std::cerr << e.toString("wlib::uninitializePlugin") << std::endl;
		stat = e.stat;
	}

	//メニューの解除
	//MGlobal::executeCommand("wlib_DeleteUI");

	return stat;
}