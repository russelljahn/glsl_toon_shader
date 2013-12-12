
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
    vec3 H = normalize(L + E);

    vec3 Warm = vec3(0.6 ,0.6 ,0);
    vec3 Cool = vec3(0, 0 ,0.6);
    float Outline = 0.4;
    
    float diffuse = dot(L,N);
    float specular = pow(dot(N,H),32.0);
    
    vec3 cool = min(Cool.xyz+LMd.xyz,1.0);
    vec3 warm = min(Warm.xyz+LMd.xyz,1.0);
    
    vec4 Color = vec4( min(mix(cool,warm,diffuse)+specular,1.0), 1.0);
    
    if (dot(N,E)<Outline) {
        Color = vec4(0.0,0.0,0.0,1.0);
    }

    gl_FragColor = Color;
}