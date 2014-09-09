Assignment 02 - User Interaction
Eric Klukovich
CS 680


This program adds user interaction (mouse/keyboard) controls to the program. All movement features only affect the center cube. The color changing option can be done to both of the cubes.


To Compile the Program:
----------------------
1) Open a terminal and change to the assignment folder.
2) Navigate to the "build" folder. (cd build)
3) Once in build, simply type "make".


To Run the Program:
------------------
1) Navigate to the "bin" folder. (cd ../bin)
2) Once in bin, simply type "./Matrix -f fragment_shader.glsl -v vertex_shader.glsl"
3) The program should now be running. If it doesn't run, check the shader file names.


Program Controls:
----------------
Keyboard:

   'a' - Rotate center cube counter clockwise
   's' - Rotate center cube clockwise
   Right and Up Arrow Keys - Increase center cube's rotation speed
   Left and Down Arrow Keys - Decrease center cube's rotation speed
   ESC - Closes the program

Mouse:
   
   Left Mouse Click - Toggle center cube's rotation direction
   Right Mouse Click - Open menu


Menu Options:
------------
Start Rotation - Make the center cube rotate along Y-axis
Stop Rotation - Make the center cube stop rotating
Change Cube Color - Opens a submenu that lets the user select which cube to change color. Another submenu opens and the color can be selected.
Quit - Closes the program.
