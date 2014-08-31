#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <string>
using namespace std;

class shader{

   public:

      shader(GLenum shaderType);

      bool setupShader(char * fileName);
     
      GLuint getShader();

     

   private:
      GLuint shaderData;
      GLenum type;

      const char * readShaderFile(char * fileName);
      GLint compileShader(const char * shaderSrc);

};
