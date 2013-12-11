
uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform vec2 uvs; // in
uniform vec3 lightPosition; // Object-space
uniform vec3 eyePosition; // Object-space

varying vec2 normalMapTexCoord; // out
varying vec3 lightDirection; // out
varying vec3 eyeDirection; // out
varying vec3 normal; // out

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;

void main()
{
    vec3 N = normalize(normal);
    vec3 eye = normalize(eyeDirection);
    
    float diffuse = dot(lightDirection, normal);
    
	float amountLightToEye = dot(reflect(lightDirection, normal), eyeDirection);
	float specular = pow(amountLightToEye, shininess);
    
	vec4 color = clamp(LMa + LMd*diffuse + specular*LMs, 0.0, 1.0);
    
    float op = min(1.0, 1.0/ abs(dot(eye, N)));
    gl_FragColor = vec4(vec3(color[0],color[1],color[2]), op);
}
#version 330 compatibility

in vec3  vMCposition;
in float vLightIntensity;

layout(location=0) out vec4 fFragColor;

uniform vec4 uSkyColor;
uniform vec4 uCloudColor;
uniform float uBias;
uniform sampler3D Noise3;

void main( )
{
    vec4  nv  = texture3D( Noise3, vMCposition );
    
    float intensity = ( nv[0] + nv[1] + nv[2] + nv[3] - 1. ) / 2.;
    
    vec3 color   = mix( uSkyColor.rgb, uCloudColor.rgb, clamp( uBias+intensity, 0., 1. ) ) * vLightIntensity;
    
    fFragColor = vec4(color, 1.0);
}