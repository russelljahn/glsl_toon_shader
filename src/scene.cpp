#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
using std::vector;

#include <ctime>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <Cg/double.hpp>
#include <Cg/vector/xyzw.hpp>
#include <Cg/vector.hpp>
#include <Cg/mul.hpp>
#include <Cg/normalize.hpp>
#include <Cg/distance.hpp>
#include <Cg/radians.hpp>
#include <Cg/length.hpp>
#include <Cg/sin.hpp>
#include <Cg/cos.hpp>
#include <Cg/mul.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>

#include "global.hpp"
#include "scene.hpp"
#include "glmatrix.hpp"
#include "matrix_stack.hpp"


using namespace Cg;

#define OUTPUT(x) std::cout << #x " = " << x << std::endl;

extern bool verbose;

GLSLShader::GLSLShader(GLenum type, int len, const char* s)
    : shader_type(type)
    , bytes(len)
    , shader_object(0)
    , dirty(true)
{
    source = new GLchar[bytes];
    if (source) {
        for (int i=0; i<bytes; i++) {
            source[i] = s[i];
        }
    }
}

GLSLShader::GLSLShader(GLenum type)
    : shader_type(type)
    , source(NULL)
    , shader_object(0)
    , dirty(true)
{
}

extern const char* program_name;

bool GLSLShader::readTextFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file) {
        long size;

        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        fseek(file, 0L, SEEK_SET);
        delete source;
        source = new GLchar[size];
        if (source == NULL) {
            fprintf(stderr, "%s: malloc failed\n", program_name);
            return false;
        }
        bytes = (int) fread(source, 1, size, file);
        assert(bytes == size);
        fclose(file);
        dirty = true;
    } else {
        fprintf(stderr, "%s: could not open file %s\n",
            program_name, filename);
        return false;
    }
    return true;
}

void GLSLShader::validate()
{
    if (dirty || !shader_object) {
        if (!shader_object) {
            shader_object = glCreateShader(shader_type);
        }
        if (shader_object) {
            const GLchar** s = (const GLchar**) &source;
            glShaderSource(shader_object, 1, s, &bytes);
            glCompileShader(shader_object);
            dirty = false;
            GLint compiled = 0;
            glGetShaderiv(shader_object, GL_COMPILE_STATUS, &compiled);
            if (verbose || !compiled) {
                showInfoLog("shader");
            }
        }
    }
}

GLuint GLSLShader::getShader()
{
    validate();
    return shader_object;
}

void GLSLShader::showInfoLog(const char* msg)
{
    validate();

    GLint max_length, length;
    GLchar *info_log;

    glGetShaderiv(shader_object,
        GL_INFO_LOG_LENGTH, &max_length);
    if (max_length > 1) {
        info_log = (GLchar *) malloc(max_length);
        if (info_log != NULL) {
            glGetShaderInfoLog(shader_object, max_length, &length, info_log);
            printf("=== %s info log contents ===\n", msg);
            printf("%s", info_log);
            printf("=== end ===\n");
            free(info_log);
        }
    } else {
        printf("=== %s has an <empty> info log (a good thing) ===\n", msg);
    }
}

void GLSLShader::reset()
{
    if (shader_object) {
        if (verbose) {
            printf("Deleting shader 0x%x\n", shader_object);
        }
        glDeleteShader(shader_object);
        shader_object = 0;
    }
}

void GLSLShader::release()
{
    shader_object = 0;
}

GLSLShader::~GLSLShader()
{
    reset();
    delete source;
}

FragmentShader::FragmentShader(int len, const char *s)
    : GLSLShader(GL_FRAGMENT_SHADER, len, s)
{}

FragmentShader::FragmentShader()
    : GLSLShader(GL_FRAGMENT_SHADER)
{}

VertexShader::VertexShader(int len, const char *s)
    : GLSLShader(GL_VERTEX_SHADER, len, s)
{}

VertexShader::VertexShader()
    : GLSLShader(GL_VERTEX_SHADER)
{}

GLSLProgram::GLSLProgram(GLuint vs, GLuint fs)
    : vertex_shader(vs)
    , fragment_shader(fs)
    , program_object(0)
    , dirty(true)
{
}

GLSLProgram::GLSLProgram()
    : vertex_shader(0)
    , fragment_shader(0)
    , program_object(0)
    , dirty(true)
{
}

#define ERR_CHECK() glutReportErrors(); printf("%s: %d\n", __FILE__, __LINE__);

bool GLSLProgram::validate()
{
    if (dirty || !program_object) {
        if (!program_object) {
            program_object = glCreateProgram();
            assert(program_object);  // API failure!
        }
        if (program_object) {
            glAttachShader(program_object, vertex_shader);
            glAttachShader(program_object, fragment_shader);
            glLinkProgram(program_object);
            dirty = false;
            GLint linked = 0;
            glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
            if (verbose || !linked) {
                showInfoLog("linked program");
                return false;
            }
            assert(linked == GL_TRUE);
        }
    }
    return true;
}

void GLSLProgram::showInfoLog(const char* msg)
{
    validate();

    GLint max_length, length;
    GLchar *info_log;

    glGetProgramiv(program_object,
        GL_INFO_LOG_LENGTH, &max_length);
    if (max_length > 1) {
        info_log = (GLchar *) malloc(max_length);
        if (info_log != NULL) {
            glGetProgramInfoLog(program_object, max_length, &length, info_log);
            printf("=== %s info log contents ===\n", msg);
            printf("%s", info_log);
            printf("=== end ===\n");
            free(info_log);
        }
    } else {
        printf("=== %s has an <empty> info log (a good thing) ===\n", msg);
    }
}

void GLSLProgram::use()
{
    validate();
    assert(program_object);
    glUseProgram(program_object);
}

