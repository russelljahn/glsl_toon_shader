//-------------------------------------
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
varying float noise;

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;





float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main (void)
{

  gl_TexCoord[0] = gl_MultiTexCoord0;
  vertexInModelViewSpace = vec3(gl_Vertex);
  normal = normalize(gl_Normal);

  vec4 temp = gl_ModelViewMatrix *gl_Vertex;
  c = vec3(-temp[0],-temp[1], -temp[2]);

  lightDirection = normalize(lightPosition - vertexInModelViewSpace);
  eyeDirection = normalize(eyePosition - vertexInModelViewSpace);

  vertexInModelViewSpace = vec3(gl_Vertex);


  float displacement = 3.0*cos(2.0*timePreviousFrame)*rand(vec2(lightDirection.x, lightDirection.y));

  // move the position along the normal and transform it
  vec3 newPosition = vertexInModelViewSpace + normalize(normal) * displacement;
  gl_Position = gl_ModelViewProjectionMatrix * vec4( newPosition, 1.0 );


    

}