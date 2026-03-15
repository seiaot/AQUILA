#pragma once

//Sceneクラスを継承するので#include
#include "Scene.h"

//タイトルシーンクラス
class SceneGameOver : public Scene
{
public:
	//初期化
	void Init();
	//更新
	void Update();
};