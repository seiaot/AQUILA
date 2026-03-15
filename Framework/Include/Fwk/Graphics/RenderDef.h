#pragma once

enum class RENDER_MODE : int
{
	None = -1,

	Mesh_Default,		//標準3dメッシュ

	Texture_Opaque,		//不透明2dテクスチャ
	Texture_Transparent,	//透過2dテクスチャ
	Texture_Blend_Add,	//加算合成2dテクスチャ
	Texture_Blend_Sub,	//減算合成2dテクスチャ
	Texture_Blend_Inverse,	//反転合成2dテクスチャ

	Color_Opaque,			//不透明2dカラー
	Color_Transparent,		//透過2dカラー
	Color_Blend_Add,		//加算合成2dカラー
	Color_Blend_Sub,		//減算合成2dカラー
	Color_Blend_Inverse,	//反転合成2dカラー

	LineSegments,	//線分


	//ブレンド設定とか入ってきたらここ増える

};

//描画パス
enum class RENDER_PASS : int 
{
	None = -1,

	Mesh_Opaque,		//不透明メッシュ
	Mesh_Transparent,	//透過アリメッシュ
	Sprite,				//スプライト
	SpriteFont,			//スプライトフォント
};

enum class BLEND_MODE : int {
	Opaque,
	Transparent,
	Add,
	Sub,
	Inverse,
};

//コンスタントバッファのフォーマット
namespace Fwk
{

struct alignas(256) CB_Transform
{
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Proj;
};

}