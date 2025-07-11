#include "scene_parser.h"
#include "scene.h"
#include "sphere.h"
#include "mesh.h"
#include "material.h"
#include "light.h"
#include "camera.h"
#include <fstream>
#include <sstream>
#include <string>
#include <tinyxml2.h>
#include <iostream>

bool SceneParser::parse( Scene& scene, const std::string& filename ) {
    tinyxml2::XMLDocument doc;
    if ( doc.LoadFile( filename.c_str() ) != tinyxml2::XML_SUCCESS ) {
        return false;
    }

    tinyxml2::XMLElement* root = doc.RootElement();
    if ( !root || strcmp( root->Name(), "scene" ) != 0 ) {
        return false;
    }

    // Parse output file
    const char* output_file = root->Attribute( "output_file" );
    if ( output_file ) {
        scene.output_file = output_file;
    } else {
        // Try parsing background color directly
        float r = root->FloatAttribute( "r", 0.0f );
        float g = root->FloatAttribute( "g", 0.0f );
        float b = root->FloatAttribute( "b", 0.0f );
        scene.background_color = Vec( r, g, b );
    }

    // Parse background color
    tinyxml2::XMLElement* bg = root->FirstChildElement( "background_color" );
    if ( bg ) {
        float r = bg->FloatAttribute( "r", 0.0f );
        float g = bg->FloatAttribute( "g", 0.0f );
        float b = bg->FloatAttribute( "b", 0.0f );
        scene.background_color = Vec( r, g, b );
    }

    // Parse camera
    tinyxml2::XMLElement* camera = root->FirstChildElement( "camera" );
    if ( camera ) {
        // Parse camera position
        tinyxml2::XMLElement* pos = camera->FirstChildElement( "position" );
        if ( pos ) {
            float x = pos->FloatAttribute( "x", 0.0f );
            float y = pos->FloatAttribute( "y", 0.0f );
            float z = pos->FloatAttribute( "z", 0.0f );
            
            scene.camera.position = Vec( x, y, z );
        }

        // Parse camera lookat
        tinyxml2::XMLElement* look = camera->FirstChildElement( "lookat" );
        if ( look ) {
            float x = look->FloatAttribute( "x", 0.0f );
            float y = look->FloatAttribute( "y", 0.0f );
            float z = look->FloatAttribute( "z", 0.0f );
            scene.camera.look_at = Vec( x, y, z );
        }

        // Parse camera up vector
        tinyxml2::XMLElement* up = camera->FirstChildElement( "up" );
        if ( up ) {
            float x = up->FloatAttribute( "x", 0.0f );
            float y = up->FloatAttribute( "y", 1.0f );
            float z = up->FloatAttribute( "z", 0.0f );
            scene.camera.up = Vec( x, y, z );
        }

        // Parse camera resolution
        tinyxml2::XMLElement* res = camera->FirstChildElement( "resolution" );
        if ( res ) {
            scene.camera.width = res->IntAttribute( "horizontal", 800 );
            scene.camera.height = res->IntAttribute( "vertical", 600 );
        }

        // Parse camera FOV
        tinyxml2::XMLElement* fov = camera->FirstChildElement( "horizontal_fov" );
        if ( fov ) {
            float angle = fov->FloatAttribute( "angle", 60.0f );
            scene.camera.fov = angle;
        }

        // Parse max bounces
        tinyxml2::XMLElement* bounces = camera->FirstChildElement( "max_bounces" );
        if ( bounces ) {
            scene.max_bounces = bounces->IntAttribute( "n", 5 );
        }
    }

    // Remove hardcoded lighting adjustments for specific examples
    // These should be handled by proper scene setup, not manual boosts

    // Parse lights
    tinyxml2::XMLElement* lights = root->FirstChildElement( "lights" );
    if ( lights ) {
        // Parse ambient light
        tinyxml2::XMLElement* ambient = lights->FirstChildElement( "ambient_light" );
        if ( ambient ) {
            tinyxml2::XMLElement* color = ambient->FirstChildElement( "color" );
            if ( color ) {
                float r = color->FloatAttribute( "r", 0.1f );
                float g = color->FloatAttribute( "g", 0.1f );
                float b = color->FloatAttribute( "b", 0.1f );
                
                scene.ambientLight = Vec( r, g, b );
                // Add ambient light to lights list
                scene.lights.push_back( new AmbientLight( Vec( r, g, b ) ) );
            }
        }

        // Parse point lights
        for ( tinyxml2::XMLElement* light = lights->FirstChildElement( "point_light" ); 
              light; 
              light = light->NextSiblingElement( "point_light" ) ) {
            tinyxml2::XMLElement* color = light->FirstChildElement( "color" );
            tinyxml2::XMLElement* pos = light->FirstChildElement( "position" );
            if ( color && pos ) {
                float r = color->FloatAttribute( "r", 1.0f );
                float g = color->FloatAttribute( "g", 1.0f );
                float b = color->FloatAttribute( "b", 1.0f );
                float x = pos->FloatAttribute( "x", 0.0f );
                float y = pos->FloatAttribute( "y", 0.0f );
                float z = pos->FloatAttribute( "z", 0.0f );
                
                scene.lights.push_back( new PointLight( Vec( x, y, z ), Vec( r, g, b ) ) );
            }
        }

        // Parse parallel lights
        for ( tinyxml2::XMLElement* light = lights->FirstChildElement( "parallel_light" ); 
              light; 
              light = light->NextSiblingElement( "parallel_light" ) ) {
            tinyxml2::XMLElement* color = light->FirstChildElement( "color" );
            tinyxml2::XMLElement* dir = light->FirstChildElement( "direction" );
            if ( color && dir ) {
                float r = color->FloatAttribute( "r", 1.0f );
                float g = color->FloatAttribute( "g", 1.0f );
                float b = color->FloatAttribute( "b", 1.0f );
                float x = dir->FloatAttribute( "x", 0.0f );
                float y = dir->FloatAttribute( "y", 0.0f );
                float z = dir->FloatAttribute( "z", 0.0f );
                
                scene.lights.push_back( new ParallelLight( Vec( x, y, z ), Vec( r, g, b ) ) );
            }
        }

        // Parse spot lights
        for ( tinyxml2::XMLElement* light = lights->FirstChildElement( "spot_light" ); 
              light; 
              light = light->NextSiblingElement( "spot_light" ) ) {
            tinyxml2::XMLElement* color = light->FirstChildElement( "color" );
            tinyxml2::XMLElement* pos = light->FirstChildElement( "position" );
            tinyxml2::XMLElement* dir = light->FirstChildElement( "direction" );
            tinyxml2::XMLElement* falloff = light->FirstChildElement( "falloff" );
            if ( color && pos && dir && falloff ) {
                float r = color->FloatAttribute( "r", 1.0f );
                float g = color->FloatAttribute( "g", 1.0f );
                float b = color->FloatAttribute( "b", 1.0f );
                float px = pos->FloatAttribute( "x", 0.0f );
                float py = pos->FloatAttribute( "y", 0.0f );
                float pz = pos->FloatAttribute( "z", 0.0f );
                float dx = dir->FloatAttribute( "x", 0.0f );
                float dy = dir->FloatAttribute( "y", 0.0f );
                float dz = dir->FloatAttribute( "z", 0.0f );
                float a1 = falloff->FloatAttribute( "alpha1", 0.0f );
                float a2 = falloff->FloatAttribute( "alpha2", 0.0f );
                
                scene.lights.push_back( new SpotLight( Vec( px, py, pz ), Vec( dx, dy, dz ), 
                                                     Vec( r, g, b ), a1, a2 ) );
            }
        }
    } else {
        // Try parsing ambient light directly from root
        tinyxml2::XMLElement* ambient = root->FirstChildElement( "ambient" );
        if ( ambient ) {
            float r = ambient->FloatAttribute( "r", 0.1f );
            float g = ambient->FloatAttribute( "g", 0.1f );
            float b = ambient->FloatAttribute( "b", 0.1f );
            scene.ambientLight = Vec( r, g, b );
        }
    }

    // Parse surfaces
    tinyxml2::XMLElement* surfaces = root->FirstChildElement( "surfaces" );
    if ( surfaces ) {
        // Parse spheres
        for ( tinyxml2::XMLElement* sphere = surfaces->FirstChildElement( "sphere" ); 
              sphere; 
              sphere = sphere->NextSiblingElement( "sphere" ) ) {
            float radius = sphere->FloatAttribute( "radius", 1.0f );
            tinyxml2::XMLElement* pos = sphere->FirstChildElement( "position" );
            if ( pos ) {
                float x = pos->FloatAttribute( "x", 0.0f );
                float y = pos->FloatAttribute( "y", 0.0f );
                float z = pos->FloatAttribute( "z", 0.0f );
                Sphere* s = new Sphere( Vec( x, y, z ), radius );

                // Parse material
                tinyxml2::XMLElement* material = sphere->FirstChildElement( "material_solid" );
                if ( !material ) {
                    material = sphere->FirstChildElement( "material_textured" );
                }
                if ( material ) {
                    Material* mat = parse_material( material );
                    if ( mat ) {
                        s->material = mat;
                        scene.materials.push_back( mat );
                    }
                }

                // Parse transformations
                tinyxml2::XMLElement* transforms = sphere->FirstChildElement( "transform" );
                if ( transforms ) {
                    s->transform = parse_transforms( transforms );
                }

                scene.objects.push_back( s );
            }
        }

        // Parse meshes
        for ( tinyxml2::XMLElement* mesh = surfaces->FirstChildElement( "mesh" ); 
              mesh; 
              mesh = mesh->NextSiblingElement( "mesh" ) ) {
            const char* name = mesh->Attribute( "name" );
            if ( name ) {
                Mesh* m = new Mesh();
                if ( m->load( name ) ) {
                    // Parse material
                    tinyxml2::XMLElement* material = mesh->FirstChildElement( "material_solid" );
                    if ( !material ) {
                        material = mesh->FirstChildElement( "material_textured" );
                    }
                    if ( material ) {
                        Material* mat = parse_material( material );
                        if ( mat ) {
                            m->material = mat;
                            scene.materials.push_back( mat );
                        }
                    }

                    // Parse transformations
                    tinyxml2::XMLElement* transforms = mesh->FirstChildElement( "transform" );
                    if ( transforms ) {
                        m->transform = parse_transforms( transforms );
                    }

                    scene.objects.push_back( m );
                } else {
                    delete m;
                }
            }
        }
    } else {
        // Try parsing spheres directly from root
        for ( tinyxml2::XMLElement* sphere = root->FirstChildElement( "sphere" ); 
              sphere; 
              sphere = sphere->NextSiblingElement( "sphere" ) ) {
            float radius = sphere->FloatAttribute( "radius", 1.0f );
            tinyxml2::XMLElement* pos = sphere->FirstChildElement( "position" );
            if ( pos ) {
                float x = pos->FloatAttribute( "x", 0.0f );
                float y = pos->FloatAttribute( "y", 0.0f );
                float z = pos->FloatAttribute( "z", 0.0f );
                Sphere* s = new Sphere( Vec( x, y, z ), radius );

                // Parse material
                const char* material_id = sphere->Attribute( "material" );
                if ( material_id ) {
                    // Find material by ID
                    tinyxml2::XMLElement* material = root->FirstChildElement( "material" );
                    while ( material ) {
                        if ( strcmp( material->Attribute( "id" ), material_id ) == 0 ) {
                            Material* mat = parse_material( material );
                            if ( mat ) {
                                s->material = mat;
                                scene.materials.push_back( mat );
                            }
                            break;
                        }
                        material = material->NextSiblingElement( "material" );
                    }
                }

                scene.objects.push_back( s );
            }
        }
    }

    return true;
}

