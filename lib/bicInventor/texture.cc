/**
 * Translate MNI file types to inventor texture and related classes.
 **/

#include "bicInventor.h"

extern "C" {
#   include <bicpl.h>
// Undo some of the volume_io braindamage.
#   undef ON
#   undef OFF
}

#include <iostream>
#include <fstream>
#include <string>


#include <Inventor/fields/SoSFImage.h>

#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>



/*! Generate an SoSFImage image from an MNI colour code.
 * This is typically for use as a texture for vertex colouring.
 * (This version is NOT for use with type \b USER_DEFINED_COLOUR_MAP)
 * 
 * \param height height of resulting image
 * \param width width of resulting image
 *
 * \note bic_vertex_info_to_texture_coordinate uses vertex values
 *    as the second coordinate, so the height represents the number
 *    of shades available.  The width can remain as 1.
 */
void bic_colourcode( SoSFImage& image,
		     Colour_coding_types type,
		     int height, int width )
{
    colour_coding_struct ccd;

    // We're not going to deal with under- and over-colours, yet.
    initialize_colour_coding( &ccd, type, WHITE, WHITE, 0, 1 );
    set_colour_coding_under_colour( &ccd, get_colour_code(&ccd, 0.001) );
    set_colour_coding_over_colour( &ccd, get_colour_code(&ccd, 0.999) );

    // Generate colours for value 0 to value 1 in vertical direction.
    unsigned char pixels[3*height*width];
    int i = 0;
    for( int y = 0; y < height; ++y ) {
	Colour col = get_colour_code( &ccd, double(y) / double(height-1) );
	int r = get_Colour_r(col);
	int g = get_Colour_g(col);
	int b = get_Colour_b(col);

	for( int x = 0; x < width; ++x ) {
	    pixels[i++] = r;
	    pixels[i++] = g;
	    pixels[i++] = b;
	}
    }
    delete_colour_coding( &ccd );
    image.setValue( SbVec2s(width,height), 3, pixels );
}




/*! Reads a text file which contains information in the form of a
 *  floating point value about each vertex. These types of files are
 *  produced by, for example, volume_object_evaluate and thickness or
 *  curvature measurements. Moreover, it can also accept multi-column
 *  files, column's separated by spaces. The column that is returned
 *  is specified by the column variable. And it can invert the array
 *  as well (multiply by -1).
 *
 *  \return A texture coordinate node which contains the floating point value
 *          associated with each vertex.
 *  \note Assumes that the vertex information is implicitly presented in the
 *        same order as the vertices
 *  \note A texture index generally works in two dimensions. The second
 *        dimension is, in this case, ignored and always set at 0.5
 */
     
SoTextureCoordinate2* 
bic_vertex_info_to_texture_coordinate( const char* filename,
				       int column,
				       bool invert)
{
    using namespace std;
    SoTextureCoordinate2 *texCoord = new SoTextureCoordinate2;
    int startF, endF;
    string line;
    // open the file with the texture info
    ifstream vertexInfo(filename);
  
    
    // read each vertex;
    int i = 0;
    while (! vertexInfo.eof() ) {
      float vertexValue;
      getline(vertexInfo,line);
      
      // split the string to get just the column that is wanted
      endF = 0;
      for (int j = 0; j <= column; j++) {
        if (endF != -1) {
          startF = endF;
          endF = line.find_first_of(" ", startF+1);
          if (endF == 0) {
            endF = line.length();
          }
        }
      }
      string tmpstring = line.substr(startF,endF);
      vertexValue = atof(tmpstring.c_str());
      if (invert == true) {
        vertexValue *= -1;
      }

      texCoord->point.set1Value(i, SbVec2f(0.5, vertexValue));
      i++;
    }
    return texCoord;
}

    
    

    
