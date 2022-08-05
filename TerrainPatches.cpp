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

GLuint mvMatrixLoc, mvpMatrixLoc, norMatrixLoc, lgtLoc; //Initial model-view & model-view-projection & normal & light
GLuint eyeLoc;
float toRad = 3.14159265/180.0;     //Conversion from degrees to rad

float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];     //Element array for 9x9 = 81 quad patches
GLuint texID[TEXTURES_NUM]; // init textures number




//----------start - file path----------
string heightMapsPath = "./src/height_maps/";
string texturesPath = "./src/textures/";
string shadersPath = "./src/shaders/";

string testPath = "./src/test/";
//----------end - file path----------

//----------start - any postion value----------
float eye_x = 0, eye_y = 40, eye_z = 30;      //Initial camera position/Eye postion
float look_x = 0, look_y = 0, look_z = -60;    //"Look-at" point along -z direction
float angle = 0;	//Rotation angle which is degree
float rotation_angle = 0.1; //Rotation speed
float move_speed = 1; // move speed

// eye max and min
float eye_x_max = 180.0, eye_x_min = -180.0; 
float eye_z_max = 135.0, eye_z_min = -225.0;
//----------end - any postion value----------

//----------start - textures and hight map----------
const char* terrain_maps[] = {"", "MtCook.tga", "MtRuapehu.tga"}; // Initial hight map
int terain_model_id = 2; // Initial hight map id default MtCook.tga

GLuint waterHeightLoc;
GLuint snowHeightLoc;
float water_level = 2.0; // Todo update later
float snow_level = 5.0; // Todo update later
float update_level_speed = 0.01;
//----------end - textures and hight map----------

//----------start - some suppot variable--------
bool is_line_wireframe = false;
bool is_show_fog = false;
float fog_density = 0.5;
bool more_fog = true;
float update_fog_speed = 0.01;

GLuint showFogLoc;
GLuint fogDensityLoc;

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