Material* SceneParser::parse_material( tinyxml2::XMLElement* material ) {
    // Check if this is a material_textured element
    tinyxml2::XMLElement* texture_elem = material->FirstChildElement( "texture" );
    if ( texture_elem ) {
        // Parse textured material
        const char* texture_name = texture_elem->Attribute( "name" );
        if ( texture_name ) {
            float ka = 0.3f;
            float kd = 0.9f;
            float ks = 1.0f;
            float shininess = 200.0f;
            float reflection = 0.0f;
            float transmission = 0.0f;
            float ior = 1.0f;

            // Parse Phong parameters
            tinyxml2::XMLElement* phong = material->FirstChildElement( "phong" );
            if ( phong ) {
                ka = phong->FloatAttribute( "ka", 0.3f );
                kd = phong->FloatAttribute( "kd", 0.9f );
                ks = phong->FloatAttribute( "ks", 1.0f );
                shininess = phong->FloatAttribute( "exponent", 200.0f );
            }

            // Parse reflectance
            tinyxml2::XMLElement* reflectance = material->FirstChildElement( "reflectance" );
            if ( reflectance ) {
                reflection = reflectance->FloatAttribute( "r", 0.0f );
            }

            // Parse transmittance
            tinyxml2::XMLElement* transmittance = material->FirstChildElement( "transmittance" );
            if ( transmittance ) {
                transmission = transmittance->FloatAttribute( "t", 0.0f );
            }

            // Parse refraction
            tinyxml2::XMLElement* refraction = material->FirstChildElement( "refraction" );
            if ( refraction ) {
                ior = refraction->FloatAttribute( "iof", 1.0f );
            }

            TexturedMaterial* mat = new TexturedMaterial( texture_name, ka, kd, ks, shininess, reflection, transmission, ior );
            return mat;
        }
    }

    // Try parsing material_solid format
    tinyxml2::XMLElement* color = material->FirstChildElement( "color" );
    if ( color ) {
        float r = color->FloatAttribute( "r", 1.0f );
        float g = color->FloatAttribute( "g", 1.0f );
        float b = color->FloatAttribute( "b", 1.0f );
        float ka = 0.3f;
        float kd = 0.9f;
        float ks = 1.0f;
        float shininess = 200.0f;
        float reflection = 0.0f;
        float transmission = 0.0f;
        float ior = 2.3f;

        // Parse Phong parameters
        tinyxml2::XMLElement* phong = material->FirstChildElement( "phong" );
        if ( phong ) {
            ka = phong->FloatAttribute( "ka", 0.3f );
            kd = phong->FloatAttribute( "kd", 0.9f );
            ks = phong->FloatAttribute( "ks", 1.0f );
            shininess = phong->FloatAttribute( "exponent", 200.0f );
        }

        // Parse reflectance
        tinyxml2::XMLElement* reflectance = material->FirstChildElement( "reflectance" );
        if ( reflectance ) {
            reflection = reflectance->FloatAttribute( "r", 0.0f );
        }

        // Parse transmittance
        tinyxml2::XMLElement* transmittance = material->FirstChildElement( "transmittance" );
        if ( transmittance ) {
            transmission = transmittance->FloatAttribute( "t", 0.0f );
        }

        // Parse refraction
        tinyxml2::XMLElement* refraction = material->FirstChildElement( "refraction" );
        if ( refraction ) {
            ior = refraction->FloatAttribute( "iof", 2.3f );
        }

        Material* mat = new Material( Vec( r, g, b ), ka, kd, ks, shininess, reflection, transmission, ior );
        return mat;
    }

    // Try parsing simple material format
    float r = material->FloatAttribute( "r", 1.0f );
    float g = material->FloatAttribute( "g", 1.0f );
    float b = material->FloatAttribute( "b", 1.0f );
    float ka = material->FloatAttribute( "ka", 0.3f );
    float kd = material->FloatAttribute( "kd", 0.9f );
    float ks = material->FloatAttribute( "ks", 1.0f );
    float shininess = material->FloatAttribute( "shininess", 200.0f );
    float reflection = material->FloatAttribute( "reflection", 0.0f );
    float transmission = material->FloatAttribute( "transmission", 0.0f );
    float ior = material->FloatAttribute( "ior", 1.0f );

    return new Material( Vec( r, g, b ), ka, kd, ks, shininess, reflection, transmission, ior );
}

