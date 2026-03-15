#include "Lib/Math/Vector.h"

namespace Lib{
namespace Math {

template <>
bool operator==<float>(const TVector2<float>& v0, const TVector2<float>& v1) {
	return (
		fabs(v0.x - v1.x) < FLT_EPSILON
		&& fabs(v0.y - v1.y) < FLT_EPSILON
		);
}

template <>
bool operator==<double>(const TVector2<double>& v0, const TVector2<double>& v1) {
	return (
		fabs(v0.x - v1.x) < DBL_EPSILON
		&& fabs(v0.y - v1.y) < DBL_EPSILON
		);
}

template <>
bool operator==<float>(const TVector3<float>& v0, const TVector3<float>& v1) {
	return (
		fabs(v0.x - v1.x) < FLT_EPSILON
		&& fabs(v0.y - v1.y) < FLT_EPSILON
		&& fabs(v0.z - v1.z) < FLT_EPSILON
		);
}

//doubleで特殊化
template <>
bool operator==<double>(const TVector3<double>& v0, const TVector3<double>& v1) {
	return (
		fabs(v0.x - v1.x) < DBL_EPSILON
		&& fabs(v0.y - v1.y) < DBL_EPSILON
		&& fabs(v0.z - v1.z) < DBL_EPSILON
		);
}

// Vector3 から Vector2 へのキャスト演算子の実装
template <typename _T>
TVector3<_T>::operator TVector2<_T>() const {
	return TVector2<_T>(x, y);  // x, y 成分だけを取り出して Vector2 にキャスト
}

// Vector2 から Vector3 へのキャスト演算子の実装
template <typename _T>
TVector2<_T>::operator TVector3<_T>() const {
	return TVector3<_T>(x, y,(_T)0);  // x, y 成分だけを取り出してzは0で固定し Vector3 にキャスト
}

// 明示的なインスタンス化
template class TVector3<float>;  // Vector3<float> のインスタンス化
template class TVector2<float>;  // Vector2<float> のインスタンス化
template class TVector3<int>;  // Vector3<int> のインスタンス化
template class TVector2<int>;  // Vector2<int> のインスタンス化
template class TVector3<double>;  // Vector3<double> のインスタンス化
template class TVector2<double>;  // Vector2<double> のインスタンス化

}
}//namespace Fwk