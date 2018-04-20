/////////////////////////////////////////////////////
//wlibTools.hpp
//
//Copyright (c) 2017 Shotaro Shirao
/////////////////////////////////////////////////////

//環境構築時に参考にしたURL
//【Maya C++】Maya C++の始め方(maya2017 update4)
//https://unpyside.wixsite.com/unpyside/single-post/2017/07/03/%E3%80%90Maya-C%E3%80%91Maya-C%E3%81%AE%E5%A7%8B%E3%82%81%E6%96%B9maya2017-update4


#pragma once
#ifndef wlib_WLIBTOOLS_HPP_
#define wlib_WLIBTOOLS_HPP_

#include <maya/MFnPlugin.h>
#include <maya/MObject.h>

namespace wlib {

/** initializePlugin
 * プラグインの初期化処理を行う。maya側からコールされる関数。
 */
extern MStatus initializePlugin(MObject _obj);

/** uninitializePlugin
 * プラグインのアンロード処理を行う。maya側からコールされる関数。
 */
extern MStatus uninitializePlugin(MObject _obj);

};

#endif //end of include guard
