/******************** Header Files ********************/
#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting

#include <iostream>
#include <chrono>
#include <locale>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

#include "shader.h"
#include <string.h>
#include <vector>
#include "model.h"
#include "camera.h"
#include "physicsEngine.h"
#include "bowlingBall.h"

#include <irrKlang.h>
#include "audio_files/conio.h"
#include <stdlib.h>

using namespace irrklang;

using namespace std;


/******************** Global Variables and Constants ********************/

// Window variables
int window;
int w = 640, h = 480; // Window size

//transform matrices
glm::mat4 view;  // world->eye
glm::mat4 projection, ortho; // eye->clip
glm::mat4 model; // eye->clip
glm::mat4 mvp; // eye->clip

// shader varaibles
string textureVertexShaderName;
string textureFragmentShaderName;
GLuint texture_program;
GLint texture_loc_mvpmat; // Location of the modelviewprojection matrix in the shader
GLint texture_loc_position;
GLint loc_texture_coord;


// index 0 is vertex, index 1 is fragment
LightingEngine lightingEngine[2];
int toggleShader = 1;

// Random time things
float getDT();
chrono::time_point<chrono::high_resolution_clock> t1,t2;

// User interaction variables
float speed = 2.0f;
int direction = 1;

vector<colorVertex> circleGeom;
GLuint VB;
glm::mat4 ring; // eye->clip
glm::mat4 ringMvp; // eye->clip

// camera variables
Camera camera;
bool freeCamera = true;
bool cameraZoom = true;
int zoomCount = 0;

// keyboard variables
bool keys[256];

// Mouse Position
float mouseX, mouseY;

// model variables
Model room;
Model room2;
BowlingBall balls [8];
Model arrow;
Model lrArrow;
Model strikeSign;
Model spareSign;


int cameraPosition;

Physics* physics;

float ballAngle;

int turn;
int frame;
int score;
string scoreStr;

bool strike = false, spare = false;

int mode, ballIndex;

/******************** Function Declarations ********************/
// GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboardListener(unsigned char key, int x_pos, int y_pos);
void keyboardUpListener(unsigned char key, int x_pos, int y_pos);
void specialKeyHandler(int key, int xx, int yy);
void specialKeyReleaseHandler(int key, int xx, int yy);
void getMousePos(int x, int y);
void checkKeyboard();
void renderBitmapString(float x, float y, float z, void *font, string text);

// Resource management
bool initialize();
void cleanUp();

// Shader Programs
bool createTextureShader();


// Menu Functions
void createMenu();
void menuListener(int selection);

// File parse
bool loadData(string fileName);
bool loadRealisticData(string fileName);
void glCircle( unsigned int numPoints, unsigned int radius );

// Song picker
void chooseSong();

void initializeBowlingBalls();
void resetRound();

/******************** Main Program ********************/
int main(int argc, char **argv)
{
  // get commandline args
  physics = new Physics();
  physics->engine = createIrrKlangDevice();

 // physics->engine->play2D("../src/audio_files/media/labyrinth.ogg", true);

      // Initialize glut
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA);
  glutInitWindowSize(w, h);
      //glutFullScreen();

      // Name and create the Window
  window = glutCreateWindow("Assignment 12 - Bowling");
  //glutFullScreen();
      // Now that the window is created the GL context is fully set up
      // Because of that we can now initialize GLEW to prepare work with shaders
  GLenum status = glewInit();
  if( status != GLEW_OK)
  {
    cerr << "[F] GLEW NOT INITIALIZED: ";
    cerr << glewGetErrorString(status) << endl;
    return -1;
  }

      // Set all of the callbacks to GLUT that we need
      glutDisplayFunc(render); // Called when its time to display
      glutReshapeFunc(reshape); // Called if the window is resized
      glutIdleFunc(update); // Called if there is nothing else to do
      glutKeyboardFunc(keyboardListener); // Called if there is keyboard input
      glutKeyboardUpFunc(keyboardUpListener);
      glutSpecialFunc(specialKeyHandler);// Called if a special keyboard key is pressed
      glutSpecialUpFunc(specialKeyReleaseHandler);// Called if a special key is releasedz
      glutPassiveMotionFunc(getMousePos);

      // initialize menu
      createMenu();

      // Initialize all of our resources(shaders, geometry)
      if(initialize())
      {
        t1 = chrono::high_resolution_clock::now();
        glutMainLoop();
      }

      // Clean up after ourselves
      cleanUp();


    // exit program
      return 0;
    }


