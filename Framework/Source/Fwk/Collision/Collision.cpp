#include "Fwk/Collision/Collision.h"

namespace Fwk {
namespace Collision {

Collision::Collision()
	: mShape({ ShapeType::None })
	, mPosition(0.0f, 0.0f)
	, mTag()
	, mIsActive(true)
	, mActiveDuration(ActiveDuration::Default)
	, mIsCollide(false)
	, mOnOverlapped(nullptr)
	, mOnOverlappedEx(nullptr)
	, mpOwner(nullptr)
	, mGroup(0)
	, mHitGroup(0)
{
	;
}

Collision::~Collision()
{
	if (mOnDestroyed != nullptr) {
		mOnDestroyed(this);
	}
}

void Collision::SetPosition(float _x, float _y) {
	mPosition = { _x, _y };
}
void Collision::SetPosition(const Vector2f& _position) {
	mPosition = _position;
}

//中心位置の取得
const Vector2f& Collision::GetPosition()const {
	return mPosition;
}

void Collision::SetCircle(const Circle& _circle) {
	mShape.circle = _circle;
	mShape.shapeType = ShapeType::Circle;
}
void Collision::SetCircle(const Vector2f& _position, float _r) {
	mShape.circle.position = _position;
	mShape.circle.radius = _r;
	mShape.shapeType = ShapeType::Circle;
}
void Collision::SetCircle(float _x, float _y, float _r) {
	mShape.circle.position = { _x,_y };
	mShape.circle.radius = _r;
	mShape.shapeType = ShapeType::Circle;
}

void Collision::SetRect(const Rect& _rect ){
	mShape.rect = _rect;
	mShape.shapeType = ShapeType::Rect;
}
void Collision::SetRect(const Vector2f& _position, float _width, float _height) {
	mShape.rect.position = _position;
	mShape.rect.width = _width;
	mShape.rect.height = _width;
	mShape.shapeType = ShapeType::Rect;
}
void Collision::SetRect(float _x, float _y, float _width, float _height) {
	mShape.rect.position = { _x,_y};
	mShape.rect.width = _width;
	mShape.rect.height = _width;
	mShape.shapeType = ShapeType::Rect;
}

//形状の取得
ShapeType Collision::GetShapeType()const {
	return mShape.shapeType;
}
const Circle& Collision::GetShapeCircle()const {
	return mShape.circle;
}
const Rect& Collision::GetShapeRect()const {
	return mShape.rect;
}

bool Collision::IsOverlapped(Collision& collision)const {

	Shape colA = mShape;
	Shape colB = collision.mShape;

	bool isHit = false;

	//無効設定のコリジョンは処理しない
	if (mIsActive == false || collision.mIsActive == false) {
		return false;
	}

	//コリジョン形状がないものは処理対象外
	if (colA.shapeType == ShapeType::None || colB.shapeType == ShapeType::None) {
		return false;
	}

	//形状によって衝突判定関数を変える
	if (colA.shapeType == ShapeType::Circle && colB.shapeType == ShapeType::Circle){
		colA.circle.position += mPosition;
		colB.circle.position = collision.mPosition;
		//円と円の衝突
		isHit = IsCirclesOverlapped(colA.circle, colB.circle);
	}
	else if (colA.shapeType == ShapeType::Rect && colB.shapeType == ShapeType::Rect){
		colA.rect.position += mPosition;
		colB.rect.position = collision.mPosition;
		//矩形と矩形の衝突
		isHit = IsRectsOverlapped(colA.rect, colB.rect);
	}
	else if (colA.shapeType == ShapeType::Circle && colB.shapeType == ShapeType::Rect){
		colA.circle.position += mPosition;
		colB.rect.position = collision.mPosition;
		//Aが円,Bが矩形の衝突
		isHit = IsCircleAndRectOverlapped(colA.circle, colB.rect);
	}
	else {
		colA.rect.position += mPosition;
		colB.circle.position = collision.mPosition;
		//Aが矩形,Bが円の衝突
		isHit = IsCircleAndRectOverlapped(colB.circle, colA.rect);
	}

	return isHit;
}

//名前の設定
void Collision::SetName(const std::string& _name) {
	mName = _name;
}
const std::string& Collision::GetName()const {
	return mName;
}

void Collision::SetTag(const std::string& _tag) {
	mTag = _tag;
}
const std::string& Collision::GetTag()const {
	return mTag;
}

void Collision::SetActive(bool _isActive) {
	mIsActive = _isActive;
}

bool Collision::IsActive()const {
	return mIsActive;
}

void Collision::SetOwner(void* _pOwner) {
	mpOwner = _pOwner;
}

void* Collision::GetOwner()const{
	return mpOwner;
}

void Collision::SetCallbackFunction(const std::function<void(const Collision& colA, const Collision& colB)>& _OnOverlapped) {
	mOnOverlapped = _OnOverlapped;
}

void Collision::SetCallbackFunctionEx(const std::function<void(const CollisionEvent& collisionEvent)>& _OnOverlapped) {
	mOnOverlappedEx = _OnOverlapped;
}

//グループの設定
void Collision::SetGroup(int _group) {
	mGroup = _group;
}

//グループの解除
void Collision::ClearGroup() {
	mGroup = 0;
}

//グループの取得
unsigned int Collision::GetGroup()const{
	return mGroup;
}

//衝突対象グループの追加
void Collision::AddHitGroup(int _hitGroup) {
	mHitGroup |= (1 << _hitGroup);
}
//衝突対象グループの削除
void Collision::RemoveHitGroup(int _hitGroup) {
	mHitGroup &= ~(1 << _hitGroup);

}
//衝突対象グループのクリア
void Collision::ClearHitGroup() {
	mHitGroup = 0;

}
//衝突対象グループの取得
unsigned int Collision::GetHitGroup()const {
	return mHitGroup;
}
	
//活性期間の設定
void Collision::SetActiveDuration(ActiveDuration _activationDuration) {
	mActiveDuration = _activationDuration;
}

}
}