void GLSLProgram::reset()
{
    if (program_object) {
        glDeleteProgram(program_object);
        program_object = 0;
    }
    if (vertex_shader) {
        if (verbose) {
            printf("Deleting vertex shader 0x%x\n", vertex_shader);
        }
        glDeleteShader(vertex_shader);
        vertex_shader = 0;
    }
    if (fragment_shader) {
        if (verbose) {
            printf("Deleting fragment shader 0x%x\n", fragment_shader);
        }
        glDeleteShader(fragment_shader);
        fragment_shader = 0;
    }
}

void GLSLProgram::swap(GLSLProgram &other)
{
    GLuint tmp;

    tmp = program_object;
    program_object = other.program_object;
    other.program_object = tmp;

    tmp = vertex_shader;
    vertex_shader = other.vertex_shader;
    other.vertex_shader = tmp;

    tmp = fragment_shader;
    fragment_shader = other.fragment_shader;
    other.fragment_shader = tmp;

    bool btmp;
    btmp = dirty;
    dirty = other.dirty;
    other.dirty = btmp;
}

void GLSLProgram::setVec1f(const char *name, float v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform1f(loc, v);
        
    }
}

void GLSLProgram::setVec2f(const char *name, float2 v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform2f(loc, v.x, v.y);
    }
}

void GLSLProgram::setVec3f(const char *name, float3 v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform3f(loc, v.x, v.y, v.z);
    }
}

void GLSLProgram::setVec4f(const char *name, float4 v)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
}

void GLSLProgram::setMat3f(const char *name, const Transform &transform)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        float4x4 mm = transform.getMatrix();
        GLfloat m[3][3];

        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                m[i][j] = mm[j][i];  // transpose for GLSL
            }
        }
        GLboolean needs_transpose = GL_FALSE;
        glUniformMatrix3fv(loc, 1, needs_transpose, &m[0][0]);
    }
}

void GLSLProgram::setSampler(const char *name, int texture_unit)
{
    use();
    GLint loc = glGetUniformLocation(program_object, name);
    if (loc >= 0) {
        glUniform1i(loc, texture_unit);
    }
}

GLint GLSLProgram::getLocation(const char *name)
{
  GLint location = glGetUniformLocationARB(program_object, name);

  if (location < 0) {
    fprintf(stderr, "%s: could not get location of %s (fix your shader so it is used)\n",
      program_name, name);
  }
  return location;
}

GLSLProgram::~GLSLProgram()
{
    reset();
}

void Torus::loadProgram()
{
    VertexShader vs;
    FragmentShader fs;
    bool vs_ok = vs.readTextFile(vertex_filename.c_str());
    bool fs_ok = fs.readTextFile(fragment_filename.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs.getShader(), fs.getShader());
        vs.release();
        fs.release();
        bool ok = new_program.validate();
            if (ok) {
            program.swap(new_program);
            glBindAttribLocation(program.program_object, 0, "parametric");
            glLinkProgram(program.program_object);
            GLint torusInfo_location = program.getLocation("torusInfo");
            const float outerRadius = 1.5, innerRadius = 0.5;
            program.use();
            glUniform2f(torusInfo_location, outerRadius, innerRadius);

            // Assign samplers statically to texture units 0 through 3
            program.setSampler("normalMap", 0);
            program.setSampler("texture", 1);
            program.setSampler("heightField", 2);
            program.setSampler("envmap", 3);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }
}

Torus::Torus(Transform t, MaterialPtr m)
    : Object(t, m)
{
    vertex_filename = "glsl/torus.vert";
    fragment_filename = "glsl/00_red.frag";

    mesh2d = Mesh2DPtr(new Mesh2D(float2(0,0), float2(1,1), int2(80,40)));

    loadProgram();

    //material->bindTextures();
}

Torus::~Torus() {
}

void Torus::draw(const View& view, LightPtr light) {
    program.use();

    float4 eye_position_object_space = mul(transform.getInverseMatrix(), float4(view.eye_position,1));
    eye_position_object_space.xyz /= eye_position_object_space.w;
    program.setVec3f("eyePosition", eye_position_object_space.xyz);

    float4 light_position_object_space = mul(transform.getInverseMatrix(), light->getPosition());
    light_position_object_space.xyz /= light_position_object_space.w;
    program.setVec3f("lightPosition", light_position_object_space.xyz);

    // LM = Light color modulated by Matrial color
    // a,d,s = ambient, diffuse, specular
    float4 LMa = material->ambient*light->getColor();
    program.setVec4f("LMa", LMa);
    float4 LMd = material->diffuse*light->getColor();
    program.setVec4f("LMd", LMd);
    float4 LMs = material->specular*light->getColor();
    program.setVec4f("LMs", LMs);
    program.setVec1f("shininess", material->shininess);

    program.setMat3f("objectToWorld", transform);
    
    ERR_CHECK();

    pushAndMultGLMatrix(GL_MODELVIEW, transform.getMatrix());
    mesh2d->draw();
    popGLMatrix(GL_MODELVIEW);
}

void ModelObject::setOutline(){
    if(outline){
        outline = false;
    }
    else{
        outline = true;
    }
}
void ModelObject::setExplosion(bool val){
    explosion = val;
}
void ModelObject::setExplosion2(bool val){
    explosion2 = val;
}

