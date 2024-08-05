//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>


int glWindowWidth = 1920;
int glWindowHeight = 1080;
GLfloat currentLightAngle = 0;
int retina_width, retina_height;

float light = 0.2f;

GLFWwindow *glWindow = NULL;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

const GLfloat near_plane = 0.5f, far_plane = 80.0f;
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint gridTexture;

GLint gridTextureLoc;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;

GLuint normalMatrixLoc;
glm::mat4 lightRotation;
glm::vec3 lightDir;
GLuint lightDirLoc;

glm::vec3 lightColor;

GLuint lightColorLoc;
gps::Camera myCamera(
        glm::vec3(3.0f, 2.0f, 5.5f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
float moveLeftRight, moveForwardBackward = 0.0f;
float lastX, lastY;
float pitch;
float yaw;
bool firstMouse = true;

float sensitivity = 0.0005f;
float cameraSpeed = 0.5f;
bool pressedKeys[1024];


float angleY = 0.0f;
GLfloat lightAngle;
gps::Model3D nanosuit;
gps::Model3D nanosuit1;
gps::Model3D nanosuit2;
gps::Model3D nanosuit4;
gps::Model3D fishBank[10];
gps::Model3D ship;
gps::Model3D buoy;
gps::Model3D ground;
gps::Model3D luffy;
gps::Model3D chopper;
gps::Model3D shark;
gps::Model3D ghostShip;

gps::Model3D lightCube;
gps::Model3D screenQuad;

gps::SkyBox mySkyBox;

gps::Shader skyboxShader;
gps::SkyBox myNightSkyBox;
gps::SkyBox myUnderWaterSkyBox;
gps::Shader myCustomShader;

gps::Shader lightShader;

gps::Shader waveShader;
GLuint gridPointsVBO;

GLuint gridTrianglesEBO;
GLuint gridVAO;
const float GRID_WIDTH = 100.0f;
const float GRID_HEIGHT = 100.0f;

const int GRID_NUM_POINTS_WIDTH = 300;
const int GRID_NUM_POINTS_HEIGHT = 300;

gps::Shader screenQuadShader;

gps::Shader depthMapShader;
GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

const int oceanSize = 256;
const float oceanFrequency = 0.1;

const float oceanAmplitude = 0.5;

std::vector<const GLchar *> faces;
std::vector<const GLchar *> underwaterFaces;
std::vector<const GLchar *> nightFaces;

//simulation time
float simTime;
GLint simTimeLoc;

// matrices
glm::mat4 waveModel;
glm::mat4 waveView;
glm::mat4 waveProjection;
glm::mat3 waveNormalMatrix;

// light parameters
glm::vec3 waveLightDir;
glm::vec3 waveLightColor;

// shader uniform locations
GLint waveModelLoc;
GLint waveViewLoc;
GLint waveProjectionLoc;
GLint waveNormalMatrixLoc;
GLint waveLightDirLoc;
GLint waveLightColorLoc;

void initSkyBoxTextures() {
    faces.push_back("skybox/px.png");
    faces.push_back("skybox/nx.png");
    faces.push_back("skybox/py.png");
    faces.push_back("skybox/ny.png");
    faces.push_back("skybox/pz.png");
    faces.push_back("skybox/nz.png");
}

void initUnderWaterSkyBoxTextures() {
    underwaterFaces.push_back("skybox/px (1).png");
    underwaterFaces.push_back("skybox/nx (1).png");
    underwaterFaces.push_back("skybox/py (1).png");
    underwaterFaces.push_back("skybox/ny (1).png");
    underwaterFaces.push_back("skybox/pz (1).png");
    underwaterFaces.push_back("skybox/nz (1).png");
}

void initNightSkyBoxTextures() {
    nightFaces.push_back("skybox/px (2).png");
    nightFaces.push_back("skybox/nx (2).png");
    nightFaces.push_back("skybox/py (2).png");
    nightFaces.push_back("skybox/ny (2).png");
    nightFaces.push_back("skybox/pz (2).png");
    nightFaces.push_back("skybox/nz (2).png");
}

void generateOcean() {
    // Set up the vertices for the ocean grid
    GLfloat vertices[oceanSize][oceanSize][3];
    for (int x = 0; x < oceanSize; x++) {
        for (int z = 0; z < oceanSize; z++) {
            vertices[x][z][0] = x;
            vertices[x][z][1] = 0.0;
            vertices[x][z][2] = z;
        }
    }

    // Generate the sinewaves for the ocean surface
    for (int x = 0; x < oceanSize; x++) {
        for (int z = 0; z < oceanSize; z++) {
            vertices[x][z][1] = oceanAmplitude * sin(oceanFrequency * x + oceanFrequency * z);
        }
    }

    // Enable vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    // Draw the ocean as a set of triangle strips
    for (int x = 0; x < oceanSize - 1; x++) {
        glDrawArrays(GL_TRIANGLE_STRIP, x * oceanSize, oceanSize * 2);
    }

    // Disable vertex arrays
    glDisableClientState(GL_VERTEX_ARRAY);
}

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
    //TODO
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            pressedKeys[key] = true;
        else if (action == GLFW_RELEASE)
            pressedKeys[key] = false;
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {

    if (firstMouse) {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }
    float xDiff = (float) xpos - lastX;
    float yDiff = (float) ypos - lastY;
    lastX = (float) xpos;
    lastY = (float) ypos;

    xDiff *= sensitivity;
    yDiff *= sensitivity;

    // printf("x=%f, y= %f", xDiff, yDiff);

    yaw -= xDiff;
    pitch -= yDiff;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myCustomShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}


bool moveSun = false;
bool present = false;
float cameraAngle = 0.0f;
bool underPresent = false;

void processMovement() {
    if (pressedKeys[GLFW_KEY_Q]) {
        angleY -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angleY += 1.0f;

    }

    if (pressedKeys[GLFW_KEY_0]) {
        if (moveSun) {
            moveSun = false;
        } else {
            moveSun = true;
        }
    }

    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
        /*lightDirAux -= 0.01f;
		lightDir = glm::vec3(lightDirAux, 1.0f, 1.5f);*/
    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 1.0f;
        /*lightDirAux += 0.01f;
        lightDir = glm::vec3(lightDirAux, 1.0f, 1.5f);*/
    }

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_5]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        printf("x: %f, y: %f, z: %f", myCamera.cameraPosition.x, myCamera.cameraPosition.y, myCamera.cameraPosition.z);
        printf("\n");
    }

    if (pressedKeys[GLFW_KEY_6]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_7]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_8]) {
        std::cout << light << std::endl;
         //white light
        light += 0.1;
    }

    if (pressedKeys[GLFW_KEY_9]) {
        std::cout << light << std::endl;
        light -= 0.1;
    }

    if (pressedKeys[GLFW_KEY_SLASH]) {
        present = true;
    }

    if (pressedKeys[GLFW_KEY_COMMA]) {
        present = false;
    }
    if (pressedKeys[GLFW_KEY_UP]) {
        underPresent = true;
    }

    if (pressedKeys[GLFW_KEY_DOWN]) {
        underPresent = false;
    }

}

