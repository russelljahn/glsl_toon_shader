
attribute vec2 parametric;

uniform vec3 lightPosition;  // Object-space
uniform vec3 eyePosition;    // Object-space
uniform vec2 torusInfo;

varying vec2 normalMapTexCoord;

varying vec3 lightDirection;
varying vec3 halfAngle;
varying vec3 eyeDirection;
varying vec3 c0, c1, c2;

void main()
{
  float M = torusInfo[0];
  float N = torusInfo[1];
  vec2 cosST , sinST;
  const float pi2 = 6.28318530;  // 2 times Pi 
 
  sinST = sin( pi2 * parametric);
  cosST = cos( pi2 * parametric);
 
  vec3 torusPosition = vec3( (M + N *cosST[1] )*cosST[0] , (M + N *cosST[1] ) * sinST[0], N *sinST[1]);

  vec3 tangent = normalize(vec3(-sinST[0] * (M + N * cosST[1]) ,cosST[0] * (M+N*cosST[1]),0)); 
  vec3 norm = vec3(cosST[0] *cosST[1],sinST[0]*cosST[1],sinST[1]);
  vec3 binormal = cross(norm, tangent); 

  mat3 rot = mat3( tangent , binormal , norm);
  
  eyeDirection = eyePosition - torusPosition;  // XXX fix me
  lightDirection = lightPosition - torusPosition;  // XXX fix me
  //lightDirection = rot * lightDirection;
  //eyeDirection = rot * eyeDirection;
  halfAngle = normalize( normalize(lightDirection) + normalize(eyeDirection));  // XXX fix me
  
  
  
  c0 = tangent;  // XXX fix me
  c1 = binormal;  // XXX fix me
  c2 = norm;  // XXX fix me

  normalMapTexCoord = parametric * vec2(-6 ,2 ); 

  gl_Position = gl_ModelViewProjectionMatrix * vec4( torusPosition, 1) ;

}