void ModelObject::setGodsRay(){
    godsRay = false;
}
void ModelObject::loadGodsRay(){
    program.reset();
    godsRay = true;
    VertexShader vs_lighting;
    FragmentShader fs_lighting;

    string vertex_f = "glsl/lighting.vert";
    string frag_f = "glsl/lighting.frag";

    bool vs_ok = vs_lighting.readTextFile(vertex_f.c_str());
    bool fs_ok = fs_lighting.readTextFile(frag_f.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs_lighting.getShader(), fs_lighting.getShader());
        vs_lighting.release();
        fs_lighting.release();
        bool ok = new_program.validate();
        if (ok) {
            lighting.swap(new_program);
            lighting.use();
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }

    VertexShader vs_ray;
    FragmentShader fs_ray;

    vertex_f = "glsl/gods_ray.vert";
    frag_f = "glsl/gods_ray.frag";

    vs_ok = vs_ray.readTextFile(vertex_f.c_str());
    fs_ok = fs_ray.readTextFile(frag_f.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs_ray.getShader(), fs_ray.getShader());
        vs_ray.release();
        fs_ray.release();
        bool ok = new_program.validate();
        if (ok) {
            rays.swap(new_program);
            rays.use();
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }
}
void ModelObject::loadExplosionProgram()
{
   VertexShader vs_explosion;
    FragmentShader fs_explosion;

    string vertex_f = "glsl/explosion.vert";
    string frag_f = "glsl/explosion.frag";

    bool vs_ok = vs_explosion.readTextFile(vertex_f.c_str());
    bool fs_ok = fs_explosion.readTextFile(frag_f.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs_explosion.getShader(), fs_explosion.getShader());
        vs_explosion.release();
        fs_explosion.release();
        bool ok = new_program.validate();
        if (ok) {
            explosion_program.swap(new_program);
            explosion_program.use();
            explosion_program.setSampler("normalMap", 0);
            explosion_program.setSampler("texture", 1);
            explosion_program.setSampler("heightField", 2);
            explosion_program.setSampler("envmap", 3);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }

}

void ModelObject::loadExplosion2Program()
{
   VertexShader vs_explosion;
    FragmentShader fs_explosion;

    string vertex_f = "glsl/explosion2.vert";
    string frag_f = fragment_filename;

    bool vs_ok = vs_explosion.readTextFile(vertex_f.c_str());
    bool fs_ok = fs_explosion.readTextFile(frag_f.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs_explosion.getShader(), fs_explosion.getShader());
        vs_explosion.release();
        fs_explosion.release();
        bool ok = new_program.validate();
        if (ok) {
            explosion2_program.swap(new_program);
            explosion2_program.use();
            explosion2_program.setSampler("normalMap", 0);
            explosion2_program.setSampler("texture", 1);
            explosion2_program.setSampler("heightField", 2);
            explosion2_program.setSampler("envmap", 3);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }

}
void ModelObject::setRandom(bool val){
    random = val;
}
void ModelObject::loadRandomProgram()
{
   VertexShader vs_explosion;
    FragmentShader fs_explosion;

    string vertex_f = "glsl/random.vert";
    string frag_f = fragment_filename;

    bool vs_ok = vs_explosion.readTextFile(vertex_f.c_str());
    bool fs_ok = fs_explosion.readTextFile(frag_f.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs_explosion.getShader(), fs_explosion.getShader());
        vs_explosion.release();
        fs_explosion.release();
        bool ok = new_program.validate();
        if (ok) {
            explosion2_program.swap(new_program);
            explosion2_program.use();
            explosion2_program.setSampler("normalMap", 0);
            explosion2_program.setSampler("texture", 1);
            explosion2_program.setSampler("heightField", 2);
            explosion2_program.setSampler("envmap", 3);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }

}
void ModelObject::loadProgram()
{
    VertexShader vs;
    FragmentShader fs;
    bool vs_ok = vs.readTextFile(vertex_filename.c_str());
    bool fs_ok = fs.readTextFile(fragment_filename.c_str());
    if (vs_ok && fs_ok) {
        GLSLProgram new_program(vs.getShader(), fs.getShader());
        vs.release();
        fs.release();
        bool ok = new_program.validate();
        if (ok) {
            program.swap(new_program);
           // glBindAttribLocation(program.program_object, 0, "parametric");
           // glLinkProgram(program.program_object);
           // GLint torusInfo_location = program.getLocation("torusInfo");
           // const float outerRadius = 1.5, innerRadius = 0.5;
            program.use();
           // glUniform2f(torusInfo_location, outerRadius, innerRadius);
            
            // Assign samplers statically to texture units 0 through 3
            program.setSampler("normalMap", 0);
            program.setSampler("texture", 1);
            program.setSampler("heightField", 2);
            program.setSampler("envmap", 3);
        } else {
            printf("GLSL shader compilation failed\n");
        }
    } else {
        if (!vs_ok) {
            printf("Vertex shader failed to load\n");
        }
        if (!fs_ok) {
            printf("Fragment shader failed to load\n");
        }
    }
}

ModelObject::ModelObject(std::string file_name, std::string folder_path, Transform t, MaterialPtr m)
: Object(t, m), filename(file_name), folderpath(folder_path)
{
    std::cout << "Constructing '" << filename << "'" << std::endl;
    outline = false;
    explosion =false;
    random = false;
    explosion2 = false;
    std::string err = tinyobj::LoadObj(shapes, (folderpath+filename).c_str(), folderpath.c_str());
    

    if (!err.empty()) {
        std::cerr << err << std::endl;
        return;
    }
    
    // print(); 
    
    vertex_filename = "glsl/model.vert";
    fragment_filename = "glsl/phong.frag";
    
    clockStartProgram = clock();

    loadTexture();
    if(explosion){
        loadExplosionProgram();
    }
    else if(explosion2){
        loadExplosion2Program();
    }
    else if(random){
        loadRandomProgram();
    }
    else{
       loadProgram();
    }
    
    material->bindTextures();
}

ModelObject::~ModelObject() {
    std::cout << "Destructing '" << filename << "'" << std::endl;
}

