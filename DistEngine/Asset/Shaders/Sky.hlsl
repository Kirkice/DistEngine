//=============================================================================
// Sky.fx by Frank Luna (C) 2011 All Rights Reserved.
//=============================================================================

// Include common HLSL code.
#include "Core.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION0;
};

float3 RotateAroundYInDegrees (float3 vertex, float degrees)
{
    float alpha 											= degrees * PI / 180.0;
    float sina, cosa;
    sincos(alpha, sina, cosa);
    float2x2 												m = float2x2(cosa, -sina, sina, cosa);
    return 													float3(mul(m, vertex.xz), vertex.y).xzy;
}

float3 aces_approx(float3 v)
{
    v 														*= 0.6f;
    float a 												= 2.51f;
    float b 												= 0.03f;
    float c 												= 2.43f;
    float d 												= 0.59f;
    float e 												= 0.14f;
    return 													clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosL = vin.PosL;

	SkyBoxMaterialData matData                             	= gSkyMaterialData[gMaterialIndex];

    float3 rotated 											= RotateAroundYInDegrees(vin.PosL, matData.SkyBoxRotation);
	float4 posW 											= mul(float4(rotated, 1.0f), gWorld);
	posW.xyz 												+= gEyePosW;
	vout.PosH 												= mul(posW, gViewProj).xyww;
	return 													vout;
}


#define PLANET_POS float3(0,0,0)							/* the position of the planet */
#define PLANET_RADIUS 6371e3								/* radius of the planet */
#define ATMOS_RADIUS 6471e3									/* radius of the atmosphere */
#define G 0.7												/* mie scattering direction, or how big the blob around the sun is */
#define PRIMARY_STEPS 128									/* primary steps, affects quality the most */
#define LIGHT_STEPS 8										/* light steps, how much steps in the light direction are taken */


#define RAY_BETA float3(5.5e-6, 13.0e-6, 22.4e-6)			/* rayleigh, affects the color of the sky */
#define MIE_BETA float3(21e-6, 21e-6, 21e-6)				/* mie, affects the color of the blob around the sun */
#define AMBIENT_BETA float3(0,0,0)							/* ambient, affects the scattering color when there is no lighting from the sun */
#define ABSORPTION_BETA float3(2.04e-5, 4.97e-5, 1.95e-6)	/* what color gets absorbed by the atmosphere (Due to things like ozone) */

#define Height -0.99
#define GroundColor float3(0.32, 0.37, 0.47)
#define HEIGHT_RAY 8000
#define HEIGHT_MIE 220
#define HEIGHT_ABSORPTION 56000
#define ABSORPTION_FALLOFF 3000
#define _ScreenParams float2(1920,1080)

