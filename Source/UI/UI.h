#pragma once

#include "Fwk/Graphics.h"

class UI
{
public:

	//初期化
	virtual void Init();
	//更新
	virtual void Update();
	//描画
	virtual void Render();
	//終了
	virtual void Term();

protected:
	//表示
	Sprite mSprite;
	Texture mTexture;
};