void ModelObject::print() {
    std::cout << "# of shapes : " << shapes.size() << std::endl;

    for (size_t i = 0; i < shapes.size(); i++) {
        printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
        printf("shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
        assert((shapes[i].mesh.indices.size() % 3) == 0);
        for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) {
            printf("  idx[%ld] = %d\n", f, shapes[i].mesh.indices[f]);
        }

        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            printf("  v[%ld] = (%f, %f, %f)\n", v,
                   shapes[i].mesh.positions[3*v+0],
                   shapes[i].mesh.positions[3*v+1],
                   shapes[i].mesh.positions[3*v+2]);
        }

        printf("shape[%ld].material.name = %s\n", i, shapes[i].material.name.c_str());
        printf("  material.Ka = (%f, %f ,%f)\n", shapes[i].material.ambient[0], shapes[i].material.ambient[1], shapes[i].material.ambient[2]);
        printf("  material.Kd = (%f, %f ,%f)\n", shapes[i].material.diffuse[0], shapes[i].material.diffuse[1], shapes[i].material.diffuse[2]);
        printf("  material.Ks = (%f, %f ,%f)\n", shapes[i].material.specular[0], shapes[i].material.specular[1], shapes[i].material.specular[2]);
        printf("  material.Tr = (%f, %f ,%f)\n", shapes[i].material.transmittance[0], shapes[i].material.transmittance[1], shapes[i].material.transmittance[2]);
        printf("  material.Ke = (%f, %f ,%f)\n", shapes[i].material.emission[0], shapes[i].material.emission[1], shapes[i].material.emission[2]);
        printf("  material.Ns = %f\n", shapes[i].material.shininess);
        printf("  material.Ni = %f\n", shapes[i].material.ior);
        printf("  material.map_Ka = %s\n", shapes[i].material.ambient_texname.c_str());
        printf("  material.map_Kd = %s\n", shapes[i].material.diffuse_texname.c_str());
        printf("  material.map_Ks = %s\n", shapes[i].material.specular_texname.c_str());
        printf("  material.map_Ns = %s\n", shapes[i].material.normal_texname.c_str());
        std::map<std::string, std::string>::iterator it(shapes[i].material.unknown_parameter.begin());
        std::map<std::string, std::string>::iterator itEnd(shapes[i].material.unknown_parameter.end());
        for (; it != itEnd; it++) {
            printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
        }
    }
    printf("\n");
}

void ModelObject::draw(const View& view, LightPtr light) {


    if(godsRay){

        //printf("HERER\n" );
        bool WireFrame = false;
        bool blur = false;

// generate textures ------------------------------------------------------------------------------------------------------

    glGenTextures(1, &ColorBuffer);
    glGenTextures(1, &DepthBuffer);

    // generate FBO -----------------------------------------------------------------------------------------------------------

    glGenFramebuffersEXT(1, &FBO);

    if(blur)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ColorBuffer, 0);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, DepthBuffer, 0);
    }



        pushAndMultGLMatrix(GL_MODELVIEW, transform.getMatrix());
        lighting.use();
