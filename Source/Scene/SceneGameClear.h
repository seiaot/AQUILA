#pragma once

//Sceneクラスを継承するので#include
#include "Scene.h"

//タイトルシーンクラス
class SceneGameClear : public Scene
{
public:
	//初期化
	void Init();
	//更新
	void Update();
};