#pragma once
#ifndef WLIB_ADHEREOBJECTCMD_HPP_
#define WLIB_ADHEREOBJECTCMD_HPP_

#include <memory>
#include <maya/MPxCommand.h>
#include <maya/MFnTransform.h>
#include <maya/MPointArray.h>
#include <maya/MStatus.h>
#include "Vector.hpp"

namespace wlib {

class AdhereObjectCmd : public MPxCommand {
public:

	AdhereObjectCmd() = default;
	virtual ~AdhereObjectCmd() = default;

	virtual MStatus doIt(const MArgList & args) override;
	virtual MStatus undoIt(void) override;
	virtual MStatus redoIt(void) override;

	virtual bool isUndoable(void) const;

private:

	std::unique_ptr<MFnTransform> before_;
	std::unique_ptr<MFnTransform> after_;

	/**
	* 三角面とベクトルの交点までの距離を求める
	* @param points 三角面の頂点
	* @param ray_point レイの始点
	* @param ray_vector レイベクトル
	* @param is_cross 交わったかどうか
	* @param max_distance 最大距離
	* @return 距離
	* @throws MStatusException 計算上エラーが発生した場合
	*/
	static double _checkHitPolygon(const MPointArray & points, const VectorF & ray_point, const VectorF & ray_vector, bool & is_cross, VectorF & normal, const double max_distance = 100000.0);


};
#endif
