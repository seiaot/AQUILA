#pragma once

#include "Fwk/Graphics.h"

//シーン種別の列挙型を使うので#include
#include "GameDef.h"

//タイトルシーンクラス
class Scene
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
	//次のシーンを取得する
	SceneType GetNextScene();

protected:
	//次のシーン
	SceneType mNextScene;
	//テクスチャ
	Texture mTexture;
	//スプライト
	Sprite mSprite;

};