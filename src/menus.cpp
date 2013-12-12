
#include <assert.h>

#if !defined(__Cg_vector_hpp__)
#include <Cg/double.hpp>
#endif
#include <Cg/vector.hpp>
#include <Cg/matrix.hpp>
#include "countof.h"

#include <cstring>

#include "texture.hpp"
#include "scene.hpp"
#include "menus.hpp"
#include "global.hpp"

static const struct {
    const char *name;
    const char *pathToFolder; // From media folder
    const char *filename;
} model_list[] = {
    { "Cubey",       "cube/",          "cube.obj" },
    { "Statue",      "statue/",        "statue.obj" },
    { "Mario",       "mario/",         "mario.obj" },
    { "Giant Robot", "giant_robot/",   "giant_robot.obj" },
    { "R2D2",        "r2d2/",          "R2.obj" },
    { "Skull",       "skull/",         "skull.obj" },
    { "Shark",       "shark/",         "shark.obj" },
    // { "Buddha",      "buddha/",        "buddha.obj" }, // Maybe add back in if model loading gets WAYYYY faster.
    { "Monkey",      "monkey/",        "monkey.obj" },
    { "Dragon",      "dragon_smooth/", "dragon_smooth.obj" },
    { "Bunny",       "bunny/",         "bunny.obj" },
};

void modelMenu(int item)
{
    assert(item < (int)countof(model_list));

    const std::string pathToMediaFolder = "../media/";
    const std::string file_name = std::string(model_list[item].filename);

    const std::string folder_path = (
        pathToMediaFolder + 
        std::string(model_list[item].pathToFolder)
    );

    // std::cout << "file_name: " << file_name << std::endl;
    // std::cout << "folder_path: " << folder_path << std::endl;
    
    scene->changeModel(file_name, folder_path);

    glutPostRedisplay();
}

static const struct {
    const char *name;
    const char *filename_pattern;
} envmap_list[] = {
    { "Cloudy hills", "tga/cloudyhills_%s.tga" },
    { "Foggy desert", "tga/foggydesert_%s.tga" },
    { "In clouds", "tga/inclouds_%s.tga" },
    { "Nighttime", "tga/night_%s.tga" },
    { "Tron world", "tga/tron_%s.tga" },
    { "Lava world", "tga/lava_%s.tga" },
};

void envMapMenu(int item)
{
    assert(item < (int)countof(envmap_list));

    CubeMapPtr envmap(new CubeMap(envmap_list[item].filename_pattern));
    envmap->load();
    envmap->tellGL();

    material->envmap = envmap;
    scene->setEnvMap(envmap);

    glutPostRedisplay();
}

static const struct {
    const char *name;
    const char *filename;
} bumpy_list[] = {
    { "Outward bumps", "tga/bumps_out.tga" },
    { "Inward bumps", "tga/bumps_in.tga" },
    { "Brick", "tga/brick.tga" },
    { "GeForce cell", "tga/geforce_cell.tga" },
    { "GeForce etch", "tga/geforce_etch.tga" },
    { "Mosaic", "tga/mosaic.tga" },
    { "Stripes", "tga/stripes.tga" },
    { "Texas Longhorn", "tga/texas_longhorn.tga" },
    { "Texas Longhorn 2", "tga/texas_longhorn2.tga" },
};

void bumpyMenu(int item)
{
    assert(item < (int)countof(bumpy_list));

    const char *filename = bumpy_list[item].filename;

    NormalMapPtr normal_map = NormalMapPtr(new NormalMap(filename));
    normal_map->load(bump_height);
    normal_map->tellGL();

    Texture2DPtr height_field(new Texture2D(filename));
    height_field->load();
    height_field->tellGL();

    material->normal_map = normal_map;
    material->height_field = height_field;

    material->bindTextures();

    glutPostRedisplay();
}

static const struct {
    const char *name;
    const char *filename;
} texture_list[] = {
    { "Texas Longhorn", "tga/texas_longhorn_texture.tga" },
    { "Solid white", "tga/solid_white.tga" },
    { "Brick", "tga/brick_texture.tga" },
    { "GeForce", "tga/geforce.tga" },
    { "Solid green", "tga/solid_green.tga" },
    { "Green stripes", "tga/green_stripes.tga" },
};

void textureMenu(int item)
{
    assert(item < (int)countof(texture_list));

    const char *filename = texture_list[item].filename;

    Texture2DPtr texture(new Texture2D(filename));
    texture->load();
    texture->tellGL();

    material->texture = texture;
    material->bindTextures();
    glutPostRedisplay();
}

