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
 * FollowGroundノードクラス
 * 始点からのベクトルレイと、地面との交点座標を算出するノード
 * @version 1.0
 * @see MPxNode http://download.autodesk.com/us/maya/2011help/api/class_m_px_node.html
 */
class FollowGround : public MPxNode{
public:
	//////////////////////////////
	// PUBLIC MEMBER CONSTANT
	//////////////////////////////
	/**
	 * ノードID定数
	 */
	enum { kFollowGroundID = 0x70000 };

	//////////////////////////////
	// PUBLIC MEMBER VALIABLES
	//////////////////////////////

	/** 入力メッシュ
	 * 地面として使用される。
	 */
	static MObject inMesh;

	/** 出力ポイント
	 * 地面に接する点の座標
	 */
	static MObject outPoint;

	/** 入力ベクトル
	 * 始点からのベクトルレイ
	 */
	static MObject rayVector;
	
	/** レイ始点
	 */
	static MObject rayPoint;

	/** 距離
	 */
	static MObject distance;

	/** 最大距離
	 */
	static MObject maxDistance;
	
	/** ノードID
	 */
	static const MTypeId id;
	
	//////////////////////////////
	// PUBLIC MEMBER FUNCTION
	//////////////////////////////

	/** コンストラクタ
	 */	
	FollowGround();

	/** デストラクタ
	 */
	virtual ~FollowGround();

	/**
	 * ノードの計算処理部分
	 * 距離を測って物体を移動させる
	 * @param plug 計算中のプラグ
	 * @param data 編集可能な内部データ
	 * @return 計算に成功した場合kSuccessを返す
	 * @see MPxNode::compute http://download.autodesk.com/us/maya/2011help/api/class_m_px_node.html#3dd5454e3fa92598836018ae9ab220c3
	 */
	virtual MStatus compute(const MPlug & plug,	MDataBlock & data) override;

	/**
	 * 新しいインスタンスの生成関数
	 * @return 新しいインスタンスのnewポインタ
	 */
	static void * creator(void);

	/**
	 * ノード登録時に呼ばれる関数
	 * @return 初期化に成功した場合kSuccessを返す
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
	 * inMeshのアトリビュート追加
	 * @throws MStatusException ステータスがkSuccess以外の時に、詳細を含めて例外を投げる
	 * @see cma::MStatusException
	 */
	static void _addAttr_inMesh(void);
	
	/**
	 * inVectorのアトリビュート追加
	 * @throws MStatusException ステータスがkSuccess以外の時に、詳細を含めて例外を投げる
	 * @see cma::MStatusException
	 */
	static void _addAttr_rayVector(void);
	
	/**
	 * outPointのアトリビュート追加
	 * @throws MStatusException ステータスがkSuccess以外の時に、詳細を含めて例外を投げる
	 * @see cma::MStatusException
	 */
	static void _addAttr_outPoint(void);

	/**
	 * rayPointのアトリビュート追加
	 * @throws MStatusException ステータスがkSuccess以外の時に、詳細を含めて例外を投げる
	 * @see cma::MStatusException
	 */
	static void _addAttr_rayPoint(void);

	/**
	 * distance,maxDistanceのアトリビュート追加
	 * @throws MStatusException ステータスがkSuccess以外の時に、詳細を含めて例外を投げる
	 * @see cma::MStatusException
	 */
	static void _addAttr_distance(void);

	/**
	 * 再計算(compute)時に使用するアトリビュート値の取得
	 * @param data compute関数の引数であるdataの参照
	 * @param ground グラウンド
	 * @param ray_from レイ始点
	 * @param ray_vector レイベクトル
	 * @param max_distance 最大距離
	 * @throws MStatusException それぞれの取得結果がkSuccess以外だった場合
	 * @see cma::MStatusException
	 */
	static void _getInputValue(MDataBlock & data, MDataHandle * ground, MDataHandle * ray_from, MDataHandle * ray_vector, MDataHandle * max_distance);

	/**
	 * 再計算された値をアトリビュートへ反映させる
	 * @param data compute関数の引数であるdataの参照
	 * @param result 格納する結果の値
	 * @throws MStatusException 格納時の結果がkSuccess以外だった場合
	 * @see cma::MStatusException
	 */
	static void _setOutputValue(MDataBlock & data, const float x, const float y, const float z);
	
	/** 
	 * フェースを取得し、それぞれのフェースとの距離を求める。
	 * @param ground グラウンドのオブジェクト
	 * @param ray_point レイ始点のポイント
	 * @param ray_vector レイのベクトル
	 * @param is_cross 交わったかどうかを格納する
	 * @param max_distance 最大距離
	 * @throws MStatusException 実行中にkSuccess以外のエラーが発生した場合
	 * @return 距離
	 */
	static double _getLengthToCrossPoint(const MObject & ground, const VectorF ray_point, const VectorF ray_vector, bool * is_cross = nullptr, const double max_distance = 100000.0);

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
	static double _checkHitPolygon(const MPointArray & points, const VectorF ray_point, const VectorF ray_vector, bool * is_cross = nullptr, const double max_distance = 100000.0);

};

};

#endif //end of include guard
