# Linux
CC=g++
LIBS= -lglut -lGLEW -lGL

# For Macs uncomment the next line and comment out the previous one
#CC=clang++
#LIBS=  -framework OpenGL -framework GLUT -framework Cocoa -lGLEW -stdlib=libc++
# Assuming you want to use a recent compiler

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/Matrix

../bin/Matrix: ../src/main.cpp ../build/shader.o
	$(CC) $(CXXFLAGS) ../build/shader.o ../src/main.cpp -o ../bin/Matrix $(LIBS)


../build/shader.o: ../src/shader.cpp
	g++ -c ../src/shader.cpp

clean:
	rm -rf ../build/*o hello
