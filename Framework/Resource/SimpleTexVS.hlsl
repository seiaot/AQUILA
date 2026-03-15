struct VSInput
{
	float3 Position : POSITION; //座標
	float3 Normal   : NORMAL;   //法線
	float4 Color    : COLOR;	//頂点カラー
	float2 TexCoord	: TEXCOORD; //テクスチャ座標
	float3 Tangent  : TANGENT;   //接線ベクトル
    float3 AddColor	: ADD_COLOR;	//追加カラー
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
    float3 AddColor : ADD_COLOR;
};

cbuffer Transform : register(b0)
{
	float4x4 World : packoffset(c0); //ワールド行列
	float4x4 View  : packoffset(c4); //ビュー行列
	float4x4 Proj  : packoffset(c8); //射影行列
}

VSOutput main(VSInput input) 
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.Position, 1.0f );
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.Position = projPos;
	output.TexCoord = input.TexCoord;
	output.Color = input.Color;
    output.AddColor = input.AddColor;

	return output;
}