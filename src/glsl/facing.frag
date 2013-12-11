
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
varying vec3 vertexInModelViewSpace; // out
varying vec3 c;


uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;

void main()
{ 
    vec4 outer = LMd*0.25;
    vec4 inner = LMd*0.75;
    float ratio = dot(normalize(c),normalize(normal));
    clamp(ratio,0.0,1.0);
    gl_FragColor = mix(outer,inner,ratio);
}