#ifndef BICINVENTOR_OBJECTCONVERTERS_H      // -*- C++ -*-
#define BICINVENTOR_OBJECTCONVERTERS_H

class SoSeparator;

extern "C" {
#   include <bicpl.h>
// Undo some of the volume_io braindamage.
#   undef ON
#   undef OFF
}


// functions to be used when working with scene graphs

//! Convert BIC lines structure to Inventor scene graph.
SoSeparator* bic_lines_to_iv( const lines_struct& l );

//! Convert BIC marker structure to Inventor scene graph.
SoSeparator* bic_marker_to_iv( const marker_struct& m );

//! Convert BIC polygons structure to Inventor scene graph.
SoSeparator* bic_polygons_to_iv( const polygons_struct& p );

//! Convert BIC quadmesh structure to Inventor scene graph.
SoSeparator* bic_quadmesh_to_iv( const quadmesh_struct& q );


#endif
