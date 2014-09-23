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
#include <fstream>


/******************** Data Type Declarations ********************/
struct Vertex
{
  GLfloat position[3];
  GLfloat color[3];
};



/******************** Global Variables and Constants ********************/

// Window variables
int window;
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry; // VBO handle for our geometry
Vertex geometrySolid[36];

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
string vertexShaderName;
string fragmentShaderName;
string modelOBJName;
string modelMaterialName;

// Random time things
float getDT();
chrono::time_point<chrono::high_resolution_clock> t1,t2;

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
void initializeModels();
void loadObjModel(string, vector<Vertex>&);


// Command Line Arguments
bool getCommandlineArgs(int argc, char **argv);

// Menu Functions
void createMenu();
void menuListener(int selection);


void renderBitmapString(float x, float y, float z, void *font, string string);
void importMaterial(string mat, string matFilepath, float &r, float &g, float &b);



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
   window = glutCreateWindow("Assignment 4 - Model Loader");

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
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    initializeModels();

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
      cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << endl;
      return false;
    }


    // Now we set the locations of the attributes and uniforms
    // this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program, const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
      cerr << "[F] POSITION NOT FOUND" << endl;
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

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

    glDrawArrays(GL_TRIANGLES, 0, numVertices);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color); 



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
    cerr << "Usage: " << argv[0] << " -v VERTEX_SHADER_FILENAME -f FRAGMENT_SHADER_FILENAME -m MODEL_SHADER_FILENAME" << endl;
    return false;
  }

  // get the args
  for(int i = 1; i < argc; i++)
  {
    // check for vertex shader filename
    if(string(argv[i]) == "-v")
    {
      // check if we are not at the end of the array
      if(i + 1 < argc && (string(argv[i+1]) != "-f" || string(argv[i+1]) != "-m") )
        {
          vertexShaderName = argv[i+1];
        }  
      else
        {
          // print error
          cerr << "-v requires one argument." << endl;
          return false;
        }  
    } 

    // check for fragment shader filename
    else if(string(argv[i]) == "-f")
      {
        // check if we are not at the end of the array
        if(i + 1 < argc && (string(argv[i+1]) != "-v" || string(argv[i+1]) != "-m"))
          {
            fragmentShaderName = argv[i+1];
          }
        else
          {
            // print error
            cerr << "-f requires one argument." << endl;
            return false;
          }  
      } 
    else if(string(argv[i]) == "-m")
      {
        // check if we are not at the end of the array
        if(i + 1 < argc && (string(argv[i+1]) != "-f" || string(argv[i+1]) != "-v"))
          {
            modelOBJName = argv[i+1]; 
            string temp = argv[i+1];
            size_t filetype = temp.find(".");
            modelMaterialName = temp.replace (filetype, 4, ".mtl", 0, 4);
          }
        else
          {
            // print error
            cerr << "-m requires one argument." << endl;
            return false;
          }  
      } 
    else 
      {
        cerr << "Usage: " << argv[0] << " -v VERTEX_SHADER_FILENAME -f FRAGMENT_SHADER_FILENAME" << endl;
        return false;
      }
    i++;
  }

  return true;
}


void loadObjModel(string objFilepath, string matFilepath, vector<Vertex>& vertices)
{
  // initialize variables
  ifstream fin;
  string valueType;
  vector<Vertex> tempVertices;
  Vertex tempVertex;
  int vertexNumber;
  string mat;
  float r, g, b;

  // try to open the file, if cannot open then print error and exit
  fin.open(objFilepath);

  if(!fin.is_open())
    { 
      cerr << "Cannot open " << objFilepath << endl; 
      exit(1); 
    }

  // loop until the end of the file is reached
  while (!fin.eof())
    {
      // get string
      fin >> valueType;

      // get the vertex value
      if(valueType == "v") 
        {
          // load in vertex x, y, z values
          fin >> tempVertex.position[0];
          fin >> tempVertex.position[1];
          fin >> tempVertex.position[2];

          // store vertex into temp vertex vector
          tempVertices.push_back(tempVertex);
        }

      // get the face value    
      else if(valueType == "f") 
        {
          for(int i = 0; i < 3; i++)
            {
              fin >> vertexNumber;
 
                  // set color value to blue
                  tempVertices[vertexNumber - 1].color[0] = r;
                  tempVertices[vertexNumber - 1].color[1] = g;
                  tempVertices[vertexNumber - 1].color[2] = b;

              vertices.push_back(tempVertices[vertexNumber - 1]);
            }  
        }

      // get the material value
      else if(valueType == "usemtl") 
        {
          // get material name and import rgb values
          fin >> mat;
          importMaterial(mat, matFilepath, r, g, b);
        }
    }

  // store the number of vertices
  numVertices = vertices.size();
}


void importMaterial(string mat, string matFilepath, float &r, float &g, float &b)
{
    // initialize variables
    ifstream fin;
    string label;

    // try to open the file, if cannot open then print error and set to default color values
    fin.open(matFilepath);

    if(!fin) 
      { 
        cerr << "Cannot open " << matFilepath << ", using default colors!" << endl;  
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
      }
    else
      {
        // loop through entire file
        while (!fin.eof()) 
          {
            fin >> label;

            // if a new material is found then check if its the desired material
            if(label == "newmtl") 
              {
                fin >> label;

                // check if desired material
                if(strcmp(mat.c_str(), label.c_str()) == 0) 
                  {
                    // loop until diffuse color
                    while (label != "Kd")
                      fin >> label;

                    // read in red, green, and blue color values
                    fin >> r;
                    fin >> g;
                    fin >> b;
                  }
              }
          }

        // close file  
        fin.close();
      }
}


void initializeModels()
{
  vector<Vertex> sphereVector;
  loadObjModel(modelOBJName, modelMaterialName, sphereVector);
  Vertex* sphereVertices = new Vertex[sphereVector.size()];
  for(int i = 0; i < numVertices; i++)
   {
      sphereVertices[i] = sphereVector[i];
   }

   glGenBuffers(1, &vbo_geometry);
   glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
   glBufferData(GL_ARRAY_BUFFER, (sizeof(*sphereVertices) * numVertices), sphereVertices, GL_STATIC_DRAW);
}





