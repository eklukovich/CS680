/******************** Header Files ********************/
#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting

#include <iostream>
#include <chrono>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include "shader.h"
#include <string.h>
#include <vector>
#include "model.h"


/******************** Global Variables and Constants ********************/

// Window variables
int window;
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry; // VBO handle for our geometry
Model object;

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_color;

//transform matrices
glm::mat4 model; // obj->world each object should have its own model matrix
glm::mat4 view;  // world->eye
glm::mat4 projection; // eye->clip
glm::mat4 mvp; // premultiplied modelviewprojection

// shader filenames
std::string vertexShaderName;
std::string fragmentShaderName;
std::string modelOBJName;
std::string modelMaterialName;

// Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

// User interaction variables
float speed = 2.0f;
int direction = 1;
bool rotationFlag = true;

int numVertices;

/******************** Function Declarations ********************/
// GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboardListener(unsigned char key, int x_pos, int y_pos);
void mouseListener(int button, int state, int x, int y);
void specialKeyListener(int key, int xx, int yy);

// Resource management
bool initialize();
void cleanUp();

// Command Line Arguments
bool getCommandlineArgs(int argc, char **argv);

// Menu Functions
void createMenu();
void menuListener(int selection);


void renderBitmapString(float x, float y, float z, void *font, std::string string);
void importMaterial(std::string mat, std::string matFilepath, float &r, float &g, float &b);



/******************** Main Program ********************/
int main(int argc, char **argv)
{
    // get commandline args
  bool validArgs = getCommandlineArgs(argc, argv);
  if(validArgs)
  {
       // Initialize glut
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(w, h);

// Name and create the Window
   window = glutCreateWindow("Assignment 5 - Assimp Model Loader");

       // Now that the window is created the GL context is fully set up
       // Because of that we can now initialize GLEW to prepare work with shaders
   GLenum status = glewInit();
   if( status != GLEW_OK)
   {
     std::cerr << "[F] GLEW NOT INITIALIZED: ";
     std::cerr << glewGetErrorString(status) << std::endl;
     return -1;
   }

       // Set all of the callbacks to GLUT that we need
       glutDisplayFunc(render); // Called when its time to display
       glutReshapeFunc(reshape); // Called if the window is resized
       glutIdleFunc(update); // Called if there is nothing else to do
       glutKeyboardFunc(keyboardListener); // Called if there is keyboard input
       glutMouseFunc(mouseListener);
       glutSpecialFunc(specialKeyListener);

       // initialize menu
       createMenu();

       // Initialize all of our resources(shaders, geometry)
       bool init = initialize();
       if(init)
       {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
      }

       // Clean up after ourselves
      cleanUp();
    }

    // exit program
    return 0;
  }



/******************** Function Implementations ********************/
  bool initialize()
  {
    // initialize the models
    object.loadModel(modelOBJName);

    // Shader Sources
    shader vertex_shader(GL_VERTEX_SHADER);
    bool status = vertex_shader.setupShader(vertexShaderName);
    if(!status)
      return false;

    shader fragment_shader(GL_FRAGMENT_SHADER);
    status = fragment_shader.setupShader(fragmentShaderName);
    if(!status)
      return false;


    // Now we link the 2 shader objects into a program
    // This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader.getShader());
    glAttachShader(program, fragment_shader.getShader());
    glLinkProgram(program);

    // check if everything linked ok
    GLint shader_status;
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
      std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
      return false;
    }


    // Now we set the locations of the attributes and uniforms
    // this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program, const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
      std::cerr << "[F] POSITION NOT FOUND" << std::endl;
      return false;
    }

    loc_color = glGetAttribLocation(program, const_cast<const char*>("v_color"));
    if(loc_color == -1)
    {
      std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
      return false;
    }

    loc_mvpmat = glGetUniformLocation(program, const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
      std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
      return false;
    }
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine

    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
  }


  void render()
  {
    //--Render the scene
    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //premultiply the matrix for this example
    mvp = projection * view * model;

    //enable the shader program
    glUseProgram(program);
    

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    object.renderModel(loc_position, loc_color);

    //swap the buffers
      glutSwapBuffers();
    }


    void update()
    {
    //total time
      static float angle = 0.0;
    float dt = getDT(); // if you have anything moving, use dt.

    // check to see if the cube should be rotated

    // move through 90 degrees a second
    if(rotationFlag)
      angle += dt * M_PI/2 * direction; 

    // translate the center cube in a orbiting pattern
    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle)));

    // Update the state of the scene
    glutPostRedisplay(); // call the display callback
  }


  void reshape(int n_w, int n_h)
  {
    w = n_w;
    h = n_h;
      //Change the viewport to be correct
    glViewport( 0, 0, w, h);
      //Update the projection matrix as well
      //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);
  }


    void keyboardListener(unsigned char key, int x_pos, int y_pos)
    {
    // Handle keyboard input
      switch(key)
      {
       // esc key to quit program
       case 27:
       glutDestroyWindow(window);
       break;

       // 'a' key to rotate cube counter clockwise
       case 97:
       direction = 1;  
       break;

       // 's' key to rotate cube clockwise
       case 115:
       direction = -1;  
       break;
     }
   }


   void specialKeyListener(int key, int xx, int yy)
   {
    switch (key)
    {
     // left arrow key to rotate cube counter clockwise
     case GLUT_KEY_LEFT :
     direction = 1;  
     break;

     // right arrow key to rotate cube clockwise
     case GLUT_KEY_RIGHT :
     direction = -1;
     break;
   }
 }


 void mouseListener(int button, int state, int x, int y)
 {
    // set the left mouse click to change rotation direction
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    direction *= -1;
  }
}


