## Contents

1. [Project Presentation](#project-presentation)
2. [Scenario](#scenario)
3. [Implementation Details](#implementation-details)
4. [User Manual](#user-manual)
5. [Conclusions and Future Developments](#conclusions-and-future-developments)
6. [References](#references)

## Chapter 1: Project Presentation

The subject of this project is the photorealistic presentation of 3D objects using the OpenGL library. Open Graphics Library is a specification of a standard that broadly defines a multiplatform API used for programming the 2D and 3D components of computer programs. The scene I chose to implement is a maritime scene captured in the middle of an ocean. The user manipulates the object scene directly by using the mouse and keyboard and can move around them.

## Chapter 2: Scenario

1. **Scene and Object Description**

The scene presents a calm ocean, enveloping a tumultuous story. The 4 skeleton pirates (Andrew, Caesar, Johnny, Steven) are relaxing in the gentle breeze after a hard-fought battle they won.

In the depths of the ocean, the result of the battle can be seen, the remains of an enemy ship defeated by the four pirate friends, fading into the fog of the marine depths.

The dangers of the battle attracted a bloodthirsty maritime monster. The red shark with a split belly, the last of its kind, was drawn by the smell of death and is now swimming around the ship, hiding in the waves and waiting for the right moment to attack.

The pirates, however, know one thing for sure. Due to the buoy flashing a reddish light, they realize that a piece of land is nearby.

2. **Functionalities**

Using the keyboard keys or the mouse, the user can view the entire scene. They can also switch between different display modes, such as solid view and polygon/smooth faces. First, we can move the sun's position around the world to see the change of shadows. I implemented the ocean using a sinusoidal generation of waves, which was an easier variant of a tessellation shader generation model using Fourier transforms. Another feature added was the animation of the pirate ship, creating the impression that it is hit by waves and moves on them. Additionally, the pirates were animated to follow the ship's movement. The third important feature added was the addition of the buoy, which was transformed to imitate the movement of a buoy in water, and a red point light was added to give the impression of a warning light. The shark, using some sinusoidal rotations, was added along with fish and the ship to give a realistic impression. Finally, I implemented the fog effect in several places to create a more natural image.

## Chapter 3: Implementation Details

1. **Scene and Object Description**

- **Possible Solutions**

  One of the most important functions I used is `renderScene()`. This is responsible for setting up and drawing the scene. First, the function clears the color buffer and depth buffer of the framebuffer. Within `renderScene()`, I implemented the following functionalities:

  - Changing light intensity from code
  - Rotating the sun
  - Changing the skybox depending on the camera position
  - Creating two camera animations
  - Drawing objects

  In the `drawObjects()` method, we load each object and perform the necessary movements to position, scale, rotate, or animate them.

  Another important function is `initUniforms()`, as here we set the address of the variable locations found in shaders and initialize them with a specific value, such as light details, camera perspective, normal matrix. Without it, the program would not function correctly. Here we also specify the `initModels()` function, which we use to import a 3D object into OpenGL, and `initShaders()` where we load the shader contents. Each object will usually be drawn using a shader.

  Two methods that are fundamental to the scene are `initVBOs()` and `renderGrid()`, without which the ocean could not exist. In the first of the two methods, we create a VAO where we will store VBOs containing information about each point. We initialize a point mesh for which we calculate texCoords and x and y coordinates. We then calculate the triangle indices that we will send using EBO. The `renderGrid()` method will use the shaders (vertex and fragment) described for this, to which we send variables such as view, model, projection, normal matrix, and wave simulation time as uniform variables. Finally, we texture the mesh and draw the ocean.

  The algorithms used include shadowMapping for shadows and Phong for lighting.

- **Reason for the chosen approach**

  During the semester, we learned how to use all the things I included in this project, so it was a good practice exercise. I also decided to implement a water scene because I saw a YouTube video about generating waves using Tessellation shaders and Fourier transforms, and it seemed like an interesting topic from which I could learn a lot.

2. **Graphic Models**

The objects and textures were downloaded from the internet and imported into Blender, then exported to code. It was a laborious process since many objects did not map textures correctly. I tried texture baking, but often this did not solve the problems.

3. **Data Structures**

The data structures used are the ones we used throughout the labs.

4. **Class Hierarchy**

- **Camera.cpp**
  Responsible for creating and initializing the camera.

- **Mesh.cpp**
  Used to describe a 3D object using vertices and textures.

- **Model3D.cpp**
  Contains methods for creating meshes using shader programs.

- **main.cpp**
  The application itself.

- **Shader.cpp**
  Contains boilerplate code for shaders.

- **SkyBox.cpp**
  Contains shader implementation

## Chapter 4: User Manual

The user can interact with the application using the following buttons:

- 0 - start moving the sun
- J - move Sun left
- L - move Sun right
- 5 - fill mode
- 6 - point mode
- 7 - line mode
- 8 - increase sunlight
- 9 - decrease sunlight
- A - move camera to the left
- D - move camera to the right
- W - move camera to the front
- S - move camera backwards
- / - start animation from upside
- , - stop animation from upside
- upkey - start animation from bottom
- downkey - stop animation from bottom

## Chapter 5: Conclusions and Future Developments

One of my plans for this project in the future is to add collisions and object control in the scene.
