#ifndef BICINVENTOR_H      // -*- C++ -*-
#define BICINVENTOR_H

class SoSeparator;
class SoTextureCoordinate2;


//! Convert entire BIC .obj file to scene graph.
SoSeparator* bic_graphics_file_to_iv( char* filename );

//! Read a file containing information about each vertex
SoTextureCoordinate2* 
bic_vertex_info_to_texture_coordinate( const char* filename,
				       int column = 0,
				       bool invert = false);

#endif
