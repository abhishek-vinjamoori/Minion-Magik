#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ao/ao.h>
#include <mpg123.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

class VAO {
	public:
		GLuint VertexArrayID;
		GLuint VertexBuffer;
		GLuint ColorBuffer;

		GLenum PrimitiveMode;
		GLenum FillMode;
		int NumVertices;
		float cx;
		float cy;
		float cz;
		float cradius;
		float normmax;
		float min;
		float xvel;
		float yvel;
		float acc;
		int life;
		int xind;
		int yind;
		VAO()
		{

		}
};


struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
	GLuint TexMatrixID;
} Matrices;

#define GLFW_ARROW_CURSOR   0x00036001

#define GLFW_CROSSHAIR_CURSOR   0x00036003

#define GLFW_HAND_CURSOR   0x00036004

#define GLFW_HRESIZE_CURSOR   0x00036005

#define GLFW_IBEAM_CURSOR   0x00036002

#define GLFW_VRESIZE_CURSOR   0x00036006

#define BITS 8


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[1;31m"
#define KGRN  "\x1B[1;32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[1;34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

//Function definitions

/*--------------------------------------Start here ---------------------------*/
void JumpSound();
void LifeLost();
void CollectCoin();
void ArtificialIntelligence(int);
void switchtotowerview(void);
void switchtotopview(void);
void switchtofirstpersonview(void);
void switchtoplayerbackview(void);
void switchtotopfollowview(void);
void switchtostandardview(void);
void switchtohelicopterview(void);
void zoomin();
void zoomout();
GLuint Texture_load(const char *);
void Texture_bind(GLuint, int);
bool CheckBorders(int keynum);
void CheckEnemy();
void precalculatepositions();
void MakeLevel(int);
void CheckBlock(void);
void MoveOn();
int GameDone();
void MoveUp();
void MoveRight();
void MoveLeft();
void MoveDown();
void EnemyMoveUp(int);
void EnemyMoveRight(int);
void EnemyMoveLeft(int);
void EnemyMoveDown(int);
void KeepMoving();
void EnemyKeepMoving(int);
void Reset();
void EnemyReset(int);
void NewGame();
void drawlives();
void drawrear(GLFWwindow*);
void drawgameover(GLFWwindow*);
/* ------------------------------------End Here -------------------------------*/







//Global Variable definitons
/*-------------------------------- Start here ------------------------*/
float eyex = 0,eyey=0,eyez=20,targetx=0,targety=0,targetz=0,tiltx=0,tilty=1,tiltz=0;
float playerangle=0;
GLuint sky,minion,programID,backgroundid,character,minionid,platformtex,platformid, enemytid,enemyl,coinid,cointex,gameoverid,gametex ;
GLuint RedPlatform,GreenPlatform,BluePlatform,BrownPlatform;
double last_time, curr_time;
int enablemove = 0;
int enablejump = 0;
float Jumpspace = 0.25;
int globalfps = 0;
float hrangex=0,hrangey=0;
int camerastate=0; //0 - Top View //1-Tower.Follow view
int fbwidth=1920, fbheight=1080;
int starx  = -10.0,stary = -10.0,starz =0;
float scalebox=2.0,scalecube=0.5;
float charpositions[20][20][2]={0};
int maxlife=5;
VAO *backgroundimage[6], *Player[5], *platform[200][200][200],*Enemy[5],*rectangle,*coins[50];
float MoveD=0.048,JumpD=0.05,ChangeMove=2.2;
float MoveE=0.048,JumpE=0.05;
float borders[][2]={
	{4,-7.5},
	{4,6.45},
	{-15,20},
	{-15,-20}
};

int panning_state = 0;
float paninitx,paninity;

float helicopter[2][3]={
	{-10,-10 ,20},
	{-10,-10 ,0}
};
int holes[100][2];
int currentpos[2]={0,0};
int enemymovestate=4;
int enemypos[2]={5,5};
float coinspositions[][2]={{0,0}};
int movestate=0;
float lifepositions[10];
/*
   MoveState=1 ----> UP
   MoveState=2 ----> LEFT
   MoveState=3 ----> DOWN
   MoveState=4 ----> RIGHT
 */


bool Endcurrentgame = false;
int Timelapsebetween = 0;
int CharacterBlink = 0;
bool Illegal = false;
int currentlevel=1;
int MaxBlocks=10;
glm::mat4 OriginalPerspective = glm::perspective(90.0f, 1920 /1080.0f, 0.1f, 500.0f); 
double mousex,mousey;
double prevmousex,prevmousey,curx,cury;
float screenleft = 0,screentop=0,screenright=1920,screenbottom = 1080;


//Sound trials 

float deltax = 1.5,deltay = 1.5;


pid_t Spid;
pid_t Jumpid,Lifeid,Coinsound;
int Gamestate=1;

int score=0;
int CoinMax = 15;

/* ----------------------End Here -------------------------------------*/

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
	kill(Spid,SIGKILL);
	kill(Jumpid,SIGKILL);
	kill(Lifeid,SIGKILL);
	kill(Coinsound,SIGKILL);
    

	system("clear");
    printf(KCYN"Game Over\n"RESET);
	printf(KBLU"Final Score - %d\n"RESET,score);
	printf(KGRN"Thank you for Playing Minion Magik\n"RESET);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, float cx,float cy,float cz,float cradius,GLenum fill_mode=GL_FILL)
{
	VAO* vao = new VAO();
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;
	vao->cx=cx;
	vao->cy=cy;
	vao->cz=cz;
	vao->cradius=cradius;
	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, float cx, float cy, float cz,float cradius,GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data,cx,cy,cz,cradius, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
GLfloat *CarVertexData,*CarColorData,*CubeVertices,*CubeColor;




/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.

	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_W:
				eyez+=0.5;
				break;
			case GLFW_KEY_S:
				MoveD/=2;
				JumpD/=2;
				break;
			case GLFW_KEY_F:
				MoveD*=2;
				JumpD*=2;
				break;				
			case GLFW_KEY_A:
				eyex-=0.5;
				break;
			case GLFW_KEY_D:
				eyex+=0.5;
				break;
			case GLFW_KEY_E:
				eyey+=0.5;
				break;
			case GLFW_KEY_R:
				eyey-=0.5;
				break;																
			default:
				break;
		}
	}
	else if (action == GLFW_REPEAT || action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_1:
				camerastate = 0;
				switchtotowerview();
				break;
			case GLFW_KEY_2:
				camerastate = 1;
				switchtotopfollowview();
				break;
			case GLFW_KEY_3:
				camerastate = 2;
				switchtotopview();
				break;
			case GLFW_KEY_4:
				camerastate = 3;
				switchtofirstpersonview();
				break;
			case GLFW_KEY_5:
				camerastate = 4;
				switchtoplayerbackview();
				break;		
			case GLFW_KEY_6:
				camerastate = 5;
				switchtostandardview();
				break;				
			case GLFW_KEY_7:
				camerastate = 6;
				switchtohelicopterview();
				break;		
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			case GLFW_KEY_UP:
				//MoveUp();
				movestate = 1;
				/*if(enablemove)
				  {
				  Player->yvel = MoveD;
				  Player->xvel = 0;
				  playerangle = 180;
				  currentpos[0]++ ;
				  if(CheckBorders(1))
				  {
				  Player->cy -= MoveD;
				  }

				  }
				  last_time = glfwGetTime();
				  hrangey=Player->cy;
				  enablemove = 0;*/
				break;
			case GLFW_KEY_LEFT:
				//MoveLeft();
				movestate = 2;
				/*				if(enablemove)
								{
								Player->xvel = -MoveD;
								Player->yvel = 0;
								playerangle = -90;
								currentpos[1]--;
								if(CheckBorders(2))
								{
								Player->cx += MoveD;
								}
								}
								hrangex=Player->cx;
								enablemove = 0;
								last_time = glfwGetTime();*/
				break;
			case GLFW_KEY_RIGHT:
				//MoveRight();
				movestate = 4;
				/*				if(enablemove)
								{
								Player->xvel = MoveD;
								Player->yvel = 0;
								playerangle = 90;
								currentpos[1]++;
								if(CheckBorders(4))
								{
								Player->cx -= MoveD;
								}
								}	
								enablemove = 0;
								last_time = glfwGetTime();
								hrangex=Player->cx;		*/	
				break;
			case GLFW_KEY_DOWN:
				//	MoveDown();
				movestate = 3;
				/*				if(enablemove)
								{
								Player->yvel = -MoveD;
								Player->xvel = 0;
								playerangle = 0;
								if(CheckBorders(3))
								{
								Player->cy += MoveD;
								}
								}		
								last_time = glfwGetTime();
								hrangey=Player->cy;
								enablemove = 0;	*/	
				break;
			case GLFW_KEY_SPACE:
				if(enablejump)
				{
					if(Player[0]->cz-Player[0]->min<=(Jumpspace+0.25) && Player[0]->cz-Player[0]->min>=Jumpspace-0.1)
					{
						//cout << Player[0]->cz - Player[0]->min << "Special" << endl;
						Player[0]->cz += JumpD;
						MoveD*=ChangeMove;
						KeepMoving();
					}
				}
				enablejump = 0 ;
				break;		
			default:
				break;
		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
			quit(window);
			break;
		default:
			break;
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cout << yoffset << endl;
	if(yoffset>0)
	{
		zoomout();
	}
	else
	{
		zoomin();
	}
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{

}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	/*curx = ((screenright - screenleft)/1920.0f)*xpos + screenleft;
	  cury = ((screenbottom - screentop)/1080.0f)*ypos + screentop;
	  if(panning_state == 1){
	  if(prevmousex - curx < 0 && screenleft >= -600 + fabs(prevmousex - curx)){
	  helicopter[0][0] -= fabs(prevmousex - curx);
	//screenright -= fabs(prevmousex - curx);
	}
	if(prevmousex - curx > 0 && screenright <= 600 - fabs(prevmousex -curx)){
	helicopter[0][0] += fabs(prevmousex -curx);
	screenright += fabs(prevmousex -curx);
	}
	if(prevmousey - cury < 0  && screentop >= -300 + fabs(prevmousey - cury)){
	helicopter[0][1] -= fabs(prevmousey - cury);
	screenbottom -= fabs(prevmousey - cury);
	}
	if(prevmousey - cury > 0 && screenbottom <= 300 - fabs(prevmousey -cury)){
	helicopter[0][1] += fabs(prevmousey - cury);
	screenbottom += fabs(prevmousey -cury);
	}
	}*/
	mousex = xpos;
	mousey = ypos;
	if(camerastate==6)
	{
		if(panning_state==1)
		{
			if(fabs(xpos-prevmousex)>fabs(ypos-prevmousey))
			{
				helicopter[0][0] += 0.001*(xpos-prevmousex);
				helicopter[1][0] += 0.001*(xpos-prevmousex);
			}
			else if(fabs(xpos-prevmousex)<fabs(ypos-prevmousey))
			{
				helicopter[0][1] += 0.001*(ypos-prevmousey);
				helicopter[1][1] += 0.001*(ypos-prevmousey);
			}
		}
		else if(panning_state==2)
		{
			if(fabs(xpos-prevmousex)>fabs(ypos-prevmousey))
			{
				helicopter[1][0] += 0.001*(xpos-prevmousex);
			}
			else if(fabs(xpos-prevmousex)<fabs(ypos-prevmousey))
			{
				helicopter[1][1] += 0.001*(ypos-prevmousey);
			}				    		
		}
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_PRESS)
			{
				panning_state = 1;
				prevmousex = mousex;
				prevmousey = mousey;
			}
			else
			{
				panning_state = 0;
				//helicopter[0][0] += 0.01*(mousex - prevmousex);
				//helicopter[0][1] +=  0.01*(mousey - prevmousey);
				/*printf("%lf %lf\n",mousex - prevmousex, mousey - prevmousey );*/
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_PRESS) {
				panning_state = 2;
				prevmousex = mousex;
				prevmousey = mousey;
			}
			else
			{
				panning_state = 0;
			}
			break;
		default:
			break;
	}
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	   is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 30.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	// Ortho projection for 2D views
	//Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}


