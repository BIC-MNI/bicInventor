/**
 * Combine one or more BIC polygonal meshes into a single inventor-format
 * file.
 *
 * Output scene graph has structure:
 *
 *                 switch
 *                   |
 *     -------------------------
 *     |         |             |
 *   mesh1     mesh2   ...   meshN
 *
 * Where each mesh subtree is the result of bic_polygons_to_iv().
 *
 **/

#include <iostream.h>

#include <Inventor/SoDB.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>

// group nodes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>


extern "C" {
#  include "ParseArgv.h"
#  include "bicpl.h"
}

#include "bicInventor.h"



char* output_filename = 0;
int check_isomorphism = 0;
polygons_struct* first_mesh = 0;


ArgvInfo argTable[] = {
    { "-check_isomorphism", ARGV_CONSTANT, (char*)1, (char*)&check_isomorphism,
      "verify that input meshe graphs are isomorphic" },
    { "-out_file", ARGV_STRING, (char*)1, (char*)&output_filename,
      "write output to file instead of stdout" },
    { NULL, ARGV_END, NULL, NULL, NULL }
};


void usage( char* argv0 )
{
    char* s = strrchr( argv0, '/' );
    if ( s ) argv0 = s+1;

    cerr << "usage: " << argv0 << " [options] file ..." << endl;
    cerr << "\tconvert BIC obj file to inventor format." << endl;
}



/** 
 * Check that polygons p and q have the same graph structure.
 **/
static bool
is_isomorphic( const polygons_struct& p,
	       const polygons_struct& q)
{
    int n_faces = p.n_items;
    int f;

    if ( p.n_points != q.n_points ) {
	cerr << "Input polygons differ in number of vertices." << endl;
	return 0;
    }


    if ( p.n_items != q.n_items ) {
	cerr << "Input polygons differ in number of faces." << endl;
	return 0;
    }

    for( f = 0; f < n_faces; ++f ) {
	int n_vertices = GET_OBJECT_SIZE(p,f);
	int p_start = START_INDEX(p.end_indices,f);
	int q_start = START_INDEX(q.end_indices,f);
	int v;

	if ( n_vertices != GET_OBJECT_SIZE(q,f) ) {
	    cerr << "Input polygons differ in size of face " << f << endl;
	    return 0;
	}

	if ( p_start != q_start )
	    cerr << "Start vertices differ for face " << f << ".  Tell Steve!" 
		 << endl;

	for( v = 0; v < n_vertices; ++v ) {
	    if ( p.indices[p_start+v] != q.indices[q_start+v] ) {
		cerr << "Input polygons differ: face " << f << ", vertex " << v
		     << endl;
		return 0;
	    }
	}
    }
    return 1;
}



static SoSeparator*
read_polygons( char* filename ) 
{
    File_formats format;
    int num_objects;
    object_struct** object_list;

    if ( input_graphics_file( filename, &format, 
			      &num_objects, &object_list ) != OK ) {
	fprintf( stderr, "input_graphics_file( %s ) failed.\n", filename );
	exit(1);
    }

    if ( num_objects != 1 )
	cerr << "Warning: ignoring extra objects in file " << filename << endl;

    if ( (object_list[0])->object_type != POLYGONS ) {
	cerr << "Warning: first object not polygons in file " << filename 
	     << endl;
	return 0;
    }

    polygons_struct& mesh = (object_list[0])->specific.polygons;

    if ( first_mesh == 0 )
	first_mesh = &mesh;
    else if ( check_isomorphism ) {
	if (!is_isomorphic( *first_mesh, mesh )) {
	    cerr << "Skipping " << filename << endl;
	    return 0;
	}
    }

    return bic_polygons_to_iv( mesh );
}
  


int main( int ac, char** av ) 
{
    if ( ParseArgv( &ac, av, argTable, 0 ) || ac < 2 ) {
	usage( av[0] );
	return 1;
    }

    // Set up the inventor scene database.
    SoDB::init();
    SoSwitch* root = new SoSwitch;
    root->ref();

    for( int i = 1; i < ac; ++i ) {
	SoSeparator* mesh = read_polygons(av[i]);
	if ( mesh != 0 )
	    root->addChild(mesh);
    }
	

    // write file
    SoWriteAction wa;

    if ( output_filename ) {
	SoOutput* out = wa.getOutput();
	if ( out->openFile(output_filename) == 0 ) {
	    cerr << "cannot open file: " << output_filename << endl;
	    return 1;
	}
    }
    wa.apply(root);

    return 0;
}




