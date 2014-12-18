#include "physicsEngine.h"
#include <algorithm>


bool Physics::initializePhysicsEngine()
{
    paused = false;


    engine = createIrrKlangDevice();

    broadphase = new btDbvtBroadphase();

    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
   // dynamicsWorld->setGravity(btVector3(0, 0, 0));

    return setupWorld();
}

bool Physics::setupWorld()
    {
        BodyObject tableObject, ballObject, ballObject2, planeObject, goalObject;
        BowlingPin pins[10];
        bool status;

        // set object collision filters
        int ballCollidesWith =  COL_LANE | COL_PIN;
        int laneCollidesWith = COL_BALL | COL_PIN;
        int pinsCollidesWith = COL_LANE | COL_BALL | COL_PIN;

        tableObject.xAxisRotation = 0.0f;
        tableObject.zAxisRotation = 0.0f;


        // initialize bowling lane
        status = tableObject.initializeStaticBody("playable_lane.obj", btVector3(0,0,0), btVector3(1.0f, 1.0f, 1.0f));
        if(!status){return false; }
        dynamicsWorld->addRigidBody(tableObject.rigidBody, COL_LANE, laneCollidesWith);
        objects.push_back(tableObject);

        // initialize bowling ball
        status = ballObject.initializeSphereObject("ball1.obj", btVector3(-25, 5, 0), 5, btVector3(0,0.5,0), btVector3(0.5f, 0.5f, 0.5f), btVector3(1,1,1));
        if(!status){return false;}
        dynamicsWorld->addRigidBody(ballObject.rigidBody, COL_BALL, ballCollidesWith);
        objects.push_back(ballObject);

        // initialize 10 pins

        // first row pin
        status = pins[0].initializeBowlingPinObject("bowling_pin.obj", btVector3(24.5,0.96,0), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}

        // second row pins
        if(!status){return false;}
        status = pins[5].initializeBowlingPinObject("bowling_pin.obj", btVector3(25.4,0.96,-0.7), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}
        status = pins[6].initializeBowlingPinObject("bowling_pin.obj", btVector3(25.4,0.96,0.7), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}

        // third row pins
        status = pins[7].initializeBowlingPinObject("bowling_pin.obj", btVector3(26,0.96,1.4), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}
        status = pins[8].initializeBowlingPinObject("bowling_pin.obj", btVector3(26,0.96,0), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}
        status = pins[9].initializeBowlingPinObject("bowling_pin.obj", btVector3(26,0.96,-1.4), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}

        // fourth row pins
        status = pins[1].initializeBowlingPinObject("bowling_pin.obj", btVector3(27,0.96,2.1), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}
        status = pins[2].initializeBowlingPinObject("bowling_pin.obj", btVector3(27,0.96,-2.1), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}
        status = pins[3].initializeBowlingPinObject("bowling_pin.obj", btVector3(27,0.96,0.7), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));
        if(!status){return false;}
        status = pins[4].initializeBowlingPinObject("bowling_pin.obj", btVector3(27,0.96,-0.7), 1, btVector3(0,0,0), btVector3(1.f, 1.f, 1.f));


        for(int i = 0; i < 10; i++)
        {
           dynamicsWorld->addRigidBody(pins[i].rigidBody, COL_PIN, pinsCollidesWith);
           pinObjects.push_back(pins[i]);
       }
        return true;
    }

void Physics::updateWorld(float dt){

        dynamicsWorld->stepSimulation(dt, 10);

        static btTransform transTable, transBall, transPin;
        static int timeTicks = 0;

        // udate playable lanes
        objects[0].rigidBody->getMotionState()->getWorldTransform(transTable);
        btScalar mTable[16];
        transTable.getOpenGLMatrix(mTable);
        objects[0].model.model = glm::make_mat4(mTable);

        // update bowling ball
        objects[1].rigidBody->getMotionState()->getWorldTransform(transBall);
        btScalar mBall[16];
        transBall.getOpenGLMatrix(mBall);
        objects[1].model.model = glm::make_mat4(mBall);
        objects[1].model.model = glm::rotate(objects[1].model.model, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
        objects[1].model.model = glm::scale(objects[1].model.model, glm::vec3(0.5f, 0.5f, 0.5f));

        // update pins
        btScalar mPin[16];

        vector<BowlingPin>::iterator it = pinObjects.begin(), end = pinObjects.end();
        vector<BowlingPin>::iterator it2 = pinObjects.begin(), end2 = pinObjects.end();

        for ( ; it != end; )
        {
            it->rigidBody->getMotionState()->getWorldTransform(transPin);
            transPin.getOpenGLMatrix(mPin);
            it->model.model = glm::make_mat4(mPin);
            it->model.model = glm::scale(it->model.model, glm::vec3(1.0f, 1.0f, 1.0f));
            it++;
        }

        if(controlState == 3)
        {
            timeTicks++;
            //cout << "TIME TICKS: " << timeTicks << endl;
        }

        if (timeTicks >= ((25 - powerFactor) * 4) + 150)
        {
            for ( ; it2 != end2; )
            {
                if (controlState == 3)
                {
                    //cout << " : " << "PIN HEIGHT: " << transPin.getOrigin().y() << endl;
                    if ( (it2->getHeight() < 0.90)) //|| (it2->checkRotationAngle()))
                    {
                        removedPins.push_back(*it2);
                        it2  = pinObjects.erase(it2);
                        end2 = pinObjects.end();
                    }
                    else
                        it2++;
                }
                else
                {
                    it2++;
                }
            }
            timeTicks = 0;
            controlState = 4;
        }

        //cout << " SIZE: " << pinObjects.size() << endl;

        btVector3 position = transBall.getOrigin();
        ballPos = glm::vec3(mBall[12], 5.0, mBall[14]);
}

void Physics::resetPins()
{
    for (int i = 0; i < removedPins.size(); i++)
    {
        pinObjects.push_back(removedPins[i]);
    }

    removedPins.clear();

    for (int i = 0; i < pinObjects.size(); i++)
    {
        pinObjects[i].resetPosition();
    }
}

void Physics::cleanup() {
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i].cleanup(dynamicsWorld);
    }


    engine->drop();
    objects.clear();
    pinObjects.clear();
    removedPins.clear();
    delete broadphase;
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;
    delete dynamicsWorld;
}

void Physics::changeBall(string path)
{
    int ballCollidesWith =  COL_LANE | COL_PIN;
    BodyObject ballObject;

    // remove old ball
    dynamicsWorld->removeRigidBody(objects[1].rigidBody);

    // add new ball
    bool status = ballObject.initializeSphereObject(path, btVector3(-25, 5, 0), 5, btVector3(0,0.5,0), btVector3(0.5f, 0.5f, 0.5f), btVector3(1,1,1));
    if(!status){return;}
    dynamicsWorld->addRigidBody(ballObject.rigidBody, COL_BALL, ballCollidesWith);
    objects[1] = ballObject;
}
