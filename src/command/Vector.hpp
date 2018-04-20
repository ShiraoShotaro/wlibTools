
#pragma once
#ifndef WLIB_VECTOR_HPP_
#define WLIB_VECTOR_HPP_

#include <cmath>
#include <string>
#include <iostream>
#include <maya/MTypes.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>

namespace wlib {

/** 
 * ベクトルクラス
 * 型はdouble、float、intを想定している
 */
template <typename T> class Vector{
public:

	//////////////////////////////
	// PUBLIC MEMBER VALIABLES
	//////////////////////////////
	/** 座標パラメータ
	 */
	T x, y, z;
	
	//////////////////////////////
	// PUBLIC MEMBER FUNCTION
	//////////////////////////////

	/** デフォルトコンストラクタ
	 */	
	Vector()
		: x(static_cast<T>(0.0)), y(static_cast<T>(0.0)), z(static_cast<T>(0.0)) {}

	/** パラメータコンストラクタ
	 * @param x x座標
	 * @param y y座標
	 * @param z z座標
	 */
	Vector(const T x, const T y, const T z)
		: x(x), y(y), z(z) {}
	
	/** キャスト的コンストラクタ
	 * @param cast 3つの要素を持った配列
	 */
	template <typename N> Vector(const N & cast);

	/** Vectorキャストコンストラクタ
	 * @param cast 他の型のベクトル
	 */
	template <typename N> Vector(const Vector<N> & cast);
	
	/** MPointからのコンストラクタ
	 * @param m_point MPoint型からキャスト
	 */
	Vector(MPoint m_point);

	/** MVectorからのコンストラクタ
	 * @param m_vector MVector型からキャスト
	 */
	Vector(MVector m_vector);

	/** デストラクタ
	 */
	~Vector() {}

	/**
	 * ベクトルの長さを計算する
	 * @return ベクトルの長さ
	 */
	template <typename N> N length(void) const;

	/**
	 * 逆向きのベクトルを取得する
	 * @return 逆向きベクトル
	 */
	Vector<T> inv(void) const;
	
	/**
	 * ベクトル同士の加算処理
	 * @param v 加算するベクトル
	 * @return 加算後のベクトル
	 */
	Vector<T> operator+(const Vector<T> & v) const;
	
	/**
	 * ベクトル同士の減算処理
	 * @param v 減算するベクトル
	 * @return 減算後のベクトル
	 */
	Vector<T> operator-(const Vector<T> & v) const;

	/**
	* ベクトルの乗算処理
	* @param val 倍率
	* @return 乗算後のベクトル
	*/
	template <typename N> Vector<T> operator*(const N & val) const;

	/**
	* ベクトルの外積
	* @return 乗算後のベクトル
	*/
	Vector<T> operator*(const T & vec) const;

	/** 表示用文字列に変換
	 * @return 文字列
	 */
	std::string toString(void) const;

	/**
	 * 外積を求める
	 * @param v1 ベクトル1
	 * @param v2 ベクトル2
	 * @param v3 ベクトル3
	 * @return 外積
	 */
	static T det(const Vector<T> & v1, const Vector<T> & v2, const Vector<T> & v3);

protected:

private:

};

template class Vector<int>;
template class Vector<double>;
template class Vector<float>;

template<> template<>
inline wlib::Vector<double>::Vector(const double3 & cast) {
	this->x = cast[0]; this->y = cast[1]; this->z = cast[2];
}

template<> template<>
inline wlib::Vector<float>::Vector(const float3 & cast) {
	this->x = cast[0]; this->y = cast[1]; this->z = cast[2];
}

template<> template<>
inline wlib::Vector<int>::Vector(const int3 & cast) {
	this->x = cast[0]; this->y = cast[1]; this->z = cast[2];
}

template<typename T>
template<typename N>
inline wlib::Vector<T>::Vector(const Vector<N> & cast) {
	this->x = static_cast<T>(cast.x);
	this->y = static_cast<T>(cast.y);
	this->z = static_cast<T>(cast.z);
}

template<typename T>
template<typename N>
inline N Vector<T>::length(void) const {
	return static_cast<N>(
		std::sqrt(
			static_cast<double>(this->x) * this->x + 
			static_cast<double>(this->y) * this->y +
			static_cast<double>(this->z) * this->z
		)
	);
}

template<typename T>
template<typename N>
inline Vector<T> Vector<T>::operator*(const N & val) const {
	return Vector<T>(
		static_cast<T>(this->x * val),
		static_cast<T>(this->y * val),
		static_cast<T>(this->z * val)	
	);
}

typedef Vector<double> VectorD;
typedef Vector<float> VectorF;
typedef Vector<int> VectorI;

template<typename T>
inline Vector<T>::Vector(MPoint m_point) {
	m_point.cartesianize();
	this->x = static_cast<T>(m_point.x);
	this->y = static_cast<T>(m_point.y);
	this->z = static_cast<T>(m_point.z);
}

template<typename T>
inline Vector<T>::Vector(MVector m_vector) {
	this->x = static_cast<T>(m_vector.x);
	this->y = static_cast<T>(m_vector.y);
	this->z = static_cast<T>(m_vector.z);
}

template<typename T>
inline Vector<T> Vector<T>::inv(void) const {
	return Vector<T>(-this->x, -this->y, -this->z);
}

template<typename T>
inline Vector<T> Vector<T>::operator+(const Vector<T>& v) const {
	return Vector<T>(this->x + v.x, this->y + v.y, this->z + v.z);
}

template<typename T>
inline Vector<T> Vector<T>::operator-(const Vector<T>& v) const {
	return Vector<T>(this->x - v.x, this->y - v.y, this->z - v.z);
}

template<typename T>
inline Vector<T> Vector<T>::operator*(const T & vec) const
{
	return Vector<T>((this->y * vec.z) - (this->z * vec.y), (this->z * vec.x) - (this->x * vec.z), (this->x * vec.y) - (this->y * vec.x));
}

template<typename T>
inline std::string Vector<T>::toString(void) const {
	return std::string("(x, y, z) = (" + 
		std::to_string(this->x) + ", " +
		std::to_string(this->y) + ", " +
		std::to_string(this->z) + ")");
}

template<typename T>
inline T Vector<T>::det(const Vector<T>& v1, const Vector<T>& v2, const Vector<T>& v3) {
	return ((v1.x * v2.y * v3.z)
          + (v1.y * v2.z * v3.x)
          + (v1.z * v2.x * v3.y)
          - (v1.x * v2.z * v3.y)
          - (v1.y * v2.x * v3.z)
          - (v1.z * v2.y * v3.x));
}

};

#endif //end of include guard
