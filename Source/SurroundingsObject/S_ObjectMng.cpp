#include "S_ObjectMng.h"

//SaFE_DELETEを使う為に#include
#include "Lib/LibDef.h"

//生成する弾のクラスの宣言を#include
#include "Cloud.h"
#include "GameClear.h"

#include "Fwk/Framework.h"

#include "GameObjectMng/GameObjectMng.h"

//初期化
void S_ObjectMng::Init()
{
	//アクティブな弾のポインタの配列を空にしておく
	mActiveS_Objects.clear();
}

//後片付け
void S_ObjectMng::Term()
{
	//アクティブな弾のポインタの配列を空にしておく
	mActiveS_Objects.clear();
	//すべての弾プールを破棄
	DestroyPoolAll();
}

//更新
void S_ObjectMng::Update()
{
	//アクティブな弾を全て更新する
	for (int i = 0; i < mActiveS_Objects.size(); ++i)
	{
		//更新
		mActiveS_Objects[i]->Update();
	}

	//更新の結果、非アクティブとなった要素を取り除く
	for (int i = 0; i < mActiveS_Objects.size(); /*ここは書かない*/)
	{
		if (mActiveS_Objects[i]->IsActive())
		{
			//mActiveS_Objects[i]がアクティブのままであれば
			//次の要素を処理するのでiをインクリメント
			++i;
		}
		else
		{
			//mActiveS_Objects[i]が非アクティブとなっていたらその要素を取り除く
			mActiveS_Objects.erase(mActiveS_Objects.begin() + i);

			//※ここでは i をインクリメントしない
		}
	}
}

//描画
void S_ObjectMng::Render()
{
	//アクティブな弾を全て描画する
	for (int i = 0; i < mActiveS_Objects.size(); ++i)
	{
		//描画
		mActiveS_Objects[i]->Render();
	}
}

//オブジェクトプールの生成
void S_ObjectMng::GeneratePool(string sObjectName, int poolSize)
{
	//雲プール作成
	if (sObjectName == "Cloud")
	{
		//生成するクラスにSlimeを指定
		mS_ObjectPools[sObjectName].Init<Cloud>(poolSize);
	}
	if (sObjectName == "GameClear")
	{
		//生成するクラスにSlimeを指定
		mS_ObjectPools[sObjectName].Init<GameClear>(poolSize);
	}
}

//全ての弾のオブジェクトプールの破棄
void S_ObjectMng::DestroyPoolAll()
{
	//雲バッファを破棄
	DestroyPool("Cloud");
	DestroyPool("GameClear");
}

//指定した弾のオブジェクトプールの破棄
void S_ObjectMng::DestroyPool(string sObjectName)
{
	//弾プール破棄
	if (mS_ObjectPools.count(sObjectName) > 0)
	{
		mS_ObjectPools[sObjectName].Term();
		mS_ObjectPools.erase(sObjectName);
	}
}

//弾の生成
void S_ObjectMng::CreateS_Object(string sObjectName, Vector3f pos, Vector3f vDirection, float speed,float maxSpeed,Sizef size, int collisionGroup)
{
	//指定の弾プールから非アクティブのオブジェクトを取得する
	S_Object* pS_Object = nullptr;

	//弾プールから空きオブジェクトを取得
	if (mS_ObjectPools.count(sObjectName) > 0)
	{
		pS_Object = mS_ObjectPools[sObjectName].Allic();
	}

	//非アクティブの弾オブジェクトがなければ生成失敗
	if (pS_Object == nullptr)
	{
		return;
	}

	//位置を設定
	pS_Object->SetPosition(pos);
	//向きを設定
	pS_Object->SetDirection(vDirection);
	//速度の設定
	pS_Object->SetSpeed(speed,maxSpeed);
	//大きさの設定
	pS_Object->SetSize(size);
	//衝突グループの設定
	pS_Object->SetCollisionGroup(collisionGroup);

	//生成時の関数を呼び出す
	pS_Object->OnCreated();

	//アクティブな弾配列に追加しておく
	mActiveS_Objects.push_back(pS_Object);
}

//残りの弾の数
int S_ObjectMng::GetS_ObjectCount()
{
	return (int)mActiveS_Objects.size();
}