#include "EffectMng.h"

//SaFE_DELETEを使う為に#include
#include "Lib/LibDef.h"

//生成する弾のクラスの宣言を#include
#include "Hit.h"
#include "Explosion.h"
#include "SubExplosion.h"
#include "ExplosionEX.h"

#include "Fwk/Framework.h"

//初期化
void EffectMng::Init()
{
	//アクティブな弾のポインタの配列を空にしておく
	mActiveEffects.clear();
}

//後片付け
void EffectMng::Term()
{
	//アクティブな弾のポインタの配列を空にしておく
	mActiveEffects.clear();
	//すべての弾プールを破棄
	DestroyPoolAll();
}

//更新
void EffectMng::Update()
{
	//アクティブな弾を全て更新する
	for (int i = 0; i < mActiveEffects.size(); ++i)
	{
		//更新
		mActiveEffects[i]->Update();
	}

	//更新の結果、非アクティブとなった要素を取り除く
	for (int i = 0; i < mActiveEffects.size(); /*ここは書かない*/)
	{
		if (mActiveEffects[i]->IsActive())
		{
			//mActiveEffects[i]がアクティブのままであれば
			//次の要素を処理するのでiをインクリメント
			++i;
		}
		else
		{
			//mActiveEffects[i]が非アクティブとなっていたらその要素を取り除く
			mActiveEffects.erase(mActiveEffects.begin() + i);

			//※ここでは i をインクリメントしない
		}
	}
}

//描画
void EffectMng::Render()
{
	//アクティブな弾を全て描画する
	for (int i = 0; i < mActiveEffects.size(); ++i)
	{
		//描画
		mActiveEffects[i]->Render();
	}
}

//オブジェクトプールの生成
void EffectMng::GeneratePool(string effectName, int poolSize)
{
	//通常弾のプール作成
	if (effectName == "Hit")
	{
		//生成するクラスにSlimeを指定
		mEffectPools[effectName].Init<Hit>(poolSize);
	}
	if (effectName == "Explosion")
	{
		//生成するクラスにSlimeを指定
		mEffectPools[effectName].Init<Explosion>(poolSize);
	}
	if (effectName == "SubExplosion")
	{
		//生成するクラスにSlimeを指定
		mEffectPools[effectName].Init<SubExplosion>(poolSize);
	}
	if (effectName == "ExplosionEX")
	{
		//生成するクラスにSlimeを指定
		mEffectPools[effectName].Init<ExplosionEX>(poolSize);
	}
}

//全ての弾のオブジェクトプールの破棄
void EffectMng::DestroyPoolAll()
{
	//通常弾のバッファを破棄
	DestroyPool("Hit");
	DestroyPool("Explosion");
	DestroyPool("SubExplosion");
	DestroyPool("ExplosionEX");
}

//指定した弾のオブジェクトプールの破棄
void EffectMng::DestroyPool(string effectName)
{
	//弾プール破棄
	if (mEffectPools.count(effectName) > 0)
	{
		mEffectPools[effectName].Term();
		mEffectPools.erase(effectName);
	}
}

//弾の生成
void EffectMng::CreateEffect(string effectName, Vector3f pos, Vector3f vDirection, float speed,float maxSpeed,Sizef size, int collisionGroup)
{
	//指定の弾プールから非アクティブのオブジェクトを取得する
	Effect* pEffect = nullptr;

	//弾プールから空きオブジェクトを取得
	if (mEffectPools.count(effectName) > 0)
	{
		pEffect = mEffectPools[effectName].Allic();
	}

	//非アクティブの弾オブジェクトがなければ生成失敗
	if (pEffect == nullptr)
	{
		return;
	}

	//位置を設定
	pEffect->SetPosition(pos);
	//向きを設定
	pEffect->SetDirection(vDirection);
	//速度の設定
	pEffect->SetSpeed(speed,maxSpeed);
	//大きさの設定
	pEffect->SetSize(size);
	//衝突グループの設定
	pEffect->SetCollisionGroup(collisionGroup);

	//生成時の関数を呼び出す
	pEffect->OnCreated();

	//アクティブな弾配列に追加しておく
	mActiveEffects.push_back(pEffect);
}

//残りの弾の数
int EffectMng::GetEffectCount()
{
	return (int)mActiveEffects.size();
}