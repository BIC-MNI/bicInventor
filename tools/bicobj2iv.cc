/**
 * Translate BIC obj file to inventor format.
 **/

#include <iostream>

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
#include <Inventor/nodes/SoTransform.h>

// group nodes
#include <Inventor/nodes/SoSeparator.h>


extern "C" {
#  include <bicpl.h>
#  include "ParseArgv.h"
}



char* output_filename = NULL;

ArgvInfo argTable[] = {
    { "-out_file", ARGV_STRING, (char*)1, (char*)&output_filename,
      "write output to file instead of stdout" },
    { NULL, ARGV_END, NULL, NULL, NULL }
};



/*! \brief Add material binding and base colour nodes to the group.
 *
 */
SoBaseColor* 
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



SoNode* process_lines( const lines_struct& l )
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
SoNode* process_marker( const marker_struct& m ) 
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


SoNode* process_model( const model_struct& m )
{
    cerr << "Skipping model object." << endl;
    return NULL;
}


SoNode* process_pixels( const pixels_struct& p )
{
    cerr << "Skipping pixels object." << endl;
    return NULL;
}


SoNode* process_polygons( const polygons_struct& p )
{
    SoSeparator* root = new SoSeparator;
    SoBaseColor* color = add_color_nodes( root, p.colour_flag, p.colours[0] );

    SoNormalBinding* normal_binding = new SoNormalBinding;
    SoNormal* normal = new SoNormal;

    if ( p.normals ) {
	normal_binding->value = SoNormalBinding::PER_VERTEX_INDEXED;
	normal->vector.setNum( p.n_points );
    }
    root->addChild(normal_binding);
    root->addChild(normal);


    SoCoordinate3* coord = new SoCoordinate3;
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



SoNode* process_quadmesh( const quadmesh_struct& q )
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


SoNode* process_text( const text_struct& t )
{
    cerr << "Skipping text object." << endl;
    return NULL;
}



void process_file( char* filename, SoGroup* root ) 
{
    File_formats format;
    int num_objects;
    object_struct** object_list;

    cerr << "Reading " << filename;
    if ( input_graphics_file( filename, &format, &num_objects, &object_list ) != OK ) {
	cerr << "Cannot read input file: " << filename << endl;
	return;
    }
    cerr << " done." << endl;

    for ( int i = 0; i < num_objects; ++i ) {
	SoNode* obj = NULL;

	//cerr << "Processing object " << i << endl;

	switch( (object_list[i])->object_type ) {
	case LINES: 
	    obj = process_lines( (object_list[i])->specific.lines );
	    break;
	case MARKER: 
	    obj = process_marker( (object_list[i])->specific.marker );
	    break;
	case MODEL: 
	    obj = process_model( (object_list[i])->specific.model ); 
	    break;
	case PIXELS: 
	    obj = process_pixels( (object_list[i])->specific.pixels ); 
	    break;
	case POLYGONS: 
	    obj = process_polygons( (object_list[i])->specific.polygons ); 
	    break;
	case QUADMESH: 
	    obj = process_quadmesh( (object_list[i])->specific.quadmesh ); 
	    break;
	case TEXT: 
	    obj = process_text( (object_list[i])->specific.text ); 
	    break;
//      case N_OBJECT_TYPES: dump_nobj( (object_list[i])->specific.??? ); break;
	default: 
	    cerr << "Unknown" << endl;
	}

	if ( obj ) {
	    root->addChild(obj);
	}
    }
}
  


void usage( char* argv0 )
{
    char* s = strrchr( argv0, '/' );
    if ( s ) argv0 = s+1;

    cerr << "usage: " << argv0 << " [options] file ..." << endl;
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
    SoSeparator* root = new SoSeparator;
    root->ref();

    // Process BIC obj file, creating scene nodes.
    process_file( av[1], root );

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


