#ifndef BICINVENTOR_H      // -*- C++ -*-
#define BICINVENTOR_H

class SoSeparator;
class SoTextureCoordinate2;
class SoShapeKit;

#if 1
/** SGI's CC has a problem with redeclaring structs,
    though oddly it is okay with redeclaring classes.... **/
struct lines_struct;
struct marker_struct;
struct polygons_struct;
struct quadmesh_struct;

// functions to be used when working with scene graphs
SoSeparator* bic_lines_to_iv( const lines_struct& l );
SoSeparator* bic_marker_to_iv( const marker_struct& m );
SoSeparator* bic_polygons_to_iv( const polygons_struct& p );
SoSeparator* bic_quadmesh_to_iv( const quadmesh_struct& q );

#endif

//! Convert entire BIC .obj file to scene graph.
SoSeparator* bic_graphics_file_to_iv( char* filename );

//! Read a file containing information about each vertex
SoTextureCoordinate2* bic_vertex_info_to_texture_coordinate( char* filename,
                                                             int column = 0,
                                                             bool invert=false);


#endif
