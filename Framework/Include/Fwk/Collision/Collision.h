#pragma once

#include <string>
#include <functional>

#include "CollisionShape.h"
#include "CollisionTypes.h"
#include "Lib/Math/Vector.h"

//クラスのメンバ関数を衝突イベントのコールバックにするときのマクロ
#define CreateCollisionEventCallback(_func) std::bind(&_func, this, std::placeholders::_1, std::placeholders::_2)
#define CreateCollisionEventCallbackEx(_func) std::bind(&_func, this, std::placeholders::_1)

using namespace Lib::Math;
using namespace std;

namespace Fwk {
namespace Collision {

class Collision
{

	friend class CollisionManager;

public:

	Collision();
	~Collision();

	//中心位置の設定
	void SetPosition(float _x, float _y);
	void SetPosition(const Vector2f& _position);
	//中心位置の取得
	const Vector2f& GetPosition()const;

	//コリジョンの形状を円形に設定
	//形状の位置情報はコリジョンの中心位置を原点とする位置
	void SetCircle(const Circle& _circle);
	void SetCircle(const Vector2f& _position, float _r);
	void SetCircle(float _x, float _y, float _r);

	//コリジョンの形状を矩形に設定
	//形状の位置情報はコリジョンの中心位置を原点とする位置
	void SetRect(const Rect& _rect);
	void SetRect(const Vector2f& _position, float _width, float _height);
	void SetRect(float _x, float _y, float _width, float _height);

	//形状の取得
	ShapeType GetShapeType()const;
	const Circle& GetShapeCircle()const;
	const Rect& GetShapeRect()const;

	//衝突チェック
	bool IsOverlapped(Collision& collision)const;

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
	void SetCallbackFunction(const std::function<void(const Collision& colA, const Collision& colB)>& _OnOverlapped);
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

private:

	Shape mShape;
	Vector2f mPosition;
	std::string mName;
	std::string mTag;

	bool mIsActive;
	ActiveDuration mActiveDuration;
	bool mIsCollide;

	std::function<void(const Collision& colA,const Collision& colB)>  mOnOverlapped;
	std::function<void(const CollisionEvent& collisionEvent)>  mOnOverlappedEx;

	std::function<void(Collision* pCol)> mOnDestroyed;

	void* mpOwner;

	unsigned int mGroup;
	unsigned int mHitGroup;

};

}
}
