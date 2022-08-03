//  ========================================================================
//  COSC422: Assignment 1
//	Name: Gongzai Li
//  FILE NAME: TerrainPatches.cpp
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  This program requires the following files:
//         TerrainPatches.vert, TerrainPatches.frag
//         TerrainPatches.cont, TerrainPatches.eval
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadTGA.h" 
using namespace std;


//---start - Defines Value-----
#define TEXTURES_NUM 4

//---end - Defines Value-------



GLuint vaoID;
GLuint theProgram;
GLuint mvpMatrixLoc, eyeLoc;
float toRad = 3.14159265/180.0;     //Conversion from degrees to rad

float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];     //Element array for 9x9 = 81 quad patches

GLuint texID[TEXTURES_NUM]; // init textures number

//----------start - file path----------
string heightMapsPath = "src\\height_maps\\";
string texturesPath = "src\\textures\\";
string shadersPath = "src\\shaders\\";
//----------end - file path----------

//----------start - any postion value----------
float eye_x = 0, eye_y = 20, eye_z = 30;      //Initial camera position/Eye postion
float look_x = 0, look_y = 0, look_z = -60;    //"Look-at" point along -z direction
float angle = 0;	//Rotation angle which is degree
float rotation_angle = 0.1; //Rotation speed
float move_speed = 1; // move speed
//----------end - any postion value----------

//----------start - textures and hight map----------
const char* terrain_maps[] = {"", "MtCook.tga", "MtRuapehu.tga"}; // Initial hight map
int terain_model_id = 1; // Initial hight map id default MtCook.tga

//----------end - textures and hight map----------



glm::mat4 projView;

//Generate vertex and element data for the terrain floor
void generateData()
{
	int indx, start;

	//verts array (100 vertices on a 10x10 grid)
	for (int j = 0; j < 10; j++)   //z-direction
	{
		for (int i = 0; i < 10; i++)  //x-direction
		{
			indx = 10 * j + i;
			verts[3 * indx] = 10 * i - 45;		//x
			verts[3 * indx + 1] = 0;			//y
			verts[3 * indx + 2] = -10 * j;		//z
		}
	}

	//elems array
	indx = 0;
	for (int j = 0; j < 9; j++)
	{
		for (int i = 0; i < 9; i++)
		{
			start = 10 * j + i;
			elems[indx] = start;
			elems[indx + 1] = start + 1;
			elems[indx + 2] = start + 11;
			elems[indx + 3] = start + 10;
			indx += 4;
		}
	}
}

// Load high map
void loadHighMap()
{

	glBindTexture(GL_TEXTURE_2D, texID[0]); // hight map
	loadTGA(heightMapsPath + terrain_maps[terain_model_id]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Load texture
void loadSurfacesTexture(string texture_name, int texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texID[texture_id]); // hight map
	loadTGA(texturesPath + texture_name);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//Loads height map
void loadTexture()
{
	

    glGenTextures(TEXTURES_NUM, texID);
    glActiveTexture(GL_TEXTURE0);

	loadHighMap();
	loadSurfacesTexture("Water.tga", 1);
	loadSurfacesTexture("Grass.tga", 2);
	loadSurfacesTexture("Snow.tga", 3);

}



//Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

//Initialise the shader program, create and load buffer data
void initialise()
{
//--------Load terrain height map-----------
	loadTexture();
//--------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, shadersPath + "TerrainPatches.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, shadersPath + "TerrainPatches.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, shadersPath + "TerrainPatches.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, shadersPath + "TerrainPatches.eval");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	eyeLoc = glGetUniformLocation(program, "eyePos");

	GLuint texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);

//---------Load buffer data-----------------------
	generateData();

	GLuint vboID[2];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{


	//--------Compute matrices----------------------
	glm::mat4 proj = glm::perspective(30.0f * toRad, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
	glm::mat4 view = lookAt(glm::vec3(eye_x, eye_y, eye_z), glm::vec3(look_x, look_y, look_z), glm::vec3(0.0, 1.0, 0.0)); //view matri
	projView = proj * view;  //Product matrix

	//----------Uniform to shaders----------
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &projView[0][0]);

	glm::vec3 cameraPosn = glm::vec3(eye_x, eye_y, eye_z);
	glUniform3fv(eyeLoc, 1, &cameraPosn[0]); // glm::value_ptr






	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	//glDrawElements(GL_QUADS, 81 * 4, GL_UNSIGNED_SHORT, NULL);
	glDrawElements(GL_PATCHES, 81 * 4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}


//To contorl camera position
void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		eye_x += sin(angle) * move_speed;
		eye_z -= cos(angle) * move_speed;
		break;
	case GLUT_KEY_DOWN:
		eye_x -= sin(angle) * move_speed;
		eye_z += cos(angle) * move_speed;
		break;
	case GLUT_KEY_LEFT:
		angle -= rotation_angle;
		break;
	case GLUT_KEY_RIGHT:
		angle += rotation_angle;
		break;
	default:
		break;
	}
	// update the look pos
	look_x = eye_x + 90 * sin(angle);
	look_z = eye_z - 90 * cos(angle);


	//cout << "eye_x " << eye_x << endl;
	//cout << "eye_y " << eye_x << endl;
	//cout << "eye_z " << eye_z << endl;
	//cout << "angle" << angle << endl;
	//cout << "look_x" << look_x << endl;
	//cout << "look_z" << look_z << endl;


	glutPostRedisplay();

}

// switch hight map
void switchHightMap(int id)
{
	if (terain_model_id == id) return;
	terain_model_id = id;
	// cout << "runner id " << terain_model_id << endl;
	loadHighMap();
	display();
}

// ASCII user interaction event
void keyboardEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		switchHightMap(1);
		break;
	case '2':
		switchHightMap(2);
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Terrain");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboardEvent);
	glutMainLoop();
	return 0;
}

