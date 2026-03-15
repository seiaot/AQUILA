#include "SceneInGame.h"

#include "Fwk/Framework.h"

#include "GameObjectMng/GameObjectMng.h"

//初期化
void SceneInGame::Init()
{
	mTexture.Load("Images/GP3/GameTitle.png");
	Scene::Init();

	//下半分
	mSprite.SetPivot(Pivot::Bottom);
	mSprite.SetPosition(0.0f, -WINDOW_HEIGHT / 2.0f);
	mSprite.SetTexCoord(0.0f, 0.5f, 1.0f, 0.5f);
	mSprite.SetSize(1920.0f, 540.0f);

	//上半分
	mSprite2.Init();
	mSprite2.SetTexture(mTexture);
	mSprite2.SetRenderLayer("Scene");
	mSprite2.SetSize(1920.0f, 540.0f);
	mSprite2.SetPosition(0.0f, 0.0f);
	mSprite2.SetPivot(Pivot::Top);
	mSprite2.SetPosition(0.0f, WINDOW_HEIGHT / 2.0f);
	mSprite2.SetTexCoord(0.0f, 0.0f, 1.0f, 0.5f);

	//ゲームオブジェクトの初期化
	GameObjectInit();
	//BGMファイルの読み込み
	mSound.Load("Sound/bgmC.mp3");
	//音源の初期化
	mSoundSource.Init(mSound);
	//BGMを繰り返し再生
	//mSoundSource.Play(-1);

	//”次のシーン”の初期値を”なし”にしておく
	mNextScene = SceneType::None;

	mRot = 0.0f;

	//ゲームオーバー
	mOverTexture.Load("Images/GP3/GameOver.png");

	mOverSprite.Init();
	mOverSprite.SetTexture(mOverTexture);
	mOverSprite.SetRenderLayer("Scene");
	mOverSprite.SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	mOverSprite.SetPosition(0.0f, 0.0f);
	mOverSprite.SetPriority(INT_MAX);
	mOverSprite.SetAlpha(0.0f);
	mOverAlpha = 0.0f;
}

//終了
void SceneInGame::Term()
{
	mSprite.Term();
	mSprite2.Term();
	mTexture.Unload();

	mOverSprite.Term();
	mOverTexture.Unload();

	//ゲームオブジェクトの解放
	GameObjectTerm();

	//BGMを停止
	mSoundSource.Stop();
	//音源を解放
	mSoundSource.Term();
	//読み込んだ音声ファイルを破棄
	mSound.Unload();
}

//更新
void SceneInGame::Update()
{
	//画像が開く
	if (mRot <= 100.0f)
	{
		mRot += 1.0f;
		mSprite.SetRotation(mRot, 0.0f, 0.0f);
		mSprite.Update();
		mSprite2.SetRotation(-mRot, 0.0f, 0.0f);
		mSprite2.Update();
	}

	mOverSprite.SetAlpha(mOverAlpha);
	mOverSprite.Update();

	//ゲームオブジェクトの更新
	GameObjectUpdate();

	//プレイヤーを取得
	Player* player = GetPlayer();
	if (player)
	{
		//プレイヤーがゴールに到達していて、ゲームパッドのAボタンが押されていたら
		if (player->IsClear())
		{
			if (!mCallResult)
			{
				Vector3f pos = { 0.0f,0.0f,player->Get3DPosition().z + 1800.0f };
				Sizef size = { 960.0f,720.0f,10.0f };
				GetSObjectMng()->CreateS_Object("GameClear", pos, Vector3f(), 0.0f, 0.0f, size, (int)CollisionGroup::S_Object);
			}

			if (Input_I->IsKeyDown('X') || Input_I->IsButtonDown(0, GAMEPAD_BUTTON::B))
			{
				//次のシーンをゲームクリアシーンにする
				mNextScene = SceneType::GameClear;
			}
		}

		//プレイヤーがやられ状態で一定時間経過でシーン遷移
		if (player->IsDead())
		{
			mOverAlpha += 0.4f * Time_I->GetDeltaTime();
			if (mOverAlpha > 1.0f)
			{
				mOverAlpha = 1.0f;
			}
		}
		if( (int)mOverAlpha == 1)
		{
			//次のシーンをゲームオーバーシーンにする
			mNextScene = SceneType::GameOver;
		}
	}
}

//描画
void SceneInGame::Render()
{
	mSprite.Draw();
	mSprite2.Draw();
	mOverSprite.Draw();

	//ゲームオブジェクトの描画
	GameObjectRender();
}