static const struct {
    const char *name;
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float shininess;
} material_list[] = {
    { "Matte white", float3(0.1, 0.1, 0.1), float3(0.9, 0.9, 0.9), float3(0), 0 },
    { "Emerald", float3(0.0215, 0.1745, 0.0215), float3(0.07568, 0.61424, 0.07568), float3(0.633, 0.727811, 0.633), 0.6*128 },
    { "Jade", float3(0.135, 0.2225, 0.1575), float3(0.54, 0.89, 0.63), float3(0.316228, 0.316228, 0.316228), 0.1*128 },
    { "Obsidian", float3(0.05375, 0.05, 0.06625), float3(0.18275, 0.17, 0.22525), float3(0.332741, 0.328634, 0.346435), 0.3*128 },
    { "Pearl", float3(0.25, 0.20725, 0.20725), float3(1, 0.829, 0.829), float3(0.296648, 0.296648, 0.296648), 0.088*128 },
    { "Ruby", float3(0.1745, 0.01175, 0.01175), float3(0.61424, 0.04136, 0.04136), float3(0.727811, 0.626959, 0.626959), 0.6*128 },
    { "Turquoise", float3(0.1, 0.18725, 0.1745), float3(0.396, 0.74151, 0.69102), float3(0.297254, 0.30829, 0.306678), 0.1*128 },
    { "Brass", float3(0.329412, 0.223529, 0.027451), float3(0.780392, 0.568627, 0.113725), float3(0.992157, 0.941176, 0.807843), 0.21794872*128 },
    { "Bronze", float3(0.2125, 0.1275, 0.054), float3(0.714, 0.4284, 0.18144), float3(0.393548, 0.271906, 0.166721), 0.2*128 },
    { "Chrome", float3(0.25, 0.25, 0.25), float3(0.4, 0.4, 0.4), float3(0.774597, 0.774597, 0.774597), 0.6*128 },
    { "Copper", float3(0.19125, 0.0735, 0.0225), float3(0.7038, 0.27048, 0.0828), float3(0.256777, 0.137622, 0.086014), 0.1*128 },
    { "Gold", float3(0.24725, 0.1995, 0.0745), float3(0.75164, 0.60648, 0.22648), float3(0.628281, 0.555802, 0.366065), 0.4*128 },
    { "Silver", float3(0.19225, 0.19225, 0.19225), float3(0.50754, 0.50754, 0.50754), float3(0.508273, 0.508273, 0.508273), 0.4*128 },
    { "Black plastic", float3(0.0, 0.0, 0.0), float3(0.01, 0.01, 0.01), float3(0.50, 0.50, 0.50), .25*128 },
    { "Cyan plastic", float3(0.0, 0.1, 0.06), float3(0.0, 0.50980392, 0.50980392), float3(0.50196078, 0.50196078, 0.50196078), .25*128 },
    { "Green plastic", float3(0.0, 0.0, 0.0), float3(0.1, 0.35, 0.1), float3(0.45, 0.55, 0.45), .25*128 },
    { "Red plastic", float3(0.0, 0.0, 0.0), float3(0.5, 0.0, 0.0), float3(0.7, 0.6, 0.6), .25*128 },
    { "White plastic", float3(0.0, 0.0, 0.0), float3(0.55, 0.55, 0.55), float3(0.70, 0.70, 0.70), .25*128 },
    { "Yellow plastic", float3(0.0, 0.0, 0.0), float3(0.5, 0.5, 0.0), float3(0.60, 0.60, 0.50), .25*128 },
    { "Black rubber", float3(0.02, 0.02, 0.02), float3(0.01, 0.01, 0.01), float3(0.4, 0.4, 0.4), .078125*128 },
    { "Cyan rubber", float3(0.0, 0.05, 0.05), float3(0.4, 0.5, 0.5), float3(0.04, 0.7, 0.7), .078125*128 },
    { "Green rubber", float3(0.0, 0.05, 0.0), float3(0.4, 0.5, 0.4), float3(0.04, 0.7, 0.04), .078125*128 },
    { "Red rubber", float3(0.05, 0.0, 0.0), float3(0.5, 0.4, 0.4), float3(0.7, 0.04, 0.04), .078125*128 },
    { "White rubber", float3(0.05, 0.05, 0.05), float3(0.5, 0.5, 0.5), float3(0.7, 0.7, 0.7), .078125*128 },
    { "Yellow rubber", float3(0.05, 0.05, 0.0), float3(0.5, 0.5, 0.4), float3(0.7, 0.7, 0.04), .078125*128 }
};

void materialMenu(int item)
{
    assert(item < (int)countof(material_list));

    material->ambient = float4(material_list[item].ambient, 1);
    material->diffuse = float4(material_list[item].diffuse, 1);
    material->specular = float4(material_list[item].specular, 1);
    material->shininess = material_list[item].shininess;
    glutPostRedisplay();
}

static const struct {
    const char *name;
    float3 color;
} light_list[] = {
    { "White", float3(1) },
    { "Yellow white", float3(1,1,0.8) },
    { "Bright white", float3(1.8) },
    { "Red", float3(1,0,0) },
    { "Green", float3(0,1,0) },
    { "Blue", float3(0,0,1) },
    { "Yellow", float3(1,1,0) },
};