float3 calculate_scattering(
	float3 start, 				// the start of the ray (the camera position)
    float3 dir, 				// the direction of the ray (the camera vector)
    float max_dist, 			// the maximum distance the ray can travel (because something is in the way, like an object)
    float3 scene_color,			// the color of the scene
    float3 light_dir, 			// the direction of the light
    float3 light_intensity,		// how bright the light is, affects the brightness of the atmosphere
    float3 planet_position, 	// the position of the planet
    float planet_radius, 		// the radius of the planet
    float atmo_radius, 			// the radius of the atmosphere
    float3 beta_ray, 			// the amount rayleigh scattering scatters the colors (for earth: causes the blue atmosphere)
    float3 beta_mie, 			// the amount mie scattering scatters colors
    float3 beta_absorption,   	// how much air is absorbed
    float3 beta_ambient,		// the amount of scattering that always occurs, cna help make the back side of the atmosphere a bit brighter
    float g, 					// the direction mie scatters the light in (like a cone). closer to -1 means more towards a single direction
    float height_ray, 			// how high do you have to go before there is no rayleigh scattering?
    float height_mie, 			// the same, but for mie
    float height_absorption,	// the height at which the most absorption happens
    float absorption_falloff,	// how fast the absorption falls off from the absorption height
    int steps_i, 				// the amount of steps along the 'primary' ray, more looks better but slower
    int steps_l 				// the amount of steps along the light ray, more looks better but slower
) {
			    start -= planet_position;
			    // calculate the start and end position of the ray, as a distance along the ray
			    // we do this with a ray sphere intersect
			    float a = dot(dir, dir);
			    float b = 2.0 * dot(dir, start);
			    float c = dot(start, start) - (atmo_radius * atmo_radius);
			    float d = (b * b) - 4.0 * a * c;
			    
			    // stop early if there is no intersect
			    if (d < 0.0) return scene_color;
			    
			    // calculate the ray length
			    float2 ray_length = float2(
			        max((-b - sqrt(d)) / (2.0 * a), 0.0),
			        min((-b + sqrt(d)) / (2.0 * a), max_dist)
			    );
			    
			    // if the ray did not hit the atmosphere, return a black color
			    if (ray_length.x > ray_length.y) return scene_color;
			    // prevent the mie glow from appearing if there's an object in front of the camera
			    bool allow_mie = max_dist > ray_length.y;
			    // make sure the ray is no longer than allowed
			    ray_length.y = min(ray_length.y, max_dist);
			    ray_length.x = max(ray_length.x, 0.0);
			    // get the step size of the ray
			    float step_size_i = (ray_length.y - ray_length.x) / float(steps_i);
			    
			    // next, set how far we are along the ray, so we can calculate the position of the sample
			    // if the camera is outside the atmosphere, the ray should start at the edge of the atmosphere
			    // if it's inside, it should start at the position of the camera
			    // the min statement makes sure of that
			    float ray_pos_i = ray_length.x + step_size_i * 0.5;
			    
			    // these are the values we use to gather all the scattered light
			    float3 total_ray = float3(0,0,0); // for rayleigh
			    float3 total_mie = float3(0,0,0); // for mie
			    
			    // initialize the optical depth. This is used to calculate how much air was in the ray
			    float3 opt_i = float3(0,0,0);
			    
			    // also init the scale height, avoids some float2's later on
			    float2 scale_height = float2(height_ray, height_mie);
			    
			    // Calculate the Rayleigh and Mie phases.
			    // This is the color that will be scattered for this ray
			    // mu, mumu and gg are used quite a lot in the calculation, so to speed it up, precalculate them
			    float mu = dot(dir, light_dir);
			    float mumu = mu * mu;
			    float gg = g * g;
			    float phase_ray = 3.0 / (50.2654824574 /* (16 * pi) */) * (1.0 + mumu);
			    float phase_mie = allow_mie ? 3.0 / (25.1327412287 /* (8 * pi) */) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg)) : 0.0;
			    
			    // now we need to sample the 'primary' ray. this ray gathers the light that gets scattered onto it
			    for (int i = 0; i < steps_i; ++i) {
			        
			        // calculate where we are along this ray
			        float3 pos_i = start + dir * ray_pos_i;
			        
			        // and how high we are above the surface
			        float height_i = length(pos_i) - planet_radius;
			        
			        // now calculate the density of the particles (both for rayleigh and mie)
			        float3 density = float3(exp(-height_i / scale_height), 0.0);
			        
			        // and the absorption density. this is for ozone, which scales together with the rayleigh, 
			        // but absorbs the most at a specific height, so use the sech function for a nice curve falloff for this height
			        // clamp it to avoid it going out of bounds. This prevents weird black spheres on the night side
			        float denom = (height_absorption - height_i) / absorption_falloff;
			        density.z = (1.0 / (denom * denom + 1.0)) * density.x;
			        
			        // multiply it by the step size here
			        // we are going to use the density later on as well
			        density *= step_size_i;
			        
			        // Add these densities to the optical depth, so that we know how many particles are on this ray.
			        opt_i += density;
			        
			        // Calculate the step size of the light ray.
			        // again with a ray sphere intersect
			        // a, b, c and d are already defined
			        a = dot(light_dir, light_dir);
			        b = 2.0 * dot(light_dir, pos_i);
			        c = dot(pos_i, pos_i) - (atmo_radius * atmo_radius);
			        d = (b * b) - 4.0 * a * c;

			        // no early stopping, this one should always be inside the atmosphere
			        // calculate the ray length
			        float step_size_l = (-b + sqrt(d)) / (2.0 * a * float(steps_l));

			        // and the position along this ray
			        // this time we are sure the ray is in the atmosphere, so set it to 0
			        float ray_pos_l = step_size_l * 0.5;

			        // and the optical depth of this ray
			        float3 opt_l = float3(0,0,0);
			            
			        // now sample the light ray
			        // this is similar to what we did before
			        for (int l = 0; l < steps_l; ++l) {

			            // calculate where we are along this ray
			            float3 pos_l = pos_i + light_dir * ray_pos_l;

			            // the heigth of the position
			            float height_l = length(pos_l) - planet_radius;

			            // calculate the particle density, and add it
			            // this is a bit verbose
			            // first, set the density for ray and mie
			            float3 density_l = float3(exp(-height_l / scale_height), 0.0);
			            
			            // then, the absorption
			            float denom = (height_absorption - height_l) / absorption_falloff;
			            density_l.z = (1.0 / (denom * denom + 1.0)) * density_l.x;
			            
			            // multiply the density by the step size
			            density_l *= step_size_l;
			            
			            // and add it to the total optical depth
			            opt_l += density_l;
			            
			            // and increment where we are along the light ray.
			            ray_pos_l += step_size_l;
			            
			        }
			        
			        // Now we need to calculate the attenuation
			        // this is essentially how much light reaches the current sample point due to scattering
			        float3 attn = exp(-beta_ray * (opt_i.x + opt_l.x) - beta_mie * (opt_i.y + opt_l.y) - beta_absorption * (opt_i.z + opt_l.z));

			        // accumulate the scattered light (how much will be scattered towards the camera)
			        total_ray += density.x * attn;
			        total_mie += density.y * attn;

			        // and increment the position on this ray
			        ray_pos_i += step_size_i;
    				
			    }
			    
			    // calculate how much light can pass through the atmosphere
			    float3 opacity = exp(-(beta_mie * opt_i.y + beta_ray * opt_i.x + beta_absorption * opt_i.z));
			    
				// calculate and return the final color
			    return (
        				phase_ray * beta_ray * total_ray // rayleigh color
       					+ phase_mie * beta_mie * total_mie // mie
			            + opt_i.x * beta_ambient // and ambient
			    ) * light_intensity + scene_color * opacity; // now make sure the background is rendered correctly
}

