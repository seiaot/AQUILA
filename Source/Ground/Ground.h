#pragma once

#include "Fwk/Graphics.h"
#include "Lib/Math/TSize.h"
#include "AppDef.h"

class Ground
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

	bool IsActive();

	void SetActive(bool bActive);

	void SetPosition(Vector3f pos);

	//描画順の設定
	void SetPriority(int iPriority);

	//テクスチャの変更
	void ChangeTexture(string texturePas);

private:
	//視点とのZ軸の距離を取得
	float GetDistanceCameraZ() const;

	//距離に応じた倍率を取得
	float GetRate();
	//表示用の位置を設定
	void SetSpritePosition();
	//表示サイズを変更
	void SetSpriteSize();

private:

	Texture mTexture;
	Sprite mSprite;

	Sizef mSize;

	Vector3f mPos;

	bool mIsActive;
};