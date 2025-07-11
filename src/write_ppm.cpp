#include <fstream>
#include <filesystem>
#include "write_ppm.h"

void write_ppm( const char* path, int w, int h, const unsigned char* data ) {
    // Create output directory if it doesn't exist
    std::filesystem::path file_path( path );
    std::filesystem::create_directories( file_path.parent_path() );

    // Open in text mode since we're writing P3 format
    std::ofstream out( path );
    if ( !out ) { 
        return;
    } 

    out << "P3\n" << w << ' ' << h << "\n255\n";

    for ( int i = 0; i < w * h; i++ ) {
        out << (int)data[i * 3] << ' ' 
            << (int)data[i * 3 + 1] << ' ' 
            << (int)data[i * 3 + 2] << '\n';
    }

    out.close();
}
