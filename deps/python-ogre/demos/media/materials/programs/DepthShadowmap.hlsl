/* This file implements standard programs for depth shadow mapping. 
   These particular ones are suitable for additive lighting models, and
   include 3 techniques to reduce depth fighting on self-shadowed surfaces,
   constant bias, gradient (slope-scale) bias, and a fuzzy shadow map comparison*/


// Shadow caster vertex program.
void casterVP(
	float4 position			: POSITION,
	out float4 outPos		: POSITION,
	out float4 outDepth		: TEXCOORD0,

	uniform float4x4 worldViewProj,
	uniform float4 texelOffsets
	)
{
	outPos = mul(worldViewProj, position);

	// fix pixel / texel alignment
	outPos.xy += texelOffsets.zw * outPos.w;
	// linear depth storage
	// offset / scale range output
	outDepth = outPos;
}


// Shadow caster fragment program for high-precision single-channel textures	
void casterFPraw(
	float4 depth			: TEXCOORD0,
	out float4 result		: COLOR)
	
{
	depth = depth / depth.w;
	// just smear across all components 
	// therefore this one needs high individual channel precision
	//result = depth;
	result = float4(depth.zzz, 1);
}


	
void receiverVP(
	float4 position		: POSITION,
	float4 normal		: NORMAL,

	out float4 outPos			: POSITION,
	out float4 outColour		: COLOR,
	out float4 outShadowUV		: TEXCOORD0,

	uniform float4x4 world,
	uniform float4x4 worldIT,
	uniform float4x4 worldViewProj,
	uniform float4x4 texViewProj,
	uniform float4 lightPosition,
	uniform float4 lightColour,
	uniform float fixedDepthBias
	)
{
	float4 worldPos = mul(world, position);
	outPos = mul(worldViewProj, position);

	float3 worldNorm = mul(worldIT, normal).xyz;

	// calculate lighting (simple vertex lighting)
	float3 lightDir = normalize(
		lightPosition.xyz -  (worldPos.xyz * lightPosition.w));
	lightDir = normalize(lightDir);

	outColour = lightColour * max(dot(lightDir, worldNorm), 0.0);

	// calculate shadow map coords
	outShadowUV = mul(texViewProj, worldPos);
	outShadowUV.z -= fixedDepthBias * outShadowUV.w;


	
}

void receiverFPraw(
	float4 position			: POSITION,
	float4 shadowUV			: TEXCOORD0,
	float4 vertexColour		: COLOR,

	uniform sampler2D shadowMap : register(s0),
	uniform float inverseShadowmapSize,
	uniform float gradientClamp,
	uniform float gradientScaleBias,
	uniform float shadowFuzzyWidth,
	
	out float4 result		: COLOR)
{
	// point on shadowmap
	shadowUV = shadowUV / shadowUV.w;
	float centerdepth = tex2D(shadowMap, shadowUV.xy).x;
    
    // gradient calculation
  	float pixeloffset = inverseShadowmapSize * 0.5;    
    float4 depths = float4(
    	tex2D(shadowMap, shadowUV.xy + float2(-pixeloffset, 0)).x,
    	tex2D(shadowMap, shadowUV.xy + float2(+pixeloffset, 0)).x,
    	tex2D(shadowMap, shadowUV.xy + float2(0, -pixeloffset)).x,
    	tex2D(shadowMap, shadowUV.xy + float2(0, +pixeloffset)).x);

	float2 differences = abs( depths.yw - depths.xz );
	float gradient = min(gradientClamp, max(differences.x, differences.y));

	// visibility function
	float delta_z = centerdepth + gradient * gradientScaleBias - shadowUV.z;
   
    //float visibility = saturate(1 + delta_z / (gradient * shadowFuzzyWidth));
	// Hmm, why does this work?
	float visibility = saturate(1 + delta_z * 5);
       

	result = vertexColour * visibility;

	
}