void createMenu()
{
    // create color submenu entries
    // create main menu entries
  glutCreateMenu(menuListener);
  glutAddMenuEntry("Start rotation", 2);
  glutAddMenuEntry("Stop Rotation", 3);
  glutAddMenuEntry("Quit", 1);

    // set right mouse click to open menu
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void menuListener(int selection)
{
    // check which menu option was selected
  switch(selection) 
  {
       // exit the program
   case 1: 
   glutDestroyWindow(window);
   break;

       // start rotation
   case 2: 
   rotationFlag = true;
   break;

       // stop rotation
   case 3: 
   rotationFlag = false;
   break;
 } 
 if(selection!= 1)
  glutPostRedisplay();
}

void cleanUp()
{
    // Clean up, Clean up
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_geometry);

}


//returns the time delta
float getDT()
{
  float ret;
  t2 = std::chrono::high_resolution_clock::now();
  ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
  t1 = std::chrono::high_resolution_clock::now();
  return ret;
}


bool getCommandlineArgs(int argc, char **argv)
{
    // if there are less then 5 args then return an error
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0] << " -v VERTEX_SHADER_FILENAME -f FRAGMENT_SHADER_FILENAME -m MODEL_SHADER_FILENAME" << std::endl;
    return false;
  }

  // get the args
  for(int i = 1; i < argc; i++)
  {
    // check for vertex shader filename
    if(std::string(argv[i]) == "-v")
    {
      // check if we are not at the end of the array
      if(i + 1 < argc && (std::string(argv[i+1]) != "-f" || std::string(argv[i+1]) != "-m") )
        {
          vertexShaderName = argv[i+1];
        }  
      else
        {
          // print error
          std::cerr << "-v requires one argument." << std::endl;
          return false;
        }  
    } 

    // check for fragment shader filename
    else if(std::string(argv[i]) == "-f")
      {
        // check if we are not at the end of the array
        if(i + 1 < argc && (std::string(argv[i+1]) != "-v" || std::string(argv[i+1]) != "-m"))
          {
            fragmentShaderName = argv[i+1];
          }
        else
          {
            // print error
            std::cerr << "-f requires one argument." << std::endl;
            return false;
          }  
      } 
    else if(std::string(argv[i]) == "-m")
      {
        // check if we are not at the end of the array
        if(i + 1 < argc && (std::string(argv[i+1]) != "-f" || std::string(argv[i+1]) != "-v"))
          {
            modelOBJName = argv[i+1]; 
            std::string temp = argv[i+1];
            size_t filetype = temp.find(".");
            modelMaterialName = temp.replace (filetype, 4, ".mtl", 0, 4);
          }
        else
          {
            // print error
            std::cerr << "-m requires one argument." << std::endl;
            return false;
          }  
      } 
    else 
      {
        std::cerr << "Usage: " << argv[0] << " -v VERTEX_SHADER_FILENAME -f FRAGMENT_SHADER_FILENAME" << std::endl;
        return false;
      }
    i++;
  }

  return true;
}

