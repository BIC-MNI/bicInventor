/**
 * Translate BIC obj file to inventor format.
 **/

#include <cstring>
#include <iostream.h>

#include <Inventor/SoDB.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>

// group nodes
#include <Inventor/nodes/SoSeparator.h>


#include "bicInventor.h"

extern "C" {
#  include "ParseArgv.h"
}



char* output_filename = NULL;

ArgvInfo argTable[] = {
    { "-out_file", ARGV_STRING, (char*)1, (char*)&output_filename,
      "write output to file instead of stdout" },
    { NULL, ARGV_END, NULL, NULL, NULL }
};


void usage( char* argv0 )
{
    char* s = strrchr( argv0, '/' );
    if ( s ) argv0 = s+1;

    cerr << "usage: " << argv0 << " [options] file" << endl;
    cerr << "\tconvert BIC obj file to inventor format." << endl;
}


int main( int ac, char** av ) 
{
    if ( ParseArgv( &ac, av, argTable, 0 ) || ac < 2 ) {
	usage( av[0] );
	return 1;
    }

    // Set up the inventor scene database.
    SoDB::init();
    SoNode* root = bic_graphics_file_to_iv( av[1] );
    root->ref();

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


