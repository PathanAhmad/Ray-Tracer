#include "obj_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool load_obj_mesh( const std::string& path, ObjMeshData& out ) {
    std::ifstream file( path );
    if ( !file.is_open() ) {
        std::cerr << "Error: Cannot open OBJ file: " << path << std::endl;
        return false;
    }
    
    std::string line;
    while ( std::getline( file, line ) ) {
        std::istringstream iss( line );
        std::string type;
        iss >> type;
        if ( type == "v" ) {
            float x, y, z;
            iss >> x >> y >> z;
            out.vertices.push_back( Vec( x, y, z ) );
        } else if ( type == "vn" ) {
            float x, y, z;
            iss >> x >> y >> z;
            out.normals.push_back( Vec( x, y, z ) );
        } else if ( type == "vt" ) {
            float u, v;
            iss >> u >> v;
            out.texcoords.push_back( Vec( u, v, 0 ) );
        } else if ( type == "f" ) {
            ObjMeshData::Face face;
            for ( int i = 0; i < 3; i++ ) {
                std::string vert;
                iss >> vert;
                int v = -1, t = -1, n = -1;
                try {
                    size_t p1 = vert.find( '/' );
                    if ( p1 == std::string::npos ) {
                        v = std::stoi( vert ) - 1;
                    } else {
                        v = std::stoi( vert.substr( 0, p1 ) ) - 1;
                        size_t p2 = vert.find( '/', p1 + 1 );
                        if ( p2 == std::string::npos ) {
                            std::string t_str = vert.substr( p1 + 1 );
                            if ( !t_str.empty() ) t = std::stoi( t_str ) - 1;
                        } else {
                            std::string t_str = vert.substr( p1 + 1, p2 - p1 - 1 );
                            if ( !t_str.empty() ) t = std::stoi( t_str ) - 1;
                            std::string n_str = vert.substr( p2 + 1 );
                            if ( !n_str.empty() ) n = std::stoi( n_str ) - 1;
                        }
                    }
                } catch ( const std::exception& e ) {
                    std::cerr << "Error parsing face indices in OBJ file: " << path << std::endl;
                    continue;
                }
                face.v[i] = v;
                face.t[i] = t;
                face.n[i] = n;
            }
            out.faces.push_back( face );
        }
    }
    
    return true;
} 