/* For every shape... */
    for (size_t shapeId = 0; shapeId < shapes.size(); ++shapeId) {
        
        // assert((shapes[shapeId].mesh.indices.size() % 3) == 0);
        
        /* For every triangle face in the mesh... */
        for (size_t indexId = 0; indexId < shapes[shapeId].mesh.indices.size(); indexId+=3) {
            
            float2 uvs;
            
            glBegin(GL_TRIANGLES);
            
            /* Vertex 0. */
            size_t vertexId = shapes[shapeId].mesh.indices[indexId];
            if (!shapes[shapeId].mesh.texcoords.empty()) {
                uvs[0] = shapes[shapeId].mesh.texcoords[2*vertexId];
                uvs[1] = shapes[shapeId].mesh.texcoords[2*vertexId+1];
                                if( uvs[1] >10000000000000000 or   uvs[1]  < -10000000000000000){
                    uvs[1] =0;
                }
              if( uvs[0] >10000000000000000 or   uvs[0]  < -10000000000000000){
                    uvs[0] =0;
                }
                glTexCoord2f(uvs[0],uvs[1]);
            }
            if (shapes[shapeId].mesh.normals.size() > 0) {
                glNormal3f(
                           shapes[shapeId].mesh.normals[3*vertexId],
                           shapes[shapeId].mesh.normals[3*vertexId+1],
                           shapes[shapeId].mesh.normals[3*vertexId+2]
                           );
            }

            glVertex3f(
                       shapes[shapeId].mesh.positions[3*vertexId],
                       shapes[shapeId].mesh.positions[3*vertexId+1],
                       shapes[shapeId].mesh.positions[3*vertexId+2]
                       );
            
            /* Vertex 1. */
            vertexId = shapes[shapeId].mesh.indices[indexId+1];
            if (!shapes[shapeId].mesh.texcoords.empty()) {
                uvs[0] = shapes[shapeId].mesh.texcoords[2*vertexId];
                uvs[1] = shapes[shapeId].mesh.texcoords[2*vertexId+1];
                                if( uvs[1] >10000000000000000 or   uvs[1]  < -10000000000000000){
                    uvs[1] =0;
                }
              if( uvs[0] >10000000000000000 or   uvs[0]  < -10000000000000000){
                    uvs[0] =0;
                }
                glTexCoord2f(uvs[0],uvs[1]);
            }
            if (shapes[shapeId].mesh.normals.size() > 0) {
                glNormal3f(
                           shapes[shapeId].mesh.normals[3*vertexId],
                           shapes[shapeId].mesh.normals[3*vertexId+1],
                           shapes[shapeId].mesh.normals[3*vertexId+2]
                           );
            }

            glVertex3f(
                       shapes[shapeId].mesh.positions[3*vertexId],
                       shapes[shapeId].mesh.positions[3*vertexId+1],
                       shapes[shapeId].mesh.positions[3*vertexId+2]
                       );
            
            /* Vertex 2. */
            vertexId = shapes[shapeId].mesh.indices[indexId+2];
            if (!shapes[shapeId].mesh.texcoords.empty()) {
                uvs[0] = shapes[shapeId].mesh.texcoords[2*vertexId];
                uvs[1] = shapes[shapeId].mesh.texcoords[2*vertexId+1];
                                if( uvs[1] >10000000000000000 or   uvs[1]  < -10000000000000000){
                    uvs[1] =0;
                }
              if( uvs[0] >10000000000000000 or   uvs[0]  < -10000000000000000){
                    uvs[0] =0;
                }
                glTexCoord2f(uvs[0],uvs[1]);
            }
            if (shapes[shapeId].mesh.normals.size() > 0) {
                glNormal3f(
                           shapes[shapeId].mesh.normals[3*vertexId],
                           shapes[shapeId].mesh.normals[3*vertexId+1],
                           shapes[shapeId].mesh.normals[3*vertexId+2]
                           );
            }

            glVertex3f(
                       shapes[shapeId].mesh.positions[3*vertexId],
                       shapes[shapeId].mesh.positions[3*vertexId+1],
                       shapes[shapeId].mesh.positions[3*vertexId+2]
                       );
            
            glEnd();
        }
    }


        glUseProgram(0);

    if(blur)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        glBindTexture(GL_TEXTURE_2D, ColorBuffer);

        rays.use();

        glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(1.0f, 0.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(0.0f, 1.0f);
        glEnd();

        glUseProgram(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }


        popGLMatrix(GL_MODELVIEW);
    }

    else{


    float4 eye_position_object_space = mul(transform.getInverseMatrix(), float4(view.eye_position,1));
    eye_position_object_space.xyz /= eye_position_object_space.w;
    
    float4 light_position_object_space = mul(transform.getInverseMatrix(), light->getPosition());
    light_position_object_space.xyz /= light_position_object_space.w;
    // LM = Light color modulated by Matrial color
    // a,d,s = ambient, diffuse, specular
    float4 LMa = material->ambient*light->getColor();
    float4 LMd = material->diffuse*light->getColor();
    float4 LMs = material->specular*light->getColor();
    if(explosion){
        glUseProgram(0);
        explosion_program.use();
        explosion_program.setVec3f("eyePosition", eye_position_object_space.xyz);
        explosion_program.setVec3f("lightPosition", light_position_object_space.xyz);
        explosion_program.setVec4f("LMa", LMa);
        explosion_program.setVec4f("LMd", LMd);
        explosion_program.setVec4f("LMs", LMs);
        explosion_program.setVec1f("shininess", material->shininess);
        
        explosion_program.setMat3f("objectToWorld", transform);
        // Send current frame time and previous frame time to gfx card.
        explosion_program.setVec1f("timePreviousFrame", timePreviousFrame );
        explosion_program.setVec1f("timeCurrentFrame", timeCurrentFrame );
    }
    else if(explosion2){
        glUseProgram(0);
        explosion2_program.use();
        explosion2_program.setVec3f("eyePosition", eye_position_object_space.xyz);
        explosion2_program.setVec3f("lightPosition", light_position_object_space.xyz);
        explosion2_program.setVec4f("LMa", LMa);
        explosion2_program.setVec4f("LMd", LMd);
        explosion2_program.setVec4f("LMs", LMs);
        explosion2_program.setVec1f("shininess", material->shininess);
        
        explosion2_program.setMat3f("objectToWorld", transform);
        // Send current frame time and previous frame time to gfx card.
        explosion2_program.setVec1f("timePreviousFrame", timePreviousFrame );
        explosion2_program.setVec1f("timeCurrentFrame", timeCurrentFrame );  
    }
    else if(random){
        glUseProgram(0);
        random_program.use();
        random_program.setVec3f("eyePosition", eye_position_object_space.xyz);
        random_program.setVec3f("lightPosition", light_position_object_space.xyz);
        random_program.setVec4f("LMa", LMa);
        random_program.setVec4f("LMd", LMd);
        random_program.setVec4f("LMs", LMs);
        random_program.setVec1f("shininess", material->shininess);
        
        random_program.setMat3f("objectToWorld", transform);
        // Send current frame time and previous frame time to gfx card.
        random_program.setVec1f("timePreviousFrame", timePreviousFrame );
        random_program.setVec1f("timeCurrentFrame", timeCurrentFrame );  
    }
    else{
        glUseProgram(0);
        program.use();        
        program.setVec3f("eyePosition", eye_position_object_space.xyz);
        program.setVec3f("lightPosition", light_position_object_space.xyz);
        program.setVec4f("LMa", LMa);
        program.setVec4f("LMd", LMd);
        program.setVec4f("LMs", LMs);
        program.setVec1f("shininess", material->shininess);
        
        program.setMat3f("objectToWorld", transform);
        // Send current frame time and previous frame time to gfx card.
        program.setVec1f("timePreviousFrame", timePreviousFrame );
        program.setVec1f("timeCurrentFrame", timeCurrentFrame );
    }
    
    pushAndMultGLMatrix(GL_MODELVIEW, transform.getMatrix());
    if(!outline){
    /* For every shape... */
    for (size_t shapeId = 0; shapeId < shapes.size(); ++shapeId) {
        // assert((shapes[shapeId].mesh.indices.size() % 3) == 0);
        /* For every triangle face in the mesh... */
        for (size_t indexId = 0; indexId < shapes[shapeId].mesh.indices.size(); indexId+=3) {
        float2 uvs = float2(1.0,1.0);
            glBegin(GL_TRIANGLES);
            /* Vertex 0. */
            size_t vertexId = shapes[shapeId].mesh.indices[indexId];
            if (!shapes[shapeId].mesh.texcoords.empty()) {
                uvs[0] = shapes[shapeId].mesh.texcoords[2*vertexId];
                uvs[1] = shapes[shapeId].mesh.texcoords[2*vertexId+1];
                                if( uvs[1] >10000000000000000 or   uvs[1]  < -10000000000000000){
                    uvs[1] =0;
                }
              if( uvs[0] >10000000000000000 or   uvs[0]  < -10000000000000000){
                    uvs[0] =0;
                }
                glTexCoord2f(uvs[0],uvs[1]);
            }
            if (shapes[shapeId].mesh.normals.size() > 0) {
                glNormal3f(
                           shapes[shapeId].mesh.normals[3*vertexId],
                           shapes[shapeId].mesh.normals[3*vertexId+1],
                           shapes[shapeId].mesh.normals[3*vertexId+2]
                           );
            }

            glVertex3f(
                       shapes[shapeId].mesh.positions[3*vertexId],
                       shapes[shapeId].mesh.positions[3*vertexId+1],
                       shapes[shapeId].mesh.positions[3*vertexId+2]
                       );
            
            /* Vertex 1. */
            vertexId = shapes[shapeId].mesh.indices[indexId+1];
            if (!shapes[shapeId].mesh.texcoords.empty()) {
                uvs[0] = shapes[shapeId].mesh.texcoords[2*vertexId];
                uvs[1] = shapes[shapeId].mesh.texcoords[2*vertexId+1];
                                if( uvs[1] >10000000000000000 or   uvs[1]  < -10000000000000000){
                    uvs[1] =0;
                }
              if( uvs[0] >10000000000000000 or   uvs[0]  < -10000000000000000){
                    uvs[0] =0;
                }
                glTexCoord2f(uvs[0],uvs[1]);
            }
            if (shapes[shapeId].mesh.normals.size() > 0) {
                glNormal3f(
                           shapes[shapeId].mesh.normals[3*vertexId],
                           shapes[shapeId].mesh.normals[3*vertexId+1],
                           shapes[shapeId].mesh.normals[3*vertexId+2]
                           );
            }

            glVertex3f(
                       shapes[shapeId].mesh.positions[3*vertexId],
                       shapes[shapeId].mesh.positions[3*vertexId+1],
                       shapes[shapeId].mesh.positions[3*vertexId+2]
                       );
            
            /* Vertex 2. */
            vertexId = shapes[shapeId].mesh.indices[indexId+2];
            if (!shapes[shapeId].mesh.texcoords.empty()) {
                uvs[0] = shapes[shapeId].mesh.texcoords[2*vertexId];
                uvs[1] = shapes[shapeId].mesh.texcoords[2*vertexId+1];
                if( uvs[1] >10000000000000000 or   uvs[1]  < -10000000000000000){
                    uvs[1] =0;
                }
              if( uvs[0] >10000000000000000 or   uvs[0]  < -10000000000000000){
                    uvs[0] =0;
                }
                glTexCoord2f(uvs[0],uvs[1]);
            }
            if (shapes[shapeId].mesh.normals.size() > 0) {
                glNormal3f(
                           shapes[shapeId].mesh.normals[3*vertexId],
                           shapes[shapeId].mesh.normals[3*vertexId+1],
                           shapes[shapeId].mesh.normals[3*vertexId+2]
                           );
            }
            glVertex3f(
                       shapes[shapeId].mesh.positions[3*vertexId],
                       shapes[shapeId].mesh.positions[3*vertexId+1],
                       shapes[shapeId].mesh.positions[3*vertexId+2]
                       );
            glEnd();
        }
    }
    }
    else{
        glPushMatrix();
        float       outlineColor[3] = { 255.0f, 255.0f, 255.0f };

        glEnable (GL_BLEND);                // Enable Blending
        glBlendFunc (GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);       // Draw Backfacing Polygons As Wireframes
        glLineWidth (0.3);         // Set The Line Width
        glCullFace (GL_FRONT);              // Don't Draw Any Front-Facing Polygons
        glDepthFunc (GL_LEQUAL);            // Change The Depth Mode
        glColor3fv (&outlineColor[0]);          // Set The Outline Color
        

            /* For every shape... */
            for (size_t shapeId = 0; shapeId < shapes.size(); ++shapeId) {
                /* For every triangle face in the mesh... */
                for (size_t indexId = 0; indexId < shapes[shapeId].mesh.indices.size(); indexId+=3) {
                    glBegin(GL_TRIANGLES);
                    
                    /* Vertex 0. */
                    size_t vertexId = shapes[shapeId].mesh.indices[indexId];
                    glVertex3f(
                               shapes[shapeId].mesh.positions[3*vertexId],
                               shapes[shapeId].mesh.positions[3*vertexId+1],
                               shapes[shapeId].mesh.positions[3*vertexId+2]
                               );
                    
                    /* Vertex 1. */
                    vertexId = shapes[shapeId].mesh.indices[indexId+1];
                    glVertex3f(
                               shapes[shapeId].mesh.positions[3*vertexId],
                               shapes[shapeId].mesh.positions[3*vertexId+1],
                               shapes[shapeId].mesh.positions[3*vertexId+2]
                               );
                    
                    /* Vertex 2. */
                    vertexId = shapes[shapeId].mesh.indices[indexId+2];
                    glVertex3f(
                               shapes[shapeId].mesh.positions[3*vertexId], 
                               shapes[shapeId].mesh.positions[3*vertexId+1], 
                               shapes[shapeId].mesh.positions[3*vertexId+2]
                               );
                    
                    glEnd();
                }
            }
		glDepthFunc (GL_LESS);									// Reset The Depth-Testing Mode ( NEW )
		glCullFace (GL_FRONT);									// Reset The Face To Be Culled ( NEW )
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);						// Reset Back-Facing Polygon Drawing Mode ( NEW )
		glDisable (GL_BLEND);
    }
   glPopMatrix();

    }
}


