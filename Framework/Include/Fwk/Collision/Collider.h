#pragma once

#include <string>
#include <functional>

#ifdef _DEBUG
#include "Fwk/Graphics/LineSegments.h"
#endif

#include "CollisionShape.h"
#include "CollisionTypes.h"
#include "Lib/Math/Math.h"

//クラスのメンバ関数を衝突イベントのコールバックにするときのマクロ
#define CreateCollisionEventCallback(_func) std::bind(&_func, this, std::placeholders::_1, std::placeholders::_2)
#define CreateCollisionEventCallbackEx(_func) std::bind(&_func, this, std::placeholders::_1)

using namespace Lib::Math;
using namespace std;

namespace Fwk {
namespace Collision {

class Collider
{

	friend class CollisionManager;

public:

	Collider();
	~Collider();

	//中心位置の設定
	void SetPosition(float _x, float _y,float _z);
	void SetPosition(const Vector3f& _position);
	//中心位置の取得
	const Vector3f& GetPosition()const;

	//コリジョンの形状を円形に設定
	//形状の位置情報はコリジョンの中心位置を原点とする位置
	void SetSphere(const Sphere& _sphere);
	void SetSphere(const Vector3f& _position, float _r);
	void SetSphere(float _x, float _y,float _z, float _r);

	//コリジョンの形状を矩形に設定
	//形状の位置情報はコリジョンの中心位置を原点とする位置
	void SetCube(const Cube& _cube);
	void SetCube(const Vector3f& _position, Sizef _size, Anglef _angle = {0.0f,0.0f,0.0f});
	//コリジョンの大きさの設定
	void SetCubeSize(const Sizef _size);

	//立方体の回転を設定
	void SetCubeRotation(Anglef rot, Vector3f axisX = Vector3f(1.0f,0.0f,0.0f), Vector3f axisY = Vector3f(0.0f, 1.0f, 0.0f),Vector3f axisZ = Vector3f(0.0f, 0.0f, 1.0f));
	//立方体の回転を追加
	void AddCubeRotation(float _rx, float _ry, float _rz);
	//立方体が回転しているか調べる
	bool IsCubeRotated(Shape _shape) const;

	//形状の取得
	ShapeType GetShapeType()const;
	const Sphere& GetShapeSphere()const;
	const Cube& GetShapeCube()const;

	//衝突チェック
	bool IsOverlapped(Collider& collider)const;

	//名前の設定
	void SetName(const std::string& _name);
	//名前の取得
	const std::string& GetName()const;

	//タグの設定
	void SetTag(const std::string& _tag);
	//タグの取得
	const std::string& GetTag()const;

	//活性・非活性設定
	void SetActive(bool _isActive);
	bool IsActive()const;

	//所有者情報の設定
	void SetOwner(void* pOwner);
	void* GetOwner()const;

	//衝突時のコールバック関数を設定する
	void SetCallbackFunction(const std::function<void(const Collider& colA, const Collider& colB)>& _OnOverlapped);
	void SetCallbackFunctionEx(const std::function<void(const CollisionEvent& collisionEvent)>& _OnOverlapped);

	//グループの設定
	void SetGroup(int _group);
	//グループの解除
	void ClearGroup();
	//グループの取得
	unsigned int GetGroup()const;

	//衝突対象グループの追加
	void AddHitGroup(int _hitGroup);
	//衝突対象グループの削除
	void RemoveHitGroup( int _hitGroup);
	//衝突対象グループのクリア
	void ClearHitGroup();
	//衝突対象グループの取得
	unsigned int GetHitGroup()const;

	//活性期間の設定
	void SetActiveDuration(ActiveDuration _activationDuration);

	//デバッグ表示時のラインの色
	void SetOutlineColor(float r, float g, float b);

#ifdef _DEBUG
	void GetOutlineVertices(std::vector<XMFLOAT3>& vertices);
	void GetOutlineColor(XMFLOAT3* pOutColor );
#endif


private:

	Shape mShape;
	Vector3f mPosition;
	std::string mName;
	std::string mTag;

	bool mIsActive;
	ActiveDuration mActiveDuration;
	bool mIsCollide;

	std::function<void(const Collider& colA,const Collider& colB)>  mOnOverlapped;
	std::function<void(const CollisionEvent& collisionEvent)>  mOnOverlappedEx;

	std::function<void(Collider* pCol)> mOnDestroyed;

	void* mpOwner;

	unsigned int mGroup;
	unsigned int mHitGroup;

#ifdef _DEBUG
	float mOutlineColor[3];
#endif
};

}
}
