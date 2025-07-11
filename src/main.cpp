#include <iostream>
#include <filesystem>
#include "scene.h"

int main( int argc, char* argv[] ) {
    std::cout << "[DEBUG] main() started" << std::endl;
    if ( argc != 3 ) {
        std::cerr << "Usage: " << argv[0] << " <input.xml> <output.png>" << std::endl;
        return 1;
    }

    // Create output directory if it doesn't exist
    std::filesystem::path output_path( argv[2] );
    if ( output_path.parent_path().empty() ) {
        output_path = std::filesystem::path( "output" ) / output_path;
    }
    std::filesystem::create_directories( output_path.parent_path() );

    Scene scene;
    if ( !scene.load( argv[1] ) ) {
        std::cerr << "Failed to load scene file: " << argv[1] << std::endl;
        return 1;
    }
    scene.render( output_path.string() );

    return 0;
}
