/**
 * Translate BIC obj file to inventor format.
 **/

#include <iostream.h>
#include <fstream.h>
#include <string>

#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>

// shape nodes
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoQuadMesh.h>

// property nodes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTransform.h>

// group nodes
#include <Inventor/nodes/SoSeparator.h>

extern "C" {
#  include <bicpl.h>
}

// GCC will complain if you declare "struct foo", then "typedef struct
// { ... } foo".  However, it is happy if the order of declarations is
// reversed.
#include "bicInventor.h"




/*! \brief Add material binding and base colour nodes to the group.
 *
 * \return the base colour node
 */
static SoBaseColor* 
add_color_nodes( SoGroup* root, 
		 Colour_flags bic_colour_flag,
		 Colour bic_colour )
{
    SoMaterialBinding* mat_binding = new SoMaterialBinding;
    SoBaseColor* base_color = new SoBaseColor;

    switch( bic_colour_flag ) {
    case ONE_COLOUR:
	mat_binding->value = SoMaterialBinding::OVERALL;
	base_color->rgb.set1Value( 0, float(get_Colour_r_0_1(bic_colour)),
				      float(get_Colour_g_0_1(bic_colour)),
				      float(get_Colour_b_0_1(bic_colour)) );
	break;
    case PER_VERTEX_COLOURS:
	mat_binding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
	break;
    default:
	cerr << "Unable to handle BIC colour flag: " << bic_colour << endl;
    }

    root->addChild(mat_binding);
    root->addChild(base_color);
    return base_color;
}



/* The process_*() functions are listed in order of the 
   "union {} specific" member of object_struct.  See <bicpl/obj_defs.h>.
*/



SoSeparator* bic_lines_to_iv( const lines_struct& l )
{
    SoSeparator* root = new SoSeparator;
    SoBaseColor* color = add_color_nodes( root, l.colour_flag, l.colours[0] );


    SoCoordinate3* coord = new SoCoordinate3;
    coord->point.setNum( l.n_points );

    for( int v = 0; v < l.n_points; ++v ) {
	coord->point.set1Value( v, Point_x(l.points[v]),
				   Point_y(l.points[v]),
				   Point_z(l.points[v]) );

	if ( l.colour_flag == PER_VERTEX_COLOURS ) {
	    color->rgb.set1Value( v, float(get_Colour_r_0_1(l.colours[v])),
				     float(get_Colour_g_0_1(l.colours[v])),
				     float(get_Colour_b_0_1(l.colours[v])) );
	}
    }

    root->addChild(coord);

    SoIndexedLineSet* line_set = new SoIndexedLineSet;

    int ls_index = 0;
    for( int i = 0; i < l.n_items; ++i ) {
	int i_size = GET_OBJECT_SIZE( l, i );
	for( int v = 0; v < i_size; ++v ) {
	    int ind = l.indices[POINT_INDEX(l.end_indices,i,v)];
	    line_set->coordIndex.set1Value( ls_index++, ind );
	}
	// End of polyline indicated by index -1
	line_set->coordIndex.set1Value( ls_index++, -1 );
    }

    root->addChild(line_set);
    return root;
}


// We ignore label, structure_id, and patient_id fields.
SoSeparator* bic_marker_to_iv( const marker_struct& m ) 
{
    SoSeparator* root = new SoSeparator;
    add_color_nodes( root, ONE_COLOUR, m.colour );

    SoTransform* trans = new SoTransform;
    trans->translation.setValue( Point_x(m.position),
				 Point_y(m.position),
				 Point_z(m.position) );
    root->addChild(trans);

    switch( m.type ) {

    case BOX_MARKER: {
	// Display draws a box using "size" for the width, height and depth.
	// The field "position" specifies the centre of the box.
	SoCube* box = new SoCube;
	box->width = m.size;
	box->height = m.size;
	box->depth = m.size;
	root->addChild(box);
	break;
    }
    case SPHERE_MARKER: {
	// Display also draws a "sphere" markers as a cube, but we'll do it right.
	SoSphere* sphere = new SoSphere;
	sphere->radius = m.size;
	root->addChild(sphere);
	break;
    }

    default:
	cerr << "Cannot handle marker type " << m.type << endl;
    }

    return root;
}  


SoSeparator* bic_model_to_iv( const model_struct& m )
{
    cerr << "Skipping model object." << endl;
    return NULL;
}


SoSeparator* bic_pixels_to_iv( const pixels_struct& p )
{
    cerr << "Skipping pixels object." << endl;
    return NULL;
}


