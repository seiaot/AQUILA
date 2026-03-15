#pragma once 

#include "Player/Player.h"
#include "Enemy/EnemyMng.h"
#include "Bullet/BulletMng.h"
#include "Ground/GroundMng.h"
#include "SurroundingsObject//S_ObjectMng.h"
#include "Effect/EffectMng.h"
#include "BG/BG.h"

//全オブジェクト初期化
void GameObjectInit();

//全オブジェクト更新
void GameObjectUpdate();

//全オブジェクト描画
void GameObjectRender();

//全オブジェクト解放
void GameObjectTerm();

//クラスのポインタを返す
Player* GetPlayer();

EnemyMng* GetEnemyMng();

BulletMng* GetBulletMng();

S_ObjectMng* GetSObjectMng();

EffectMng* GetEffectMng();

GroundMng* GetGroundMng();

BG* GetBG();