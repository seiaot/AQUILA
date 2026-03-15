#pragma once

//衝突グループを示す定数
enum class CollisionGroup
{
	Player,
	Enemy,
	PlayerBullet,
	EnemyBullet,
	S_Object,
	Effect,
};

//シーン種別
enum class SceneType
{
	None = -1,
	Title,
	InGame,
	GameOver,
	GameClear,
};