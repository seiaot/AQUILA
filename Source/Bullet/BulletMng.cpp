#include "BulletMng.h"

//SaFE_DELETEを使う為に#include
#include "Lib/LibDef.h"

//生成する弾のクラスの宣言を#include
#include "NormalBullet.h"
#include "Fire.h"

#include "Fwk/Framework.h"

//初期化
void BulletMng::Init()
{
	//アクティブな弾のポインタの配列を空にしておく
	mActiveBullets.clear();
}

//後片付け
void BulletMng::Term()
{
	//アクティブな弾のポインタの配列を空にしておく
	mActiveBullets.clear();
	//すべての弾プールを破棄
	DestroyPoolAll();
}

//更新
void BulletMng::Update()
{
	//アクティブな弾を全て更新する
	for (int i = 0; i < mActiveBullets.size(); ++i)
	{
		//更新
		mActiveBullets[i]->Update();
	}

	//更新の結果、非アクティブとなった要素を取り除く
	for (int i = 0; i < mActiveBullets.size(); /*ここは書かない*/)
	{
		if (mActiveBullets[i]->IsActive())
		{
			//mActiveBullets[i]がアクティブのままであれば
			//次の要素を処理するのでiをインクリメント
			++i;
		}
		else
		{
			//mActiveBullets[i]が非アクティブとなっていたらその要素を取り除く
			mActiveBullets.erase(mActiveBullets.begin() + i);

			//※ここでは i をインクリメントしない
		}
	}
}

//描画
void BulletMng::Render()
{
	//アクティブな弾を全て描画する
	for (int i = 0; i < mActiveBullets.size(); ++i)
	{
		//描画
		mActiveBullets[i]->Render();
	}
}

//オブジェクトプールの生成
void BulletMng::GeneratePool(string bulletName, int poolSize)
{
	//通常弾のプール作成
	if (bulletName == "NormalBullet")
	{
		//生成するクラスにSlimeを指定
		mBulletPools[bulletName].Init<NormalBullet>(poolSize);
	}
	//炎のプール作成
	if (bulletName == "Fire")
	{
		//生成するクラスにSlimeを指定
		mBulletPools[bulletName].Init<Fire>(poolSize);
	}
}

//全ての弾のオブジェクトプールの破棄
void BulletMng::DestroyPoolAll()
{
	//通常弾のバッファを破棄
	DestroyPool("NormalBullet");
	//炎のバッファを破棄
	DestroyPool("Fire");
}

//指定した弾のオブジェクトプールの破棄
void BulletMng::DestroyPool(string bulletName)
{
	//弾プール破棄
	if (mBulletPools.count(bulletName) > 0)
	{
		mBulletPools[bulletName].Term();
		mBulletPools.erase(bulletName);
	}
}

//弾の生成
void BulletMng::CreateBullet(string bulletName, Vector3f pos, Vector3f vDirection, float speed,float maxSpeed,Sizef size, int collisionGroup)
{
	//指定の弾プールから非アクティブのオブジェクトを取得する
	Bullet* pBullet = nullptr;

	//弾プールから空きオブジェクトを取得
	if (mBulletPools.count(bulletName) > 0)
	{
		pBullet = mBulletPools[bulletName].Allic();
	}

	//非アクティブの弾オブジェクトがなければ生成失敗
	if (pBullet == nullptr)
	{
		return;
	}

	//位置を設定
	pBullet->SetPosition(pos);
	//向きを設定
	pBullet->SetDirection(vDirection);
	//速度の設定
	pBullet->SetSpeed(speed,maxSpeed);
	//大きさの設定
	pBullet->SetSize(size);
	//衝突グループの設定
	pBullet->SetCollisionGroup(collisionGroup);

	//生成時の関数を呼び出す
	pBullet->OnCreated();

	//アクティブな弾配列に追加しておく
	mActiveBullets.push_back(pBullet);
}

//残りの弾の数
int BulletMng::GetBulletCount()
{
	return (int)mActiveBullets.size();
}