bool initOpenGLWindow() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwGetPrimaryMonitor()
    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);

    glfwSwapInterval(1);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte *version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    //for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    return true;
}

void initOpenGLState() {
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glViewport(0, 0, retina_width, retina_height);

    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

    glEnable(GL_FRAMEBUFFER_SRGB);
}

gps::Model3D underWaterSquare;

void initObjects() {
    nanosuit.LoadModel("objects/pirates/skeleteon/skeleton.obj");
    nanosuit1.LoadModel("objects/pirates/skeleteon/skeleton.obj");
    nanosuit2.LoadModel("objects/pirates/skeleteon/skeleton.obj");
    nanosuit4.LoadModel("objects/pirates/skeleteon/skeleton.obj");
    ground.LoadModel("objects/ground/LAKE.obj");
    lightCube.LoadModel("objects/cube/cube.obj");
    screenQuad.LoadModel("objects/quad/quad.obj");
    ship.LoadModel("objects/ship/Ship_01B_Close_Sails.obj");
    buoy.LoadModel("objects/buoy/untitled.obj");
    shark.LoadModel("objects/Low_fish/shark.obj");
    initSkyBoxTextures();
    initUnderWaterSkyBoxTextures();
    initNightSkyBoxTextures();
    mySkyBox.Load(faces);
    myNightSkyBox.Load(nightFaces);
    myUnderWaterSkyBox.Load(underwaterFaces);
    for (auto & i : fishBank) {
        i.LoadModel("objects/fish/02_02_position2.obj");
    }
    ghostShip.LoadModel("objects/ship/ghostShip/ghostShip.obj");
}

