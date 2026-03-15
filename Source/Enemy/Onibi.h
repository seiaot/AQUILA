#pragma once 

//(敵)鬼火クラス

//親クラス
#include "Enemy.h"

class Onibi : public Enemy
{
public:
	//初期化
	void Init();
	//生成された時に呼び出される
	void OnCreated();

private:
	//アニメーションの初期化
	void _initAnimation();

	void PersonalUpdate();
};