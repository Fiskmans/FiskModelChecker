
#include "../VertexToPixel.hlsl"

struct Output
{
    float4 color : SV_Target;
};

Output pixelShader(VertexToPixel i)
{
    Output o;
    o.color = float4(0.5,0.2,0.2,1);
    return o;
}