Assignment 05 - Assimp Model Loader
Eric Klukovich
CS 680


This program takes in a model of a game board (board.obj) that was created in Blender and displays it on the screen. The .obj file is opened and all the data is loaded using the assimp library. The program now uses vertices and indices to render the object on the screen. Assimp also reads the mtl file for the object to add the appropriate color to the model, but if an mtl file is not given, the program defaults the model to gray.


To Compile the Program:
----------------------
1) Open a terminal and change to the assignment folder.
2) Navigate to the "build" folder. (cd build)
3) Once in build, simply type "make".


To Run the Program:
------------------
1) Navigate to the "bin" folder. (cd ../bin)
2) Once in bin, simply type "./Matrix -f <FRAGMENT_SHADER_FILE> -v <VERTEX_SHADER_FILE> -m <MODEL_FILE>". Example: "./Matrix -f fragment_shader.glsl -v vertex_shader.glsl -m board.obj"
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
Renders the model using indices.