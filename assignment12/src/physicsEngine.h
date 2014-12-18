#ifndef PHYSICS_H
#define PHYSICS_H

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include "bodyObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "bowlingPin.h"

#include <irrKlang.h>
#include "audio_files/conio.h"

using namespace std;
using namespace irrklang;

#define BIT(x) (1<<(x))

class Physics
{

    public:

        bool initializePhysicsEngine();
        bool setupWorld();
        void cleanup();
        void resetPins();
        void changeBall(string path);

        ISoundEngine* engine;


        void updateWorld(float dt);
        float getInterceptTime(btTransform puckTransform);

        vector<BodyObject> objects;
        vector<BowlingPin> pinObjects;
        vector<BowlingPin> removedPins;


        bool paused;
        int controlState;
        bool remove[10];
        int powerFactor;
        
        string levelFileName;
        glm::vec3 ballPos;
        glm::vec3 ball2Pos;
        glm::vec3 goalPos;

    private:

        btBroadphaseInterface* broadphase;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;

        enum collisiontypes {
            COL_NOTHING = 0,
            COL_BALL = BIT(1),
            COL_PLANE = BIT(2),
            COL_LANE = BIT (3),
            COL_PIN = BIT(4)
        };
};

#endif
