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

PSOutput main(VSOutput input)
{
	PSOutput output = (PSOutput)0;

    output.Color.rgb = input.Color.rgb + input.AddColor.rgb;
    output.Color.a = input.Color.a;

	return output;
}