void ModelObject::loadTexture() {

     const std::string filename = folderpath+shapes[0].material.diffuse_texname;
     //const std::string filename = "tga/capsule0.png"; // Should work, but doesn't.

    std::cout << "Loading image at: " << filename << std::endl;

    Texture2DPtr texture(new Texture2D(filename.c_str()));
    texture->load();
    texture->tellGL();

    material->texture = texture;
    material->bindTextures();
}


void Camera::validate()
{
    projection_matrix = perspective(fov_degrees, aspect_ratio, znear, zfar);
}

Camera::Camera(float fov, float ar, float zn, float zf)
    : fov_degrees(fov)
    , aspect_ratio(ar)
    , znear(zn)
    , zfar(zf)
{
    validate();
}

void Camera::setAspectRatio(float ar)
{
    aspect_ratio = ar;
    validate();
}

void Camera::tellGL()
{
    loadGLMatrix(GL_PROJECTION, projection_matrix);
}

Camera& Camera::operator =(const Camera& rhs)
{
    if (this != &rhs) {
        fov_degrees = rhs.fov_degrees;
        aspect_ratio = rhs.aspect_ratio;
        znear = rhs.znear;
        zfar = rhs.zfar;
        projection_matrix = rhs.projection_matrix;
    }
    return *this;
}