// Creates the triangle object used in this sample code
void createTriangle ()
{
	/* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

	/* Define vertex array as used in glBegin (GL_TRIANGLES) */
	static const GLfloat vertex_buffer_data [] = {
		0, 1,1, // vertex 0
		-1,-1,1, // vertex 1
		1,-1,1, // vertex 2
	};
	int cnt=0;
	fstream myfile("./Data/MiV", ios_base::in);
	float a;
	while (myfile >> a)
	{
		if(cnt==0)
		{
			CarVertexData = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CarVertexData[cnt-1]=a;

		cnt++;
	}
	cnt=0;
	fstream colorfile("./Data/MiC", ios_base::in);
	while (colorfile >> a)
	{
		if(cnt==0)
		{
			CarColorData = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CarColorData[cnt-1]=a;

		cnt++;
	}	
	/*for(int vount=0;vount<cnt;vount++)
	  {
	  cout << CarVertexData[vount];
	  }*/

	static const GLfloat color_buffer_data [] = {
		1,1,1, // color 0
		1,1,1, // color 1
		1,1,1, // color 2
	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	Player[0] = create3DObject(GL_TRIANGLES, (cnt-1)/3, CarVertexData, CarColorData,starx,stary,starz,0.5,GL_FILL);
	//Initializing
	Player[0]->min=0.2;
	Player[0]->normmax = Player[0]->min+1;
	Player[0]->life = maxlife;
}



void createEnemies ()
{
	/* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

	/* Define vertex array as used in glBegin (GL_TRIANGLES) */
	static const GLfloat vertex_buffer_data [] = {
		0, 1,1, // vertex 0
		-1,-1,1, // vertex 1
		1,-1,1, // vertex 2
	};
	int cnt=0;
	fstream myfile("./Data/EnemyVertices1", ios_base::in);
	float a;
	while (myfile >> a)
	{
		if(cnt==0)
		{
			CarVertexData = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CarVertexData[cnt-1]=a;

		cnt++;
	}
	cnt=0;
	fstream colorfile("./Data/EnemyColors1", ios_base::in);
	while (colorfile >> a)
	{
		if(cnt==0)
		{
			CarColorData = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CarColorData[cnt-1]=a;

		cnt++;
	}	
	/*for(int vount=0;vount<cnt;vount++)
	  {
	  cout << CarVertexData[vount];
	  }*/

	static const GLfloat color_buffer_data [] = {
		1,1,1, // color 0
		1,1,1, // color 1
		1,1,1, // color 2
	};

	for(int ki=0;ki<5;ki++)
	{
		// create3DObject creates and returns a handle to a VAO that can be used later
		Enemy[ki] = create3DObject(GL_TRIANGLES, (cnt-1)/3, CarVertexData, CarColorData,starx+10,stary+10,starz+1,0.5,GL_FILL);
		//Initializing
		Enemy[ki]->min=1.2;
		Enemy[ki]->normmax = Enemy[ki]->min+1;
		Enemy[ki]->life = rand()%4+1;
		Enemy[ki]->xind = rand()%10;
		Enemy[ki]->yind = rand()%10;
		Enemy[ki]->acc = JumpE;
		Enemy[ki]->cradius = 0;
	}
}
// Creates the rectangle object used in this sample code
void createRectangle ()
{
	static const GLfloat color_buffer_data [] = {
		0,1,0, // color 1
		1,1,0, // color 2
		1,0,0, // color 3

		1,0,0, // color 3
		0,0,0, // color 4
		0,1,0  // color 1
	};
	const GLfloat vertex_buffer_data [] = {
		-1.2,-1,0, // vertex 1
		1.2,-1,0, // vertex 2
		1.2, 1,0, // vertex 3

		1.2, 1,0, // vertex 3
		-1.2, 1,0, // vertex 4
		-1.2,-1,0  // vertex 1
	};
	// GL3 accepts only Triangles. Quads are not supported
	GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};


	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};
	int cnt=0;
	fstream myfile("./Data/FinalCubeVertices", ios_base::in);
	float a;
	while (myfile >> a)
	{
		if(cnt==0)
		{
			CubeVertices = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CubeVertices[cnt-1]=a;

		cnt++;
	}
	cnt=0;
	fstream colorfile("./Data/FinalCubecolors", ios_base::in);
	while (colorfile >> a)
	{
		if(cnt==0)
		{
			CubeColor = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CubeColor[cnt-1]=a;

		cnt++;
	}
	// create3DObject creates and returns a handle to a VAO that can be used later
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			for(int k=0;k<10;k++)
			{
				platform[i][j][k] = create3DObject(GL_TRIANGLES, (cnt-1)/3, CubeVertices, CubeColor, 0,0,0,0.75,GL_FILL);
				platform[i][j][k]->life = -1;
			}
		}
	}
	rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, 0,0,0,1.5,GL_FILL);
	free(CubeVertices);
	free(CubeColor);
}







void createcoins ()
{
	
	int cnt=0;
	fstream myfile("./Data/CoinVertices", ios_base::in);
	float a;
	while (myfile >> a)
	{
		if(cnt==0)
		{
			CubeVertices = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CubeVertices[cnt-1]=a;

		cnt++;
	}
	cnt=0;
	fstream colorfile("./Data/CoinColors", ios_base::in);
	while (colorfile >> a)
	{
		if(cnt==0)
		{
			CubeColor = (GLfloat*)malloc(sizeof(GLfloat)*(int)a);
			cnt++;
			continue;
		}
		CubeColor[cnt-1]=a;

		cnt++;
	}
	// create3DObject creates and returns a handle to a VAO that can be used later
		for(int k=0;k<CoinMax;k++)
		{
			coins[k] = create3DObject(GL_TRIANGLES, (cnt-1)/3, CubeVertices, CubeColor, 0,0,2,0.75,GL_FILL);
			coins[k]->life = 1;
			coins[k]->acc = 0;
		}

	free(CubeVertices);
	free(CubeColor);
}



int skyposy = 250, skyposx = 300, skyposz = 300;
void createbackgroundimage()
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data[] = {
		-skyposx, skyposy, -skyposz,
		-skyposx, -skyposy, -skyposz,
		skyposx, skyposy, -skyposz,

		-skyposx, -skyposy, -skyposz,
		skyposx, skyposy, -skyposz,
		skyposx, -skyposy, -skyposz
	};
	static const GLfloat vertex_buffer_data1[] = {
		-skyposx, skyposy, skyposz,
		-skyposx, skyposy, -skyposz,
		skyposx, skyposy, skyposz,

		-skyposx, skyposy, -skyposz,
		skyposx, skyposy, skyposz,
		skyposx, skyposy, -skyposz
	};
	static const GLfloat vertex_buffer_data2[] = {
		-skyposx, skyposy, skyposz,
		-skyposx, -skyposy, skyposz,
		-skyposx, skyposy, -skyposz,

		-skyposx, -skyposy, skyposz,
		-skyposx, skyposy, -skyposz,
		-skyposx, -skyposy, -skyposz
	};
	static const GLfloat vertex_buffer_data3[] = {
		skyposx, skyposy, -skyposz,
		skyposx, -skyposy, -skyposz,
		skyposx, skyposy, skyposz,

		skyposx, -skyposy, -skyposz,
		skyposx, skyposy, skyposz,
		skyposx, -skyposy, skyposz
	};
	static const GLfloat vertex_buffer_data4[] = {
		skyposx, skyposy, skyposz,
		skyposx, -skyposy, skyposz,
		-skyposx, skyposy, skyposz,

		skyposx, -skyposy, skyposz,
		-skyposx, skyposy, skyposz,
		-skyposx, -skyposy, skyposz
	};
	skyposy = 150;
	static const GLfloat vertex_buffer_data5[] = {
		-skyposx, -skyposy, -skyposz,
		-skyposx, -skyposy, skyposz,
		skyposx, -skyposy, -skyposz,

		-skyposx, -skyposy, skyposz,
		skyposx, -skyposy, -skyposz,
		skyposx, -skyposy, skyposz
	};

	static const GLfloat texture_buffer_data[] = {
		0,0,0,
		0,1,0,
		1,0,0,

		0,1,0,
		1,0,0,
		1,1,0
	};
	backgroundimage[0] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, texture_buffer_data, 0,0,0,0, GL_FILL);
	backgroundimage[1] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data1, texture_buffer_data, 0,0,0,0, GL_FILL);
	backgroundimage[2] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data2, texture_buffer_data, 0,0,0,0, GL_FILL);
	backgroundimage[3] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data3, texture_buffer_data, 0,0,0,0, GL_FILL);
	backgroundimage[4] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data4, texture_buffer_data, 0,0,0,0, GL_FILL);
	backgroundimage[5] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data5, texture_buffer_data, 0,0,0,0, GL_FILL);
}









