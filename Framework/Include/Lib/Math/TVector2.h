#pragma once

#include <math.h>
#include <float.h>
#include <directxmath.h>

using namespace DirectX;

namespace Lib{
namespace Math {

template <class _T>
class TVector3;

template <class _T>
class TVector2
{
public:
	_T x;
	_T y;

public:

	TVector2() :x((_T)0), y((_T)0) {}
	TVector2(_T _x,_T _y) :x(_x), y(_y) {}

	TVector2<_T>& operator=(const TVector2<_T>& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	TVector2<_T>& operator+=(const TVector2<_T>& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	TVector2<_T>& operator-=(const TVector2<_T>& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	TVector2<_T>& operator*=(_T s) {
		x *= s;
		y *= s;
		return *this;
	}

	TVector2<_T>& operator/=(_T s) {
		x /= s;
		y /= s;
		return *this;
	}

	_T GetLength()const {
		FXMVECTOR v = { (float)x,(float)y };
		float len = 0.0f;
		XMStoreFloat(&len, DirectX::XMVector2Length(v));
		return (_T)len;
	}
	void Rotate(float radian) {
		float c = cosf(radian);
		float s = sinf(radian);

		float _x = (c * (float)x) + (-s * (float)y);
		float _y = (s * (float)x) + (c * (float)y);

		x = (_T)_x;
		y = (_T)_y;
	}
	TVector2<_T> GetRotated(float radian) const{
		float c = cosf(radian);
		float s = sinf(radian);

		float _x = (c * (float)x) + (-s * (float)y);
		float _y = (s * (float)x) + (c * (float)y);

		return TVector2<_T>((_T)_x,(_T)_y );
	}

	//自分を正規化します
	void Normalize() {
		FXMVECTOR v = { (float)x,(float)y };
		DirectX::XMFLOAT2 rv{};
		XMStoreFloat2(&rv, DirectX::XMVector2Normalize(v));
		x = (_T)rv.x;
		y = (_T)rv.y;
	}

	//単位ベクトル（大きさ1のベクトル）を返します。
	TVector2<_T> GetNormalized()const
	{
		FXMVECTOR v = { (float)x,(float)y };
		DirectX::XMFLOAT2 rv{};
		XMStoreFloat2(&rv, DirectX::XMVector2Normalize(v));
		TVector2<_T> result((_T)rv.x, (_T)rv.y);
		return result;
	}

	//指定のベクトルを基準としたときのなす角（ラジアン）を返す
	float GetRadian(const TVector2<_T>& _v) {
		TVector2<_T> n0 = GetNormalized();
		TVector2<_T> n1 = _v.GetNormalized();
		return acosf((float)Dot(n0, n1));
	}

	static _T Dot(const TVector2<_T>& v0, const TVector2<_T>& v1) {
		return (v0.x * v1.x) + (v0.y * v1.y);
	}
	static _T Cross(const TVector2<_T>& v0, const TVector2<_T>& v1) {
		return (v0.x * v1.y) - (v0.y * v1.x);
	}

	static TVector2<_T> Lerp(const TVector2<_T>& v0, const TVector2<_T>& v1, float t) {

		FXMVECTOR _v0 = { (float)v0.x, (float)v0.y };
		FXMVECTOR _v1 = { (float)v1.x, (float)v1.y };
		XMVECTOR v = XMVectorLerp(_v0, _v1, t);
		TVector2<_T> result((_T)v.m128_f32[0], (_T)v.m128_f32[1]);
		return result;
	}

	static TVector2<_T> SLerp(const TVector2<_T>& v0, const TVector2<_T>& v1, float t) {

		const auto toQuaternion = [](const TVector2<_T>& _v) 
		{
			const FXMVECTOR axis = { 0.0f,0.0f,1.0f };
			const TVector2<_T> right((_T)1,(_T)0);

			TVector2<_T> vN = _v.GetNormalized();

			_T _cos = Dot(right, vN);
			_T _sin = Cross(right, vN);
			float rad = acosf((float)_cos);
			if (_sin < 0.0f) {
				rad = XM_2PI - rad;
			}
			return XMQuaternionRotationAxis(axis, rad);
		};


		XMVECTOR q0 = toQuaternion(v0);
		XMVECTOR q1 = toQuaternion(v1);
		XMVECTOR s = XMQuaternionSlerp(q0, q1, t);

		_T d0 = v0.GetLength();
		_T d1 = v1.GetLength();
		_T d = (_T)(d0 + (d1 - d0) * t);

		TVector2<_T> v2 = v0.GetNormalized() * d;

		FXMVECTOR _v2 = { (float)v2.x, (float)v2.y };
		FXMVECTOR v = XMVector3Rotate(_v2, s);

		TVector2<_T> result((_T)v.m128_f32[0], (_T)v.m128_f32[1]);
		return result;
	}


	// Vector3 へのキャスト演算子（前方宣言）
	operator TVector3<_T>() const;

};

//  +演算子のオーバーロード
template <typename _T>
TVector2<_T> operator+(const TVector2<_T>& v0, const TVector2<_T>& v1) {
	TVector2<_T> vr(v0.x + v1.x, v0.y + v1.y);
	return vr;
}

//  -演算子のオーバーロード
template <typename _T>
TVector2<_T> operator-(const TVector2<_T>& v0, const TVector2<_T>& v1) {
	TVector2<_T> vr(v0.x - v1.x, v0.y - v1.y);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TVector2<_T> operator*(const _T s, const TVector2<_T>& v) {
	TVector2<_T> vr(v.x * s, v.y * s);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TVector2<_T> operator*(const TVector2<_T>& v, const _T s) {
	TVector2<_T> vr(v.x * s, v.y * s);
	return vr;
}

//  /演算子のオーバーロード
template <typename _T>
TVector2<_T> operator/(const TVector2<_T>& v, const _T s) {
	TVector2<_T> vr(v.x / s, v.y / s);
	return vr;
}

//  ==演算子のオーバーロード
template <typename _T>
bool operator==(const TVector2<_T>& v0, const TVector2<_T>& v1) {
	return (v0 == v1);
}

//floatで特殊化
template <>
bool operator==<float>(const TVector2<float>& v0, const TVector2<float>& v1);

//doubleで特殊化
template <>
bool operator==<double>(const TVector2<double>& v0, const TVector2<double>& v1);

//intで特殊化
template <>
bool operator==<int>(const TVector2<int>& v0, const TVector2<int>& v1);

//  !=演算子のオーバーロード
template <typename _T>
bool operator!=(const TVector2<_T>& v0, const TVector2<_T>& v1) {
	return !(v0 == v1);
}

//汎用ベクトル2次元ベクトル型定義
typedef TVector2<float> Vector2f;
typedef TVector2<int> Vector2i;
typedef TVector2<double> Vector2d;


}//namespace Math
}//namespace Lib