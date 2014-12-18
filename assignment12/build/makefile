# Linux
CC=g++
LIBS= -I/usr/include/bullet -lglut -lGLEW -lGL -lassimp -lfreeimageplus -lBulletDynamics -lBulletSoftBody -lBulletCollision -lLinearMath
OPTS= -I"../src/audio_files/include" -L"/usr/lib" ../src/audio_files/libIrrKlang.so -pthread


# For Macs uncomment the next line and comment out the previous one
#CC=clang++
#LIBS=  -framework OpenGL -framework GLUT -framework Cocoa -lGLEW -stdlib=libc++
# Assuming you want to use a recent compiler

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/Bowling

../bin/Bowling: ../src/main.cpp ../build/mesh.o ../build/model.o ../build/shader.o ../build/camera.o ../build/physicsEngine.o ../build/bodyObject.o ../build/lightingEngine.o ../build/bowlingPin.o
	$(CC) $(CXXFLAGS)  ../build/physicsEngine.o ../build/shader.o ../build/camera.o ../build/mesh.o ../build/model.o ../build/bodyObject.o ../build/bowlingPin.o ../build/lightingEngine.o ../src/main.cpp -o ../bin/Bowling $(LIBS) $(OPTS)

../build/model.o: ../src/model.cpp ../build/mesh.o
	$(CC) -c ../src/model.cpp	$(LIBS)

../build/mesh.o: ../src/mesh.cpp
	$(CC) -c ../src/mesh.cpp

../build/shader.o: ../src/shader.cpp
	$(CC) -c ../src/shader.cpp

../build/camera.o: ../src/camera.cpp
	$(CC) -c ../src/camera.cpp

../build/physicsEngine.o: ../src/physicsEngine.cpp 
	$(CC) -c ../src/physicsEngine.cpp	$(LIBS) $(OPTS)

../build/bodyObject.o: ../src/bodyObject.cpp
	$(CC) -c ../src/bodyObject.cpp	$(LIBS) $(OPTS)

../build/lightingEngine.o: ../src/lightingEngine.cpp 
	$(CC) -c ../src/lightingEngine.cpp	

../build/bowlingPin.o: ../src/bowlingPin.cpp 
	$(CC) -c ../src/bowlingPin.cpp	$(LIBS) $(OPTS)

clean:
	rm -rf *o ../bin/Bowling
