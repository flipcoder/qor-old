
uniform sampler2D tex;
uniform sampler2D nmap;
uniform sampler2D disp;
uniform sampler2D occ;
uniform sampler2D spec;
uniform vec3 LightAtten;
uniform vec3 LightColor;
/*varying vec3 vLightAtten;*/
/*varying vec3 vLightColor;*/
varying vec3 vTangent;
varying vec3 vBitangent;
varying vec2 vUV;
varying vec3 vLightDir;
varying vec3 vLight;
varying vec3 vViewDir;
varying vec3 vNormal;
varying vec3 vVertex;

#define EPSILON 0.00001

void main(void)
{
	float eye_dist = length(vViewDir);
	vec3 eye = normalize(vViewDir);
	float light_dist = length(vLightDir);
	vec3 light = normalize(vLightDir);
	/*vec3 wlight = normalize(vLight);*/
	
	float height = texture2D(disp, vUV).r * 0.04 - 0.02;
	vec2 uvp = vUV + (eye.xy * height);
	
	vec4 texel = texture2D(tex, uvp);
	if(texel.a < 0.5)
		discard;
	vec3 bump = normalize(texture2D(nmap, uvp).rgb * 2.0 - 1.0);
	float occ = texture2D(occ, uvp).r;
	
	/*float ambient = 0.1;*/
	float diffuse = max(dot(light, bump), 0.0) * 1.0;
	
	float shine = texture2D(spec, uvp).r;
	float specular;
	if(shine > EPSILON)
	{
		shine = 1.0 / shine;
		specular = pow(clamp(dot(reflect(-eye, bump), light), 0.0, 1.0), shine) * 0.5; // 1.0
	}
	else
		specular = 0.0f;
	
	vec4 lightcolor = vec4(LightColor, 1.0);
	
	vec4 color_frag =
		/*texel * (ambient + diffuse)*/
	   diffuse*texel
	   + vec4(vec3(specular), 1.0);
	
	/*vec3 Atten = vec3(0.0, 0.1, 0.0);*/
	vec3 Atten = LightAtten;
	
	/*float Atten = 1.0 / (kc + kl*light_dist + kq*light_dist*light_dist);*/
	/*if(Atten.x < EPSILON && light_dist < EPSILON)*/
	/*{*/
	/*    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);*/
	/*    return;*/
	/*    //discard;*/
	/*}*/
	float atten_frag;
	float atten_denom = Atten.x + Atten.y*light_dist + Atten.z*light_dist*light_dist;
	float abs_atten_denom = abs(atten_denom);
	/*if(abs_atten_denom > 1.0f)*/
	/*{*/
		/*setFragColor(vec4(0.0f, 0.0f, 0.0f, 1.0));*/
		/*return;*/
	/*}*/
	/*else */if(abs_atten_denom > EPSILON)
		atten_frag = clamp(1.0 / atten_denom, 0.0, 3.0);
	else
		atten_frag = 1.0;
	
    /*vec4 fog_color = vec4(0.0, 0.0, 0.0, 1.0);*/
    /*float fog_factor = eye_dist / 20.0f;*/
    
    vec4 lit = color_frag * occ * atten_frag * lightcolor;
	
    gl_FragColor = lit;
    /*gl_FragColor = mix(lit, fog_color, clamp(fog_factor,0.0,1.0));*/
}

