#include <fstream>
#include "write_ppm.h"

void make_black( int w , int h , const char* path )
{

    std::ofstream out(path, std::ios::out | std::ios::binary);
    if( !out ){ 
        return;
    } 

    out << "P3\n" << w << ' ' << h << "\n255\n";

    for(int i = 0; i < w*h; ++i)   // This should make all pixels black
    {
        out << "0 0 0\n";
    }

    out.close();

}
