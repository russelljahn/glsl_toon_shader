uniform vec4 LMa; // Light-Material ambient
uniform vec4 LMd; // Light-Material diffuse
uniform vec4 LMs; // Light-Material specular
uniform float shininess;

uniform vec2 uvs; // in

uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;

uniform mat3 objectToWorld;

varying vec2 normalMapTexCoord;
varying vec3 lightDirection;
varying vec3 eyeDirection;
varying vec3 halfAngle;
varying vec3 c0, c1, c2;


void main() {

  // gl_FragColor = gl_FragColor = texture2D(texture, uvs);
  gl_FragColor = textureCube(envmap, vec3(uvs, 1.0));

  // gl_FragColor = gl_FragColor = texture2D(texture, vec2(0.0, 0.0));
  // gl_FragColor = gl_FragColor = texture2D(texture, vec2(1.0, 1.0));


}


