#ifndef BICINVENTOR_H      // -*- C++ -*-
#define BICINVENTOR_H

class SoNode;

#if 0
/** SGI's CC has a problem with redeclaring structs,
    though oddly it is okay with redeclaring classes.... **/
struct lines_struct;
struct marker_struct;
struct polygons_struct;
struct quadmesh_struct;

SoNode* bic_lines_to_iv( const lines_struct& l );
SoNode* bic_marker_to_iv( const marker_struct& m );
SoNode* bic_polygons_to_iv( const polygons_struct& p );
SoNode* bic_quadmesh_to_iv( const quadmesh_struct& q );
#endif

//! Convert entire BIC .obj file to scene graph.
SoNode* bic_graphics_file_to_iv( char* filename );



#endif
