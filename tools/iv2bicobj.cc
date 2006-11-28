/**
 * Translate inventor scene graph to BIC obj file
 **/

#include <config.h>

#if HAVE_CSTRING
#   include <cstring>
#endif
#include <iostream>

#include <Inventor/SoDB.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>

// group nodes
#include <Inventor/nodes/SoSeparator.h>


#include "bicInventor.h"

extern "C" {
#  include "ParseArgv.h"
#  include <bicpl.h>
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

    std::cerr << "usage: " << argv0 << " input.iv output.obj" << std::endl;
    std::cerr << "\tconvert Inventor scene graph to BIC obj file." << std::endl;
}


int main( int ac, char** av ) 
{
    
    
    if (ac != 3) {
	usage( av[0] );
	return 1;
    }

    // Read in the inventor scene graph
    SoInput sceneInput;
    if (!sceneInput.openFile(av[1])) {
	std::cerr << "Cannot open file " << av[1] << std::endl;
	return 1;
    }

    // Read the whole file into the database
    SoSeparator *graph = SoDB::readAll(&sceneInput);
    graph->ref();
    if (graph == NULL) {
	std::cerr <<  "Problem reading file" << std::endl;
	return 1;
    } 

    object_struct *object = iv_to_bic_polygons( graph );

    output_graphics_file(av[2], ASCII_FORMAT, 1, &object);

    sceneInput.closeFile();

    return 0;
}