float2 ray_sphere_intersect(
    float3 start,						// starting position of the ray
    float3 dir,							// the direction of the ray
    float radius						// and the sphere radius
) {
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, start);
    float c = dot(start, start) - (radius * radius);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return float2(1e5,-1e5);
    return float2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

float3 skylight(float3 sample_pos, float3 surface_normal, float3 light_dir, float3 background_col) 
{
    surface_normal = normalize(lerp(surface_normal, light_dir, 0.6));
    return calculate_scattering(
		sample_pos,						// the position of the camera
        surface_normal, 				// the camera vector (ray direction of this pixel)
        3.0 * ATMOS_RADIUS, 			// max dist, since nothing will stop the ray here, just use some arbitrary value
        background_col,					// scene color, just the background color here
        light_dir,						// light direction
        float3(40,40,40),				// light intensity, 40 looks nice
        PLANET_POS,						// position of the planet
        PLANET_RADIUS,                  // radius of the planet in meters
        ATMOS_RADIUS,                   // radius of the atmosphere in meters
        RAY_BETA,						// Rayleigh scattering coefficient
        MIE_BETA,                       // Mie scattering coefficient
        ABSORPTION_BETA,                // Absorbtion coefficient
        AMBIENT_BETA,					// ambient scattering, turned off for now. This causes the air to glow a bit when no light reaches it
        G,                          	// Mie preferred scattering direction
        HEIGHT_RAY,                    // Rayleigh scale height
        HEIGHT_MIE,                    // Mie scale height
        HEIGHT_ABSORPTION,				// the height at which the most absorption happens
        ABSORPTION_FALLOFF,			// how fast the absorption falls off from the absorption height
        LIGHT_STEPS, 					// steps in the ray direction
        LIGHT_STEPS 					// steps in the light direction
    );
}

