#include "bowlingPin.h"

bool BowlingPin::initializeBowlingPinObject(string fileName, btVector3 loc, btScalar mass, btVector3 inertia, btVector3 scale)
	{
		if(!model.loadModel(fileName))
			{
				return false;
			}

		location = loc;

		btCompoundShape * compoundShape = new btCompoundShape();
		btCollisionShape * tempShape;
		btTransform suppLocalTrans, centerOfMass;

		// create bottom cylinder shape
		tempShape = new btCylinderShape (btVector3(0.5, 1.0, 0.5));
		suppLocalTrans.setIdentity();
		suppLocalTrans.setOrigin(btVector3(0,0.06,0));
		compoundShape->addChildShape (suppLocalTrans, tempShape);

		// create cone shape
		btCollisionShape * tempShape1 = new btConeShape (0.3375, 0.5);
		suppLocalTrans.setIdentity();
		suppLocalTrans.setOrigin(btVector3(0,0.3,0));
		compoundShape->addChildShape (suppLocalTrans, tempShape1);

		// create top cylinder shape
		btCollisionShape * tempShape2 = new btCylinderShape (btVector3(0.38, 0.75, 0.38));
		suppLocalTrans.setIdentity();
		suppLocalTrans.setOrigin(btVector3(0,0.6,0));
		compoundShape->addChildShape (suppLocalTrans, tempShape2);

		// create sphere shape
		btCollisionShape * tempShape3 = new btSphereShape (0.19);
		suppLocalTrans.setIdentity();
		suppLocalTrans.setOrigin(btVector3(0,1.70,0));
		compoundShape->addChildShape (suppLocalTrans, tempShape3);



		compoundShape->setLocalScaling(scale);
		motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), loc));
		compoundShape->calculateLocalInertia(mass, inertia);
		btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, motionState, compoundShape, inertia);
		sphereRigidBodyCI.m_restitution = 0.7f;
		sphereRigidBodyCI.m_friction = 2.f;


		rigidBody = new btRigidBody(sphereRigidBodyCI);

		centerOfMass.setIdentity();
		centerOfMass.setOrigin(btVector3(loc.x(), 1.0f, loc.z()));
		rigidBody->setCenterOfMassTransform(centerOfMass);
		rigidBody->setLinearFactor(btVector3(1,1,1));
		rigidBody->setAngularFactor(btVector3(1,1,1));
		rigidBody->setDamping(btScalar(0.0f), btScalar(0.6));
		rigidBody->forceActivationState(DISABLE_DEACTIVATION);

		return true;
	}


float BowlingPin::getHeight()
{
	btTransform transPin;
	rigidBody->getMotionState()->getWorldTransform(transPin);
	return transPin.getOrigin().y();
}

bool BowlingPin::checkRotationAngle()
{
	btTransform transPin;
	rigidBody->getMotionState()->getWorldTransform(transPin);

	btQuaternion q = transPin.getRotation();
	cout << "Q: " << q.x() << " | " << q.y() << " | " << q.z() << endl;
	return (q.x() < -0.5 || q.x() > 0.5) || (q.y() < -0.5 || q.y() > 0.5) || (q.z() < -0.5 || q.z() > 0.5);
}

void BowlingPin::resetPosition()
{
  btTransform transform;
  btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), location));
  rigidBody->setMotionState(motionState);
  rigidBody->setLinearVelocity(btVector3(0,0,0));
}
