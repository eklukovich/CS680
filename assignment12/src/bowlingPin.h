#ifndef BOWLING_PIN_H
#define BOWLING_PIN_H


#include <iostream>
#include <btBulletDynamicsCommon.h>
#include "model.h"

#include <irrKlang.h>
#include "audio_files/conio.h"


using namespace irrklang;
using namespace std;

class BowlingPin {

    public:

        //BowlingPin(float x, float y, float z);

        bool initializeBowlingPinObject(string fileName, btVector3 loc, btScalar mass, btVector3 inertia, btVector3 scale);
        void cleanup(btDiscreteDynamicsWorld* dynamicsWorld);

        float getHeight();
        bool checkRotationAngle();
        void resetPosition();
        
        Model model;

        btCollisionShape* shape;
        btDefaultMotionState* motionState;
        btRigidBody* rigidBody;

        btVector3 location;
};
#endif