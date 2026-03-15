#pragma once

#include "Lib/Math/Math.h"

using namespace Lib::Math;

namespace Fwk {
namespace Collision {

enum class ShapeType
{
	None = -1,
	Sphere,
	Cube,
};

struct Sphere
{
	Vector3f position;
	float radius;
};

struct Cube
{
	Vector3f position;
	Vector3f vOriginDirection[3];
	Vector3f vDirection[3];
	Sizef size;
};

struct Shape {
	ShapeType shapeType;
	Sphere sphere;
	Cube cube;
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};
//---------------------
//OBBとOBBの衝突を判定する
//---------------------
bool IsOBBsOverlapped(Cube cubeA, Cube cubeB);

//---------------------
//矩形と矩形の衝突を判定する
//---------------------
bool IsCubesOverlapped(Cube cubeA, Cube cubeB);

//---------------------
//球とOBBの衝突を判定する
//---------------------
bool IsSphereAndOBBOverlapped(Sphere sphere, Cube cube);

//---------------------
//球と立方体の衝突を判定する
//---------------------
bool IsSphereAndCubeOverlapped(Sphere sphere, Cube cube);

//---------------------
//球と球の衝突を判定する
//---------------------
bool IsSpheresOverlapped(Sphere s1, Sphere s2);

}
}