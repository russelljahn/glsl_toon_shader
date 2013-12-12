
uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform vec2 uvs; // in
uniform vec3 lightPosition; // Object-space
uniform vec3 eyePosition; // Object-space

uniform float timePreviousFrame; // in
uniform float timeCurrentFrame; // in

varying vec2 normalMapTexCoord; // out
varying vec3 lightDirection; // out
varying vec3 eyeDirection; // out
varying vec3 normal; // out
varying vec3 vertexInModelViewSpace; // out
varying vec3 c ;

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;




void main (void)
{
    vertexInModelViewSpace = vec3(gl_Vertex);
    normal = normalize(gl_Normal);

    vec4 temp = gl_ModelViewMatrix *gl_Vertex;
    c = vec3(-temp[0],-temp[1], -temp[2]);
    
    lightDirection = normalize(lightPosition - vertexInModelViewSpace);
    eyeDirection = normalize(eyePosition - vertexInModelViewSpace);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}