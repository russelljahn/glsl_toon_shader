uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 objectToWorld;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;


float step(float edge, float x)
{
    return x < edge ? 0.0 : 1.0;
}

void main()
{
     
	vec3 N = normalize(c2);
	vec3 L = normalize(lightDirection);
	vec3 E = vec3(0,0,1);
	vec3 H =  normalize(L + E);

	float df = max(0.0, dot(N, L));
    	float sf = max(0.0, dot(N, H));
    	sf = pow(sf, shininess);

    	float A = 0.1;
    	float B = 0.3;
    	float C = 0.6;
    	float D = 1.0;

	if (df < A) df = 0.0;
	else if (df < B) df = B;
    	else if (df < C) df = C;
    	else df = D;
	sf = step(0.5, sf);
	
    	vec4 color = LMa + df * LMd + sf * LMs;
	gl_FragColor = vec4(color[0],color[1],color[2],1.0); 
}
