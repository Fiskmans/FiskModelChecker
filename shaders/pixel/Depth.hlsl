
#include "../VertexToPixel.hlsl"

struct Output
{
    float4 color : SV_Target;
    float depth : SV_DEPTH;
};

Output pixelShader(VertexToPixel i)
{
    Output o;
    
    o.color = float4(0.5,0.2,0.2,1) * i.position.z;
    o.depth = i.position.z;
    
    return o;
}