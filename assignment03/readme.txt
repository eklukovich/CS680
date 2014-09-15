Assignment 03 - Moons
Eric Klukovich
CS 680


This program adds a second cube and the new cube orbits around the old cube. Arrow key interaction has been added to change the rotation direction and text has been rendered on the screen to show what direction the cube is rotating (clockwise or counter clockwise).


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
   Left Arrow Key - Rotate center cube counter clockwise
   Right Arrow Keys - Rotate center cube clockwise
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


Extra Credit:
------------
The solar system data file has been created for extra credit, it is located in the bin folder (solar_system_data.txt). The format for the data file is described at the top of the file.
