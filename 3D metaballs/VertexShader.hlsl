cbuffer ConstantBuffer : register(b0)
{
    matrix transform; // Transformation matrix
};


struct VS_INPUT
{
    float3 pos : POSITION;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.pos = mul(float4(input.pos, 1.0f), transform);
    return output;
}