/*void createbackground()
{
	// GL3 accepts only Triangles. Quads are not supported
	GLfloat g_vertex_buffer_data[] = {
		-1.2,-1,0, // vertex 1
		1.2,-1,0, // vertex 2
		1.2, 1,0, // vertex 3

		1.2, 1,0, // vertex 3
		-1.2, 1,0, // vertex 4
		-1.2,-1,0  // vertex 1
	};


	static const GLfloat g_color_buffer_data[] = {
		0,1,0, // color 1
		1,1,0, // color 2
		1,0,0, // color 3

		1,0,0, // color 3
		0,0,0, // color 4
		0,1,0  // color 1
	};


	backgroundimage = create3DObject(GL_TRIANGLES, 6, g_vertex_buffer_data, g_color_buffer_data, 0,0,0,1,GL_FILL);

}
*/










float camera_rotation_angle = 45;
float rectangle_rotation = 0;
float triangle_rotation = 0;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (GLFWwindow* window)
{
	/*cout << Player[0]->cx << Player[0]->cy  << endl;*/
	// clear the color and depth in the frame buffer

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	// Don't change unless you know what you are doing

	// Ortho projection for 2D views

	//cout << "Movestate " << movestate << endl;



	switch(camerastate)
	{
		case 0:
			switchtotowerview();
			Matrices.projection = OriginalPerspective;
			break;
		case 1:
			switchtotopfollowview();
			Matrices.projection = OriginalPerspective;
			//Matrices.projection = glm::ortho(-10.667f, 10.667f, -6.0f, 6.0f, 0.1f, 500.0f);
			break;
		case 2:
			switchtotopview();
			//Matrices.projection = glm::ortho(-10.667f, 10.667f, -6.0f, 6.0f, 0.1f, 500.0f);

			Matrices.projection = OriginalPerspective;
			break;
		case 3:
			switchtofirstpersonview();
			Matrices.projection = OriginalPerspective;
			break;
		case 4:
			switchtoplayerbackview();
			Matrices.projection = OriginalPerspective;
			break;	
		case 5:
			switchtostandardview();
			Matrices.projection = OriginalPerspective;
			break;
		case 6:
			switchtohelicopterview();
			Matrices.projection = OriginalPerspective;
			break;
	}
	// Eye - Location of camera. Don't change unless you are sure!!
	/*glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );*/
	glm::vec3 eye ( eyex, eyey, eyez );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (targetx, targety, targetz);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (tiltx, tilty, tiltz);

	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	//  Don't change unless you are sure!!
	//Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	glm::mat4 MVP;	// MVP = Projection * View * Model



	glUseProgram(programID);
	
	VP = Matrices.projection * Matrices.view;
	
	for(int ii=0;ii<6;ii++)
	{

		glUseProgram(backgroundid);
		Texture_bind(sky,0);
		 
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translatebg = glm::translate (glm::vec3(0, 100, 15));        // glTranslatef
		glm::mat4 scalebg = glm::scale(glm::vec3(1.0f, 1.0f,1.0f));
		glm::mat4 rotatebg = glm::rotate(glm::radians(90.0f),glm::vec3(1,0,0));
		Matrices.model *= (scalebg);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// draw3DObject draws the VAO given to it using current MVP matrix
		draw3DObject(backgroundimage[ii]);

	}
	// Load identity to model matrix




	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	//Matrices.projection = glm::perspective(90.0f, 1920 /1080.0f, 0.1f, 500.0f);
	
	VP = Matrices.projection * Matrices.view;



	glUseProgram (minionid);
	Texture_bind(minion,0);
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateTriangle = glm::translate (glm::vec3(Player[0]->cx, Player[0]->cy, Player[0]->cz)); // glTranslatef
	glm::mat4 rotateCar = glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
	glm::mat4 normalrot = glm::rotate(glm::radians(playerangle),glm::vec3(0,1,0));
	glm::mat4 scaleTraingle = glm::scale(glm::vec3(0.375,0.375,0.375));
	glm::mat4 triangleTransform = translateTriangle  * rotateCar * normalrot * scaleTraingle;
	Matrices.model *= triangleTransform; 
	MVP = VP * Matrices.model; // MVP = p * V * M

	//  Don't change unless you are sure!!
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);




	Player[0]->cx += Player[0]->xvel;
	Player[0]->cy += Player[0]->yvel;
	if(!Illegal)
	{
		Player[0]->cz += JumpD;
	}
	/*	else
		{
		switch(movestate)
		{
		case 1:
		Player->cy+=0.5;
		break;
		case 2:
		Player->cx -= 0.5;
		break;
		case 3:
		Player->cy-=0.5;
		break;
		case 4:
		Player->cx += 0.5;
		break;
		}
		}*/
	/*	if(Player->cx-hrangex>2&&Player->cx-hrangex>0)
		{
		Player->cx=hrangex+2.0;
		}
		else if(Player->cx-hrangex<-2&&Player->cx-hrangex<0)
		{
		Player->cx=hrangex-2.0;
		}
		if(Player->cy-hrangey>2&&Player->cy-hrangey>0)
		{
		Player->cy=hrangey+2.0;
		}
		else if(Player->cy-hrangey<-2&&Player->cy-hrangey<0)
		{
		Player->cy=hrangey-2.0;
		}*/
	if(Player[0]->cz-Player[0]->min<=Jumpspace && Player[0]->cz-Player[0]->min>0)
	{
		//cout << Player->cz - Player->min << endl;

		enablejump=1;
	}

	

	if(Player[0]->cz<Player[0]->min)
	{
		if(MoveD>0.050)
		{
			MoveD/=ChangeMove;
		}

		//printf("Range = %f\t%f\n",Player[0]->cx ,Player[0]->cy);
		CheckBlock();
		/*		printf("Illegal:%d\n",Illegal );*/
		if(Illegal)
		{
			if(!Endcurrentgame)
			{
				Player[0]->life-=1;
			}
			Endcurrentgame = true;
			if(Gamestate)
			LifeLost();
			Gamestate = 0;
		}
		else
		{
			Player[0]->cz = Player[0]->min;
		}		

		hrangex=Player[0]->cx;
		hrangey=Player[0]->cy;
		enablemove=1;
		curr_time = glfwGetTime();
		//printf("Time Period:%f\n",curr_time -last_time);
		last_time = curr_time;


		printf("%d %d\n",currentpos[0],currentpos[1]);
		CheckEnemy();
		if(!GameDone() && !Endcurrentgame)
		{
			if(platform[currentpos[0]][currentpos[1]][0]->life == 1)
			{	
				platform[currentpos[0]][currentpos[1]][0]->life = 0;
				score+=10;
			}
			else if (platform[currentpos[0]][currentpos[1]][0]->life==2)
			{
				platform[currentpos[0]][currentpos[1]][0]->life = 1;
				score+=20;
			}
			else if (platform[currentpos[0]][currentpos[1]][0]->life==3)
			{
				platform[currentpos[0]][currentpos[1]][0]->life = 4;
				score+=30;
			}
			MoveOn();
			JumpSound();

		}
		else if(GameDone())
		{
			Endcurrentgame = true;
		}
		if(Player[0]->life<=0)
		{
			int x=250;
			while(x--)
			{
				drawgameover(window);
				glfwSwapBuffers(window);
			}	
			quit(window);
		}



	}
	else if(Player[0]->cz>=Player[0]->normmax)
	{
		Player[0]->cz=Player[0]->normmax;
		JumpD*=-1;
	}

	if(Endcurrentgame)
	{
		if(Illegal)
		{
			Player[0]->cz -= 0.3;
		}
		Player[0]->xvel=0;
		Player[0]->yvel=0;
		Timelapsebetween+=1;
		if(Timelapsebetween>120)
		{
			NewGame();
		}
	}
	// draw3DObject draws the VAO given to it using current MVP matrix

	/*	if(CharacterBlink>120)
		{
		globalfps = 0;
		}
		else
		{
		CharacterBlink++;
		}*/
	/*CheckBlock();
	  if(Illegal)
	  {
	  printf("%d :%d\n",currentpos[0],currentpos[1]);
	  }
	  else
	  {
	  if()
	  {

	  }
	  printf("Else %d :%d\n",currentpos[0],currentpos[1]);
	  }*/

	draw3DObject(Player[0]);



	/* Render your scene */
	glUseProgram (enemytid);
	Texture_bind(enemyl,0);
	for(int pl=0;pl<4;pl++)
	{
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translateTriangle = glm::translate (glm::vec3(Enemy[pl]->cx,Enemy[pl]->cy,Enemy[pl]->cz)); // glTranslatef
		glm::mat4 rotateCar = glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
		glm::mat4 normalrot = glm::rotate(glm::radians(playerangle),glm::vec3(0,1,0));
		glm::mat4 scaleTraingle = glm::scale(glm::vec3(0.3,0.3,0.3));
		glm::mat4 triangleTransform = translateTriangle  * rotateCar * scaleTraingle;
		Matrices.model *= triangleTransform; 
		MVP = VP * Matrices.model; // MVP = p * V * M


		if(Enemy[pl]->cradius>180)
		{
			Enemy[pl]->life=rand()%4+1;
			Enemy[pl]->cradius = 0;
		}
		else
		{
			Enemy[pl]->cradius++;
		}

		Enemy[pl]->cx += Enemy[pl]->xvel;
		Enemy[pl]->cy += Enemy[pl]->yvel;
		Enemy[pl]->cz += Enemy[pl]->acc;


		ArtificialIntelligence(pl);
		if(Enemy[pl]->cz<=Enemy[pl]->min)
		{
			Enemy[pl]->cz = Enemy[pl]->min;

			EnemyReset(pl);
			EnemyKeepMoving(pl);
			Enemy[pl]->acc*=-1;

		}
		else if(Enemy[pl]->cz>=Enemy[pl]->normmax)
		{	

			Enemy[pl]->cz=Enemy[pl]->normmax;
			Enemy[pl]->acc*=-1;

		}








		//  Don't change unless you are sure!!
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(Enemy[0]);
	}



	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	glUseProgram (programID);

	starx = -10;
	stary = -10.0;
	starz=0;
	int holnum = 0;


	/*
	   if(globalfps>120)
	   {
	   for(holnum=0;holnum<5;holnum++)
	   {
	   holes[holnum][0] = rand()%10;
	   holes[holnum][1] = rand()%10;
	   }
	   globalfps = 0;
	   }
	   else
	   {
	   globalfps++;
	   }*/
	for (int shiftx = 0; shiftx < MaxBlocks; shiftx++)
	{

		for(int shifty = 0; shifty<MaxBlocks;shifty++)
		{


			for(int shiftz=0;shiftz<1;shiftz++)
			{


				if(platform[shiftx][shifty][shiftz]->cz<=-20)
				{
					platform[shiftx][shifty][shiftz]->life = -1;
				}

				if(platform[shiftx][shifty][shiftz]->life == 4)
				{

					platform[shiftx][shifty][shiftz]->cz -= 0.5;
				}


				if(platform[shiftx][shifty][shiftz]->life==-1)
				{
					//printf("Illegal - %d %d\n",shiftx,shifty);
					continue;
				}

				glUseProgram (platformid);
				
				glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);
				glUniform1i(glGetUniformLocation(platformid, "ttemp[2]"), 0);

				GLint myUniformLocation = glGetUniformLocation(platformid, "playerPosition");
				glUniform3f(myUniformLocation,Player[0]->cx, Player[0]->cy, Player[0]->cz);

				if(platform[shiftx][shifty][shiftz]->life == 0)	
				{
					//if(currentlevel==2)
					//printf("Red - %d %d\n",shiftx,shifty);
					Texture_bind(RedPlatform,0);
				}
				else if(platform[shiftx][shifty][shiftz]->life == 1)
				{

					Texture_bind(GreenPlatform,0);
				}
				else if(platform[shiftx][shifty][shiftz]->life == 2)
				{
					Texture_bind(BluePlatform,0);
				}
				else if(platform[shiftx][shifty][shiftz]->life == 3)
				{
					Texture_bind(BrownPlatform,0);
				}
				Matrices.model = glm::mat4(1.0f);
				float incrr=0;
				glm::mat4 translateRectangle = glm::translate (glm::vec3(starx+scalebox*shifty+incrr, stary+scalebox*shiftx+incrr, starz+scalebox*shiftz+incrr+platform[shiftx][shifty][shiftz]->cz));        // glTranslatef
				glm::mat4 rotateRectangle = glm::rotate(glm::radians(-90.0f), glm::vec3(0,1,0)); // rotate about vector (-1,1,1)
				glm::mat4 scaleRectangle = glm::scale(glm::vec3(scalecube, scalecube,scalecube));

				Matrices.model *= (translateRectangle * rotateRectangle * scaleRectangle);
				MVP = VP * Matrices.model;
				glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

				// draw3DObject draws the VAO given to it using current MVP matrix
				draw3DObject(platform[0][0][0]);		
			}
		}
	}



	glUseProgram(coinid);
	Texture_bind( cointex ,0);
	 
	for(int cnum=0;cnum<CoinMax;cnum++)
	{
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translatebg = glm::translate (glm::vec3(coins[cnum]->cx, coins[cnum]->cy, coins[cnum]->cz));        // glTranslatef
		glm::mat4 scalebg = glm::scale(glm::vec3(0.4f, 0.4f,0.4f));
		glm::mat4 rotatebg = glm::rotate(glm::radians(90.0f),glm::vec3(1,0,0));
		glm::mat4 rotatecoin = glm::rotate(glm::radians(coins[cnum]->acc),glm::vec3(0,0,1));
		Matrices.model *= (translatebg * rotatecoin * scalebg);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// draw3DObject draws the VAO given to it using current MVP matrix
		if(coins[cnum]->life)
		draw3DObject(coins[cnum]);
		coins[cnum]->acc+=1;
	}















	// Increment angles
	float increments = 1;

	//camera_rotation_angle++; // Simulating camera rotation
	triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
	rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Minion Magik", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	   is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	unsigned char pixels[16 * 16 * 4];
	memset(pixels, 0xff, sizeof(pixels));
	GLFWimage image;
	image.width = 16;
	image.height = 16;
	image.pixels = pixels;
	//GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

	glfwSetCursor(window, cursor);
	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);



	/*glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	 */
	return window;


}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	/* Objects should be created before any other gl function and shaders */
	// Create the models
	createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
	createRectangle ();
	//createbackground();
	createbackgroundimage();
	createEnemies();
	createcoins();
	precalculatepositions();
	MakeLevel(currentlevel);
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	backgroundid = LoadShaders( "Sample_GL.vert", "Min.frag" );
	sky = Texture_load("./Images/sky.png");


	gameoverid = LoadShaders( "Sample_GL.vert", "Min.frag" );
	gametex = Texture_load("./Images/GameOver.png");

	platformid = LoadShaders("Light.vert", "Back_GL.frag");
	BrownPlatform = Texture_load("./Images/glossywood.jpg");
	RedPlatform = Texture_load("./Images/glossyred.jpg");
	BluePlatform = Texture_load("./Images/glossyblue.jpg");
	GreenPlatform = Texture_load("./Images/glossygreen.jpg");

	Matrices.TexMatrixID = glGetUniformLocation(platformid, "MVP");


	minionid = LoadShaders("Sample_GL.vert", "Min.frag");
	minion = Texture_load("./Images/miniontest.png");

	enemytid = LoadShaders("Sample_GL.vert", "Min.frag");
	enemyl = Texture_load("./Images/body1.jpg");

	coinid = LoadShaders("Sample_GL.vert", "Min.frag");
	cointex = Texture_load("./Images/cointexture.jpg");

	/*platformtex = Texture_load("./gradient.jpg");*/

	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (135/255.0f, 206/255.0f, 250/255.0f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);


	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 1920;
	int height = 1080;

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

	double last_update_time = glfwGetTime(), current_time;


	//Initialisations for music/sounds


	Spid=fork();
	if(Spid==0)
	{
		mpg123_handle *mh;
		unsigned char *buffer;
		size_t buffer_size;
		size_t done;
		int err;
		int driver;
		ao_device *dev;

		ao_sample_format format;
		int channels, encoding;
		long rate;


		while(1)
		{
			ao_initialize();
			driver = ao_default_driver_id();
			mpg123_init();
			mh = mpg123_new(NULL, &err);
			buffer_size = mpg123_outblock(mh);
			buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

			mpg123_open(mh, "./Sounds/Background.mp3");
			mpg123_getformat(mh, &rate, &channels, &encoding);
			format.bits = mpg123_encsize(encoding) * BITS;
			format.rate = rate;
			format.channels = channels;
			format.byte_format = AO_FMT_NATIVE;
			format.matrix = 0;
			dev = ao_open_live(driver, &format, NULL);

			while (mpg123_read(mh, buffer,buffer_size, &done) == MPG123_OK)
			{
				ao_play(dev, (char*)buffer, done);

			}
			free(buffer);
			ao_close(dev);
			mpg123_close(mh);
			mpg123_delete(mh);
			mpg123_exit();
			ao_shutdown();


		}
		_exit(0);

	}

	/* Draw in loop */
	while (!glfwWindowShouldClose(window)) {
		fbwidth=width, fbheight=height;
		glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
		// OpenGL Draw commands
		draw(window);
		glViewport (0, fbheight-fbheight/3, (GLsizei) (440), (GLsizei) fbheight/4);
		//Matrices.projection = glm::perspective (90.0f, (GLfloat) (440) / (GLfloat) (fbheight/4), 0.1f, 500.0f);
		drawrear(window);
		//glViewport (0, fbheight-fbheight/3, (GLsizei) (440), (GLsizei) fbheight/4);
		//drawlives();
		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);


		// Poll for Keyboard and mouse events
		glfwPollEvents();

		// Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			last_update_time = current_time;
		}
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
void switchtotopfollowview()
{
	eyex = Player[0]->cx;
	eyey = Player[0]->cy;
	eyez = Player[0]->min+8;
	tilty=1;
	tiltx=0;
	tiltz=0;
	targetx = Player[0]->cx;
	targety = Player[0]->cy;
	targetz = 0;

}
void switchtotopview()
{
	eyex = 0;
	eyey = 0;
	eyez = 20;
	tilty=1;
	tiltx=0;
	tiltz=0;
	targetx = 0;
	targety = 0;
	targetz = Player[0]->min;

}
void switchtotowerview()
{

	//10,20,20
	float tower[]={0,-20,20};
	eyex = tower[0];
	eyey = tower[1];
	eyez = tower[2];
	tiltz=1;
	tilty=0;
	tiltx=0;
	targetx = Player[0]->cx;
	targety = Player[0]->cy;
	targetz = Player[0]->cz;
}
void switchtofirstpersonview()
{
	eyex = Player[0]->cx;
	eyey = Player[0]->cy;
	eyez = Player[0]->cz+1;
	tiltz = 1;
	tilty = 0;
	tiltx = 0;
	targetx = eyex+5;
	targety = eyey+5;
	targetz = Player[0]->cz;	
}
void switchtoplayerbackview()
{
	eyex = Player[0]->cx-2;
	eyey = Player[0]->cy-2;
	eyez = Player[0]->min+2;
	tiltz=1;
	tilty=0;
	tiltx=0;
	targetx = eyex+5;
	targety = eyey+5;
	targetz = eyez-2;	
}


