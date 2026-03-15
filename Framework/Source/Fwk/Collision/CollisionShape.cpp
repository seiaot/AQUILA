#include "Fwk/Collision/CollisionShape.h"

#include <math.h>

namespace Fwk {
namespace Collision {

//---------------------
//OBBとOBBの衝突を判定する
//---------------------
bool IsOBBsOverlapped(Cube cubeA, Cube cubeB)
{
	//両立方体の持つ方向
	Vector3f vDirections[2][3] =
	{
		{cubeA.vDirection[0],cubeA.vDirection[1],cubeA.vDirection[2]},
		{cubeB.vDirection[0],cubeB.vDirection[1],cubeB.vDirection[2]}
	};

	////cubeA
	//Vector3f DAX = cubeA.vDirection[0];
	//Vector3f DAY = cubeA.vDirection[1];
	//Vector3f DAZ = cubeA.vDirection[2];
	////cubeB
	//Vector3f DBX = cubeA.vDirection[0];
	//Vector3f DBY = cubeA.vDirection[1];
	//Vector3f DBZ = cubeA.vDirection[2];

	//二点間の距離
	Vector3f vDistanceAB = cubeA.position - cubeB.position;
	//分離軸
	Vector3f axis;
	//分離軸方向に沿う立方体の大きさの半分
	float r1 = 0.0f;
	float r2 = 0.0f;
	//分離軸方向に沿う立方体間の距離
	float fDistanceAB = 0.0f;

	//各方向から衝突しているかを判定し全方向が衝突しているなら重なっている

	/*axis = DAX.GetUnitVector();
	rA = DAX.GetLength();
	rB = (fabsf(Vector3f::Dot(axis, DBX)) + fabsf(Vector3f::Dot(axis, DBY)) + fabsf(Vector3f::Dot(axis, DBZ)));
	fDistanceAB = fabsf(Vector3f::Dot(axis, vDistanceAB));

	if (fDistanceAB > rA + rB)
	{
		return false;
	}*/

	//OBBの辺の方向をそのまま分離軸として使う時の判定
	for (int i = 0;i < 2;++i)
	{
		for (int n = 0;n < 3;++n)
		{
			//分離軸の設定
			axis = vDirections[i][n].GetUnitVector();
			//片方は最初から分離軸の向きになっている
			r1 = vDirections[i][n].GetLength();
			//前ループでの値を0に
			r2 = 0.0f;
			//分離軸の元となっていない方の、分離軸に沿った大きさの半分を入れる
			for (int t = 0;t < 3;++t)
			{
				r2 += fabsf(Vector3f::Dot(axis, vDirections[(i == 0) ? 1 : 0][t]));
			}
			//分離軸に沿ったOBB間の中心の距離
			fDistanceAB = fabsf(Vector3f::Dot(axis, vDistanceAB));

			//全ての分離軸で重なっていない限り衝突していない
			if (fDistanceAB > r1 + r2)
			{
				return false;
			}
		}
	}

	//双方の方向ベクトルに垂直な分離軸による判定
	for (int i = 0;i < 3;++i)
	{
		for (int n = 0;n < 3;++n)
		{
			axis = Vector3f::Cross(vDirections[0][i], vDirections[1][n]).GetUnitVector();
			r1 = 0.0f;
			r2 = 0.0f;
			for (int t = 0;t < 3;++t)
			{
				if (t != i)
				{
					r1 += fabsf(Vector3f::Dot(axis, vDirections[0][t]));
				}

				if (t != n)
				{
					r2 += fabsf(Vector3f::Dot(axis, vDirections[1][t]));
				}
			}
			fDistanceAB = fabsf(Vector3f::Dot(axis, vDistanceAB));

			if (fDistanceAB > r1 + r2)
			{
				return false;
			}
		}
	}

	//全ての分離線で重ならずにfalseにならず、ここまで来たらOBBは衝突していることとなる
	return true;

}

//---------------------
//立方体と立方体の衝突を判定する
//---------------------
bool IsCubesOverlapped(Cube cubeA, Cube cubeB)
{
	//矩形Aのパラメータ
	float x1 = cubeA.position.x;
	float y1 = cubeA.position.y;
	float z1 = cubeA.position.z;
	float w1 = cubeA.size.w;
	float h1 = cubeA.size.h;
	float d1 = cubeA.size.d;

	//矩形Bのパラメータ
	float x2 = cubeB.position.x;
	float y2 = cubeB.position.y;
	float z2 = cubeB.position.z;
	float w2 = cubeB.size.w;
	float h2 = cubeB.size.h;
	float d2 = cubeB.size.d;

	//二つの矩形の中心点の、X軸上の距離dx、Y軸上の距離dy、Z軸上の距離dzを算出
	//距離dxはx2-x1の絶対値、距離dyはy2-y1の絶対値、距離dzはz2-z1の絶対値
	float dx = (float)fabs(x2 - x1);
	float dy = (float)fabs(y2 - y1);
	float dz = (float)fabs(z2 - z1);

	//各軸の距離と矩形の幅・高さ・奥行から衝突を調べる

	//x軸上の距離(dx)が、(矩形Aの衝突矩形の幅の半分＋矩形Bの衝突矩形の幅の半分) 
	//以下で、かつ
	//y軸上の距離(dy)が、(矩形Aの衝突矩形の高さの半分＋矩形Bの衝突矩形の高さの半分)
	//以下で、かつ
	//Z軸上の距離(dz)が、(矩形Aの衝突矩形の奥行の半分＋矩形Bの衝突矩形の奥行の半分)
	//であれば衝突している
	return (dx <= (w1 / 2) + (w2 / 2) && dy <= (h1 / 2) + (h2 / 2) && dz <= (z1 / 2) + (z2 / 2));
}
//---------------------
//球とOBBの衝突を判定する
//---------------------
bool IsSphereAndOBBOverlapped(Sphere sphere, Cube cube)
{
	//立方体の持つ方向ベクトル
	Vector3f vDirections[3] =
	{
		cube.vDirection[0],cube.vDirection[1],cube.vDirection[2]
	};

	//二点間の距離
	Vector3f vDistanceAB = sphere.position - cube.position;
	//球からの立方体に対する最短距離
	Vector3f vShortest;
	//方向ベクトルに沿う球への距離
	float vDisLen = 0.0f;

	//各方向分繰り返す
	for (int i = 0;i < 3;++i)
	{
		//方向ベクトルに沿う球への距離の絶対値を取得
		//方向ベクトルは正規化を行わなければ値が大きくなりすぎる
		vDisLen = fabsf(Vector3f::Dot(vDirections[i].GetUnitVector(), vDistanceAB));
		//立方体の持っている方向ベクトルより長いならその向きにおいては、より外にある
		if (vDisLen >= vDirections[i].GetLength())
		{
			//立方体のその方向での大きさをより、はみ出した分の長さにする
			vDisLen -= vDirections[i].GetLength();
			//はみ出した分の三次元ベクトルを作る
			vShortest += vDirections[i].GetUnitVector() * vDisLen;
		}
	}

	//球の半径が立方体との最短距離以上なら衝突している
	if (sphere.radius >= vShortest.GetLength())
	{
		return true;
	}

	return false;
}

//---------------------
//円とと矩形の衝突を判定する
//---------------------
bool IsSphereAndCubeOverlapped(Sphere sphere, Cube cube)
{
	//円の中心点から最も近い矩形内の点の座標を調べる

	//矩形の右端のX座標
	float right = cube.position.x + (cube.size.w / 2.0f);
	//矩形の左端のX座標
	float left = cube.position.x - (cube.size.w / 2.0f);
	//矩形の上端のY座標
	float top = cube.position.y + (cube.size.h / 2.0f);
	//矩形の下端のY座標
	float bottom = cube.position.y - (cube.size.h / 2.0f);
	//矩形の最奥のZ座標
	float back = cube.position.z + (cube.size.d / 2.0f);
	//矩形の最前ののZ座標
	float front = cube.position.z - (cube.size.d / 2.0f);

	//一旦、円の中心点の座標を最短点としておく
	float x = sphere.position.x;
	float y = sphere.position.y;
	float z = sphere.position.z;

	//X,Y,Zそれぞれについて、矩形の範囲外となっていた場合、範囲内(端の座標)に収める
	if (x < left) {
		x = left;
	}
	else if (x > right) {
		x = right;
	}

	if (y < bottom) {
		y = bottom;
	}
	else if (y > top) {
		y = top;
	}

	if (z < front) {
		z = front;
	}
	else if (z > back) {
		z = back;
	}

	//座標x,y,zは円の中心点に最も近い矩形内の点となっているので、
	//この点と円の中心点との距離が円の半径以内に収まっていたら
	//円と矩形が衝突していることになる

	//二点間の距離を算出
	float d = sqrtf(
		(sphere.position.x - x) * (sphere.position.x - x)
		+ (sphere.position.y - y) * (sphere.position.y - y)
		+ (sphere.position.z - z) * (sphere.position.z - z)
	);

	//二点間の距離が円の半径よりも短ければ衝突している
	return (d <= (sphere.radius));
}

//---------------------
//円と円の衝突を判定する
//---------------------
bool IsSpheresOverlapped(Sphere s1, Sphere s2)
{
	//二点間の距離を算出
	float d = sqrtf(
		(s2.position.x - s1.position.x) * (s2.position.x - s1.position.x)
		+ (s2.position.y - s1.position.y) * (s2.position.y - s1.position.y)
		+ (s2.position.z - s1.position.z) * (s2.position.z - s1.position.z)
	);

	//二点間の距離が半径１＋半径２+半径３よりも短ければ衝突している
	return (d <= (s1.radius + s2.radius));
}

}
}