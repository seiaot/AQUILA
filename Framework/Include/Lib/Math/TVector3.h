#pragma once

#include <math.h>
#include <float.h>
#include <directxmath.h>

using namespace DirectX;

namespace Lib{
namespace Math {

template <class _T>
class TVector2;

template <class _T>
class TVector3
{
public:
	_T x;
	_T y;
	_T z;

public:

	TVector3() :x((_T)0), y((_T)0), z((_T)0) {}
	TVector3(_T _x, _T _y, _T _z) :x(_x), y(_y), z(_z) {}

	TVector3<_T>& operator=(const TVector3<_T>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	TVector3<_T>& operator+=(const TVector3<_T>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	TVector3<_T>& operator-=(const TVector3<_T>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	TVector3<_T>& operator*=(_T s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	TVector3<_T>& operator/=(_T s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	_T GetLength()const {
		FXMVECTOR v = { (float)x,(float)y,(float)z };
		float len = 0.0f;
		XMStoreFloat(&len, DirectX::XMVector3Length(v));
		return (_T)len;
	}

	//自分を正規化します
	void Normalize() {
		FXMVECTOR v = { (float)x,(float)y,(float)z };
		DirectX::XMFLOAT3 rv{};
		XMStoreFloat3(&rv, DirectX::XMVector3Normalize(v));
		x = (_T)rv.x;
		y = (_T)rv.y;
		z = (_T)rv.z;
	}

	//単位ベクトル（大きさ1のベクトル）を返します。
	TVector3<_T> GetUnitVector()const
	{
		FXMVECTOR v = { (float)x,(float)y,(float)z };
		DirectX::XMFLOAT3 rv{};
		XMStoreFloat3(&rv, DirectX::XMVector3Normalize(v));
		TVector3<_T> result((_T)rv.x, (_T)rv.y, (_T)rv.z);
		return result;
	}

	//与えた角度で回転させたベクトルを返す
	//引数は回転の対象ベクトル・回転角度・回転軸
	static TVector3<float> GetRotatedVector(TVector3<float> _vector, const float& _rot, TVector3<float>& _axis)
	{
		//これを処理の中で書き換えて最終的な戻り値とする
		XMVECTOR reVector = { _vector.x,_vector.y,_vector.z,0.0f };

		float sinHalf = sinf(XMConvertToRadians(_rot / 2.0f));	//回転角度のsin/2を代入(弧度法)

		TVector3<float> unitAxis = _axis.GetUnitVector();		//正規化

		//回転クォータニオン
		XMVECTOR rotQ =
		{
			unitAxis.x * sinHalf,
			unitAxis.y * sinHalf,
			unitAxis.z * sinHalf,
			cosf(XMConvertToRadians(_rot / 2.0f))
		};

		//回転クォータニオン*回転対象*逆クォータニオン
		reVector = XMQuaternionMultiply(XMQuaternionMultiply(rotQ, reVector), XMQuaternionInverse(rotQ));

		//三軸の回転を行った結果をVector3として返す
		return TVector3<float>(reVector.m128_f32[0], reVector.m128_f32[1], reVector.m128_f32[2]);
	}

	//与えた角度で回転させたベクトルを返す(XYZの順に回転させる)
	//引数は回転の対象ベクトル・回転角度・回転軸（回転軸は引数を渡さなければxyz方向そのまま）
	static TVector3<float> GetRotatedVector3Axis(TVector3<float> _vector,const float& _rx, const float& _ry, const float& _rz, TVector3<float> axisX = TVector3<float>(1.0f, 0.0f, 0.0f), TVector3<float> axisY = TVector3<float>(0.0f, 1.0f, 0.0f), TVector3<float> axisZ = TVector3<float>(0.0f, 0.0f, 1.0f))
	{
		//これを処理の中で書き換えて最終的な戻り値とする
		TVector3<float> reVector = _vector;

		//受け取った回転軸を配列にへ
		TVector3<float> axis[3] = { axisX,axisY,axisZ };
		//受け取った角度を配列へ
		float angle[3] = {_rx,_ry,_rz };

		//三軸分の繰り返し
		for (int i = 0;i < 3;++i)
		{
			//回転させる角度が0の時は処理しない
			if (angle[i] != 0.0f)
			{
				//回転結果を代入
				reVector = GetRotatedVector(reVector, angle[i], axis[i]);
			}
		}

		//三軸の回転を行った結果を返す
		return reVector;
	}


	static _T Dot(const TVector3<_T>& v0, const TVector3<_T>& v1) {
		return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
	}
	static TVector3<_T> Cross(const TVector3<_T>& v0, const TVector3<_T>& v1) {
		TVector3<_T> result(
			(v0.y * v1.z) - (v0.z * v1.y),
			(v0.z * v1.x) - (v0.x * v1.z),
			(v0.x * v1.y) - (v0.y * v1.x)
		);
		return result;
	}

	// Vector2 へのキャスト演算子（前方宣言）
	operator TVector2<_T>() const;

};

//  +演算子のオーバーロード
template <typename _T>
TVector3<_T> operator+(const TVector3<_T>& v0, const TVector3<_T>& v1) {
	TVector3<_T> vr(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
	return vr;
}

//  -演算子のオーバーロード
template <typename _T>
TVector3<_T> operator-(const TVector3<_T>& v0, const TVector3<_T>& v1) {
	TVector3<_T> vr(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TVector3<_T> operator*(const _T s, const TVector3<_T>& v) {
	TVector3<_T> vr(v.x * s, v.y * s, v.z * s);
	return vr;
}

//  *演算子のオーバーロード
template <typename _T>
TVector3<_T> operator*(const TVector3<_T>& v, const _T s) {
	TVector3<_T> vr(v.x * s, v.y * s, v.z * s);
	return vr;
}

//  /演算子のオーバーロード
template <typename _T>
TVector3<_T> operator/(const TVector3<_T>& v, const _T s) {
	TVector3<_T> vr(v.x / s, v.y / s, v.z / s);
	return vr;
}

//  ==演算子のオーバーロード
template <typename _T>
bool operator==(const TVector3<_T>& v0, const TVector3<_T>& v1) {
	return (v0 == v1);
}

//floatで特殊化
template <>
bool operator==<float>(const TVector3<float>& v0, const TVector3<float>& v1);

//doubleで特殊化
template <>
bool operator==<double>(const TVector3<double>& v0, const TVector3<double>& v1);

//intで特殊化
template <>
bool operator==<int>(const TVector3<int>& v0, const TVector3<int>& v1);

//  !=演算子のオーバーロード
template <typename _T>
bool operator!=(const TVector3<_T>& v0, const TVector3<_T>& v1) {
	return !(v0 == v1);
}

//汎用ベクトル3次元ベクトル型定義
typedef TVector3<float> Vector3f;
typedef TVector3<int> Vector3i;
typedef TVector3<double> Vector3d;

}//namespace Math
}//namespace Lib