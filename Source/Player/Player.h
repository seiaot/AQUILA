#pragma once

//SpriteやTexture,Animationを使うためのヘッダファイル
#include "Fwk/Graphics.h"

//向きの定数を使うのでinclude
#include "GameDef.h"

//コリジョンクラスを使うのでinclude
#include "Fwk/Collision/Collider.h"
using namespace Fwk::Collision;

//ベクターを使う
#include "Lib/Math/Math.h"
using namespace Lib::Math;

#include "UI/HP.h"

//プレイヤークラス
class Player
{
public:

	//プレイヤーの状態
	enum class Status
	{
		Moving,		//通常移動中
		Attack,		//攻撃中
		Damaged,	//被ダメージ中
		Dying,		//やられ中
		Dead,		//やられた
		Goal,		//ゴールした
	};

	//扱う効果音の種類
	enum class Sound
	{
		Jump,
		Damaged,

		Count
	};

	//プレイヤー初期化
	void Init();

	//プレイヤーの更新
	void Update();

	//プレイヤーの描画
	void Render();

	//プレイヤーの後片付け
	void Term();

	//現在位置を取得する(三次元)
	Vector3f Get3DPosition()const;
	//現在地を取得する(二次元)
	Vector2f Get2DPosition()const;
	//移動量の取得
	Vector3f GetVelocity() const;


	//ステージクリア状態の設定・取得
	void SetClear(bool b);
	bool IsClear() const;
	//やられているか否か
	bool IsDead()const;
	//HPの取得
	int GetHP()const;

private:
	//移動
	void UpdateMoving();
	//角度の更新
	void UpdateAngle(Vector3f vInput);
	//速度の更新
	void UpdateVelocity(const Vector3f vInput);
	//衝突イベントのコールバック関数
	void OnHit(const Collider& CollisionA, const Collider& CollisionB);

private:
	//表示用スプライト
	Sprite mSprite;
	//テクスチャ
	Texture mTexture;
	//コリジョン
	Collider mCollision;

	//位置
	Vector3f mPos;
	//サイズ
	Sizef mSize;
	//角度
	Anglef mAngle;

	//速度
	Vector3f mVelocity;

	//体力
	HP mHPUI;
	int mHP;

	//タイマー
	float mTimer;
	float mInvincibleTimer;

	//クリア状態
	bool mClear;
};