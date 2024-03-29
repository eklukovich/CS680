// This code is based on the example code provided by this link: http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html


/******************** Header Files ********************/
#include "model.h"



/************ Constructors and Destructors ************/
bool Model::loadModel(const string& filename)
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
       cout << "Error parsing " << filename << ": " << importer.GetErrorString() << endl;
      }

    return status;
}


bool Model::initializeModel(const aiScene* scene, const string& filename)
   {
    // initialize variables
    meshList.resize(scene->mNumMeshes);
    aiColor3D color(0.0f, 0.0f, 0.0f);
    aiMesh* mesh;
    aiMaterial* pMaterial;

    // loop and initialize all the meshes
    mTriMesh = new btTriangleMesh();
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
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    aiVector3D* position;
    aiVector3D* position2;
    textureVertex v;

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

        // Then add the triangle to the mesh:

        // store vertice color in vertex struct
        if(mesh->HasTextureCoords(0))
          {
            aiVector3D textureCoord = mesh->mTextureCoords[0][i];
            v.uv[0] = textureCoord.x;
            v.uv[1] = textureCoord.y;
          }

        // retrieve the normals so they can be used in lighting computations
        if (mesh->HasNormals()) {
            const aiVector3D* normal = &(mesh->mNormals[i]);
            v.normal[0] = normal->x;
            v.normal[1] = normal->y;
            v.normal[2] = normal->z; 
        }

        // store the vertex struct in a vector
        meshList[index].vertices.push_back(v);
       }

    // get all the mesh faces and store them in the indices vector
    for(unsigned int i = 0 ; i < mesh->mNumFaces ; i++)
       {

        // get the face from the mesh
        const aiFace& face = mesh->mFaces[i];

        // check to make sure there are 3 indices
        //cout << face.mNumIndices << endl;
        assert(face.mNumIndices == 3);

        // store the indices in the indices vector
        meshList[index].indices.push_back(face.mIndices[0]);
        meshList[index].indices.push_back(face.mIndices[1]);
        meshList[index].indices.push_back(face.mIndices[2]);

        position = &(mesh->mVertices[face.mIndices[0]]);
        btVector3 v0(position->x, position->y, position->z);
        position = &(mesh->mVertices[face.mIndices[1]]);
        btVector3 v1(position->x, position->y, position->z);
        position = &(mesh->mVertices[face.mIndices[2]]);
        btVector3 v2(position->x, position->y, position->z);
        mTriMesh->addTriangle(v0,v1,v2);
       }


    // get texture
    initializeMaterials(scene, index);

    // store the mesh data in GPU memory
    meshList[index].setData();
   }


   void Model::initializeMaterials(const aiScene* scene, unsigned meshIndex)
   {
    // initialize variables
    aiString filepath;

    // get the material info for the mesh
    const aiMaterial* material = scene->mMaterials[meshList[meshIndex].materialIndex];

    if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
     if(material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
     {
      // load texture image
      if(meshList[meshIndex].image.load(filepath.C_Str()))
      {
        meshList[meshIndex].image.convertTo32Bits();

        // bind the texture data buffer
        glGenTextures(1, &meshList[meshIndex].TB);
        glBindTexture(GL_TEXTURE_2D, meshList[meshIndex].TB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, meshList[meshIndex].image.getWidth(), meshList[meshIndex].image.getHeight(),
          0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)meshList[meshIndex].image.accessPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      }
      else
      {
       cout << "Texture: " << filepath.C_Str() << " failed to load" << endl;
     }
   }
 }
}

void Model::renderModel(glm::mat4 mvp, glm::mat4 model, LightingEngine lightingEngine, glm::vec3 ballPos, glm::vec3 cameraPos)
{
    // set light values on or off
    lightingEngine.toggleAmbientLight();
    lightingEngine.toggleDiffuseLight();

    lightingEngine.toggleSpecularLight();
    lightingEngine.togglePointLight();
    lightingEngine.toggleSpotLight();

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(lightingEngine.loc_vertexPosition);
    glEnableVertexAttribArray(lightingEngine.loc_vertexTexture);
    glEnableVertexAttribArray(lightingEngine.loc_vertexNormal);

    for (unsigned int i = 0 ; i < meshList.size() ; i++) {
        lightingEngine.setMVP(mvp);
        lightingEngine.setModelMatrix(model);
        lightingEngine.setCameraPos(cameraPos);
        lightingEngine.setMatSpecularIntensity();
        lightingEngine.setMatSpecularPower(32.0f);

        lightingEngine.spotLights[0].Position = ballPos;
        lightingEngine.spotLights[1].Position = glm::vec3(24.5,0.96,0);
        //lightingEngine.spotLights[0].Position = glm::vec3(6.5f,0.0f,11.50f);

        // directional light
        lightingEngine.setDirectionalLight(lightingEngine.directLight);

        // point lights
        lightingEngine.setPointLights(1, lightingEngine.pointLights);

        // spot lights
        lightingEngine.setSpotLights(2, lightingEngine.spotLights);

        // bind vertex buffer data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshList[i].VB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertex) * meshList[i].vertices.size(), &meshList[i].vertices[0], GL_STATIC_DRAW);

        //set pointers into the vbo for each of the attributes(position and color)
        glVertexAttribPointer(lightingEngine.loc_vertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(textureVertex), 0);
        glVertexAttribPointer(lightingEngine.loc_vertexTexture, 2, GL_FLOAT, GL_FALSE, sizeof(textureVertex), (void*)offsetof(textureVertex,uv));
        glVertexAttribPointer(lightingEngine.loc_vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(textureVertex), (void*)offsetof(textureVertex,normal));        

        // bind indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshList[i].IB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshList[i].numIndices, &meshList[i].indices[0], GL_STATIC_DRAW);

        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshList[i].TB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, meshList[i].image.getWidth(), meshList[i].image.getHeight(), 
            0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)meshList[i].image.accessPixels());

        // draw elements by using indexing
        glDrawElements(GL_TRIANGLES, meshList[i].numIndices, GL_UNSIGNED_INT, 0);
    }

    // clean up
    glDisableVertexAttribArray(lightingEngine.loc_vertexPosition);
    glDisableVertexAttribArray(lightingEngine.loc_vertexTexture);
    glDisableVertexAttribArray(lightingEngine.loc_vertexNormal);
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
    meshList.clear();

   }