void lightMenu(int item)
{
    assert(item < (int)countof(light_list));

    light->setColor(light_list[item].color);
    glutPostRedisplay();
}

static const struct {
    const char *name;
    const char *filename;
} shader_list[] = {
    { "Flat Lighting",           "glsl/phong.frag" },
    { "Negative",                "glsl/negative.frag" },
    { "Sepia",                   "glsl/sepia.frag" },
    { "Monochrome",              "glsl/monochrome.frag" },
    { "Color Cycle",             "glsl/color_cycle.frag" },
    { "Noise Stripes & Spirals", "glsl/noise_stripes_spirals.frag" },
    { "Anti-Diffuse",            "glsl/anti_diffuse.frag" },
    { "Normals",                 "glsl/normals.frag" },
    { "Facing-Ratio",            "glsl/facing.frag" },
    { "Toon Simple 1",             "glsl/toon_simple.frag" },
    { "Toon Simple 2",           "glsl/toon_simple_glossy.frag" },
    { "Gooch",                   "glsl/gooch.frag" },
    {"Gods Ray",               "glsl/gods_ray.frag"}
};

void shaderMenu(int item)
{
    assert(item < (int)countof(shader_list));

    const char *filename = shader_list[item].filename;
    printf("Switching to shader \"%s\", loaded from %s...\n", shader_list[item].name, filename);
    scene->models->setGodsRay();
    if(strcmp(shader_list[item].name,shader_list[12].name )==0){
        scene->models->loadGodsRay();
    }
    else{
        scene->models->fragment_filename = filename;
        scene->models->loadProgram();
    }
    material->bindTextures();
    glutPostRedisplay();
}
static const struct {
    const char *name;
    const char *action;
} extra_list[] = {
    { "Toggle Wireframe",   "outline" }
};

void extraMenu(int item)
{
    assert(item < (int)countof(extra_list));
    
    const char * action = extra_list[item].action;
    printf("Switching to extra \"%s\"\n", extra_list[item].name);
    
    if(strcmp(action,  extra_list[0].name)){
        scene->models->setOutline();
        scene->models->loadProgram();
    }

    glutPostRedisplay();
}

enum {
    MO_STOP_ANIMATION,
    MO_QUIT
};

static void menu(int item)
{
    switch (item) {
    case MO_QUIT:
        exit(0);
        break;
    case MO_STOP_ANIMATION:
        stopObjectSpinning();
        break;
    default:
        assert(!"unknown menu item");
        break;
    }
}

void initMenus()
{
    int model_menu = glutCreateMenu(modelMenu);
    for (size_t i=0; i<countof(model_list); i++) {
        glutAddMenuEntry(model_list[i].name, i);
    }
    // int texture_menu = glutCreateMenu(textureMenu);
    // for (size_t i=0; i<countof(texture_list); i++) {
    //     glutAddMenuEntry(texture_list[i].name, i);
    // }
    // int bumpy_menu = glutCreateMenu(bumpyMenu);
    // for (size_t i=0; i<countof(bumpy_list); i++) {
    //     glutAddMenuEntry(bumpy_list[i].name, i);
    // }
    int material_menu = glutCreateMenu(materialMenu);
    for (size_t i=0; i<countof(material_list); i++) {
        glutAddMenuEntry(material_list[i].name, i);
    }
    int envmap_menu = glutCreateMenu(envMapMenu);
    for (size_t i=0; i<countof(envmap_list); i++) {
        glutAddMenuEntry(envmap_list[i].name, i);
    }
    int light_menu = glutCreateMenu(lightMenu);
    for (size_t i=0; i<countof(light_list); i++) {
        glutAddMenuEntry(light_list[i].name, i);
    }
    int shader_menu = glutCreateMenu(shaderMenu);
    for (size_t i=0; i<countof(shader_list); i++) {
        glutAddMenuEntry(shader_list[i].name, i);
    }
    int extra_menu = glutCreateMenu(extraMenu);
    for (size_t i=0; i<countof(extra_list); i++) {
        glutAddMenuEntry(extra_list[i].name, i);
    }
    glutCreateMenu(menu);
    glutAddSubMenu("Load model...", model_menu);
    // glutAddSubMenu("Decal texture...", texture_menu);
    // glutAddSubMenu("Bump texture...", bumpy_menu);
    glutAddSubMenu("Material...", material_menu);
    glutAddSubMenu("Environments...", envmap_menu);
    glutAddSubMenu("Light color...", light_menu);
    glutAddSubMenu("Shader...", shader_menu);
    glutAddSubMenu("Extras...", extra_menu);
    glutAddMenuEntry("Stop animation", MO_STOP_ANIMATION);
    glutAddMenuEntry("Quit", MO_QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

