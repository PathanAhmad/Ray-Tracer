#ifndef OBJ_UTILS_H
#define OBJ_UTILS_H

#include <vector>
#include <string>
#include "vec.h"

struct ObjMeshData {
    std::vector<Vec> vertices;
    std::vector<Vec> normals;
    std::vector<Vec> texcoords;
    struct Face {
        int v[3];
        int t[3];
        int n[3];
    };
    std::vector<Face> faces;
};

bool load_obj_mesh( const std::string& path, ObjMeshData& out );

#endif 