float colorRed = 15.0f;

bool incrementColor = false;

float colorRedTimeStamp = 0;

float animateBuoyLight(float left, float right, float inc) {
    float currentTimeStamp = glfwGetTime();
    if (currentTimeStamp - colorRedTimeStamp > 0.01f) {
        if (colorRed > left && incrementColor) {
            colorRed -= inc;
        }
        if (colorRed <= left) {
            incrementColor = false;
        }
        if (colorRed < right && !incrementColor) {
            colorRed += inc;
        }
        if (colorRed >= right) {
            incrementColor = true;
        }
        colorRedTimeStamp = currentTimeStamp;
    }
    // std::cout<<colorRed<<std::endl;
    return colorRed;
}

void initShaders() {
    myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    depthMapShader.useShaderProgram();
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    waveShader.loadShader("shaders/basicWave.vert", "shaders/basicWave.frag");
    waveShader.useShaderProgram();
}

void initUniforms() {
    myCustomShader.useShaderProgram();

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "cameraPos"), 1,
                 glm::value_ptr(myCamera.cameraPosition));

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    projection = glm::perspective(glm::radians(45.0f), (float) retina_width / (float) retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 2.0f, 2.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    printf("lightDir %f %f %f\n", lightDir.x, lightDir.y, lightDir.z);
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //set light color
    std::cout << light << std::endl;
    lightColor = glm::vec3(light, light, light); //white light
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glm::vec3 buoyLightColor = glm::vec3(colorRed, 0.0f, 0.0f);
    GLuint buoyColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "buoyLightColor");
    glUniform3fv(buoyColorLoc, 1, glm::value_ptr(buoyLightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    waveShader.useShaderProgram();

    // create waveModel matrix for grid
    waveModel = glm::mat4(1.0f);
    waveModelLoc = glGetUniformLocation(waveShader.shaderProgram, "waveModel");

    // get waveView matrix for current camera
    waveView = myCamera.getViewMatrix();
    waveViewLoc = glGetUniformLocation(waveShader.shaderProgram, "waveView");
    // send waveView matrix to shader
    glUniformMatrix4fv(waveViewLoc, 1, GL_FALSE, glm::value_ptr(waveView));

    // compute normal matrix grid
    waveNormalMatrix = glm::mat3(glm::inverseTranspose(waveView * waveModel));
    waveNormalMatrixLoc = glGetUniformLocation(waveShader.shaderProgram, "waveNormalMatrix");

    // create waveProjection matrix
    waveProjection = glm::perspective(glm::radians(45.0f),
                                      (float) retina_width / (float) retina_height,
                                      0.1f, 1000.0f);
    waveProjectionLoc = glGetUniformLocation(waveShader.shaderProgram, "waveProjection");
    // send waveProjection matrix to shader
    glUniformMatrix4fv(waveProjectionLoc, 1, GL_FALSE, glm::value_ptr(waveProjection));

    //set the light direction (direction towards the light)
    waveLightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    waveLightDirLoc = glGetUniformLocation(waveShader.shaderProgram, "waveLightDir");
    // send light dir to shader
    glUniform3fv(waveLightDirLoc, 1,
                 glm::value_ptr(glm::vec3(glm::inverseTranspose(waveView) * glm::vec4(waveLightDir, 1.0f))));

    //set light color
    waveLightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    waveLightColorLoc = glGetUniformLocation(waveShader.shaderProgram, "waveLightColor");
    // send light color to shader
    glUniform3fv(waveLightColorLoc, 1, glm::value_ptr(waveLightColor));

    gridTextureLoc = glGetUniformLocation(waveShader.shaderProgram, "diffuseTexture");
    glm::vec2 gridSize{GRID_NUM_POINTS_WIDTH, GRID_NUM_POINTS_HEIGHT};
    glUniform2fv(glGetUniformLocation(waveShader.shaderProgram, "gridSize"), 1, glm::value_ptr(gridSize));
    glm::vec2 gridDimensions{GRID_WIDTH, GRID_HEIGHT};
    glUniform2fv(glGetUniformLocation(waveShader.shaderProgram, "gridDimensions"), 1, glm::value_ptr(gridDimensions));

    simTimeLoc = glGetUniformLocation(waveShader.shaderProgram, "time");

}

//texture
const float textureRepeatU = 1.0f; //number of times to repeat seamless texture on u axis

const float textureRepeatV = 1.0f; //number of times to repeat seamless texture on v axis


void initVBOs() {

    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    //prepare vertex data to send to shader
    GLfloat vertexData[GRID_NUM_POINTS_WIDTH * GRID_NUM_POINTS_HEIGHT * 4];

    //for each vertex in grid
    for (unsigned int i = 0; i < GRID_NUM_POINTS_HEIGHT; i++) {
        for (unsigned int j = 0; j < GRID_NUM_POINTS_WIDTH; j++) {

            //tex coords
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 0] =
                    j * textureRepeatU / (float) (GRID_NUM_POINTS_WIDTH - 1);
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 1] =
                    textureRepeatV - i * textureRepeatV / (float) (GRID_NUM_POINTS_HEIGHT - 1);
            //xy position indices in grid (for computing sine function)
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 2] = j;
            vertexData[4 * (i * GRID_NUM_POINTS_WIDTH + j) + 3] = i;
        }
    }

    glGenBuffers(1, &gridPointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridPointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    //prepare triangle indices to send to shader
    GLuint triangleIndices[(GRID_NUM_POINTS_WIDTH - 1) * (GRID_NUM_POINTS_HEIGHT - 1) * 2 * 3];

    //for each square/rectangle in grid (each four neighboring points)
    for (unsigned int i = 0; i < GRID_NUM_POINTS_HEIGHT - 1; i++) {
        for (unsigned int j = 0; j < GRID_NUM_POINTS_WIDTH - 1; j++) {

            //lower triangle
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j)] = i * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 1] = (i + 1) * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 2] = (i + 1) * GRID_NUM_POINTS_WIDTH + j + 1;

            //upper triangle
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 3] = i * GRID_NUM_POINTS_WIDTH + j;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 4] = (i + 1) * GRID_NUM_POINTS_WIDTH + j + 1;
            triangleIndices[6 * (i * (GRID_NUM_POINTS_WIDTH - 1) + j) + 5] = i * GRID_NUM_POINTS_WIDTH + j + 1;
        }
    }

    glGenBuffers(1, &gridTrianglesEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridTrianglesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);

    //split vertex attributes

    //tex coords
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    //grid XY indices
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *) (2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}


