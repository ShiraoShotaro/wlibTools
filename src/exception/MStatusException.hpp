/// @file MStatusException.hpp
/// @brief MStatusExceptionクラスヘッダファイル
/// @author Shirao Shotaro (pdi)
/// @date 2017.12

#pragma once
#ifndef CMA_MSTATUSEXCEPTION_HPP_
#define CMA_MSTATUSEXCEPTION_HPP_

#include <iostream>
#include <maya/MString.h>
#include <maya/MStatus.h>

namespace wlib {

/// @brief MStatusを例外処理として処理しやすくするクラス
/// @version 2018.02
/// @sa MStatus
///
/// OpenMaya C++ APIのMStatus型に、例外処理を加えたもの。MStatus型のエラーを例外として処理し、煩雑なif文を書かなくて済む。
/// また、この関数を使用し例外を発生させる場合は、必ずキャッチし、そのエラー結果をエラー出力へ表示すること。
class MStatusException{
public:

	const MStatus stat;		///< MStatus本体
	const MString message;	///< エラーメッセージ
	const MString place;	///< エラーの発生場所。書き方に決まりはないが、推奨はエラーが発生した時に実行中の関数名。
	
	/// @brief コンストラクタ
	/// 
	/// エラーのステータスとメッセージを登録できる。また、placeは任意に指定でき、（必ず指定することが好ましいが）指定しない場合はunloggedと表示される。
	/// また、unloggedの場合、toString関数において上書きが可能である。
	/// @param stat エラーのステータス
	/// @param message メッセージ
	/// @param place 発生個所を特定できる文字列
	MStatusException(const MStatus & stat, const MString & message, const MString & place = "<unlogged>");

	/// デストラクタ
	virtual ~MStatusException();

	/// @brief 指定フォーマットに成形されたエラーメッセージを取得する。
	/// 
	/// @return エラーメッセージ
	MString toString(void) const;

	/// @brief 指定フォーマットに成形されたエラーメッセージを取得する。
	///
	/// 発生箇所をオーバーライドすることができます。ただし、オーバーライドするにはplaceをunloggedにしておく必要があります。
	/// @param place_override オーバーライドするplace判別文字列
	/// @return エラーメッセージ
	MString toString(const MString & place_override) const;

	/// @brief MStatus型へのキャストオーバーロード
	/// @return statの値
	operator MStatus() const;

	/// @brief MString型へのキャストオーバーロード
	/// @return toString関数の戻り値
	operator MString() const;

	/// @brief エラー区分が等しいかチェックする
	/// @param comp 比べたいエラー区分
	/// @return statとcompが等しければtrue
	bool operator==(const MStatus & comp) const;

	/// @brief ストリーム型の出力に対するオーバーロード
	///
	/// この関数を使用した場合は、placeのオーバーライドはできません。
	/// @param os ストリーム
	/// @param e 例外
	/// @return 出力後のストリーム
	friend std::ostream & operator<<(ostream & os, const MStatusException & e);

	/// @brief if内包例外スローユーティリティー関数
	///
	/// もしもステータスがkSuccess以外の時に、MStatusExceptionをスローするユーティリティー関数
	/// @param stat エラーのステータス
	/// @param message メッセージ
	/// @param place 発生個所を特定できる文字列
	/// @throws MStatusException ステータスがkSuccess以外だった場合
	static void throwIf(const MStatus & stat, const MString & message, const MString & place = "<unlogged>");

protected:

private:
	MStatusException() = delete;

};

};

#endif //end of include guard
