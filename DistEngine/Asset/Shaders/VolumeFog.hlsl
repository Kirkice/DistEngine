/* $Header: VolumeFog.hlsl                                           7/16/22 20:55p KirkZhu $ */
/*--------------------------------------------------------------------------------------------*
*                                                                                             *
*                 Project Name : DistEngine                                                   *
*                                                                                             *
*                    File Name : VolumeFog.hlsl                                               *
*                                                                                             *
*                   Programmer : Kirk Zhu                                                     *
*                                                                                             *
*---------------------------------------------------------------------------------------------*/
#ifndef VOLUME_FOG_INCLUDE
#define VOLUME_FOG_INCLUDE

#include "Core.hlsl"

struct VertexIn
{ 
	float3 PosL    										: POSITION;
	float2 TexC    										: TEXCOORD;
};

struct VertexOut
{
	float4 PosH    										: SV_POSITION;
	float2 TexC    										: TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout                                      = (VertexOut)0.0f;
    vout.PosH 											= float4(vin.PosL, 1.0f);
	vout.TexC 											= vin.TexC;
    return vout;
}

#define quality  10.0;
#define near 5.0;
#define far 90.0;
#define use_noise 1
#define wigglyness (4.0 * (1.0 + sin(gTotalTime)))

#define insideOffset 0.1
#define fallofDist 20.0
#define opac 0.05
#define fogFraction 0.005

float hash(float3 p)  // replace this by something better
{
    p  = frac( p * 0.3183099 + 0.1 );
    p *= 17.0;
    return frac( p.x * p.y * p.z * (p.x + p.y + p.z) );
}

float noise(float3 x )
{
    float3 i = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
    
    return lerp(lerp(lerp( hash(i+float3(0,0,0)), 
                        hash(i+float3(1,0,0)),f.x),
                   lerp( hash(i+float3(0,1,0)), 
                        hash(i+float3(1,1,0)),f.x),f.y),
               lerp(lerp( hash(i+float3(0,0,1)), 
                        hash(i+float3(1,0,1)),f.x),
                   lerp( hash(i+float3(0,1,1)), 
                        hash(i+float3(1,1,1)),f.x),f.y),f.z);
}

const float3x3 m = float3x3( 0.00,  0.80,  0.60,
                            -0.80,  0.36, -0.48,
                            -0.60, -0.48,  0.64 );


float cnoise(float3 x)
{
    float f = noise(x);
    float3 q = 8.0 * x;
    f  = 0.5000*noise( q );
    q = mul(m, q * 4.0);
    f += 0.1750*noise( q );
    q = mul(m, q * 4.0);
    return f;
}

float sdSphere(float3 p, float s)
{
    return length(p)-s;
}

float sdBox(float3 p, float3 b) 
{
    float3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float map(float3 pos ) 
{
    float res = far;
    res = min(res, sdSphere(pos-float3(-20.0,0.25, 0.0), 10.25));
    res = min(res, sdBox(pos-float3( 20.0,0.25, 0.0), float3(10.3,10.25,10.1)));
    res += wigglyness * cnoise(0.1 * pos);
    
    return res;        
}

float raycast(float3 campos, float3 rd)
{
    float r = 0.0;    
    float t = near;
    for(int i = 0; i < 600; i++)
    {
        float dist = map(campos+rd*t);
        if(dist <= 0.01)
        { 
            r += fogFraction;
            t += insideOffset;
        }
        else
        {
            t += dist;
        }
        if (t>90 || r >= 1.0)
        {
            break;   
        }
    }   
    return r;
}

float render(float3 campos, float3 ray)
{ 
    return saturate(raycast(campos,ray));
}

float3x3 setCamera(float3 ro, float3 ta, float cr )
{
    float3 cw = normalize(ta-ro);
    float3 cp = float3(sin(cr), cos(cr),0.0);
    float3 cu = normalize( cross(cw,cp) );
    float3 cv =          ( cross(cu,cw) );
    return float3x3( cu, cv, cw );
}

float4 PS(VertexOut pin) : SV_Target
{
	PostprocessingData matData = gPostprocessingData[29];

    // camera	
    float3 ta = float3( 0, 0, 0 );
    float3 ro = ta + float3( 0 , 0, 8);

	// camera-to-world transformation
	float3x3 ca = setCamera( ro, ta, 0.0 );
	ro *= 4.0;
	float2 p = pin.TexC * float2(10,5)  - float2(5,2.5);
	// ray direction
	float3 rd = mul(ca, normalize(float3(p,1.5)));

	return float4(render(ro, rd).xxx, 1.0 );
}

#endif

