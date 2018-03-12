#include "include/Angel.h"
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

mat4 model, view, projection;
GLuint uniModel, uniProjection, uniView;


vector<vec3> creeper_vertices;
vector<vec2> creeper_uvs;
vector<vec3> creeper_normals;

const vec4 BACKGROUND_COLOR = vec4(0.5, 0.5, 0.5, 1);

// int index = 0;
enum ViewMode {
    FRONT = 0,
    TOP = 1,
    SIDE = 2,
    NUM_VIEWS = 3
};

char* TEXTURE_FILEPATH = "src/creeper_texture.bmp";
char* CREEPER_OBJ_FILEPATH = "src/creeper.obj";

int view_mode = 0;

void deleteBMPData(unsigned char* data);
unsigned char* loadBMPData(const char *imagepath, unsigned int& width, unsigned int& height);
bool loadOBJ( const char * path, 
	vector<vec3>& out_vertices, 
	vector<vec2>& out_uvs, 
	vector<vec3>& out_normals
);


void my_init( void ) {
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a vbo object
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    
    // load obj file
    loadOBJ(CREEPER_OBJ_FILEPATH, creeper_vertices, creeper_uvs, creeper_normals);

    // buffer data
    glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * creeper_vertices.size() + sizeof(vec2) * creeper_uvs.size(),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vec3) * creeper_vertices.size(), &creeper_vertices[0] );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec3) * creeper_vertices.size(), sizeof(vec2) * creeper_uvs.size(), &creeper_uvs[0]);
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "texturevshader.glsl", "texturefshader.glsl" );
    glUseProgram( program );
    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vTexture = glGetAttribLocation( program, "vTexPosition" );
    glEnableVertexAttribArray( vTexture );
    glVertexAttribPointer( vTexture, 2, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(vec3) * creeper_vertices.size()));

    uniModel = glGetUniformLocation( program, "uniModel" );
    uniView = glGetUniformLocation(program, "uniView");
    uniProjection = glGetUniformLocation( program, "uniProjection" );

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);


    unsigned width, height;
    unsigned char *data = loadBMPData(TEXTURE_FILEPATH, width, height);
    if(data == NULL) {
        cerr << "Error Loading BMP file!\n";
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] data;
 
    glEnable( GL_DEPTH_TEST );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], BACKGROUND_COLOR[3]); 
}

void reshape( int width, int height ) {
    glViewport( 0, 0, width, height );

    GLfloat  left = -10.0, right = 10.0;
    GLfloat  bottom = -10.0, top = 10.0;
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
    glUniformMatrix4fv(uniModel, 1, GL_TRUE, model);
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

void display() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if(view_mode == TOP) {
        view = LookAt(vec4(-1, -.3, 1, 1), vec4(0, 0, 0, 1), vec4(0, 1, 0, 0));
    } else if(view_mode == FRONT) {
        view = LookAt(vec4(-1, 0.1, 1, 1), vec4(0, 0, 0, 1), vec4(0, 1, 0, 0));
    } else {
        view = LookAt(vec4(1, 0, 1, 1), vec4(0, 0, 0, 1), vec4(1, 0, 0, 0));
    }
    glUniformMatrix4fv(uniView, 1, GL_TRUE, view); 
    // use identity matrix as model
    mat4 model = Scale(2);
    glUniformMatrix4fv(uniModel, 1, GL_TRUE, model);
    glDrawArrays(GL_TRIANGLES, 0, creeper_vertices.size());

    glutSwapBuffers();
}

int main( int argc, char **argv ) {
    glutInit( &argc, argv );
    // parse argv
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow( "creeper" );

    glewExperimental = GL_TRUE; 
    glewInit();
    my_init();

    glutDisplayFunc( display );
    // triggered when window is reshaped
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}

void deleteBMPData(unsigned char* data) {
    delete[] data;
}

unsigned char* loadBMPData(const char *imagepath, unsigned int& width, unsigned int& height) {
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;
    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file){printf("Image could not be opened\n"); return 0;}
    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return NULL;
    }
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
            return NULL;
    }
    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way
    // Create a vbo
    data = new unsigned char [imageSize];

    // Read the actual data from the file into the vbo
    fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);
    return data;
}

bool loadOBJ(
	const char * path, 
	vector<vec3> & out_vertices, 
	vector<vec2> & out_uvs,
	vector<vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices; 
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			// uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		vec3 vertex = temp_vertices[ vertexIndex-1 ];
		vec2 uv = temp_uvs[ uvIndex-1 ];
		vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}
	fclose(file);
	return true;
}