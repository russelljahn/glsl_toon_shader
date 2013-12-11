
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



void main() {

	float diffuse = dot(lightDirection, normal);

	float amountLightToEye = dot(reflect(lightDirection, normal), eyeDirection);
	float specular = pow(amountLightToEye, shininess);

	vec3 phong = clamp(LMa + LMd*diffuse + specular*LMs, 0.0, 1.0).rgb;

	float red = dot(phong, vec3(.393, .769, .189));
	float green = dot(phong, vec3(.349, .686, .168));
	float blue = dot(phong, vec3(.272, .534, .131));

	gl_FragColor = vec4(red, green, blue, 1.0);
}
