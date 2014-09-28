/******************** Header Files ********************/
#include "shader.h"




/************ Constructors and Destructors ************/
shader::shader(GLenum shaderType){

   shaderData = glCreateShader(shaderType);
   type = shaderType;

}


bool shader::setupShader(std::string fileName)
   {
    // initialize variables
    const char * shaderSrc;
    GLint status;

    // 
    shaderSrc = readShaderFile(fileName);

    // check if src has been read in
    if(shaderSrc == NULL)
      return false;
 
    // compile shader
    status = compileShader(shaderSrc);

    // 
    if(!status)
      {
       if(type == GL_VERTEX_SHADER)
          std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
       else
          std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
       return false;
      }

    // return
    return true;
}


// read shader string from file
const char * shader::readShaderFile(std::string fileName){

   // initialize variables
      std::ifstream fin;
      std::string line, data;

   // open file
      fin.open(fileName.c_str());

      if(fin.is_open())
	{
	 while(!fin.eof())
	   {
	    getline(fin, line);
	    data += line;
	   }
	 fin.close();
         return data.c_str();
	}

   // return  
      std::cout << "Cannot Open File: " << fileName << std::endl;
      return NULL;
}


GLint shader::compileShader(std::string shaderSrc){

    // initialize variables
       GLint shader_status; 

    // Vertex shader first
       const char * src = shaderSrc.c_str();
       glShaderSource(shaderData, 1, &src, NULL);
       glCompileShader(shaderData);

    // check the compile status
       glGetShaderiv(shaderData, GL_COMPILE_STATUS, &shader_status);

    // return compile status
       return shader_status;
}


GLuint shader::getShader(){

   return shaderData;

}










