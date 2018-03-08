#include "include/Angel.h"
#include <iostream>

using namespace std;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 0.0, 0.0, 0.0, 1.0 )   // black
};

const vec3 HEAD_SIZE = vec3(5, 5, 5);
const vec3 BODY_SIZE = vec3(5, 10, 3.5);
const vec3 FOOT_SIZE = vec3(5, 4, 3);
const color4 BACKGROUND_COLOR = vec4(0.5, 0.5, 0.5, 1);

enum Part {
    head,
    body,
    foot,
};

void quad( int a, int b, int c, int d ) {
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

// generate a cube
void colorcube() {
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

void my_init( void ) {
    colorcube();
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArraysAPPLE( 1, &vao );
    glBindVertexArrayAPPLE( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );
    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

    uniModel = glGetUniformLocation( program, "uniModel" );
    uniView = glGetUniformLocation(program, "uniView");
    uniProjection = glGetUniformLocation( program, "uniProjection" );

    glEnable( GL_DEPTH );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], BACKGROUND_COLOR[3]); 
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int current_target_x, int y ) {

    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
	// Incrase the joint angle
    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) {
	// Decrase the joint angle
    }

}

void onSpecialKeyPressed(int key, int current_target_x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:
            Theta[Axis] += THETADELTA;
            if ( Theta[Axis] > 360.0 ) { Theta[Axis] -= 360.0; }
            break;
        case GLUT_KEY_RIGHT:
            Theta[Axis] -= THETADELTA;
            if ( Theta[Axis] < 0.0 ) { Theta[Axis] += 360.0; }
            break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void menu( int option ) {
    if ( option == QUIT ) {
        exit( EXIT_SUCCESS );
    } else if(option == SWITCH_VIEW) {
        VIEW_MODE = 1 - VIEW_MODE;
        glutPostRedisplay();
    } else {
        Axis = option;
    }
}

//----------------------------------------------------------------------------

void reshape( int width, int height ) {
    glViewport( 0, 0, width, height );

    GLfloat  left = -10.0, right = 10.0;
    GLfloat  bottom = -5.0, top = 15.0;
    GLfloat  zNear = -10.0, zFar = 10.0;

    GLfloat aspect = GLfloat(width) / height;

    if ( aspect > 1.0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }

    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( uniProjection, 1, GL_TRUE, projection );

    model = mat4( 1.0 );  // An Identity matrix
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int current_target_x, int y ) {
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}

void display() {

}

int main( int argc, char **argv ) {
    glutInit( &argc, argv );
    // parse argv
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "creeper" );

    glewExperimental = GL_TRUE; 
    glewInit();
    my_init();

    glutDisplayFunc( display );
    // triggered when window is reshaped
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc(onSpecialKeyPressed);
    glutMouseFunc( mouse );

    glutCreateMenu( menu );

    glutMainLoop();
    return 0;
}
