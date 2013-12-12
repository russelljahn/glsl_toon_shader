
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
varying vec3 c; // out

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;

//uniform vec3 Warm;
//uniform vec3 Cool;
//uniform vec4 Color;
//uniform float Outline;

void main()
{     
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDirection);
    vec3 E = normalize(c);
    

    float temp1 = max(dot(N, E), 0.0);
    float temp2 = pow(max(dot(E, L),0.0),32.0);

    vec4 color = vec4(vec3(vec3(255.0,255.0,255.0)*temp1 + temp2), 1.0);

    gl_FragColor = color;
}