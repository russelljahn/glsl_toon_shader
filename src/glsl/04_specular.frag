
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
varying vec3 normal;
varying vec3 c0, c1, c2;


void main() {

	float amountLightToEye = dot(reflect(lightDirection, normal), eyeDirection);
	float specular = pow(amountLightToEye, shininess);

	gl_FragColor = clamp(specular*LMs, 0.0, 1.0);

}
