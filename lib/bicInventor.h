#ifndef BICINVENTOR_H      // -*- C++ -*-
#define BICINVENTOR_H

extern "C" {
#   include <bicpl/colour_coding.h>
#   undef ON
#   undef OFF
}

class SoSFImage;
class SoSeparator;
class SoTextureCoordinate2;


//! Convert entire BIC .obj file to scene graph.
SoSeparator* bic_graphics_file_to_iv( const char* filename );


//! Generate image of BIC colour code.
void bic_colourcode( SoSFImage& image,
		     Colour_coding_types type,
		     int height = 1000,
		     int width = 1 );

//! Generate image of BIC user-defined colour code.
void bic_colourcode( SoSFImage& image,
		     const char* filename,
		     int height = 1000,
		     int width = 1 );


//! Read a file containing information about each vertex
SoTextureCoordinate2* 
bic_vertex_info_to_texture_coordinate( const char* filename,
				       int column = 0,
				       bool invert = false );

#endif