void loadHeightMap() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID[0]); // hight map
	loadTGA(heightMapsPath + terrain_maps[terain_model_id]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


//Loads height map
void loadTexture()
{
	
    glGenTextures(TEXTURES_NUM, texID);
    
	
	loadHeightMap();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texID[1]); // hight map
	loadTGA(texturesPath + "Water.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texID[2]); // hight map
	loadTGA(texturesPath + "Grass.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texID[3]); // hight map
	loadTGA(texturesPath + "Snow.tga");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


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
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, shadersPath + "TerrainPatches.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, shadersPath + "TerrainPatches.eval");
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, shadersPath + "TerrainPatches.geom");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, shadersPath + "TerrainPatches.frag"); // testPath    shadersPath

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);
	glAttachShader(program, shaderg);

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

	//----------uniform location varible setter---------------
	eyeLoc = glGetUniformLocation(program, "eyePos");
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");
	lgtLoc = glGetUniformLocation(program, "lgtPos");
	showFogLoc = glGetUniformLocation(program, "showFog");
	fogDensityLoc = glGetUniformLocation(program, "fogDensity");



	//----------uniform texture varible setter---------------
	GLuint heightMapLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(heightMapLoc, 0); //Assign a value 0 to the variable texLoc to specify that it should use the texture from unit 0.

	// texture setter
	GLuint waterLoc = glGetUniformLocation(program, "waterSimple");
	glUniform1i(waterLoc, 1);

	GLuint grassLoc = glGetUniformLocation(program, "grassSimple");
	glUniform1i(grassLoc, 2);

	GLuint snowLoc = glGetUniformLocation(program, "snowSimple");
	glUniform1i(snowLoc, 3);

	waterHeightLoc = glGetUniformLocation(program, "waterHeight");
	snowHeightLoc = glGetUniformLocation(program, "snowHeight");



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
	glShadeModel(GL_SMOOTH); // todo 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{


	//--------Compute matrices----------------------
	glm::mat4 proj = glm::perspective(30.0f * toRad, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
	glm::mat4 view = lookAt(glm::vec3(eye_x, eye_y, eye_z), glm::vec3(look_x, look_y, look_z), glm::vec3(0.0, 1.0, 0.0)); //view matri
	projView = proj * view;  //Product matrix

	glm::vec4 light_pos = glm::vec4(-50.0, 10.0, 60.0, 1.0); // todo need to update West 
	

	//----------light Uniform setter----------
	glm::mat4 mvMatrix = view;
	glm::mat4 mvpMatrix = projView;
	glm::mat4 invMatrix = glm::inverse(view);
	glm::vec4 lightEye = view * light_pos;     //Light position in eye coordinates

	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);
	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);


	glUniform4fv(lgtLoc, 1, &lightEye[0]);
	
	
	//---------camera Uniform stter------------------------------
	glm::vec3 cameraPosn = glm::vec3(eye_x, eye_y, eye_z);
	glUniform3fv(eyeLoc, 1, &cameraPosn[0]); // glm::value_ptr

	//--------------water & snow level---------------------
	glUniform1f(waterHeightLoc, water_level);
	glUniform1f(snowHeightLoc, snow_level);
	
	glUniform1i(showFogLoc, is_show_fog);

	glUniform1f(fogDensityLoc, fog_density);

	//--------------Fog-----------------------------------
	

	// settings
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	//glDrawElements(GL_QUADS, 81 * 4, GL_UNSIGNED_SHORT, NULL);
	glDrawElements(GL_PATCHES, 81 * 4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

void checkScreenBoundary()
{
	if (eye_x >= eye_x_max) eye_x = eye_x_max;
	if (eye_x <= eye_x_min) eye_x = eye_x_min;
	if (eye_z >= eye_z_max) eye_z = eye_z_max;
	if (eye_z <= eye_z_min) eye_z = eye_z_min;
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

	checkScreenBoundary();
	look_x = eye_x + 90 * sin(angle);
	look_z = eye_z - 90 * cos(angle);


	cout << "eye_x " << eye_x << endl;
	cout << "eye_y " << eye_y << endl;
	cout << "eye_z " << eye_z << endl;
	//cout << "angle" << angle << endl;
	//cout << "look_x" << look_x << endl;
	//cout << "look_z" << look_z << endl;

	//glm::vec3 cameraPosn = glm::vec3(eye_x, eye_y, eye_z);
	//glUniform3fv(eyeLoc, 1, &cameraPosn[0]); // glm::value_ptr

	glutPostRedisplay();

}

// switch hight map
void switchHightMap(int id)
{
	if (terain_model_id == id) return;
	terain_model_id = id;
	// cout << "runner id " << terain_model_id << endl;
	loadHeightMap();

}

void upWaterLevel(float step)
{
	if (water_level >= snow_level) {
		water_level = snow_level;
	}
	else {
		water_level += step;
	}
}

void downWaterLevel(float step)
{
	if (water_level <= 0.1) {
		water_level = 0.1;
	}
	else {
		water_level -= step;
	}
}


void upSonwLevel(float step)
{
	if (snow_level >= 10) {
		snow_level = 10;
	}
	else {
		snow_level += step;
	}
}

void downSonwLevel(float step)
{
	if (snow_level <= water_level) {
		snow_level = water_level;
	}
	else {
		snow_level -= step;
	}
}

void switchWireframe()
{
	if (is_line_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	is_line_wireframe = !is_line_wireframe;
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
	case 'q':
		upWaterLevel(update_level_speed);
		break;
	case 'a':
		downWaterLevel(update_level_speed);
		break;
	case 'w':
		upSonwLevel(update_level_speed);
		break;
	case 's':
		downSonwLevel(update_level_speed);
		break;
	case ' ':
		switchWireframe();
		break;
	case 'f':
		is_show_fog = !is_show_fog;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void fogDensityChange(int value)
{
	if (is_show_fog) {
		if (more_fog) {
			fog_density += update_fog_speed;
		}
		else {
			fog_density -= update_fog_speed;
		}

		if (fog_density <= 0.0) {
			more_fog = true;
		}

		if (fog_density >= 1.0) {
			more_fog = false;
		}
	}
	glutTimerFunc(100, fogDensityChange, 0);
	glutPostRedisplay();
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
	glutTimerFunc(100, fogDensityChange, 0);
	glutMainLoop();
	return 0;
}