void switchtostandardview()
{
	eyex = Player[0]->cx-4;
	eyey = Player[0]->cy-2;
	eyez = 7;
	tiltz=1;
	tilty=0;
	tiltx=0;
	targetx = Player[0]->cx;
	targety = Player[0]->cy;
	targetz = Player[0]->min;
}
void switchtohelicopterview()
{
	eyex = helicopter[0][0];
	eyey = helicopter[0][1];
	eyez = helicopter[0][2];
	tiltz=0;
	tilty=1;
	tiltx=0;
	targetx = helicopter[1][0];
	targety = helicopter[1][1];
	targetz = helicopter[1][2];
}





bool CheckBorders(int keynum)
{
	return false;

	switch(keynum)
	{
		case 1:
			if(Player[0]->cy>borders[2][1])
			{
				return true;
			}
			return false;
		case 2:
			if(Player[0]->cx<borders[2][0])
			{
				return true;
			}
			return false;		
		case 3:
			if(Player[0]->cy<borders[3][1])
			{
				return true;
			}
			return false;
		case 4:
			if(Player[0]->cx>borders[0][0])
			{
				return true;
			}
			return false;
	}
}









GLuint Texture_load(const char *texName){
	GLuint toret;
	int width,height,numComponents;
	unsigned char *imgData=stbi_load(texName,&width,&height,&numComponents,STBI_rgb);
	if(imgData==NULL){
		fprintf(stderr,"Failed to load %s texture file\n",texName);
		exit(1);
	}
	glGenTextures(1,&toret);
	glBindTexture(GL_TEXTURE_2D,toret);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imgData);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	stbi_image_free(imgData);
	return toret;
}

