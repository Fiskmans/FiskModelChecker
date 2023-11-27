
#include "../VertexToPixel.hlsl"
#include "../ShaderBuffers.hlsl"

struct VertexInput
{
    float4 position : POSITION;
};

VertexToPixel vertexShader(VertexInput i)
{
    VertexToPixel o;
    o.position = mul(i.position, ModelToWorld);
    return o;
}