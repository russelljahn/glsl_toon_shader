
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
uniform sampler2D decal;
uniform sampler2D heightField;
uniform samplerCube envmap;

void main (void)
{

    vec4 outputColor;
    float diffuse = dot(lightDirection, normal);
    
    if (diffuse > 0.95) {
        outputColor = vec4 (1.0, 0.5, 0.5, 1.0);
    }
    else if (diffuse > 0.5) {
        outputColor = vec4 (0.6, 0.3, 0.3, 1.0);
    }
    else if (diffuse > 0.25) {
        outputColor = vec4 (0.4, 0.2, 0.2, 1.0);
    }
    else {
        outputColor = vec4 (0.2, 0.1, 0.1, 1.0);
    }

    outputColor *= LMd;
    
    gl_FragColor = outputColor;
}
