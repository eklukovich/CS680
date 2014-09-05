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



/******************** Data Type Declarations ********************/
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};



/******************** Global Variables and Constants ********************/
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry;// VBO handle for our geometry

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

//attribute locations
GLint loc_position;
GLint loc_color;

//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection

// shader filenames
char * vertexShaderName;
char * fragmentShaderName;

// Random time things
float getDT();
chrono::time_point<chrono::high_resolution_clock> t1,t2;

bool flag = true;
int window;
/******************** Function Declarations ********************/
//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);

// Resource management
bool initialize();
void cleanUp();

// Command Line Arguments
bool getCommandlineArgs(int argc, char **argv);

// Menu Functions
void createMenu();
void menuListener(int selection);


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
       window = glutCreateWindow("Matrix Example");

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
       glutDisplayFunc(render);// Called when its time to display
       glutReshapeFunc(reshape);// Called if the window is resized
       glutIdleFunc(update);// Called if there is nothing else to do
       glutKeyboardFunc(keyboard);// Called if there is keyboard input



       // Initialize all of our resources(shaders, geometry)
       bool init = initialize();
       if(init)
       {
        t1 = std::chrono::high_resolution_clock::now();
       // initialize menu
       createMenu();
        glutMainLoop();
       }

       // Clean up after ourselves
       cleanUp();
      }

    // exit program
    return 0;
   }



/******************** Function Implementations ********************/
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

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

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

if(flag){
    angle += dt * M_PI/2; // move through 90 degrees a second

    // translate the cube in a orbiting pattern
    model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0, 4.0 * cos(angle)));

    // rotate the cube on the y-axis
    model = glm::rotate(model, 2*angle, glm::vec3(0, 1, 0)); 
}
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


void keyboard(unsigned char key, int x_pos, int y_pos)
   {
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }
  }


bool initialize()
   {
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    Vertex geometry[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };

    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

    //--Geometry done


    // Shader Sources
    // Put these into files and write a loader in the future
    // Note the added uniform!
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


void createMenu()
   {
    // create a new menu and set up a listener
    glutCreateMenu(menuListener);

    // add menu options
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
      glutDestroyWindow ( window );
     // exit(0);

       break;

       // start rotation
       case 2: 
flag = true;

       break;

       // stop rotation
       case 3: 
flag = false;
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
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " -v VERTEX_SHADER_FILENAME -f FRAGMENT_SHADER_FILENAME" << endl;
        return false;
    }

    // get the args
    for(int i = 1; i < argc; i++)
       {
        // check for vertex shader filename
        if(string(argv[i]) == "-v")
          {
           // check if we are not at the end of the array
           if(i + 1 < argc && string(argv[i+1]) != "-f")
              {
               vertexShaderName = new char [string(argv[i+1]).length()];
               strcpy(vertexShaderName, argv[i+1]);
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
           if(i + 1 < argc && string(argv[i+1]) != "-v")
             {
              fragmentShaderName = new char [string(argv[i+1]).length()];
              strcpy(fragmentShaderName, argv[i+1]);  
             }
           else
             {
              // print error
              cerr << "-f requires one argument." << endl;
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



