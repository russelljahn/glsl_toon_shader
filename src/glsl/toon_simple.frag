
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

void main () {

    vec4 outputColor;
    float diffuse = dot(lightDirection, normal);
    
    if (diffuse > 0.95) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.9*LMd;
    }
    else if (diffuse > 0.5) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.625*LMd;
    }
    else if (diffuse > 0.25) {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.25*LMd;
    }
    else {
        outputColor = vec4(1.0, 1.0, 1.0, 1.0) * 0.1*LMd;
    }

    outputColor *= diffuse;
    
    gl_FragColor = outputColor;
}
