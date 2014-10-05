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
        initializeMesh(i, scene, color);
       }

    // return
    return true;

   }


void  Model::initializeMesh(unsigned int index, const aiScene* scene, aiColor3D color)
   {
    // initialize variables
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    aiVector3D* position;
    Vertex v;
   
    // get the correct mesh
    aiMesh * mesh = scene->mMeshes[index];    

    // set the location of the material
    meshList[index].materialIndex = mesh->mMaterialIndex;

    // get all the mesh vertices and store them in the vertices vector
    for(unsigned int i = 0 ; i < mesh->mNumVertices ; i++) 
       {
        // get the vertice coordinate
        position = &(mesh->mVertices[i]);

        // store vertice position in vertex struct
        v.position[0] = position->x;
        v.position[1] = position->y;
        v.position[2] = position->z;

        // store vertice color in vertex struct

        if(mesh->HasTextureCoords(0)) 
          {
            aiVector3D textureCoord = mesh->mTextureCoords[0][i];
            v.uv[0] = textureCoord.x;
            v.uv[1] = textureCoord.y;  
          }
        
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

    // get texture
    initializeMaterials(scene, index);

    // store the mesh data in GPU memory
    meshList[index].Init(vertices, indices);
   }


void Model::initializeMaterials(const aiScene* scene, unsigned meshIndex)
   {
    // initialize variables
    aiString filepath;
    fipImage image;

    // get the material info for the mesh
    const aiMaterial* material = scene->mMaterials[meshList[meshIndex].materialIndex];

    if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
      {
       if(material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
         {
          // load texture image  
          if(image.load(filepath.C_Str()))
            {
              // convert to a 32 bit image
              image.convertTo32Bits();

              // get the pixels from memory 
              char* pixels = (char*)image.accessPixels();

              // get and store image size, width, and height
              long imageSize = image.getImageSize();
              meshList[meshIndex].imageWidth = image.getWidth();
              meshList[meshIndex].imageHeight = image.getHeight();
              int size = meshList[meshIndex].imageWidth*meshList[meshIndex].imageHeight;

              // store the pixels into the mesh class
              meshList[meshIndex].image = new GLubyte[4*size];
              for(int j= 0; j<size; j++)
                 {
                  meshList[meshIndex].image[j*4+0]= pixels[j*4+0];
                  meshList[meshIndex].image[j*4+1]= pixels[j*4+1];
                  meshList[meshIndex].image[j*4+2]= pixels[j*4+2];
                  meshList[meshIndex].image[j*4+3]= pixels[j*4+3];
                 }

              // put texture into memory and apply parameters 
              glGenTextures(1, &meshList[meshIndex].TB);
              glActiveTexture(GL_TEXTURE0);
              glBindTexture(GL_TEXTURE_2D, meshList[meshIndex].TB);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
          else
            {
             std::cout << "Texture: " << filepath.C_Str() << " failed to load" << std::endl;
            }
         }
      }
   }


void Model::renderModel(GLint loc_position, GLint loc_texture)
   {
    // enable the vertex attributes for position and color
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_texture);

    // loop through entire mesh list and render each mesh
    for(unsigned int i = 0; i < meshList.size(); i++)
       {
        // get the vertices from memory and bind them to be drawn
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshList[i].VB);

        // set up how the vertex position and textures are stored
        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(loc_texture, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,uv));

        // get the indices from memory and bind them to be drawn
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshList[i].IB);

        // enable and bind texture
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshList[i].TB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, meshList[i].imageWidth, meshList[i].imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)meshList[i].image );

        // draw all the faces as triangles using indices
        glDrawElements(GL_TRIANGLES, meshList[i].numIndices, GL_UNSIGNED_INT, 0);
       }

    // disable the vertex attributes for position and color 
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_texture);
   }


void Model::deleteModel()
   {
    for(int i = 0; i < meshList.size(); i++)
    {
     // delete the vertices GPU memory
     glDeleteBuffers(1, &meshList[i].VB);

     // delete the indices GPU memory
     glDeleteBuffers(1, &meshList[i].IB);
    }

   }