GLuint initTexture(const char *file_name) {
    int x, y, n;
    int force_channels = 4;
    unsigned char *image_data = stbi_load(file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // NPOT check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(
                stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
        );
    }

    int width_in_bytes = x * 4;
    unsigned char *top = NULL;
    unsigned char *bottom = NULL;
    unsigned char temp = 0;
    int half_height = y / 2;

    for (int row = 0; row < half_height; row++) {
        top = image_data + row * width_in_bytes;
        bottom = image_data + (y - row - 1) * width_in_bytes;
        for (int col = 0; col < width_in_bytes; col++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_SRGB, //GL_SRGB,//GL_RGBA,
            x,
            y,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            image_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    //legam bufferul de culoare si de stencil din FBO la nimic
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    //dezactivam framebufferul pana ce il folosim
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

float yMove = 10.0f;

bool moving = false;

float lastTimeStamp = 0;

glm::mat4 animateWater(float left, float right, float inc) {
    float currentTimeStamp = glfwGetTime();
    if (currentTimeStamp - lastTimeStamp > 0.01f) {
        if (yMove > left && moving) {
            yMove -= inc;
        }
        if (yMove <= left) {
            moving = false;
        }
        if (yMove < right && !moving) {
            yMove += inc;
        }
        if (yMove >= right) {
            moving = true;
        }
        lastTimeStamp = currentTimeStamp;
    }
    return glm::rotate(glm::mat4(1.0f), glm::radians(yMove), glm::vec3(-50.0f, -5, 50.0f));
}

float buoyY = 6.0f;
bool buoyMoving = false;
float myLastTimeStamp = 0.0f;

glm::mat4 animateBuoy(float left, float right, float inc) {
    float currentTimeStamp = glfwGetTime();
    if (currentTimeStamp - myLastTimeStamp > 0.01f) {
        if (buoyY > left && buoyMoving) {
            buoyY -= inc;
        }
        if (buoyY <= left) {
            buoyMoving = false;
        }
        if (buoyY < right && !buoyMoving) {
            buoyY += inc;
        }
        if (buoyY >= right) {
            buoyMoving = true;
        }
        myLastTimeStamp = currentTimeStamp;
    }
    //std::cout << buoyY << std::endl;
    return glm::rotate(glm::mat4(1.0f), glm::radians(buoyY), glm::vec3(-50, 1, 40));
}


float sharkAngle = 0;
float xSharkAngle = 0;
int up = 30;

float fishAngle;

void drawObjects(gps::Shader shader, bool depthPass) {

    shader.useShaderProgram();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -50.0f, 75.0f));
    model = glm::scale(model, glm::vec3(1.7));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    ground.Draw(shader);


    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    sharkAngle += 0.1;
    xSharkAngle += 0.01;
    float sinShark = glm::sin(xSharkAngle);
    model = glm::rotate(model, glm::radians(-sharkAngle), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(sinShark * 10), glm::vec3(1, 0, 0));
    model = glm::translate(model, glm::vec3(-28.0f, -10.0f, 5.0f));
    model = glm::scale(model, glm::vec3(0.3));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    shark.Draw(shader);

    model = animateWater(-9, 13, 0.1);
    model = glm::translate(model, glm::vec3(0.0f, -3.8, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    ship.Draw(shader);


    model = animateWater(-9, 13, 0.1);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, -10.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    nanosuit.Draw(shader);

    model = animateWater(-9, 13, 0.1);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 10.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    nanosuit1.Draw(shader);


    model =  animateWater(-9, 13, 0.1);
    model = glm::translate(model, glm::vec3(-3.0f, 0.5f, -3.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    nanosuit2.Draw(shader);


    model =  animateWater(-9, 13, 0.1);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    nanosuit4.Draw(shader);


    model = animateBuoy(-15.0f, 13.0f, 0.12f);
    model = glm::translate(model, glm::vec3(-50.0f, -7.5f, 50.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    buoy.Draw(shader);
    fishAngle +=0.1;
    for(int i = 0; i < 10; i++) {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f , (float)i - 22.0f, 50.0f));
        model = glm::rotate(glm::mat4(1.0f), glm::radians(sharkAngle), glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(50.0f, ((float)i - 22.0f), -50.0f));
        model = glm::scale(model, glm::vec3(2));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // do not send the normal matrix if we are rendering in the depth map
        if (!depthPass) {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        fishBank[i].Draw(shader);
    }

    model = glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, -50.8, 25.0f));
    model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1,0,0));
    model = glm::scale(model, glm::vec3(3.0));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    ghostShip.Draw(shader);


}


void renderGrid(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    waveView = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "waveView"), 1, GL_FALSE, glm::value_ptr(waveView));


    //send grid waveModel matrix data to shader
    waveModel = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -6.0f, 0.0f));
    waveModel = glm::scale(waveModel, glm::vec3(2.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "waveModel"), 1, GL_FALSE, glm::value_ptr(waveModel));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        waveNormalMatrix = glm::mat3(glm::inverseTranspose(waveView * waveModel));
        glUniformMatrix3fv(waveNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(waveNormalMatrix));
    }
    glUniformMatrix4fv(waveModelLoc, 1, GL_FALSE, glm::value_ptr(waveModel));

    //send grid normal matrix data to shader
    glUniformMatrix3fv(waveNormalMatrixLoc, 1, GL_FALSE, glm::value_ptr(waveNormalMatrix));

    //send texture to shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gridTexture);
    glUniform1i(gridTextureLoc, 0);

    //send sim time
    glUniform1f(simTimeLoc, simTime);

    glBindVertexArray(gridVAO);

    // draw grid
    glDrawElements(GL_TRIANGLES, (GRID_NUM_POINTS_WIDTH - 1) * (GRID_NUM_POINTS_HEIGHT - 1) * 2 * 3, GL_UNSIGNED_INT,
                   0);

}