void Texture_bind(GLuint texture,int unit)
{
	if(unit<0||unit>31){
		fprintf(stderr,"unit=%d is out of range\n",unit);
		exit(1);
	}
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D,texture);
}


void zoomin()
{
	glm::mat4 scaleworld = glm::scale(glm::vec3(1.02f, 1.02f,1));
	OriginalPerspective *= scaleworld;
}

void zoomout()
{
	glm::mat4 scaleworld = glm::scale(glm::vec3(1.02f, 1.02f,1));
	OriginalPerspective /= scaleworld;
}
void precalculatepositions()
{
	float tx = starx;
	float ty = stary;
	for(int i=0;i<MaxBlocks;i++)
	{
		for(int j=0;j<MaxBlocks;j++)
		{
			charpositions[i][j][0] = tx+scalebox*j;
			charpositions[i][j][1] = ty+scalebox*i;

		}
	}
	for(int tmmp=0;tmmp<3;tmmp++)
	{
		lifepositions[tmmp]=5.0;
	}
	/*	for(int i=0;i<10;i++)
		for(int j=0;j<10;j++)
		cout << charpositions[i][j][0] << charpositions[i][j][1] << " ";*/
}
void CheckBlock()
{
	if(currentpos[0]<0 || currentpos[1]<0)
	{
		Illegal = true;
		return;
	}

	for(int ki=0;ki<4;ki++)
	{
		if(Enemy[ki]->xind == currentpos[0] && Enemy[ki]->yind == currentpos[1])
		{

			Illegal = true;
			return;
		}
	}
	for (int i = 0; i < 4; i++)
	{

		if( fabs(Player[0]->cx-Enemy[i]->cx) <= deltax && fabs(Player[0]->cy-Enemy[i]->cy) <= deltay)
		{

			Illegal = true;
			return;

		}


	}




	for(int i=0;i<100;i++)
	{
		for(int j=0;j<100;j++)
		{
			if(platform[i][j][0]->life == -1)
			{
				if(currentpos[0]==i && currentpos[1]==j)
				{

					Illegal = true;
					return;
				}


			}
		}

	}

	Illegal = false;
	return;
	/*if(currentpos[0]>=MaxBlocks || currentpos[1]>=MaxBlocks || currentpos[0] < 0 || currentpos[1] < 0)
	  {
	  Illegal = true;
	  return;
	  }*/
	/*for(int i=0;i<10;i++)
	  {
	  for(int j=0;j<10;j++)
	  {
	  if(1)
	  {
	  return; 
	  }
	  }
	  }*/
}
void MoveUp()
{
	if(enablemove)
	{
		movestate = 1;
		Player[0]->yvel = MoveD;
		Player[0]->xvel = 0;
		playerangle = 180;
		currentpos[0]++;
		if(CheckBorders(1))
		{
			Player[0]->cy -= MoveD;
		}

	}
	last_time = glfwGetTime();
	hrangey=Player[0]->cy;
	enablemove = 0;	
}
void MoveLeft()
{
	if(enablemove)
	{
		movestate = 2;
		Player[0]->xvel = -MoveD;
		Player[0]->yvel = 0;
		playerangle = -90;
		currentpos[1]--;
		if(CheckBorders(2))
		{
			Player[0]->cx += MoveD;
		}
	}
	hrangex=Player[0]->cx;
	enablemove = 0;
	last_time = glfwGetTime();	
}
void MoveDown()
{
	if(enablemove)
	{
		movestate = 3;
		Player[0]->yvel = -MoveD;
		Player[0]->xvel = 0;
		playerangle = 0;
		currentpos[0]--;
		if(CheckBorders(3))
		{
			Player[0]->cy += MoveD;
		}
	}		
	last_time = glfwGetTime();
	hrangey=Player[0]->cy;
	enablemove = 0;	
}
void MoveRight()
{
	if(enablemove)
	{
		movestate = 4;
		Player[0]->xvel = MoveD;
		Player[0]->yvel = 0;
		playerangle = 90;
		currentpos[1]++;
		if(CheckBorders(4))
		{
			Player[0]->cx -= MoveD;
		}
	}	
	enablemove = 0;
	last_time = glfwGetTime();
	hrangex=Player[0]->cx;	
}
void KeepMoving()
{
	switch(movestate)
	{
		case 0:
			return;
		case 1:
			MoveUp();
			break;
		case 2:
			MoveLeft();
			break;
		case 3:
			MoveDown();
			break;
		case 4:
			MoveRight();
			break;

	}
}
void Reset()
{
	Player[0]->cx = charpositions[currentpos[0]][currentpos[1]][0];
	Player[0]->cy = charpositions[currentpos[0]][currentpos[1]][1];
}
void EnemyReset(int pl)
{
	Enemy[pl]->cx = charpositions[Enemy[pl]->xind][Enemy[pl]->yind][0];
	Enemy[pl]->cy = charpositions[Enemy[pl]->xind][Enemy[pl]->yind][1];
}
void NewGame()
{
	Player[0]->cx = charpositions[0][0][0];
	Player[0]->cy = charpositions[0][0][1];
	Player[0]->xvel = 0;
	Player[0]->yvel = 0;
	Endcurrentgame = false;
	Timelapsebetween = 0;
	Player[0]->cz = starz;
	currentpos[0]=currentpos[1]=0;
	movestate = 0;
	Illegal = false;
	if(GameDone())
	{
		currentlevel+=1;
		MakeLevel(currentlevel);
	}
	Gamestate = 1;

}
void MakeLevel(int level)
{
	int s; 
	FILE * fp;
	char str[50]; 
	char name[50]="./Levels/.dat";
	int xx=0;
	for(int pt=0;pt<=strlen(name);pt++)
	{
		if(pt==9)
		{
			str[xx++] = level+'0';
		}
		str[xx++]=name[pt];
	}
	fp = fopen (str, "r");
	//fputs("We are in 2012", fp);
	//while((s1=getchar())!=EOF)
	int i=0,j=0,k=0;
	int cnum=CoinMax-1;
	while(((s = fgetc(fp)) != EOF))
	{
		if(s == '\n')
		{
			i++;
			j=0;
			continue;
		}
		else if(s == 'X')
		{
			platform[i][j][k]->life=1;
			int random = rand()%4;
			if(cnum>=0 && random==3)
			{
				coins[cnum]->cx=charpositions[i][j][0];
				coins[cnum--]->cy=charpositions[i][j][1];
			}
		}
		else if(s == ' ')
		{
			platform[i][j][k]->life=-1;
		}
		else if(s == 'Y')
		{
			platform[i][j][k]->life = 2;
			int random = rand()%4;
			if(cnum>=0 && random==2)
			{
				coins[cnum]->cx=charpositions[i][j][0];
				coins[cnum--]->cy=charpositions[i][j][1];
			}
		}
		else if(s == 'Z')
		{
			platform[i][j][k]->life = 3;
		}
		else
		{
			printf("%d%d%d\n",i,j,k);
			platform[i][j][k]->life=0;
		}
		j++;
		//fscanf(fp, "%c",&s1);
		printf("%c",s);
	}
	fclose(fp);


	return;
}
void MoveOn()
{
	Reset();
	KeepMoving();
	enablemove=1;		
	JumpD*=-1;
}
int GameDone()
{
	for(int i=0;i<MaxBlocks;i++)
	{
		for(int j=0;j<MaxBlocks;j++)
		{
			if(platform[i][j][0]->life==1 || platform[i][j][0]->life==2 || platform[i][j][0]->life==3)
			{
				return 0;
			}
		}
	}
	return 1;
}

