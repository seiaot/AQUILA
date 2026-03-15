struct VSOutput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
    float3 AddColor : ADD_COLOR;
};

struct PSOutput
{
	float4 Color    : SV_TARGET0;
};

SamplerState ColorSmp : register(s0);
Texture2D ColorMap : register(t0);


PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput)0;

    float4 col = ColorMap.Sample(ColorSmp, input.TexCoord);
    output.Color.rgb = col.rgb * input.Color.rgb + input.AddColor.rgb;
    output.Color.a = col.a * input.Color.a;

	return output;
}