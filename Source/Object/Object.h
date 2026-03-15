#pragma once

//テクスチャやスプライト
#include "Fwk/Graphics.h"

//コリジョンクラスを使うのでinclude
#include "Fwk/Collision/Collider.h"
using namespace Fwk::Collision;

//ベクターを使う
#include "Lib/Math/Math.h"
using namespace Lib::Math;

class Object
{
public:
	
	//初期化
	virtual void Init();
	//終了
	virtual void Term();

	//更新
	virtual void Update();

	//描画
	virtual void Render();

	//アクティブか？
	bool IsActive() const;
	//アクティブ状態の設定
	void SetActive(const bool IsActive,const bool CollisionActive);

	//位置の設定
	virtual void SetPosition(Vector3f vPos);
	//位置の取得
	Vector2f Get2dPosition() const;
	Vector3f Get3dPosition() const;

	//プレイヤーとのZ軸の距離を取得
	float GetDistancePlayerZ() const;

	//描画範囲内かどうか
	bool IsInDrawingRange();
	//視点からの距離を取得
	float GetDistanceCameraZ() const;

private:
	//距離に応じた倍率を取得
	float GetRate();
	//表示用の位置を設定
	void SetSpritePosition();
	//表示サイズを変更
	void SetSpriteSize();

protected:

	//派生ごとの更新
	virtual void PersonalUpdate();
	//非アクティブにする(派生毎)
	virtual bool Isinactive();

protected:
	//表示用スプライト
	Sprite mSprite;
	//テクスチャ
	Texture mTexture;

	//コリジョン
	Collider mCollision;

	//位置
	Vector3f mPos;
	//サイズ
	Sizef mSize;;
	//角度
	Anglef mAngle;

	//アクティブ状態
	bool mIsActive;
};