#ifndef MESH_H
#define MESH_H

#include "object.h"
#include "triangle.h"
#include "material.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "transform.h"
#include "obj_utils.h"
#include <iostream>
#include <cmath>

class Mesh : public Object {
public:
    Mesh() : Object() { }
    Mesh(Material* mat) : Object(mat) { }

    bool load(const std::string& filename) {
        ObjMeshData mesh_data;
        std::string full_path = "scenes/" + filename;
        
        if ( !load_obj_mesh( full_path, mesh_data ) ) {
            std::cerr << "Error: Failed to load mesh file: " << full_path << std::endl;
            return false;
        }
        
        if ( mesh_data.vertices.empty() || mesh_data.faces.empty() ) {
            return false;
        }
        
        vertices = mesh_data.vertices;
        normals = mesh_data.normals;
        texcoords = mesh_data.texcoords;
        
        for ( const auto& f : mesh_data.faces ) {
            if ( f.v[0] >= 0 && f.v[1] >= 0 && f.v[2] >= 0 && 
                 f.v[0] < vertices.size() && f.v[1] < vertices.size() && f.v[2] < vertices.size() ) {
                
                // Create triangle without material - material will be handled by the mesh
                Triangle* tri = new Triangle( vertices[f.v[0]], vertices[f.v[1]], vertices[f.v[2]], nullptr );
                
                if ( f.n[0] >= 0 && f.n[1] >= 0 && f.n[2] >= 0 && 
                     f.n[0] < normals.size() && f.n[1] < normals.size() && f.n[2] < normals.size() ) {
                    tri->set_normals( normals[f.n[0]], normals[f.n[1]], normals[f.n[2]] );
                }
                
                if ( f.t[0] >= 0 && f.t[1] >= 0 && f.t[2] >= 0 && 
                     f.t[0] < texcoords.size() && f.t[1] < texcoords.size() && f.t[2] < texcoords.size() ) {
                    tri->set_tex_coords( texcoords[f.t[0]], texcoords[f.t[1]], texcoords[f.t[2]] );
                }
                
                triangles.push_back( tri );
            }
        }
        
        return triangles.size() > 0;
    }

    bool intersect( const Ray& ray, Hit& hit ) const override {
        Ray local_ray = transform.inverse_transform_ray( ray );

        bool found = false;
        float closest_local_t = INFINITY;
        Hit closest_hit;

        for ( Triangle* tri : triangles ) {
            Hit temp_hit;
            temp_hit.t = INFINITY;
            
            if ( tri->intersect( local_ray, temp_hit ) ) {
                if ( temp_hit.t < closest_local_t && temp_hit.t > 0.001f ) {
                    closest_local_t = temp_hit.t;
                    closest_hit = temp_hit;
                    found = true;
                }
            }
        }

        if ( found ) {
            // Transform back to world space
            Vec world_point = transform.transform_point( closest_hit.point );
            Vec world_normal = transform.transform_normal( closest_hit.normal ).normalize();
            
            // Recalculate t for world space ray
            Vec to_hit = world_point - ray.origin;
            float world_t = Vec::dot( to_hit, ray.direction.normalize() );
            
            if ( world_t > 0.001f ) {
                hit.t = world_t;
                hit.point = world_point;
                hit.normal = world_normal;
                // Here I assign the material to the hit
                hit.material = this->material;
                
                // Here I pass through the texture coordinates
                hit.u = closest_hit.u;
                hit.v = closest_hit.v;
                return true;
            }
        }

        return false;
    }

    Vec get_normal(const Vec& point) const override {
        Vec local_point = transform.inverse_transform_point(point);
        for (Triangle* tri : triangles) {
            if (tri->contains_point(local_point)) {
                Vec local_normal = tri->get_normal(local_point);
                return transform.transform_normal(local_normal);
            }
        }
        return Vec(0, 1, 0);  // Default normal
    }

    ~Mesh() {
        for (Triangle* tri : triangles) {
            delete tri;
        }
    }

    std::vector<Vec> vertices;
    std::vector<Vec> normals;
    std::vector<Vec> texcoords;
    std::vector<Triangle*> triangles;
    Transform transform;
};

#endif 