Camera::Camera(const Camera& copy)
{
    fov_degrees = copy.fov_degrees;
    aspect_ratio = copy.aspect_ratio;
    znear = copy.znear;
    zfar = copy.zfar;
    projection_matrix = copy.projection_matrix;
}

void View::reset()
{
    eye_position = initial_eye_position;
    at_position = initial_at_position;
    eye_radius = distance(eye_position, at_position);
    eye_height = 0;
    eye_angle = 0;
}

void View::spinDegrees(float angle)
{
    eye_angle += radians(angle);
    dirty = true;
}

void View::lift(float height)
{
    eye_height += height;
    dirty = true;
}

void View::validate()
{
    if (dirty) {
        float2 on_circle_position = eye_radius*float2(sin(eye_angle), cos(eye_angle));
        eye_position = at_position + float3(on_circle_position.x, eye_height, on_circle_position.y);
#if 0
        OUTPUT(eye_position);
        OUTPUT(at_position);
        OUTPUT(up_vector);
#endif
        view_matrix = lookat(eye_position, at_position, up_vector);
        dirty = false;
    }
}

float4x4 View::getViewMatrix()
{
    validate();
    return view_matrix;
}

void View::tellGL()
{
    validate();
    loadGLMatrix(GL_MODELVIEW, view_matrix);
}

View::View(float3 eye, float3 at, float3 up)
    : initial_at_position(at)
    , initial_eye_position(eye)
    , up_vector(up)
    , eye_height(0)
    , eye_angle(0)
    , dirty(true)
{
    reset();
}

View& View::operator =(const View& rhs)
{
    if (this != &rhs) {
        initial_at_position = rhs.initial_at_position;
        initial_eye_position = rhs.initial_eye_position;
        up_vector = rhs.up_vector;

        at_position = rhs.at_position;
        eye_position = rhs.eye_position;

        eye_height = rhs.eye_height;
        eye_angle = rhs.eye_angle;
        eye_radius = rhs.eye_radius;
        dirty = rhs.dirty;
        view_matrix = rhs.view_matrix;
    }
    return *this;
}

Light::Light()
    : color(float3(1))
    , center(float3(0))
    , radius(1)
    , height(0)
    , dirty(true)
    , position(float3(0,0,1))
{
}

void Light::setColor(const float3 &color_)
{
    color = color_;
}

void Light::setCenter(float3 center_)
{
    center = center_;
    dirty = true;
}

void Light::setRadius(float radius_)
{
    radius = radius_;
    dirty = true;
}

void Light::setAngleInDegrees(float angle_)
{
    angle = radians(angle_);
    dirty = true;
}

void Light::setAngleInRadians(float angle_)
{
    angle = angle_;
    dirty = true;
}

void Light::spinDegrees(float angle_)
{
    angle += radians(angle_);
    dirty = true;
}

void Light::lift(float height_)
{
    height += height_;
    dirty = true;
}

void Light::validate()
{
    if (dirty) {
        float2 on_circle_position = radius*float2(sin(angle), cos(angle));
        position = center + float3(on_circle_position.x, height, on_circle_position.y);
#if 0
        OUTPUT(position);
#endif
        dirty = false;
    }
}

Light& Light::operator =(const Light& rhs)
{
    if (this != &rhs) {
        color = rhs.color;
        position = rhs.position;
        center = rhs.center;

        angle = rhs.angle;
        height = rhs.height;

        dirty = rhs.dirty;
    }
    return *this;
}

float4 Light::getPosition()
{
    validate();
    return float4(position, 1);
}

float4 Light::getColor()
{
    validate();
    return float4(color, 1);
}

static void glColor(float3 color)
{
    glColor3fv(reinterpret_cast<const GLfloat*>(&color));
}

#if 0  // unused
static void glColor(const float4 color)
{
    glColor4fv(reinterpret_cast<const GLfloat*>(&color));
}
#endif

void Light::draw(const View& view) {
    validate();

    glUseProgram(0);  // fixed-function
    glColor(color);
    pushGLMatrix(GL_MODELVIEW); {
        glTranslatef(position.x, position.y, position.z);
        glutSolidSphere(0.1, 20, 20);
    } popGLMatrix(GL_MODELVIEW);
}

Transform::Transform()
    : matrix(identity4x4())
    , dirty(true)
{}

Transform::Transform(const float4x4& m)
    : matrix(m)
    , dirty(true)
{}

void Transform::validate() const
{
    if (dirty) {
        inverse_matrix = inverse(matrix);
        dirty = false;
    }
}

