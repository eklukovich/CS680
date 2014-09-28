# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL -lassimp

# For Macs uncomment the next line and comment out the previous one
#CC=clang++
#LIBS=  -framework OpenGL -framework GLUT -framework Cocoa -lGLEW -stdlib=libc++
# Assuming you want to use a recent compiler

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/Matrix

../bin/Matrix: ../src/main.cpp ../build/mesh.o ../build/shader.o ../build/model.o
	$(CC) $(CXXFLAGS) ../build/mesh.o ../build/shader.o ../build/model.o ../src/main.cpp -o ../bin/Matrix $(LIBS)


../build/shader.o: ../src/shader.cpp
	g++ -c ../src/shader.cpp

../build/model.o: ../src/model.cpp ../build/mesh.o
	g++ -c ../src/model.cpp	

../build/mesh.o: ../src/mesh.cpp
	g++ -c ../src/mesh.cpp	

clean:
	rm -rf *o ../bin/Matrix
