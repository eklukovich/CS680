// This code is based on the example code provided by this link: http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html


/******************** Header Files ********************/
#include "model.h"



/************ Constructors and Destructors ************/
bool Model::loadModel(const std::string& filename)
   {
    // initialize varaibles
    bool status = false;
    Assimp::Importer importer;

    // read in the model file and store it as a scene
    const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate);
    
    // check if the scene has been loaded correctly
    if(scene)
      {
       // initialize the imported model
       status = initializeModel(scene, filename);
      }
    // throw and error for the model not loading correctly
    else
      {
       // display error for failed loading
       std::cout << "Error parsing " << filename << ": " << importer.GetErrorString() << std::endl;
      }

    return status;
}


bool Model::initializeModel(const aiScene* scene, const std::string& filename)
   { 
    // initialize variables 
    meshList.resize(scene->mNumMeshes);
    aiColor3D color(0.0f, 0.0f, 0.0f);
    aiMesh* mesh;
    aiMaterial* pMaterial;

    // loop and initialize all the meshes 
    for(unsigned int i = 0 ; i < meshList.size() ; i++)
       {
        // get the mesh from the scene
        mesh = scene->mMeshes[i];

        // get the material for the mesh
        pMaterial = scene->mMaterials[mesh->mMaterialIndex];

        // get the diffuse color from the material
        pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        // initialize the mesh
        initializeMesh(i, mesh, color);
       }

    // return
    return true;

   }


void  Model::initializeMesh(unsigned int index, const aiMesh* mesh, aiColor3D color)
   {
    // initialize variables
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    aiVector3D* pPos;
    Vertex v;
    

    // set the location of the material
    meshList[index].materialIndex = mesh->mMaterialIndex;

    // get all the mesh vertices and store them in the vertices vector
    for(unsigned int i = 0 ; i < mesh->mNumVertices ; i++) 
       {
        // get the vertice coordinate
        pPos = &(mesh->mVertices[i]);

        // store vertice position in vertex struct
        v.position[0] = pPos->x;
        v.position[1] = pPos->y;
        v.position[2] = pPos->z;

        // store vertice color in vertex struct
        v.color[0] = color.r;
        v.color[1] = color.g;
        v.color[2] = color.b;
        
        // store the vertex struct in a vector
        vertices.push_back(v);
       }

    // get all the mesh faces and store them in the indices vector
    for(unsigned int i = 0 ; i < mesh->mNumFaces ; i++) 
       {
        // get the face from the mesh
        const aiFace& face = mesh->mFaces[i];

        // check to make sure there are 3 indices
        assert(face.mNumIndices == 3);

        // store the indices in the indices vector
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
       }

    // store the mesh data in GPU memory
    meshList[index].Init(vertices, indices);
   }


void Model::renderModel(GLint loc_position, GLint loc_color)
   {
    // enable the vertex attributes for position and color
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);

    // loop through entire mesh list and render each mesh
    for(unsigned int i = 0; i < meshList.size(); i++)
       {
        // get the vertices from memory and bind them to be drawn
        glBindBuffer(GL_ARRAY_BUFFER, meshList[i].VB);

        // set up how the vertex position and color is stored
        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

        // get the indices from memory and bind them to be drawn
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshList[i].IB);

        // draw all the faces as triangles using indices
        glDrawElements(GL_TRIANGLES, meshList[i].numIndices, GL_UNSIGNED_INT, 0);
       }

    // disable the vertex attributes for position and color 
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);
   }

void Model::deleteModel()
   {
    // delete the vertices GPU memory
    glDeleteBuffers(1, &VB);

    // delete the indices GPU memory
    glDeleteBuffers(1, &IB);
   }