float k = 1;

void renderScene() {


    // depth maps creation pass
    //TODO - Send the light-space transformation matrix to the depth map creation shader and
    //		 render the scene in the depth map


    //render the scene to the depth buffer
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(computeLightSpaceTrMatrix()));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    drawObjects(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // render depth map on screen - toggled with the M key


    if (showDepthMap) {
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    } else {

        // final scene rendering pass (with shadows)


        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myCustomShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "cameraPos"), 1,
                     glm::value_ptr(myCamera.cameraPosition));

        if (moveSun) {
            lightAngle++;
            std::cout << "lightAngle: " << lightAngle << std::endl;
        } else {
            currentLightAngle = lightAngle;
        }

        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(light, light, light)));


        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        lightRotation = glm::translate(lightRotation, glm::vec3(0.0f, 0.0f, 1.0f));
        lightRotation = glm::scale(lightRotation, glm::vec3(1.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
        //bind the shadow map


        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(computeLightSpaceTrMatrix()));

        drawObjects(myCustomShader, false);

        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "buoyLightColor"), 1,
                     glm::value_ptr(glm::vec3(animateBuoyLight(0.0f, 125.0f, 1.0f), 0.0f, 0.0f)));

        //draw a white cube around the light

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


        model = lightRotation;
        //lightDir = glm::normalize(glm::mat3(model) * originalLightDir);
        model = glm::translate(model, 5.0f * lightDir);
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE,
                           glm::value_ptr(model));

        if (myCamera.cameraPosition.y > -7.0f) {
            if ( lightDir.y < -7.0f)  {
                myNightSkyBox.Draw(skyboxShader, view, projection);
            } else {
                mySkyBox.Draw(skyboxShader, view, projection);
            }
        } else {
            myUnderWaterSkyBox.Draw(skyboxShader, view, projection);
        }

        lightCube.Draw(lightShader);

        waveShader.useShaderProgram();

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(waveShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(waveShader.shaderProgram, "lightSpaceTrMatrix"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(computeLightSpaceTrMatrix()));

        renderGrid(waveShader, false);

        view = myCamera.getViewMatrix();

        projection = glm::perspective(glm::radians(45.0f), (float) retina_width / (float) retina_height, 0.1f, 1000.0f);

        if (present) {
            cameraAngle -= 0.5f;
            model = glm::mat4(1.0f);

            model = glm::rotate(model, glm::radians(cameraAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(75.0f, 50.0f, 25.0f));
            glm::vec3 pos = glm::vec3((model * glm::vec4(1.0f)));
            myCamera.cameraPosition = pos;
            myCamera.cameraTarget = glm::vec3 (0.0f, 0.0f, 0.0f);
        }

        if (underPresent) {
            cameraAngle -= 0.5f;
            model = glm::mat4(1.0f);

            model = glm::rotate(model, glm::radians(cameraAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(75.0f, -22.0f, 25.0f));
            glm::vec3 pos = glm::vec3((model * glm::vec4(1.0f)));
            myCamera.cameraPosition = pos;
            myCamera.cameraTarget = glm::vec3 (0.0f, 0.0f, 0.0f);
        }

    }

}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glfwDestroyWindow(glWindow);
    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char *argv[]) {
    if (!initOpenGLWindow()) {
        glfwTerminate();
        return 1;
    }

    initOpenGLState();
    glCheckError();
    initVBOs();
    glCheckError();
    gridTexture = initTexture("textures/another_water.jpeg");
    glCheckError();
    initObjects();
    glCheckError();
    initShaders();
    glCheckError();
    initUniforms();
    glCheckError();
    initFBO();

    glCheckError();

    while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
        renderScene();

        simTime += 0.007f;

        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }

    cleanup();

    return 0;
}
