#pragma once

#include <math.h>
#include <float.h>
#include <directxmath.h>

using namespace DirectX;

namespace Lib{
namespace Math {

template <class _T>
class TSize
{
public:
	_T w;
	_T h;
	_T d;

public:

	TSize() :w((_T)0), h((_T)0), d((_T)0) {}
	TSize(_T _w, _T _h, _T _d) :w(_w), h(_h), d(_d) {}

	TSize<_T>& operator=(const TSize<_T>& v) {
		w = v.w;
		h = v.h;
		d = v.d;
		return *this;
	}

	TSize<_T>& operator+=(const TSize<_T>& v) {
		w += v.w;
		h += v.h;
		d += v.d;
		return *this;
	}
	TSize<_T>& operator-=(const TSize<_T>& v) {
		w -= v.w;
		h -= v.h;
		d -= v.d;
		return *this;
	}
	TSize<_T>& operator*=(_T s) {
		w *= s;
		h *= s;
		d *= s;
		return *this;
	}

	TSize<_T>& operator/=(_T s) {
		w /= s;
		h /= s;
		d /= s;
		return *this;
	}

};

//  +演算子のオーバーロード
template <typename _T>
TSize<_T> operator+(const TSize<_T>& v0, const TSize<_T>& v1) {
	TSize<_T> vr(v0.w + v1.w, v0.h + v1.h, v0.d + v1.d);
	return vr;
}

//  -演算子のオーバーロード
template <typename _T>
TSize<_T> operator-(const TSize<_T>& v0, const TSize<_T>& v1) {
	TSize<_T> vr(v0.w - v1.w, v0.h - v1.h, v0.d - v1.d);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TSize<_T> operator*(const _T s, const TSize<_T>& v) {
	TSize<_T> vr(v.w * s, v.h * s, v.d * s);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TSize<_T> operator*(const TSize<_T>& v, const _T s) {
	TSize<_T> vr(v.w * s, v.h * s, v.d * s);
	return vr;
}

//  /演算子のオーバーロード
template <typename _T>
TSize<_T> operator/(const TSize<_T>& v, const _T s) {
	TSize<_T> vr(v.w / s, v.h / s, v.d / s);
	return vr;
}

//  ==演算子のオーバーロード
template <typename _T>
bool operator==(const TSize<_T>& v0, const TSize<_T>& v1) {
	return (v0 == v1);
}

//floatで特殊化
template <>
bool operator==<float>(const TSize<float>& v0, const TSize<float>& v1);

//doubleで特殊化
template <>
bool operator==<double>(const TSize<double>& v0, const TSize<double>& v1);

//intで特殊化
template <>
bool operator==<int>(const TSize<int>& v0, const TSize<int>& v1);

//  !=演算子のオーバーロード
template <typename _T>
bool operator!=(const TSize<_T>& v0, const TSize<_T>& v1) {
	return !(v0 == v1);
}

//汎用ベクトル3次元ベクトル型定義
typedef TSize<float> Sizef;
typedef TSize<int> Sizei;
typedef TSize<double> Sized;

}//namespace Math
}//namespace Lib