/******************** Function Implementations ********************/
void glCircle( float height )
    {
      colorVertex v;

      circleGeom.clear();

      v.position[0] = 1;
      v.position[1] = -1;
      v.position[2] = 0;
      v.color[0] = 0.0f;
      v.color[1] = 0.0f;
      v.color[2] = 0.0f;
      circleGeom.push_back(v);

      v.position[0] = -1;
      v.position[1] = -1;
      v.position[2] = 0;
      v.color[0] = 0.0f;
      v.color[1] = 0.0f;
      v.color[2] = 0.0f;
      circleGeom.push_back(v);

      v.position[0] = -1;
      v.position[1] = -height;
      v.position[2] = 0;
      v.color[0] = 0.0f;
      v.color[1] = 0.0f;
      v.color[1] = 0.0f;
      circleGeom.push_back(v);

      v.position[0] = -1;
      v.position[1] = -height;
      v.position[2] = 0;
      v.color[0] = 0.0f;
      v.color[1] = 0.0f;
      v.color[2] = 0.0f;
      circleGeom.push_back(v);

      v.position[0] = 1;
      v.position[1] = -height;
      v.position[2] = 0;
      v.color[0] = 0.0f;
      v.color[1] = 0.0f;
      v.color[2] = 0.0f;
      circleGeom.push_back(v);


      v.position[0] = 1;
      v.position[1] = -1;
      v.position[2] = 0;
      v.color[0] = 0.0f;
      v.color[1] = 0.0f;
      v.color[2] = 0.0f;
      circleGeom.push_back(v);
    }

bool initialize()
    {
    // set initial state
      physics->controlState = 0;

      ballIndex = 0;
      mode = 0;
      turn = 1;
      frame = 1;
      score = 0;
      scoreStr = "|  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |";

    // set ball angle
      ballAngle = M_PI/2.0f;

      physics->powerFactor = 10;
      physics->levelFileName = "table1.obj";
      lightingEngine[0].initialize("vs_vertex_shading.glsl", "fs_vertex_shading.glsl");
      lightingEngine[1].initialize("vs_fragment_shading.glsl", "fs_fragment_shading.glsl");

    // initialize bullet
      if(!physics->initializePhysicsEngine())
      {
        return false;
      }

    // load the room model
      room.loadModel("bowling_alley.obj");


      room2.loadModel("score.obj");
      room2.model = glm::translate(glm::mat4(1.0f), glm::vec3(-34.5, 5.18, 0));

      arrow.loadModel("arrow.obj");
      lrArrow.loadModel("left_right_arrow.obj");

      strikeSign.loadModel("strike.obj");
      strikeSign.model = glm::translate(glm::mat4(1.0f), glm::vec3(20, 3.25, 0));
      strikeSign.model = glm::rotate(strikeSign.model, float(-M_PI/2), glm::vec3(0, 1, 0));
      strikeSign.model = glm::rotate(strikeSign.model, float(M_PI/2), glm::vec3(1, 0, 0));
      strikeSign.model = glm::scale(strikeSign.model, glm::vec3(0.25, 0.25, 0.25));

      spareSign.loadModel("spare.obj");
      spareSign.model = glm::translate(glm::mat4(1.0f), glm::vec3(20, 3.25, 0));
      spareSign.model = glm::rotate(spareSign.model, float(-M_PI/2), glm::vec3(0, 1, 0));
      spareSign.model = glm::rotate(spareSign.model, float(M_PI/2), glm::vec3(1, 0, 0));
      spareSign.model = glm::scale(spareSign.model, glm::vec3(0.25, 0.25, 0.25));


      initializeBowlingBalls();

    // initialize the keys array to false
      for(int i = 0; i < 256; i++)
      {
        keys[i] = false;
      }

    // create the texture shader
      if(!createTextureShader())
      {
        return false;
      }


    // create vertices for the oribit paths
      glCircle( 0.85f );
      glGenBuffers(1, &VB);
      glBindBuffer(GL_ARRAY_BUFFER, VB);
      glBufferData(GL_ARRAY_BUFFER, sizeof(colorVertex) * circleGeom.size(), &circleGeom[0], GL_STATIC_DRAW);



    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   300.0f); //Distance to the far plane,

    ortho = glm::ortho (0, w, 0, h, 1, 0); //Distance to the far plane,);

    //enable depth testing
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);

    //and its done
