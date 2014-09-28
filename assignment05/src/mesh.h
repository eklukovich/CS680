// This code is based on the example code provided by this link: http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html

#ifndef MESH_H
#define	MESH_H

#include <GL/glew.h>
#include <GL/glut.h> 
#include <vector>
#include "vertex.h"

class Mesh {

    public:

        void Init(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices);

        GLuint VB;
        GLuint IB;
        unsigned int numIndices;
        unsigned int materialIndex;
};
#endif