float4 render_scene(float3 pos, float3 dir, float3 light_dir) 
{
    float4 color = float4(0.0, 0.0, 0.0, 1e12);
    float2 planet_intersect = ray_sphere_intersect(pos - PLANET_POS, dir, PLANET_RADIUS); 
    if (0.0 < planet_intersect.y) 
	{
		color.w = max(planet_intersect.x, 0.0);
        float3 sample_pos = pos + (dir * planet_intersect.x) - PLANET_POS;
        float3 surface_normal = normalize(sample_pos);
        color.xyz = GroundColor; 
        
        // get wether this point is shadowed, + how much light scatters towards the camera according to the lommel-seelinger law
        float3 N = surface_normal;
        float3 V = -dir;
        float3 L = light_dir;
        float dotNV = max(1e-6, dot(N, V));
        float dotNL = max(1e-6, dot(N, L));
        float shadow = dotNL / (dotNL + dotNV);
        
        // apply the shadow
        color.xyz *= shadow;
        
        // apply skylight
        color.xyz += clamp(skylight(sample_pos, surface_normal, light_dir, float3(0,0,0)) * GroundColor, 0.0, 1.0);
    }
    
	return color;
}


float3 get_camera_vector(float2 resolution, float2 coord)
{
	float2 uv    = coord.xy / resolution.xy - float2(0.5,0.5);
         uv.x *= resolution.x / resolution.y;
    return normalize(float3(uv.x, -uv.y, 1.0));
}


float4 PS(VertexOut pin) : SV_Target
{
	// SkyBoxMaterialData matData                             	= gSkyMaterialData[gMaterialIndex];
	// float3 color 											= gCubeMap.Sample(gsamLinearWrap, pin.PosL).rgb;
	// color													= color * matData.SkyBoxTint.rgb * matData.SkyBoxExposure;

	// // color													= pow(color.rgb,0.454545);
	// if(matData.ACES > 0.5)
	// 	color 												= aces_approx(color);

	// return float4(color,1);

	float3 camera_vector = float3(pin.PosL.xy,-1);
	float3 camera_position = float3(0.0, ATMOS_RADIUS + (Height * (ATMOS_RADIUS - PLANET_RADIUS - 1.0)), 0.0);
	float3 light_dir = float3(0.5,1,0.5);
	float4 scene = render_scene(camera_position, camera_vector, light_dir);

	float3 col = float3(0,0,0);

	col += calculate_scattering(
    	camera_position,				// the position of the camera
	    camera_vector, 					// the camera vector (ray direction of this pixel)
	    scene.w, 						// max dist, essentially the scene depth
	    scene.xyz,						// scene color, the color of the current pixel being rendered
	    light_dir,						// light direction
	    float3(40,40,40),				// light intensity, 40 looks nice
	    PLANET_POS,						// position of the planet
	    PLANET_RADIUS,                  // radius of the planet in meters
	    ATMOS_RADIUS,                   // radius of the atmosphere in meters
	    RAY_BETA,						// Rayleigh scattering coefficient
	    MIE_BETA,                       // Mie scattering coefficient
	    ABSORPTION_BETA,                // Absorbtion coefficient
	    AMBIENT_BETA,					// ambient scattering, turned off for now. This causes the air to glow a bit when no light reaches it
	    G,                          	// Mie preferred scattering direction
	    HEIGHT_RAY,                    // Rayleigh scale height
	    HEIGHT_MIE,                    // Mie scale height
	    HEIGHT_ABSORPTION,				// the height at which the most absorption happens
	    ABSORPTION_FALLOFF,			// how fast the absorption falls off from the absorption height 
	    PRIMARY_STEPS, 					// steps in the ray direction 
	    LIGHT_STEPS 					// steps in the light direction
	);
	    
	col = 1.0 - exp(-col);
    
    return float4(pow(col,2.2),1);

}