return true;
}

void initializeBowlingBalls()
{
  balls[0].m.loadModel("ball1.obj");
  balls[1].m.loadModel("ball2.obj");
  balls[2].m.loadModel("ball3.obj");
  balls[3].m.loadModel("ball4.obj");
  balls[4].m.loadModel("ball5.obj");
  balls[5].m.loadModel("ball6.obj");
  balls[6].m.loadModel("ball7.obj");
  balls[7].m.loadModel("ball8.obj");

  // place balls on left holder
  balls[0].setNewPosition(glm::vec3(-31.5, 1.2, -4.5), float(-M_PI/2));
  balls[1].setNewPosition(glm::vec3(-30.5, 1.2, -4.5), float(-M_PI/2));
  balls[2].setNewPosition(glm::vec3(-29.5, 1.2, -4.5), float(-M_PI/2));
  balls[3].setNewPosition(glm::vec3(-28.5, 1.2, -4.5), float(-M_PI/2));

  // place balls on right holder
  balls[4].setNewPosition(glm::vec3(-31.5, 1.2, 4.5), float(M_PI/2));
  balls[5].setNewPosition(glm::vec3(-30.5, 1.2, 4.5), float(M_PI/2));
  balls[6].setNewPosition(glm::vec3(-29.5, 1.2, 4.5), float(M_PI/2));
  balls[7].setNewPosition(glm::vec3(-28.5, 1.2, 4.5), float(M_PI/2));
}

void computeScore(int remainingPins)
{
  static int additionalScore = 0;
  static int pinsForThisTurn = 0;

  if(turn == 1)
    pinsForThisTurn = 0;


  if(additionalScore != 0)
  {
    for(int i = 0; i < additionalScore; i+=2)
    {
      score += ((10 - remainingPins) - pinsForThisTurn);
    }

    additionalScore--;
  }

  if ((turn == 1) && remainingPins == 0)
  {
      strike = true;

    // handle strike scoring for next frame
    additionalScore += 2;

    score += 10;
    scoreStr[6 * (frame - 1) + 2 * turn] = 'X';

    // set turn to 2 so pins will reset
    turn = 2;
  }

  else if ((turn == 2) && remainingPins == 0)
  {
        spare = true;

    // handle spare scoring for next frame
    additionalScore += 1;
    score += ((10 - remainingPins) - pinsForThisTurn);
    pinsForThisTurn = (10 - remainingPins);
    scoreStr[6 * (frame - 1) + 2 * turn] = '/';
  }
  else
  {
    score += ((10 - remainingPins) - pinsForThisTurn);
    scoreStr[6 * (frame - 1) + 2 * turn] = (((10 - remainingPins) - pinsForThisTurn) + '0');
    if(additionalScore >= 2)
      additionalScore--;
  }
  pinsForThisTurn = (10 - remainingPins);
}


