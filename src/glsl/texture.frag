uniform sampler2D my_color_texture;
uniform sampler2D normalMap;
uniform sampler2D texture;
uniform sampler2D heightField;
uniform samplerCube envmap;


void main()
{

gl_FragColor = texture2D(texture, gl_TexCoord[0].st);

}