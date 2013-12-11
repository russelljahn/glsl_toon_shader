
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

void main () {

    vec4 outputColor;
    float diffuse = dot(lightDirection, normal);
    float Outline = 0.3;

    if (diffuse > 0.95) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 1.0*LMd;
    }
    else if (diffuse > 0.5) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.625*LMd;
    }
    else if (diffuse > 0.05) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.35*LMd;
    }
    else {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.1*LMd;
    }

    outputColor *= diffuse;
    
    if (dot(normalize(normal), normalize(c))<Outline) {
        outputColor = vec4(0.0,0.0,0.0,1.0);
    }
    gl_FragColor = outputColor;
}
