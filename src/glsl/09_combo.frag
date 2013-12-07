//
// toon.frag -- from lighthouse3d.com
//
uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D decal;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 objectToWorld;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 halfAngle;
varying vec3 c0, c1, c2;

varying vec3 normal;
varying vec3 lightDir;

void main (void)
{
    float diffuse;
    vec4 color;
    vec3 n = normalize (normal);
    
    diffuse = dot (lightDir, n);
    
    if (diffuse > 0.95)
        color = vec4 (1.0, 0.5, 0.5, 1.0) * LMd;
    else if (diffuse > 0.5)
        color = vec4 (0.6, 0.3, 0.3, 1.0) * LMd;
    else if( diffuse > 0.25 )
        color = vec4 (0.4, 0.2, 0.2, 1.0) * LMd;
    else
        color = vec4 (0.2, 0.1, 0.1, 1.0) * LMd;
    
    gl_FragColor = color;
}
