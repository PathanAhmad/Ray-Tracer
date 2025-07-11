#include "scene.h"
#include "scene_parser.h"

bool Scene::load( const std::string& filename ) {
    return SceneParser::parse( *this, filename );
} 