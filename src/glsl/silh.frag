
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




varying vec3 LD, Normal, LDR;

void main()
{
    vec3 LDN = normalize(LD);
    vec3 NormalN = normalize(Normal);
    vec3 LDRN = normalize(LDR);
    float NdotLD = max(dot(NormalN, LDN), 0.0);
    float EVdotLDR = pow(max(dot(LDN, LDRN), 0.0), 32.0);
    gl_FragColor = vec4(gl_Color.rgb * NdotLD + vec3(EVdotLDR), 1.0);
}