#include <iostream.h>
#include <fstream.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>

// shape nodes

// property nodes
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTransform.h>

// group nodes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

// Manipulator nodes
#include <Inventor/draggers/SoTranslate1Dragger.h>



extern "C" {
#  include "ParseArgv.h"
#  include "bicpl.h"
}

#include "bicInventor.h"

// GUI toolkit
//#include <Inventor/Xt/SoXt.h>
//#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qslider.h>


/**
 * Read two BIC polygonal meshes and morph between them.
 *
 * Build scene graph with structure:
 *
 *                 group
 *                   |
 *     -------------------------
 *     |         |             |
 *   mesh1     mesh2   ...   meshN
 *
 * Where each mesh subtree is the result of bic_polygons_to_iv().
 *
 **/

polygons_struct* first_mesh = 0;


void usage( char* argv0 )
{
    char* s = strrchr( argv0, '/' );
    if ( s ) argv0 = s+1;

    cerr << "usage: " << argv0 << " [options] init final" << endl;
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
    else if (!is_isomorphic( *first_mesh, mesh )) {
	cerr << "Not isomorphic!  Goodbye." << endl;
	exit(1);
    }

    return bic_polygons_to_iv( mesh );
}
  

static inline void
do_interpolation( const SbVec3f* v_0, const SbVec3f* v_1,
		  SbVec3f* v_t, float t, int n )
{
    for( int i = 0; i < n; ++i ) {
	v_t[i] = v_0[i] + t * (v_1[i] - v_0[i]);
    }
}


static inline void
do_interpolation( SoCoordinate3* coord_0, 
		  SoCoordinate3* coord_1, 
		  SoCoordinate3* coord_t,
		  SoNormal* normal_0,
		  SoNormal* normal_1,
		  SoNormal* normal_t,
		  float t )
{
    if ( t < 0 ) t = 0;
    if ( t > 1 ) t = 1;

    SbVec3f* coord = coord_t->point.startEditing();
    SbVec3f* normal = normal_t->vector.startEditing();

    do_interpolation( coord_0->point.getValues(0), 
		      coord_1->point.getValues(0),
		      coord, t, coord_0->point.getNum() );
    do_interpolation( normal_0->vector.getValues(0),
		      normal_1->vector.getValues(0),
		      normal, t, normal_0->vector.getNum() );

    normal_t->vector.finishEditing();
    coord_t->point.finishEditing();
}


static SoCoordinate3* coordinate0;
static SoCoordinate3* coordinate1;
static SoCoordinate3* coordinate_int;

static SoNormal* normal0;
static SoNormal* normal1;
static SoNormal* normal_int;


////////////////////////////////////////////////////////////
// Dragger callback function.

static void
interpolate_mesh( void* data, SoDragger* dragger )
{
    float dragger_translation = ((SoTranslate1Dragger *) 
				dragger)->translation.getValue()[0];
    do_interpolation( coordinate0, coordinate1, coordinate_int,
		      normal0, normal1, normal_int, dragger_translation );
}



////////////////////////////////////////////////////////////
// Generate dragger attached to interpolate_mesh()

/** This code is no longer used. **/

SoSeparator *
make_dragger()
{
    SoSeparator *result = new SoSeparator();

#if 0
    SoComplexity *complexity = new SoComplexity();
    complexity->value = 0.8;
    result->addChild(complexity);
#endif

    SoTransform* xform = new SoTransform();
    xform->translation.setValue(0,-100,0);
    xform->scaleFactor.setValue(50,50,50);
    result->addChild(xform);

    SoTranslate1Dragger *dragger = new SoTranslate1Dragger();
    result->addChild( dragger );
    dragger->addMotionCallback( interpolate_mesh );
    dragger->translation.setValue(0,0,0);

    return result;

    SoInput dragInput;
    dragInput.openFile("draggerShape.iv");
    SoSeparator* dragSep = SoDB::readAll(&dragInput);
    dragger->setPart("translator", dragSep);

    SoInput dragActiveInput;
    dragActiveInput.openFile("draggerActiveShape.iv");
    SoSeparator* dragActiveSep = SoDB::readAll(&dragActiveInput);
    dragger->setPart("translatorActive", dragActiveSep);

    return  result;
}


class mySlider : public QSlider
{
public:
    mySlider( Orientation o, QWidget* parent, const char* name = 0 )
	: QSlider( o, parent, name ) 
    {}

    virtual void setValue( int v ) 
    {
	QSlider::setValue(v);
	do_interpolation( coordinate0, coordinate1, coordinate_int,
			  normal0, normal1, normal_int, 
			  float(v)/100.0 );
    }
};




////////////////////////////////////////////////////////////
//
// main


int
main(int ac, char **av)
{
    if ( ac != 3 ) {
	usage( av[0] );
	return 1;
    }

    // Construct the Qt gunk
    QApplication app( ac, av );
    QVBox mainwin;
    QWidget viewwin( &mainwin );
    mySlider slider( QSlider::Horizontal, &mainwin );
    slider.setRange( 0,100 );
    app.setMainWidget( &mainwin );

    // Build the inventor database
    SoQt::init( &mainwin );

    SoSeparator* mesh_0 = read_polygons(av[1]);
    mesh_0->ref();
    coordinate0 = (SoCoordinate3*) SoNode::getByName( SbName("Vertices") );
    normal0 = (SoNormal*) SoNode::getByName( "Normals" );

    SoSeparator* mesh_1 = read_polygons(av[2]);
    mesh_1->ref();
    coordinate1 = (SoCoordinate3*) SoNode::getByName( SbName("Vertices") );
    normal1 = (SoNormal*) SoNode::getByName( "Normals" );

    SoSeparator* root = new SoSeparator;
    root->ref();

    root->addChild( mesh_0->copy() );
    coordinate_int = (SoCoordinate3*) SoNode::getByName( SbName("Vertices") );
    normal_int = (SoNormal*) SoNode::getByName( "Normals" );

    //root->addChild( make_dragger() );

    // Create the viewer and show it
    SoQtExaminerViewer viewer( &viewwin );
    viewer.setSceneGraph( root );
    viewer.setTitle("Morph Window");
    viewer.show();

    SoQt::show( &mainwin );
    SoQt::mainLoop();
    return 0;
}