void drawrear(GLFWwindow* window)
{

	int	temp = 1;
	switch(temp)
	{
		case 0:
			switchtotowerview();
			Matrices.projection = OriginalPerspective;
			break;
		case 1:
			switchtotopfollowview();
			Matrices.projection = OriginalPerspective;
			//Matrices.projection = glm::ortho(-10.667f, 10.667f, -6.0f, 6.0f, 0.1f, 500.0f);
			break;
		case 2:
			switchtotopview();
			//Matrices.projection = glm::ortho(-10.667f, 10.667f, -6.0f, 6.0f, 0.1f, 500.0f);

			Matrices.projection = OriginalPerspective;
			break;
		case 3:
			switchtofirstpersonview();
			Matrices.projection = OriginalPerspective;
			break;
		case 4:
			switchtoplayerbackview();
			Matrices.projection = OriginalPerspective;
			break;	
		case 5:
			switchtostandardview();
			Matrices.projection = OriginalPerspective;
			break;
		case 6:
			switchtohelicopterview();
			Matrices.projection = OriginalPerspective;
			break;
	}
	// Eye - Location of camera. Don't change unless you are sure!!
	/*glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );*/
	glm::vec3 eye ( eyex, eyey, eyez );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (targetx, targety, targetz);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (tiltx, tilty, tiltz);

	// Compute Camera matrix (view)
	Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	//  Don't change unless you are sure!!
	//Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	glm::mat4 MVP;	// MVP = Projection * View * Model



	glUseProgram(programID);
	
	VP = Matrices.projection * Matrices.view;
	
	for(int ii=0;ii<6;ii++)
	{

		glUseProgram(backgroundid);
		Texture_bind(sky,0);
		 
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translatebg = glm::translate (glm::vec3(0, 100, 15));        // glTranslatef
		glm::mat4 scalebg = glm::scale(glm::vec3(1.0f, 1.0f,1.0f));
		glm::mat4 rotatebg = glm::rotate(glm::radians(90.0f),glm::vec3(1,0,0));
		Matrices.model *= (scalebg);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// draw3DObject draws the VAO given to it using current MVP matrix
		draw3DObject(backgroundimage[ii]);

	}
	// Load identity to model matrix




	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	//Matrices.projection = glm::perspective(90.0f, 1920 /1080.0f, 0.1f, 500.0f);
	
	VP = Matrices.projection * Matrices.view;



	glUseProgram (minionid);
	Texture_bind(minion,0);
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateTriangle = glm::translate (glm::vec3(Player[0]->cx, Player[0]->cy, Player[0]->cz)); // glTranslatef
	glm::mat4 rotateCar = glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
	glm::mat4 normalrot = glm::rotate(glm::radians(playerangle),glm::vec3(0,1,0));
	glm::mat4 scaleTraingle = glm::scale(glm::vec3(0.375,0.375,0.375));
	glm::mat4 triangleTransform = translateTriangle  * rotateCar * normalrot * scaleTraingle;
	Matrices.model *= triangleTransform; 
	MVP = VP * Matrices.model; // MVP = p * V * M

	//  Don't change unless you are sure!!
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);




	
	draw3DObject(Player[0]);



	/* Render your scene */
	glUseProgram (enemytid);
	Texture_bind(enemyl,0);
	for(int pl=0;pl<4;pl++)
	{
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translateTriangle = glm::translate (glm::vec3(Enemy[pl]->cx,Enemy[pl]->cy,Enemy[pl]->cz)); // glTranslatef
		glm::mat4 rotateCar = glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
		glm::mat4 normalrot = glm::rotate(glm::radians(playerangle),glm::vec3(0,1,0));
		glm::mat4 scaleTraingle = glm::scale(glm::vec3(0.3,0.3,0.3));
		glm::mat4 triangleTransform = translateTriangle  * rotateCar * scaleTraingle;
		Matrices.model *= triangleTransform; 
		MVP = VP * Matrices.model; // MVP = p * V * M

		//  Don't change unless you are sure!!
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(Enemy[0]);
	}



	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	glUseProgram (programID);

	starx = -10;
	stary = -10.0;
	starz=0;
	int holnum = 0;

	for (int shiftx = 0; shiftx < MaxBlocks; shiftx++)
	{

		for(int shifty = 0; shifty<MaxBlocks;shifty++)
		{


			for(int shiftz=0;shiftz<1;shiftz++)
			{


				if(platform[shiftx][shifty][shiftz]->cz<=-20)
				{
					platform[shiftx][shifty][shiftz]->life = -1;
				}

				if(platform[shiftx][shifty][shiftz]->life == 4)
				{

					platform[shiftx][shifty][shiftz]->cz -= 0.5;
				}


				if(platform[shiftx][shifty][shiftz]->life==-1)
				{
					//printf("Illegal - %d %d\n",shiftx,shifty);
					continue;
				}

				glUseProgram (platformid);
				
				glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);
				glUniform1i(glGetUniformLocation(platformid, "ttemp[2]"), 0);

				GLint myUniformLocation = glGetUniformLocation(platformid, "playerPosition");
				glUniform3f(myUniformLocation,Player[0]->cx, Player[0]->cy, Player[0]->cz);

				if(platform[shiftx][shifty][shiftz]->life == 0)	
				{
					//if(currentlevel==2)
					//printf("Red - %d %d\n",shiftx,shifty);
					Texture_bind(RedPlatform,0);
				}
				else if(platform[shiftx][shifty][shiftz]->life == 1)
				{

					Texture_bind(GreenPlatform,0);
				}
				else if(platform[shiftx][shifty][shiftz]->life == 2)
				{
					Texture_bind(BluePlatform,0);
				}
				else if(platform[shiftx][shifty][shiftz]->life == 3)
				{
					Texture_bind(BrownPlatform,0);
				}
				Matrices.model = glm::mat4(1.0f);
				float incrr=0;
				glm::mat4 translateRectangle = glm::translate (glm::vec3(starx+scalebox*shifty+incrr, stary+scalebox*shiftx+incrr, starz+scalebox*shiftz+incrr+platform[shiftx][shifty][shiftz]->cz));        // glTranslatef
				glm::mat4 rotateRectangle = glm::rotate(glm::radians(-90.0f), glm::vec3(0,1,0)); // rotate about vector (-1,1,1)
				glm::mat4 scaleRectangle = glm::scale(glm::vec3(scalecube, scalecube,scalecube));

				Matrices.model *= (translateRectangle * rotateRectangle * scaleRectangle);
				MVP = VP * Matrices.model;
				glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

				// draw3DObject draws the VAO given to it using current MVP matrix
				draw3DObject(platform[0][0][0]);		
			}
		}
	}



	glUseProgram(coinid);
	Texture_bind( cointex ,0);
	 
	for(int cnum=0;cnum<CoinMax;cnum++)
	{
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translatebg = glm::translate (glm::vec3(coins[cnum]->cx, coins[cnum]->cy, coins[cnum]->cz));        // glTranslatef
		glm::mat4 scalebg = glm::scale(glm::vec3(0.4f, 0.4f,0.4f));
		glm::mat4 rotatebg = glm::rotate(glm::radians(90.0f),glm::vec3(1,0,0));
		glm::mat4 rotatecoin = glm::rotate(glm::radians(coins[cnum]->acc),glm::vec3(0,0,1));
		Matrices.model *= (translatebg * rotatecoin * scalebg);
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// draw3DObject draws the VAO given to it using current MVP matrix
		if(coins[cnum]->life)
		draw3DObject(coins[cnum]);
		/*coins[cnum]->acc+=1;*/
	}


}