SoSeparator* bic_polygons_to_iv( const polygons_struct& p )
{
    SoSeparator* root = new SoSeparator;
    SoBaseColor* color = add_color_nodes( root, p.colour_flag, p.colours[0] );

    SoNormalBinding* normal_binding = new SoNormalBinding;
    SoNormal* normal = new SoNormal;

    normal->setName("Normals");

    if ( p.normals ) {
	normal_binding->value = SoNormalBinding::PER_VERTEX_INDEXED;
	normal->vector.setNum( p.n_points );
    }
    root->addChild(normal_binding);
    root->addChild(normal);


    SoCoordinate3* coord = new SoCoordinate3;
    coord->setName("Vertices");
    coord->point.setNum( p.n_points );

    for( int v = 0; v < p.n_points; ++v ) {
	coord->point.set1Value( v, p.points[v].coords );

	if ( p.normals )
	    normal->vector.set1Value( v, p.normals[v].coords );

	if ( p.colour_flag == PER_VERTEX_COLOURS ) {
	    color->rgb.set1Value( v, float(get_Colour_r_0_1(p.colours[v])),
				     float(get_Colour_g_0_1(p.colours[v])),
				     float(get_Colour_b_0_1(p.colours[v])) );
	}
    }

    root->addChild(coord);

    SoIndexedFaceSet* face_set = new SoIndexedFaceSet;
    face_set->setName("Polygons");

    int fs_index = 0;
    for( int f = 0; f < p.n_items; ++f ) {
	int f_size = GET_OBJECT_SIZE( p, f );
	for( int v = 0; v < f_size; ++v ) {
	    int ind = p.indices[POINT_INDEX(p.end_indices,f,v)];
	    face_set->coordIndex.set1Value( fs_index++, ind );
	}
	// End of facet indicated by index -1
	face_set->coordIndex.set1Value( fs_index++, -1 );
    }

    root->addChild(face_set);
    return root;
}



SoSeparator* bic_quadmesh_to_iv( const quadmesh_struct& q )
{
    SoSeparator* root = new SoSeparator;

    SoBaseColor* color = add_color_nodes( root, q.colour_flag, q.colours[0] );

    SoNormalBinding* normal_binding = new SoNormalBinding;
    SoNormal* normal = new SoNormal;

    if ( q.normals ) {
	normal_binding->value = SoNormalBinding::PER_VERTEX;
	//normal->vector.setNum( q.n_points );
    }
    root->addChild(normal_binding);
    root->addChild(normal);


    SoCoordinate3* coord = new SoCoordinate3;
    //coord->point.setNum( p.n_points );

    // Coordinates are stored by row.
    // Take care of "closed" meshes by duplicating row or column 0.
    int row_max = ( q.m_closed ? q.m+1 : q.m );
    int col_max = ( q.n_closed ? q.n+1 : q.n );

    int v = 0;
    for( int row = 0; row < row_max; ++row ) {
	int i = (row == q.m ? 0 : row);
	for( int col = 0; col < col_max; ++col ) {
	    int j = (col == q.n ? 0 : col);
	    // Compute index using IJ because there is no other way
	    // to get the normal vector from a quadmesh.
	    int index = IJ(i,j,q.n);

	    coord->point.set1Value( v, Point_x(q.points[index]), 
				       Point_y(q.points[index]), 
				       Point_z(q.points[index]) );

	    if ( q.normals ) {
		normal->vector.set1Value( v, Point_x(q.normals[index]),
					     Point_y(q.normals[index]),
					     Point_z(q.normals[index]) );
	    }


	    if ( q.colour_flag == PER_VERTEX_COLOURS ) {
		color->rgb.set1Value( v, float(get_Colour_r_0_1(q.colours[index])),
				         float(get_Colour_g_0_1(q.colours[index])),
				         float(get_Colour_b_0_1(q.colours[index])) );
	    }
	    ++v;
	}
    }

    root->addChild(coord);

    SoQuadMesh* mesh = new SoQuadMesh;
    mesh->verticesPerColumn = col_max;
    mesh->verticesPerRow = row_max;

    root->addChild(mesh);
    return root;
}


SoSeparator* bic_text_to_iv( const text_struct& t )
{
    cerr << "Skipping text object." << endl;
    return NULL;
}



/*! \return node representing root of scene graph, or NULL
 *          if the file could not be read
 *
 * \bug The following BIC object types are not currently
 * recognized: MODEL, PIXELS, TEXT, and N_OBJECT_TYPES.
 */
SoSeparator* bic_graphics_file_to_iv( char* filename ) 
{
    SoSeparator* root = new SoSeparator;

    File_formats format;
    int num_objects;
    object_struct** object_list;

    if ( input_graphics_file( filename, 
			      &format, &num_objects, &object_list ) != OK ) 
    {
	return 0;
    }

    for ( int i = 0; i < num_objects; ++i ) {
	SoNode* obj = NULL;

	switch( (object_list[i])->object_type ) {
	case LINES: 
	    obj = bic_lines_to_iv( (object_list[i])->specific.lines );
	    break;
	case MARKER: 
	    obj = bic_marker_to_iv( (object_list[i])->specific.marker );
	    break;
	case MODEL: 
	    obj = bic_model_to_iv( (object_list[i])->specific.model ); 
	    break;
	case PIXELS: 
	    obj = bic_pixels_to_iv( (object_list[i])->specific.pixels ); 
	    break;
	case POLYGONS: 
	    obj = bic_polygons_to_iv( (object_list[i])->specific.polygons ); 
	    break;
	case QUADMESH: 
	    obj = bic_quadmesh_to_iv( (object_list[i])->specific.quadmesh ); 
	    break;
	case TEXT: 
	    obj = bic_text_to_iv( (object_list[i])->specific.text ); 
	    break;
//      case N_OBJECT_TYPES: dump_nobj( (object_list[i])->specific.??? ); break;
	default: 
	    cerr << "BIC object type " << object_list[i]->object_type 
		 << " not handled." << endl;
	}

	if ( obj ) {
	    root->addChild(obj);
	}
    }
    return root;
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
     
SoTextureCoordinate2* bic_vertex_info_to_texture_coordinate( char* filename,
                                                             int column,
                                                             bool invert)
{
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

    
    

    
