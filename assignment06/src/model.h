// This code is based on the example code provided by this link: http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html


#ifndef MODEL_H
#define	MODEL_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "vertex.h"
#include <assert.h>
#include <FreeImagePlus.h>


class Model {

	public:

		bool loadModel(const std::string& Filename);
		void renderModel(GLint loc_position, GLint loc_color);
		void deleteModel();

	private:

		bool initializeModel(const aiScene* scene, const std::string& filename);
		void initializeMesh(unsigned int index, const aiScene* scene, aiColor3D color);
		void initializeMaterials(const aiScene* pScene, unsigned index);

		GLuint VB;
		GLuint IB;
		unsigned int numIndices;
		unsigned int MaterialIndex;

		std::vector<Mesh> meshList;
};
#endif