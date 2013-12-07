//
// Copyright 2012-2013, Syoyo Fujita.
//
// Licensed under 2-clause BSD liecense.
//

#include <string>
#include <vector>
#include <map>

    typedef struct
    {
        std::string name;
        
        float ambient[3];
        float diffuse[3];
        float specular[3];
        float transmittance[3];
        float emission[3];
        float shininess;
        float ior;                // index of refraction
        
        std::string ambient_texname;
        std::string diffuse_texname;
        std::string specular_texname;
        std::string normal_texname;
        std::map<std::string, std::string> unknown_parameter;
    } material_t;
    
    typedef struct
    {
        std::vector<float>          positions;
        std::vector<float>          normals;
        std::vector<float>          texcoords;
        std::vector<unsigned int>   indices;
    } mesh_t;
    
    typedef struct
    {
        std::string  name;
        material_t   material;
        mesh_t       mesh;
    } shape_t;
    
    /// Loads .obj from a file.
    /// 'shapes' will be filled with parsed shape data
    /// The function returns error string.
    /// Returns empty string when loading .obj success.
    /// 'mtl_basepath' is optional, and used for base path for .mtl file.
    std::string LoadObj(
                        std::vector<shape_t>& shapes,   // [output]
                        std::string filename,
                        std::string mtl_basepath = NULL);