void renderBitmapString(float x, float y, float z, void *font, string text)
{
  // preserve and clear pkeyboarrojection and model matrices
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glOrtho( 0, w, 0, h, 1, 0);
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);

  glEnable (GL_BLEND);

  glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        // set up the Vertex Buffer Object so it can be drawn
  glEnableVertexAttribArray(texture_loc_position);
  glEnableVertexAttribArray(loc_texture_coord);


  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colorVertex) * circleGeom.size(), &circleGeom[0], GL_STATIC_DRAW);

        // set pointers into the vbo for each of the attributes(position and color)
        glVertexAttribPointer(texture_loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(colorVertex), 0);//offset
        glVertexAttribPointer(loc_texture_coord, 3, GL_FLOAT, GL_FALSE, sizeof(colorVertex), (void*)offsetof(colorVertex, color));


        ringMvp = projection * view * glm::mat4(1.0f);

        glUseProgram(texture_program);


            // upload the matrix to the shader
           // glUniformMatrix4fv(texture_loc_mvpmat, 1, GL_FALSE, glm::value_ptr(ringMvp));

        glDrawArrays(GL_TRIANGLES, 0, circleGeom.size());


        glDisableVertexAttribArray(texture_loc_position);
        glDisableVertexAttribArray(loc_texture_coord);
        glDisable (GL_BLEND);
        glUseProgram(0);
        glDisable(GL_TEXTURE_2D);



  // set text color and position
        glColor3f(0,255,255);
        glRasterPos3f(x, y, z);

  // display each character in the string
        for (string::iterator i = text.begin(); i != text.end(); i++) {
          char c = *i;
          glutBitmapCharacter(font, c);
        }

  // restore projection and model matrices
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
      }



