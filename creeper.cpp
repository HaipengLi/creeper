#include "include/Angel.h"
#include <iostream>

using namespace std;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

mat4 model, view, projection;
GLuint uniModel, uniProjection, uniView;

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
const vec3 BODY_SIZE = vec3(5, 7, 3.5);
const vec3 FOOT_SIZE = vec3(5, 3, 3);
const color4 BACKGROUND_COLOR = vec4(1.0, 1.0, 1.0, 1);

int index = 0;
enum ViewMode {
    FRONT = 0,
    TOP = 1,
    SIDE = 2,
    NUM_VIEWS = 3
};

int view_mode = 0;

enum Part {
    head,
    body,
    foot,
};

void quad( int a, int b, int c, int d ) {
    colors[index] = vertex_colors[a]; points[index] = vertices[a]; index++;
    colors[index] = vertex_colors[a]; points[index] = vertices[b]; index++;
    colors[index] = vertex_colors[a]; points[index] = vertices[c]; index++;
    colors[index] = vertex_colors[a]; points[index] = vertices[a]; index++;
    colors[index] = vertex_colors[a]; points[index] = vertices[c]; index++;
    colors[index] = vertex_colors[a]; points[index] = vertices[d]; index++;
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

void onSpecialKeyPressed(int key, int current_target_x, int y) {
    switch(key) {
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
    }
    glutPostRedisplay();
}

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

    projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( uniProjection, 1, GL_TRUE, projection );

    view = mat4(1);
    glUniformMatrix4fv(uniView, 1, GL_TRUE, view);

    model = mat4(1.0);  // An Identity matrix
}

void switch_view() {
    view_mode++;
    if(view_mode >= NUM_VIEWS) {
        view_mode = 0;
    }
}

void keyboard( unsigned char key, int current_target_x, int y ) {
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    case 'c': case 'C':
        switch_view();
        break;
    }

    glutPostRedisplay();
}

void draw_cube_instance(mat4 instance) {
    glUniformMatrix4fv(uniModel, 1, GL_TRUE, model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void display() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if(view_mode == TOP) {
        view = LookAt(vec4(1, 2, 5, 1), vec4(1, 0, 5, 1), vec4(0, 0, -1, 0));
    } else if(view_mode == FRONT) {
        view = mat4(1);
    } else {
        view = RotateY(90);
    }
    glUniformMatrix4fv(uniView, 1, GL_TRUE, view); 

    mat4 instance;

    // back foot
    instance = Scale(FOOT_SIZE);
    model = Translate(0, FOOT_SIZE[1] / 2, -(BODY_SIZE[2] / 2 + FOOT_SIZE[2] / 2));
    draw_cube_instance(instance);
    
    // front foot
    // move front (z)
    model = Translate(0, FOOT_SIZE[1] / 2, (BODY_SIZE[2] / 2 + FOOT_SIZE[2] / 2));
    draw_cube_instance(instance);

    // body, move up to the feet
    model = Translate(0, FOOT_SIZE[1] + BODY_SIZE[1] / 2, 0);
    instance = Scale(BODY_SIZE);
    draw_cube_instance(instance);
    
    model *= Translate(0, HEAD_SIZE[1] / 2 + BODY_SIZE[1] / 2, 0);
    instance = Scale(HEAD_SIZE);
    draw_cube_instance(instance);


    glutSwapBuffers();
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

    glutMainLoop();
    return 0;
}
