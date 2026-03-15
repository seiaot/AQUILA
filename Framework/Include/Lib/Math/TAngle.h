#pragma once

#include <math.h>
#include <float.h>
#include <directxmath.h>

using namespace DirectX;

namespace Lib{
namespace Math {

template <class _T>
class TAngle
{
public:
	_T x;
	_T y;
	_T z;

public:

	TAngle() :x((_T)0), y((_T)0), z((_T)0) {}
	TAngle(_T _x, _T _y, _T _z) :x(_x), y(_y), z(_z) {}

	TAngle<_T>& operator=(const TAngle<_T>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	TAngle<_T>& operator+=(const TAngle<_T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	TAngle<_T>& operator-=(const TAngle<_T>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	TAngle<_T>& operator*=(_T s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	TAngle<_T>& operator/=(_T s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

};

//  +演算子のオーバーロード
template <typename _T>
TAngle<_T> operator+(const TAngle<_T>& v0, const TAngle<_T>& v1) {
	TAngle<_T> vr(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
	return vr;
}

//  -演算子のオーバーロード
template <typename _T>
TAngle<_T> operator-(const TAngle<_T>& v0, const TAngle<_T>& v1) {
	TAngle<_T> vr(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TAngle<_T> operator*(const _T s, const TAngle<_T>& v) {
	TAngle<_T> vr(v.x * s, v.y * s, v.z * s);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TAngle<_T> operator*(const TAngle<_T>& v, const _T s) {
	TAngle<_T> vr(v.x * s, v.y * s, v.z * s);
	return vr;
}

//  /演算子のオーバーロード
template <typename _T>
TAngle<_T> operator/(const TAngle<_T>& v, const _T s) {
	TAngle<_T> vr(v.x / s, v.y / s, v.z / s);
	return vr;
}

//  ==演算子のオーバーロード
template <typename _T>
bool operator==(const TAngle<_T>& v0, const TAngle<_T>& v1) {
	return (v0 == v1);
}

//floatで特殊化
template <>
bool operator==<float>(const TAngle<float>& v0, const TAngle<float>& v1);

//doubleで特殊化
template <>
bool operator==<double>(const TAngle<double>& v0, const TAngle<double>& v1);

//intで特殊化
template <>
bool operator==<int>(const TAngle<int>& v0, const TAngle<int>& v1);

//  !=演算子のオーバーロード
template <typename _T>
bool operator!=(const TAngle<_T>& v0, const TAngle<_T>& v1) {
	return !(v0 == v1);
}

//汎用ベクトル3次元ベクトル型定義
typedef TAngle<float> Anglef;
typedef TAngle<int> Anglei;
typedef TAngle<double> Angled;

}//namespace Math
}//namespace Lib