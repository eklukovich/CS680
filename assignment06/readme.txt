Assignment 06 - Assimp Texture Loader
Eric Klukovich
CS 680


This program takes in a model and will load it and apply textures to the model and then displays it on the screen. The .obj file is opened and all the data is loaded using the assimp library, and the texture image is loaded in using FreeImage. Assimp also reads the mtl file for the object to find the filename for the texture. The appropriate data is set and the model and the texture will be displayed on the screen. There are two demo models that have textures in the bin folder (earth.obj and woodCube.obj).

Compilation Requirements:
------------------------
Assimp Library
Freeimage Library


To Compile the Program:
----------------------
1) Open a terminal and change to the assignment folder.
2) Navigate to the "build" folder. (cd build)
3) Once in build, simply type "make".


To Run the Program:
------------------
1) Navigate to the "bin" folder. (cd ../bin)
2) Once in bin, simply type "./Matrix -f <FRAGMENT_SHADER_FILE> -v <VERTEX_SHADER_FILE> -m <MODEL_FILE>". Example: "./Matrix -f fragment_shader.glsl -v vertex_shader.glsl -m earth.obj"
3) The program should now be running. If it doesn't run, check the shader/model file names.


Program Controls:
----------------
Keyboard:

   'a' - Translate board counter clockwise
   's' - Translate board clockwise
   Left Arrow Key - Translate board counter clockwise
   Right Arrow Keys - Translate board clockwise
   ESC - Closes the program

Mouse:
   
   Left Mouse Click - Toggle center cube's rotation direction
   Right Mouse Click - Open menu


Menu Options:
------------
Start Rotation - Makes the board start orbiting
Stop Rotation - Makes the board start orbiting
Quit - Closes the program.


Extra Credit:
------------
None