void drawlives()
{

	switchtotopview();
	glm::vec3 eye ( eyex, eyey, eyez );
	glm::vec3 target (targetx, targety, targetz);
	glm::vec3 up (tiltx, tilty, tiltz);

	Matrices.view = glm::lookAt( eye, target, up );




	glUseProgram (minionid);
	Texture_bind(minion,0);
	glm::mat4 VP = Matrices.projection*Matrices.view;
	glm::mat4 MVP;
	for(int pl=0;pl<Player[0]->life;pl++)
	{
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translateTriangle = glm::translate (glm::vec3(Player[0]->cx+8*pl-30,10,0)); // glTranslatef
		glm::mat4 rotateCar = glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
		glm::mat4 normalrot = glm::rotate(glm::radians(playerangle),glm::vec3(0,1,0));
		glm::mat4 scaleTraingle = glm::scale(glm::vec3(2,2,2));
		glm::mat4 triangleTransform = translateTriangle*scaleTraingle;
		Matrices.model *= triangleTransform; 
		MVP = VP * Matrices.model; // MVP = p * V * M

		//  Don't change unless you are sure!!
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(Player[0]);
	}


}

void JumpSound()
{
	Jumpid=fork();
	if(Jumpid==0)
	{
		mpg123_handle *mh;
		unsigned char *buffer;
		size_t buffer_size;
		size_t done;
		int err;
		int driver;
		ao_device *dev;

		ao_sample_format format;
		int channels, encoding;
		long rate;


		ao_initialize();
		driver = ao_default_driver_id();
		mpg123_init();
		mh = mpg123_new(NULL, &err);
		buffer_size = mpg123_outblock(mh);
		buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

		mpg123_open(mh, "./Sounds/Jump.mp3");
		mpg123_getformat(mh, &rate, &channels, &encoding);
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		dev = ao_open_live(driver, &format, NULL);

		while (mpg123_read(mh, buffer,buffer_size, &done) == MPG123_OK)
		{
			ao_play(dev, (char*)buffer, done);

		}
		free(buffer);
		ao_close(dev);
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();
		ao_shutdown();
		_exit(0);

	}


}
void LifeLost()
{
	Lifeid=fork();
	if(Lifeid==0)
	{
		mpg123_handle *mh;
		unsigned char *buffer;
		size_t buffer_size;
		size_t done;
		int err;
		int driver;
		ao_device *dev;

		ao_sample_format format;
		int channels, encoding;
		long rate;


		ao_initialize();
		driver = ao_default_driver_id();
		mpg123_init();
		mh = mpg123_new(NULL, &err);
		buffer_size = mpg123_outblock(mh);
		buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

		mpg123_open(mh, "./Sounds/life.mp3");
		mpg123_getformat(mh, &rate, &channels, &encoding);
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		dev = ao_open_live(driver, &format, NULL);

		while (mpg123_read(mh, buffer,buffer_size, &done) == MPG123_OK)
		{
			ao_play(dev, (char*)buffer, done);

		}
		free(buffer);
		ao_close(dev);
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();
		ao_shutdown();
		_exit(0);

	}


}



