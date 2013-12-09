
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
    vec3 vertexInModelViewSpace = vec3(gl_Vertex);
    normal = normalize(gl_Normal);

    lightDirection = normalize(lightPosition - vertexInModelViewSpace);
    eyeDirection = normalize(eyePosition - vertexInModelViewSpace);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