void render()
{
  // clear the screen
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set the shader program
  glUseProgram(lightingEngine[toggleShader].shaderProgram);

  // render the playable lanes
  mvp = projection * view * physics->objects[0].model.model;
  physics->objects[0].model.renderModel(mvp, physics->objects[0].model.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));

  // render the playable ball
  if(mode == 0)
    {
      mvp = projection * view * physics->objects[1].model.model;
      physics->objects[1].model.renderModel(mvp, physics->objects[1].model.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));

    // draw left/right arrow if in state 0
          if(physics->controlState == 0){
            mvp = projection * view * lrArrow.model;
            lrArrow.renderModel(mvp, lrArrow.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
          }

    // draw angle direction arrow if in state 1
          if(physics->controlState == 1 || physics->controlState == 2){
            mvp = projection * view * arrow.model;
            arrow.renderModel(mvp, arrow.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
          }
        }

  // render the rest of the room
        mvp = projection * view * room.model;
        room.renderModel(mvp, room.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));



        for(int i = 0; i < 8; i++)
        {
          if(i != ballIndex){
            mvp = projection * view * balls[i].m.model;
            balls[i].m.renderModel(mvp, balls[i].m.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
          }
        }

        for (unsigned i = 0; i < physics->pinObjects.size(); i++) {
          mvp = projection * view * physics->pinObjects[i].model.model;
          physics->pinObjects[i].model.renderModel(mvp, physics->pinObjects[i].model.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
        }


        if(mode == 1)
        {
          balls[ballIndex].repositionBall(glm::vec3(-33, 3.1, 0), float(M_PI));
          mvp = projection * view * balls[ballIndex].m.model;
          balls[ballIndex].m.renderModel(mvp, balls[ballIndex].m.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
        }
  if(strike)
    {
      cout << "test!" << endl;
      mvp = projection * view * strikeSign.model;
      strikeSign.renderModel(mvp, strikeSign.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
    }

  if(spare)
    {
      mvp = projection * view * spareSign.model;
      spareSign.renderModel(mvp, spareSign.model, lightingEngine[toggleShader], physics->ballPos, glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
    }


        renderBitmapString(5, 10, 0, GLUT_BITMAP_HELVETICA_18, "Frame: " + to_string(frame) + " Round: " + to_string(turn) +
          "                Score: " + scoreStr + "   Total: " + to_string(score) + "      Power: " + to_string(physics->powerFactor));

          // strike text


        glutSwapBuffers();

        if(strike || spare)
        {
          sleep(3);
          resetRound();
        }
      }


      void update()
      {
    // total time
    float dt = getDT(); // if you have anything moving, use dt.

    if(frame == 11){
      physics->controlState = 0;
      resetRound();

      turn = 1;
      frame = 1;
      score = 0;
      scoreStr = "|  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |";

      // set ball angle
      ballAngle = M_PI/2.0f;

      physics->powerFactor = 10;

    }

    if (!physics->paused)
      physics->updateWorld(dt);

    if(freeCamera)
    {
      checkKeyboard();
    }
    glm::vec3 ViewPoint = camera.Position + camera.ViewDir;

    if (cameraZoom)
    {

      view = glm::lookAt( glm::vec3(camera.Position.x++,camera.Position.y-=0.5,camera.Position.z),
        glm::vec3(ViewPoint.x,ViewPoint.y,ViewPoint.z),
        glm::vec3(camera.UpVector.x,camera.UpVector.y,camera.UpVector.z));

      zoomCount++;
      if (zoomCount > 17)
      {
        camera.Position = glm::vec3(-32, 3.2, 0);

        camera.ViewDir.x = 1.0f;
        camera.ViewDir.y = 0.0f;
        camera.ViewDir.z = 0.0f;

        camera.UpVector.x = 0.20f;
        camera.UpVector.y = 1.0f;
        camera.UpVector.z = 0.0f;

        camera.RotatedX = camera.RotatedZ = 0.0f;
        camera.RotatedY = M_PI/2;
        cameraZoom = false;
      }
    }
    else
    {
      cout << camera.Position.x << " | " << camera.Position.y << " | " << camera.Position.z << endl;
      //cout << camera.ViewDir.x << " | " << camera.ViewDir.y << " | " << camera.ViewDir.z << endl;
      //cout << camera.UpVector.x << " | " << camera.UpVector.y << " | " << camera.UpVector.z << endl << endl;

      view = glm::lookAt( glm::vec3(camera.Position.x,camera.Position.y,camera.Position.z),
        glm::vec3(ViewPoint.x,ViewPoint.y,ViewPoint.z),
        glm::vec3(camera.UpVector.x,camera.UpVector.y,camera.UpVector.z));
    }
    if(camera.Position.x < 17 && physics->controlState == 3){
      camera.Position.x = physics->ballPos.x - 10;
    }

    if (physics->controlState == 4)
    {
      computeScore(physics->pinObjects.size());
      if(!strike && !spare)
        resetRound();
    }

    if(physics->controlState == 1 || physics->controlState == 2 ){
      arrow.model = glm::translate(glm::mat4(1.0), (glm::vec3(physics->ballPos.x+1, 1, physics->ballPos.z)));
      arrow.model = glm::rotate(arrow.model, ballAngle + (float)M_PI, glm::vec3(0.0f, 1.0f, 0.0f));

      if(physics->controlState == 2)
      {
        arrow.model = glm::scale(arrow.model, glm::vec3(1.0, 1.0f, 0.5 + physics->powerFactor/10.0f));
      }
    }

    if(physics->controlState == 0){
      lrArrow.model = glm::translate(glm::mat4(1.0), (glm::vec3(physics->ballPos.x, 0.5, physics->ballPos.z)));
      lrArrow.model = glm::rotate(lrArrow.model, (float)M_PI, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Update the state of the scene
    glutPostRedisplay(); // call the display callback
  }

  void resetRound()
  {
    physics->controlState = 0;
    physics->powerFactor = 10;
    strike = spare = false;

  // reset camera
    camera.reset();
    freeCamera = true;
    cameraZoom = true;
    zoomCount = 0;

  // reset ball
    btTransform transform;
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(-25, 5, 0)));
    physics->objects[1].rigidBody->setMotionState(motionState);
    physics->objects[1].rigidBody->setLinearVelocity(btVector3(0,0,0));
    ballAngle = M_PI/2;

  // if second turn, reset pins
    if (turn == 2)
    {
      physics->resetPins();
      turn = 1;
      frame++;
    }
    else
      turn = 2;
  }


  void reshape(int n_w, int n_h)
  {
    w = n_w;
    h = n_h;
    glViewport( 0, 0, w, h);
    projection = glm::infinitePerspective(45.0f, float(w)/float(h), 0.01f);
    if(w > 640)
      glCircle( 0.925f );

  }



  void checkKeyboard()
  {
    // Strafe left
    if(keys['a'])
      camera.StrafeRight(-0.5);

    // Strafe right
    if(keys['d'])
      camera.StrafeRight(0.5);

    // Move forward
    if(keys['w'])
      camera.MoveForward( -0.5 ) ;

    // Move backward
    if(keys['s'])
      camera.MoveForward( 0.5) ;

    // move up
    if(keys['q'])
      camera.MoveUpward(0.3);

    // move down
    if(keys['e'])
      camera.MoveUpward(-0.3);

    // look up
    if(keys['i'])
      camera.RotateX(1.0);

    // look down
    if(keys['k'])
      camera.RotateX(-1.0);

    // roll right
    if(keys['l'])
      camera.RotateZ(-2.0);

    // roll left
    if(keys['j'])
      camera.RotateZ(2.0);

    // look left
    if(keys['u'])
      camera.RotateY(2.0);

    // look right
    if(keys['o'])
      camera.RotateY(-2.0);
  }



  void keyboardListener(unsigned char key, int x_pos, int y_pos)
  {
    // check if key is escape
    if(key == 27)
    {
      glutDestroyWindow(window);
      return;
    }
    //
    else if (key == '1')
    {
      // toggle ambient
      lightingEngine[toggleShader].ambientLightOn = !lightingEngine[toggleShader].ambientLightOn;
    }
    else if (key == '2')
    {
      // toggle diffuse
      lightingEngine[toggleShader].diffuseLightOn = !lightingEngine[toggleShader].diffuseLightOn;

    }
    else if (key == '3')
    {
      // toggle specular
      lightingEngine[toggleShader].specularLightOn = !lightingEngine[toggleShader].specularLightOn;
    }
    else if (key == '4')
    {
      // toggle point light
      lightingEngine[toggleShader].pointLightOn = !lightingEngine[toggleShader].pointLightOn;
    }
    else if (key == '5')
    {
      // toggle spot light
      lightingEngine[toggleShader].spotLightOn = !lightingEngine[toggleShader].spotLightOn;
    }

    else if (key == '6')
    {
      // toggle spot light
      camera.Position = glm::vec3(17.5, 3.2, 0);

      camera.ViewDir.x = 1.0f;
      camera.ViewDir.y = 0.0f;
      camera.ViewDir.z = 0.0f;

      camera.UpVector.x = 0.20f;
      camera.UpVector.y = 1.0f;
      camera.UpVector.z = 0.0f;

      camera.RotatedX = camera.RotatedZ = 0.0f;
      camera.RotatedY = M_PI/2;
      cameraZoom = false;
    }

    // spacebar
    else if (key == 32) {
      if(mode == 1)
      {
        mode = 0;
        physics->changeBall("ball" + to_string(ballIndex+1) + ".obj");
        camera.Position = glm::vec3(-32, 3.2, 0);

        camera.ViewDir.x = 1.0f;
        camera.ViewDir.y = 0.0f;
        camera.ViewDir.z = 0.0f;

        camera.UpVector.x = 0.20f;
        camera.UpVector.y = 1.0f;
        camera.UpVector.z = 0.0f;

        camera.RotatedX = camera.RotatedZ = 0.0f;
        camera.RotatedY = M_PI/2;
        cameraZoom = false;
      }
      else
      {
        if (physics->controlState == 2)
        {
          physics->objects[1].rigidBody->setLinearVelocity(btVector3(physics->powerFactor*sin(ballAngle),0, physics->powerFactor*cos(ballAngle)));
        }
        if (physics->controlState < 3)
        {
          physics->controlState++;
        }
      }

    }

    // enter key
    else if(key == 13)
    {

    }

    else if (key == 'z')
    {
        // toggle spot light
      toggleShader = (toggleShader+1)%2 ;
    }

    // assume key pressed is for movement
    else
    {
      keys[key] = true;
      freeCamera = true;
    }

    glutPostRedisplay();

  }

  void keyboardUpListener(unsigned char key, int x_pos, int y_pos)
  {
    keys[key] = false;
    glutPostRedisplay();
  }

  void specialKeyHandler(int key, int xx, int yy)
  {
    btTransform transform;

    if(mode == 0)
    {
      if (physics->controlState == 0) {
        switch(key) {
          case GLUT_KEY_LEFT:
          physics->objects[1].rigidBody->getMotionState()->getWorldTransform(transform);
          if (transform.getOrigin().z() >= -2.0) {
            physics->objects[1].rigidBody->translate(btVector3(0,0,-0.1));
          }
          break;

          case GLUT_KEY_RIGHT:
          physics->objects[1].rigidBody->getMotionState()->getWorldTransform(transform);
          if (transform.getOrigin().z() <= 2.0) {
            physics->objects[1].rigidBody->translate(btVector3(0,0,0.1));
          }
          break;
        }
      }
      else if (physics->controlState == 1) {
        switch(key) {
          case GLUT_KEY_LEFT:

          // add 10 degrees
          ballAngle += M_PI/360.f;

          if(ballAngle > (95 * M_PI/180.f))
          {
            ballAngle = (95 * M_PI/180.f);
          }
          break;

          case GLUT_KEY_RIGHT:

          // minus 10 degrees
          ballAngle -= M_PI/360.f;;

          if(ballAngle < (85 * M_PI/180.f))
          {
            ballAngle = (85 * M_PI/180.f);
          }
          break;

          case GLUT_KEY_UP:

          break;
        }
      }
      else if (physics->controlState == 2) {
        switch(key) {
          case GLUT_KEY_UP:
          if(physics->powerFactor < 25)
            physics->powerFactor++;
          break;

          case GLUT_KEY_DOWN:
          if(physics->powerFactor > 10)
            physics->powerFactor--;
          break;
        }
      }
    }
    else
    {
      switch(key)
      {
        case GLUT_KEY_LEFT:
        balls[ballIndex].resetBallPosition();

        if(ballIndex > 0)
          ballIndex --;
        break;

        case GLUT_KEY_RIGHT:
        balls[ballIndex].resetBallPosition();

        if(ballIndex < 7)
          ballIndex++;
        break;
      }
    }
  }

  void specialKeyReleaseHandler(int key, int xx, int yy) {
    switch(key) {
      case GLUT_KEY_LEFT:
      case GLUT_KEY_RIGHT:
      break;
      case GLUT_KEY_UP:
      break;
      case GLUT_KEY_DOWN:
      break;
    }
  }


  void getMousePos(int x, int y)
  {
    mouseX = (float)x;
    mouseY = (float)y;
  }


  void createMenu()
  {
    // create main menu entries
    glutCreateMenu(menuListener);
    glutAddMenuEntry("Change Ball Style", 1);
    glutAddMenuEntry("Pause Game", 2);
    glutAddMenuEntry("Resume Game", 3);
    glutAddMenuEntry("Restart Game", 4);
    glutAddMenuEntry("Change Level", 5);
    glutAddMenuEntry("Quit", 6);

    // set right mouse click to open menu
    glutAttachMenu(GLUT_RIGHT_BUTTON);
  }


  void menuListener(int selection)
  {
    // check which menu option was selected
    switch(selection)
    {
        // change ball style
      case 1:
      mode = 1;

        camera.Position = glm::vec3(-38.5, 3.2, 0);

        camera.ViewDir.x = 1.0f;
        camera.ViewDir.y = 0.0f;
        camera.ViewDir.z = 0.0f;

        camera.UpVector.x = 0.20f;
        camera.UpVector.y = 1.0f;
        camera.UpVector.z = 0.0f;

        camera.RotatedX = camera.RotatedZ = 0.0f;
        camera.RotatedY = M_PI/2;
        cameraZoom = false;
      break;

        // pause
      case 2:
      physics->engine->setAllSoundsPaused(1);
      physics->paused = true;

      break;

        // resume
      case 3:
      physics->engine->setAllSoundsPaused(0);
      physics->paused = false;
      break;

        // restart
      case 4:

      physics->powerFactor = 10;
      resetRound();
      physics->resetPins();

      turn = 1;
      frame = 1;
      score = 0;
      scoreStr = "|  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |  -  |";

            // set ball angle
      ballAngle = M_PI/2.0f;



      camera.reset();
      freeCamera = true;
      cameraZoom = true;
      zoomCount = 0;


      break;

        // change level
      case 5:

      physics->cleanup();
      delete physics;
      physics = new Physics();

      camera.Position = glm::vec3(-15,30,0);

      camera.ViewDir.x = 10.0f;
      camera.ViewDir.y = -30.0f;
      camera.ViewDir.z = 0.0f;

      camera.UpVector.x = 1.0f;
      camera.UpVector.y = 0.0f;
      camera.UpVector.z = 0.0f;

      camera.RotatedX = camera.RotatedZ = 0.0f;
      camera.RotatedY = M_PI/2;

      physics->initializePhysicsEngine();

      break;

        // exit the program
      case 6:
      glutDestroyWindow(window);
      return;
      break;

    }
    glutPostRedisplay();
  }

  void cleanUp()
  {
    physics->engine->drop();
    physics->cleanup();
  }


//returns the time delta
  float getDT()
  {
    float ret;
    t2 = chrono::high_resolution_clock::now();
    ret = chrono::duration_cast< chrono::duration<float> >(t2-t1).count();
    t1 = chrono::high_resolution_clock::now();
    return ret;
  }


  bool createTextureShader()
  {
    // set up the texture vertex shader
    shader texture_vertex_shader(GL_VERTEX_SHADER);
    if(!texture_vertex_shader.initialize("texture_vertex_shader.glsl"))
    {
      return false;
    }

    // set up the texture fragment shader
    shader texture_fragment_shader(GL_FRAGMENT_SHADER);
    if(!texture_fragment_shader.initialize("texture_fragment_shader.glsl"))
    {
      return false;
    }

    // link the texture shader program
    texture_program = glCreateProgram();
    glAttachShader(texture_program, texture_vertex_shader.getShader());
    glAttachShader(texture_program, texture_fragment_shader.getShader());
    glLinkProgram(texture_program);

    // check if everything linked ok
    GLint texture_shader_status;
    glGetProgramiv(texture_program, GL_LINK_STATUS, &texture_shader_status);
    if(!texture_shader_status)
    {
      cerr << "[F] THE TEXTURE SHADER PROGRAM FAILED TO LINK" << endl;
      return false;
    }

    // set up the vertex position attribute
    texture_loc_position = glGetAttribLocation(texture_program, const_cast<const char*>("v_position"));
    if(texture_loc_position == -1)
    {
      cerr << "[F] POSITION NOT FOUND" << endl;
      return false;
    }

    // set up the vertex uv coordinate attribute
    loc_texture_coord = glGetAttribLocation(texture_program, const_cast<const char*>("v_color"));
    if(loc_texture_coord == -1)
    {
      cerr << "[F] V_COLOR NOT FOUND" << endl;
      return false;
    }

    // return
    return true;
  }