Transform SceneParser::parse_transforms( tinyxml2::XMLElement* transforms ) {
    Transform result;
    for ( tinyxml2::XMLElement* transform = transforms->FirstChildElement(); 
          transform; 
          transform = transform->NextSiblingElement() ) {
        const char* type = transform->Name();
        if ( strcmp( type, "translate" ) == 0 ) {
            float x = transform->FloatAttribute( "x", 0.0f );
            float y = transform->FloatAttribute( "y", 0.0f );
            float z = transform->FloatAttribute( "z", 0.0f );
            result = result * Transform::translate( Vec( x, y, z ) );
        } else if ( strcmp( type, "scale" ) == 0 ) {
            float x = transform->FloatAttribute( "x", 1.0f );
            float y = transform->FloatAttribute( "y", 1.0f );
            float z = transform->FloatAttribute( "z", 1.0f );
            result = result * Transform::scale( Vec( x, y, z ) );
        } else if ( strcmp( type, "rotateX" ) == 0 ) {
            float angle = transform->FloatAttribute( "theta", 0.0f );
            result = result * Transform::rotateX( angle );
        } else if ( strcmp( type, "rotateY" ) == 0 ) {
            float angle = transform->FloatAttribute( "theta", 0.0f );
            result = result * Transform::rotateY( angle );
        } else if ( strcmp( type, "rotateZ" ) == 0 ) {
            float angle = transform->FloatAttribute( "theta", 0.0f );
            result = result * Transform::rotateZ( angle );
        }
    }
    return result;
} 