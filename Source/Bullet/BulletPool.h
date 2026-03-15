#pragma once 

//弾プールクラス

//可変長配列std::vectorを使う
#include <vector>

#include "Bullet.h"

class BulletPool
{

public:
	//初期化
	//class Tは、Init関数の呼び出し時に具体的な弾クラスに置き換わる。 
	template <class T>
	void Init(int poolSize)
	{
		//プールの全要素分の弾オブジェクトを生成
		for (int i = 0; i < poolSize; ++i)
		{
			//指定された弾クラスの変数を1つ生成
			//Bullet*にキャスト出来るクラスが指定されるのが前提
			Bullet* pNewBullet = new T();
			//初期化
			pNewBullet->Init();
			//最初は非アクティブ
			pNewBullet->SetActive(false,false);
			//プールにアドレスを記録しておく
			mPool.push_back(pNewBullet);
		}
	}

	//後片付け
	void Term();

	//オブジェクト１つ活性化して戻す
	Bullet* Allic();

protected:
	//Bullet*型の可変長配列
	std::vector<Bullet*> mPool;
};