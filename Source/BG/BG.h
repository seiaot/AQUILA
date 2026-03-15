#pragma once

#include "Fwk/Graphics.h"

class BG
{
public:

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Render();
	//終了
	void Term();

	Vector2f GetVanishingPoint()const;
	
	//消失点の変更
	void UpdateVanishingPoint();

	//重なった雲の生成
	void CreateLayeredCloud();

private:

	//空
	Sprite mSkySprite;
	Texture mSkyTexture;

	//遠景
	Sprite mDistantSprite[3];
	Texture mDistantTexture[3];
	float mDistantUVLeft[3];

	//消失点
	Vector2f mVanishingPoint;
	//タイマー
	float mTimer;
};