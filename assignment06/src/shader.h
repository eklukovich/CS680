#ifndef SHADER_H
#define	SHADER_H

#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <string>
#include <fstream>
#include <iostream>


class shader{

   public:

      shader(GLenum shaderType);
      bool setupShader(std::string fileName);
      GLuint getShader();

   private:

      const char * readShaderFile(std::string fileName);
      GLint compileShader(std::string shaderSrc);

      GLuint shaderData;
      GLenum type;
};
#endif