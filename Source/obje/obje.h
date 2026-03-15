#pragma once

#include "Fwk/Graphics.h"

#include "Lib/Math/Vector.h"

using namespace Lib::Math;

class obje
{
public:
	
	void Init();
	void Update();
	void Render();
	void Term();
private:
	//表示用スプライト
	Sprite mSprite;
	//テクスチャ
	Texture mTexture;
	//オブジェクトの位置
	Vector2f mPosition;
	float mZ;
	float mW;
	float mH;
};