void CollectCoin()
{
	Coinsound=fork();
	if(Coinsound==0)
	{
		mpg123_handle *mh;
		unsigned char *buffer;
		size_t buffer_size;
		size_t done;
		int err;
		int driver;
		ao_device *dev;

		ao_sample_format format;
		int channels, encoding;
		long rate;


		ao_initialize();
		driver = ao_default_driver_id();
		mpg123_init();
		mh = mpg123_new(NULL, &err);
		buffer_size = mpg123_outblock(mh);
		buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

		mpg123_open(mh, "./Sounds/coin.mp3");
		mpg123_getformat(mh, &rate, &channels, &encoding);
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		dev = ao_open_live(driver, &format, NULL);

		while (mpg123_read(mh, buffer,buffer_size, &done) == MPG123_OK)
		{
			ao_play(dev, (char*)buffer, done);

		}
		free(buffer);
		ao_close(dev);
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();
		ao_shutdown();
		_exit(0);

	}


}






void EnemyMoveUp(int enemynum)
{

	Enemy[enemynum]->life = 1;
	Enemy[enemynum]->yvel = MoveE;
	Enemy[enemynum]->xvel = 0;
	//Enemyangle = 180;
	Enemy[enemynum]->xind++;


}
void EnemyMoveLeft(int enemynum)
{

	Enemy[enemynum]->life = 2;
	Enemy[enemynum]->xvel = -MoveE;
	Enemy[enemynum]->yvel = 0;
	//Enemyangle = -90;
	Enemy[enemynum]->yind--;


}
void EnemyMoveDown(int enemynum)
{
	Enemy[enemynum]->life = 3;
	Enemy[enemynum]->yvel = -MoveE;
	Enemy[enemynum]->xvel = 0;
	//Enemyangle = 0;
	Enemy[enemynum]->xind--;

}
void EnemyMoveRight(int enemynum)
{

	Enemy[enemynum]->life = 4;
	Enemy[enemynum]->xvel = MoveE;
	Enemy[enemynum]->yvel = 0;
	//Enemyangle = 90;
	Enemy[enemynum]->yind++;

}
void EnemyKeepMoving(int pl)
{
	switch(Enemy[pl]->life)
	{
		case 0:
			return;
		case 1:
			EnemyMoveUp(pl);
			break;
		case 2:
			EnemyMoveLeft(pl);
			break;
		case 3:
			EnemyMoveDown(pl);
			break;
		case 4:
			EnemyMoveRight(pl);
			break;

	}
}
void ArtificialIntelligence(int pl)
{
	if(Enemy[pl]->xind >= 9)
	{
		Enemy[pl]->xind = 9;
		Enemy[pl]->life = 3;
	}
	else if(Enemy[pl]->xind <=0)
	{
		Enemy[pl]->xind = 0;
		Enemy[pl]->life = 1;
	}
	else if(Enemy[pl]->yind>=9)
	{
		Enemy[pl]->yind = 9;
		Enemy[pl]->life = 2;
	}
	else if(Enemy[pl]->yind <= 0)
	{
		Enemy[pl]->yind = 0;
		Enemy[pl]->life = 4;
	}
}

void CheckEnemy()
{
	/*for(int ki=0;ki<4;ki++)
	{
		if(Enemy[ki]->xind == currentpos[0] && Enemy[ki]->yind == currentpos[1])
		{

			Illegal = true;
			return;
		}
	}
	for (int i = 0; i < 4; i++)
	{

		if( fabs(Player[0]->cx-Enemy[i]->cx) <= deltax && fabs(Player[0]->cy-Enemy[i]->cy) <= deltay)
		{

			Illegal = true;
			return;

		}


	}*/
	if(currentpos[0]<0 || currentpos[1]<0)
	{
		return;
	}
	for(int i=0;i<CoinMax;i++)
	{
		if(coins[i]->life)
		{
			if(charpositions[currentpos[0]][currentpos[1]][0]==coins[i]->cx && charpositions[currentpos[0]][currentpos[1]][1]==coins[i]->cy)
			{
				cout << currentpos[0] << "COINS " <<  currentpos[1] << endl;
				coins[i]->life = 0;
				score+=5;
				CollectCoin();
			}
		}
	}

}


void drawgameover(GLFWwindow *window)
{


	glm::vec3 eye ( eyex, eyey, eyez );
	glm::vec3 target (targetx, targety, targetz);
	glm::vec3 up (tiltx, tilty, tiltz);
	Matrices.projection = glm::ortho(-10.667f, 10.667f, -6.0f, 6.0f, 0.1f, 500.0f);
	Matrices.view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0) );




	glUseProgram (gameoverid);
	Texture_bind(gametex,0);
	glm::mat4 VP = Matrices.projection*Matrices.view;
	glm::mat4 MVP;
		Matrices.model = glm::mat4(1.0f);

		glm::mat4 translateTriangle = glm::translate (glm::vec3(0,0,0)); // glTranslatef
		glm::mat4 rotateCar = glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0));  // rotate about vector (1,0,0)
		glm::mat4 scaleTraingle = glm::scale(glm::vec3(10,10,1));
		glm::mat4 triangleTransform = translateTriangle*scaleTraingle;
		Matrices.model *= triangleTransform; 
		MVP = VP * Matrices.model; // MVP = p * V * M

		//  Don't change unless you are sure!!
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(rectangle);


}