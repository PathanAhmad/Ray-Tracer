#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include "scene.h"
#include "sphere.h"
#include "mesh.h"
#include "material.h"
#include "light.h"
#include "camera.h"
#include <string>
#include <tinyxml2.h>

class SceneParser {
public:
    static bool parse( Scene& scene, const std::string& filename );
    static Material* parse_material( tinyxml2::XMLElement* material );
    static Transform parse_transforms( tinyxml2::XMLElement* transforms );
};

#endif 