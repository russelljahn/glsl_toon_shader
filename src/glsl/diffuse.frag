
uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular

uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 objectToWorld;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 normal;


void main() {
	float diffuse = dot(lightDirection, normal);
 	gl_FragColor = clamp(LMa + LMd*diffuse, 0.0, 1.0);
}
