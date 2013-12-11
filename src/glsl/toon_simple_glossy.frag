
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

uniform float timePreviousFrame; // in
uniform float timeCurrentFrame; // in

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;


float step(float edge, float x)
{
    return x < edge ? 0.0 : 1.0;
}

void main() {
	float diffuse = dot(lightDirection, normal);

	float amountLightToEye = dot(reflect(lightDirection, normal), eyeDirection);
	float specular = pow(amountLightToEye, shininess);

    vec4 color;
    if (specular > 0.5) {
    	color = vec4(1.0,1.0,1.0,1.0)*LMs;
    }
	else if (diffuse > 0.5) {
	    color = vec4(0.9,0.9,0.9,0.9)*LMd;
	}
	else if (diffuse > 0.0) {
	    color = vec4(0.33,0.33,0.33,1.0)*LMd;
	}
	else {
	    color = vec4(0.0,0.0,0.0,1.0);
	}


	gl_FragColor = color;
}