float4x4 Transform::getMatrix() const
{
    return matrix;
}

float4x4 Transform::getInverseMatrix() const
{
    validate();
    return inverse_matrix;
}

void Transform::setMatrix(float4x4 m)
{
    matrix = m;
    dirty = true;
}

void Transform::setMatrix(const GLfloat m[4][4])
{
    // GL matrix is stored in column-major order so transpose it
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            matrix[i][j] = m[j][i];
        }
    }
    dirty = true;
}

void Transform::multMatrix(float4x4 m)
{
    matrix = mul(matrix, m);
    dirty = true;
}

Scene::Scene(const Camera& c, const View& v)
    : camera(c)
    , view(v)
{}

void Scene::setView(const View& v)
{
    view = v;
}

void Scene::setCamera(const Camera& c)
{
    camera = c;
}

void Scene::draw()
{
    camera.tellGL();
    view.tellGL();
    for (size_t i=0; i<object_list.size(); i++) {
        object_list[i]->draw(view, light_list[0]);
    }
    models->draw(view, light_list[0]);

    for (size_t i=0; i<light_list.size(); i++) {
        LightPtr light = light_list[i];
    
        light->draw(view);
    }
    if (envmap) {
        envmap->draw(10);
    }
}
void Scene::changeModel(std::string file_name, std::string folder_path)
{
    /* Only call delete if models points to something. */
    if (loadedModelAlready) {
        delete models;
    }
    loadedModelAlready = true;
    models = new ModelObject(file_name, folder_path, Transform(), material);
}
void Scene::addObject(ObjectPtr object)
{
    object_list.push_back(object);
}

void Scene::addLight(LightPtr light)
{
    light_list.push_back(light);
}

void Scene::setEnvMap(CubeMapPtr cm)
{
    envmap = cm;
}


Material::Material()
    : ambient(float4(0.2))
    , diffuse(float4(0.8,0.8,0.8,1.0))
    , specular(float4(0))
    , shininess(0)
{
}

void Material::bindTextures()
{
    if (normal_map) {
        normal_map->bind(GL_TEXTURE0);
    }
    if (texture) {
        texture->bind(GL_TEXTURE1);
    }
    if (height_field) {
        height_field->bind(GL_TEXTURE2);
    }
    if (envmap) {
        envmap->bind(GL_TEXTURE3);
    }
}

Material& Material::operator =(const Material& rhs)
{
    if (this != &rhs) {
        ambient = rhs.ambient;
        diffuse = rhs.diffuse;
        specular = rhs.specular;
        shininess = rhs.shininess;
        normal_map = rhs.normal_map;
        texture = rhs.texture;
        height_field = rhs.height_field;
    }
    return *this;
}

Mesh2D::Mesh2D(float2 xy_min_, float2 xy_max_, int2 steps_)
    : xy_min(xy_min_)
    , xy_max(xy_max_)
    , steps(steps_)
    , vbo(0)
    , valid(false)
    , num_ndxs(0)
    , ndx(NULL)
{}

Mesh2D::~Mesh2D()
{
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    delete ndx;
}

void Mesh2D::validate()
{
    if (vbo && valid) {
        return;  // Already valid.
    }

    float2 wh = xy_max - xy_min;
    float2 delta = wh/steps;

    int num_vtxs = (steps.x+1)*(steps.y+1);
    assert(sizeof(float2) == 2*sizeof(GLfloat));
    float2 *vtx = new float2[num_vtxs];

    num_ndxs = 2*(steps.x+1)*steps.y;
    ndx = new GLuint[num_ndxs];

    int v = 0,
        n = 0,
        i;
    for (i=0; i<steps.y; i++) {
        int bottom = i*(steps.x+1);
        int top = bottom+(steps.x+1);
        for (int j=0; j<steps.x; j++) {
            float2 xy = xy_min + delta*int2(j,i);
            vtx[v++] = xy;
            ndx[n++] = bottom++;
            ndx[n++] = top++;
        }
        // Maximum x edge
        float2 xy = float2(xy_max.x, xy_min.y + delta.y*i);
        vtx[v++] = xy;
        ndx[n++] = bottom++;
        ndx[n++] = top++;
    }
    { // Topmost row
        for (int j=0; j<steps.x; j++) {
            float2 xy = float2(xy_min.x + delta.x*j, xy_max.y);
            vtx[v++] = xy;
        }
        // Topmost maximum x edge
        float2 xy = xy_max;
        vtx[v++] = xy;
    }
    assert(v == num_vtxs);
    assert(n == num_ndxs);

    if (!vbo) {
        glGenBuffers(1, &vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, num_vtxs*2*sizeof(GLfloat), vtx, GL_STATIC_DRAW);

    valid = true;

    delete vtx;
}

#define BUFFER_OFFSET(_i) (reinterpret_cast<char *>(NULL) + (_i))

void Mesh2D::draw()
{
    validate();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    assert(glIsEnabled(GL_VERTEX_ARRAY));
    glVertexPointer(2, GL_FLOAT, 2*sizeof(GLfloat), BUFFER_OFFSET(0));
#if 1
    const GLuint *p = ndx;
    const GLsizei ndxs_per_strip = 2*(steps.x+1);
    for (int i=0; i<steps.y; i++) {
        glDrawElements(GL_TRIANGLE_STRIP, ndxs_per_strip, GL_UNSIGNED_INT, p);
        p += ndxs_per_strip;
    }
#else
    glBegin(GL_QUADS); {
#if 1
        glArrayElement(0);
        glArrayElement(4);
        glArrayElement(24);
        glArrayElement(20);
#else
        glVertex2f(0,0);
        glVertex2f(1,0);
        glVertex2f(1,1);
        glVertex2f(0,1);
#endif
    } glEnd();
#endif
}

