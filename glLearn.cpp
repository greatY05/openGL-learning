#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <string>
#include <vector>
using namespace std;

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



//forward declare/prototype for window resizing
void framebuffer_size_callback(GLFWwindow* widnow, int WIDTH, int HEIGHT);
//forward declare input processing
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//forward declae callback mouse and scroll function 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffsett, double yoffset);
//ttexture loader
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

//camera position stuff
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool firstMove = true;

//delta time!!!!
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // light source location
bool spotlightActive;





static const int WIDTH = 1000, HEIGHT = 800;
int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);


	float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
	Shader pointsShader("simpleVert.vs", "default.frag", "geometryShader.gs");

	//
	Model backpack("backpack/backpack.obj");

	// render loop
	// -----------
	

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);
		pointsShader.use();
		pointsShader.setMat4("projection", projection);
		pointsShader.setMat4("view", view);
		pointsShader.setMat4("model", model);
		pointsShader.setFloat("time", static_cast<float>(glfwGetTime()));
		
		backpack.Draw(pointsShader);



		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------


	glfwTerminate();
	return 0;
}



//processes inputs
void processInput(GLFWwindow* window) {
	float cameraSpeed = 2.5f * deltaTime; // deltas our time
	//give the window we're working on and hte key
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		//turns "should we close the window?" to true, therefore killing the while loop in main on the next iteration
		glfwSetWindowShouldClose(window, true);
	}
	//run
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.speedUp(true);
	}
	else {
		camera.speedUp(false);
	}
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_ARROW_CURSOR);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //set curser mode to disappear while window focused
		}
	}

	//walking
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.ProcessKeyboard(DOWN, deltaTime);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		spotlightActive = !spotlightActive;
	}
}


float lastX = WIDTH / 2, lastY = HEIGHT / 2; //
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMove) {
		lastX = xpos;
		lastY = ypos;
		firstMove = false;
	}
	//first time check to stop sudden mouse jumping
	// sets offsets between last and current frames
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	//applies sensitivity - happens in header class
	// 
	//adds offset - header handles
	//centers incase of boundary crossing - happens in header class
	

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

//handles viewport resizing on window resizing
void framebuffer_size_callback(GLFWwindow* widnow, int WIDTH, int HEIGHT) {
	glViewport(0, 0, WIDTH, HEIGHT);

}


unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}






//                                                         vbo
//vertrex buffer object - glGenBuffer generates a buffer bts and brings back a reference to them
//unsigned int VBO;
//glGenBuffers(1, &VBO);
////binds to object buffer we created above to our VBO buffer type
//glBindBuffer(GL_ARRAY_BUFFER, VBO);
////copies user defined data to our current buffer. takes buffer type, size, data, and managing mode

////                                                         vao
//unsigned int VAO;
//glGenVertexArrays(1, &VAO); //binds created vao object to VAO var
////bind vertex array object
//glBindVertexArray(VAO);
////copy our vertices array into a buffer for opengl
//glBindBuffer(GL_ARRAY_BUFFER, VBO);
//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//set our vertex attribute pointers
//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


//                                                            10 BOXES SHADER

/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(2);*/






//                                                     image




////creating id for the image
//unsigned int texture1, texture2;


//glGenTextures(1, &texture1); // takes the amount of images to generate and where to store them (int array type)

//glBindTexture(GL_TEXTURE_2D, texture1); //bind texture
////wrapping/filtering options
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//// set texture filtering parameters
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//int Twidth, Theight, nrChannels;
//unsigned char *data = stbi_load("container.jpg", &Twidth, &Theight, &nrChannels, 0); // takes image location, width, height and color channels
//if (data) {
//	//generate image data:
//	//parameters: 1. texture target (channel 2d), 2. mipmap level (0), 3. store texture format (with rgb values here)
//	// 4-5. wid-hi 6. (0 always, legacy thingy) 7-8. data type of source image (pass current stoired values) 9. image data
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Twidth, Theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//	glGenerateMipmap(GL_TEXTURE_2D);
//}
//else 
//{
//	
//	std::cout << "failed to load texture " << std::endl;
//}



//stbi_image_free(data); //free image memory now that its generated



//stbi_set_flip_vertically_on_load(true);
//glGenTextures(1, &texture2);

//glBindTexture(GL_TEXTURE_2D, texture2);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//// set texture filtering parameters
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//data = stbi_load("awesomeface.png", &Twidth, &Theight, &nrChannels, 0);
//if (data) {
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Twidth, Theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//	glGenerateMipmap(GL_TEXTURE_2D);
//}
//else
//{
//	std::cout << "failed to load texture " << std::endl;
//}



//stbi_image_free(data); //free image memory now that its generated

//Shader ourShader("vertex.vs", "fragment.fss");

//ourShader.use();
//ourShader.setInt("texture1", 0);
//ourShader.setInt("texture2", 1);

//                                                         ebo
	// we use EBO (element array buffer) to avoid redundancy and vertex duplication

	//unsigned int EBO;
	//glGenBuffers(1, &EBO); // create buffer EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //GL_ELEMENT_ARRAY_BUFFER for ebo (element  buffer object)
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // type buffer, size, data, type of usage (static)
	//RELATION BETWEEN VBO-VAO-EBO:
	//vbo - a buffer, raw vertex data in the gpu memory, ensures faster access
	//vao - a config object, remembers vertex data organization and how to interpet it
	//ebo - uses indexing to reuse vertex. instead of defining verticies multiple times, this creates primitives

	// sets out how to interpet the data of the vertex data as so:
	// 1. index (0), 2. size specifier (3 as in vec3), 3. type (float), 4. normalized (no), 
	// 5. space between attributes (3f since 3 point array of float), 6. offset from beginning (0 times size of type)


	//					                         				render loop - 10 boxes

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	//starts the loop, while we dont need to close it - run
	//while (!glfwWindowShouldClose(window)) {
	//	float currentFrame = glfwGetTime();
	//	deltaTime = currentFrame - lastFrame;
	//	lastFrame = currentFrame;

	//	//calls process inputs function
	//	processInput(window);
	//	//clear colorbuffer
	//	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, texture1);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, texture2);

	//	


	//	//building world layers
	//	glm::mat4 view = camera.GetViewMatrix();//glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // position, target, up
	//	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);//projection - making perspective
	//	glm::mat4 model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // transforms to global coordianates, is "on the floor"

	//	int modelLoc = glGetUniformLocation(ourShader.ID, "model");
	//	int viewLoc = glGetUniformLocation(ourShader.ID, "view");
	//	int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");

	//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // send matrix data to shader using matrix4fv as postfix
	//	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); // send matrix data to shader using matrix4fv as postfix
	//	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)); // send matrix data to shader using matrix4fv as postfix
	//	////arguments - location, matrix count,transfix(switching columns and rows, ignore), value

	//	//camera movement
	//	glfwSetCursorPosCallback(window, mouse_callback);// mouse movement
	//	glfwSetScrollCallback(window, scroll_callback); // scroll rolling

	//	//draw the triangle/use ou shader program
	//	ourShader.use();
	//	glBindVertexArray(VAO);
	//	for (unsigned int i = 0; i < 10; i++) {
	//		glm::mat4 model = glm::mat4(1.0f);
	//		model = glm::translate(model, cubePositions[i]);
	//		float angle = 20.0f * i;
	//		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	//		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(55.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	//		ourShader.setMat4("model", model);

	//		glDrawArrays(GL_TRIANGLES, 0, 36);
	//	}


	//	//swaps front and back buffer to prevent artifacts
	//	glfwSwapBuffers(window);
	//	//checks if any events happened, and calls them if so
	//	glfwPollEvents();
	//}




//                                                                                       lighting render loop:





//                                                            render loop - lighting
	//while (!glfwWindowShouldClose(window)) {
	//	float currentFrame = static_cast<float>(glfwGetTime()); //per frame time logic
	//	deltaTime = currentFrame - lastFrame;
	//	lastFrame = currentFrame;
	//	//input processing + buffer clearing/rendering
	//	processInput(window);
	//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	
	//	
	//	//lightPos = glm::vec3(sin(10 * static_cast<float>(glfwGetTime())/4), cos(10 * static_cast<float>(glfwGetTime()) / 4), sin(10 * static_cast<float>(glfwGetTime()) / 4));
	//	//lightPos.x = (sin(10 * static_cast<float>(glfwGetTime() / 2)));

	//	//set uniforms for cube
	//	lightingShader.use();
	//	//lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	//	lightingShader.setVec3("viewPos", camera.Position);
	//	


	//	// directional light
	//	lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	//	lightingShader.setVec3("dirLight.ambient", 0.7f, 0.7f, 0.7f);
	//	lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
	//	lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	//	// point lights
	//	for (int i = 0; i < 4; i++) {
	//		std::string baseStr = "pointLights[" + std::to_string(i) + "]";

	//		lightingShader.setVec3(baseStr + ".position", pointLightPositions[i]);
	//		lightingShader.setVec3(baseStr + ".ambient", 0.4f, 0.7f, 0.7f);
	//		lightingShader.setVec3(baseStr + ".diffuse", 0.1f, 0.9f, 0.1f);
	//		lightingShader.setVec3(baseStr + ".specular", 1.0f, 1.0f, 1.0f);
	//		lightingShader.setFloat(baseStr + ".constant", 1.0f);
	//		lightingShader.setFloat(baseStr + ".linear", 0.09f);
	//		lightingShader.setFloat(baseStr + ".quadratic", 0.032f);
	//	}

	//	// spotLight
	//	lightingShader.setVec3("spotLight.position", camera.Position);
	//	lightingShader.setVec3("spotLight.direction", camera.Front);
	//	lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	//	lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	//	lightingShader.setVec3("spotLight.specular", 0.0f, 1.0f, 0.0f);
	//	lightingShader.setFloat("spotLight.constant", 1.0f);
	//	lightingShader.setFloat("spotLight.linear", 0.09f);
	//	lightingShader.setFloat("spotLight.quadratic", 0.032f);
	//	lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	//	lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	//	
	//	
	//	//glm::vec3 lightColor;
	//	//lightColor.x = sin(glfwGetTime() * 2.0f);
	//	//lightColor.y = sin(glfwGetTime() * 0.7f);
	//	//lightColor.z = sin(glfwGetTime() * 1.3f);

	//	glm::vec3 ambientColor = glm::vec3(0.2f);
	//	glm::vec3 diffuseColor = glm::vec3(0.5f);
	//	glm::vec3 specularColor = glm::vec3(1.0f);

	//	lightingShader.setVec3("light.ambient", ambientColor);
	//	lightingShader.setVec3("light.diffuse", diffuseColor); // darken diffuse light a bit
	//	lightingShader.setVec3("light.specular", specularColor);
	//	lightingShader.setFloat("light.constant", 1.0f);
	//	lightingShader.setFloat("light.linear", 0.09f);
	//	lightingShader.setFloat("light.quadratic", 0.032f);

	//	lightingShader.setFloat("material.shininess", 32.0f);


	//	//binding different layers for different lightings
	//	lightingShader.setInt("material.diffuse", 0);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	//	lightingShader.setInt("material.specular", 1);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, specularMap);
	//	lightingShader.setInt("material.emission", 2);
	//	glActiveTexture(GL_TEXTURE2);
	//	glBindTexture(GL_TEXTURE_2D, emissionMap);

	//	lightCubeShader.use(); // quicikly setting the color of the light cube
	//	lightCubeShader.setVec3("color", diffuseColor + glm::vec3(0.7f));
	//	lightingShader.use();

	//	//view and projection transforms
	//	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	//	glm::mat4 view = camera.GetViewMatrix();
	//	lightingShader.setMat4("projection", projection);
	//	lightingShader.setMat4("view", view);
	//	
	//	//world transforms
	//	glm::mat4 model = glm::mat4(1.0f);
	//	lightingShader.setMat4("model", model);


	//	//drawing the 10 cubes
	//	glBindVertexArray(cubeVAO);
	//	for (unsigned int i = 0; i < 10; i++) {
	//		glm::mat4 model = glm::mat4(1.0f);
	//		model = glm::translate(model, cubePositions[i]);
	//		float angle = 20.0f * i;
	//		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

	//		lightingShader.setMat4("model", model);
	//	
	//		//draw cube
	//		glDrawArrays(GL_TRIANGLES, 0, 36);
	//	}




	//	// also draw the lamp object(s)
	//	lightCubeShader.use();
	//	lightCubeShader.setMat4("projection", projection);
	//	lightCubeShader.setMat4("view", view);

	//	// we now draw as many light bulbs as we have point lights.
	//	glBindVertexArray(lightCubeVAO);
	//	for (unsigned int i = 0; i < 4; i++)
	//	{
	//		model = glm::mat4(1.0f);
	//		model = glm::translate(model, pointLightPositions[i]);
	//		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
	//		lightCubeShader.setMat4("model", model);
	//		glDrawArrays(GL_TRIANGLES, 0, 36);
	//	}
	//	



	//	glfwSetCursorPosCallback(window, mouse_callback);
	//	glfwSetScrollCallback(window, scroll_callback);

	//	glfwSwapBuffers(window);
	//	glfwPollEvents();
	//}














//                                                                                  load model lesson:


//Shader modelShader("modelVert.vs", "modelFrag.frag");

	//Model backpack("backpack/backpack.obj"); // use the new header model loader


//#define MAX_BONE_INFLUENCE 4
//
//struct Vertex {
//	// position
//	glm::vec3 Position;
//	// normal
//	glm::vec3 Normal;
//	// texCoords
//	glm::vec2 TexCoords;
//	// tangent
//	glm::vec3 Tangent;
//	// bitangent
//	glm::vec3 Bitangent;
//	//bone indexes which will influence this vertex
//	int m_BoneIDs[MAX_BONE_INFLUENCE];
//	//weights from each bone
//	float m_Weights[MAX_BONE_INFLUENCE];
//};
//
//struct Texture {
//	unsigned int id;
//	string type;
//	string path;
//};
//
//class Mesh {
//public:
//	// mesh Data
//	vector<Vertex>       vertices;
//	vector<unsigned int> indices;
//	vector<Texture>      textures;
//	unsigned int VAO;
//
//	// constructor
//	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
//	{
//		this->vertices = vertices;
//		this->indices = indices;
//		this->textures = textures;
//
//		// now that we have all the required data, set the vertex buffers and its attribute pointers.
//		setupMesh();
//	}
//
//	// render the mesh
//	void Draw(Shader& shader)
//	{
//		// bind appropriate textures
//		unsigned int diffuseNr = 1;
//		unsigned int specularNr = 1;
//		unsigned int normalNr = 1;
//		unsigned int heightNr = 1;
//		for (unsigned int i = 0; i < textures.size(); i++)
//		{
//			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
//			// retrieve texture number (the N in diffuse_textureN)
//			string number;
//			string name = textures[i].type;
//			if (name == "texture_diffuse")
//				number = std::to_string(diffuseNr++);
//			else if (name == "texture_specular")
//				number = std::to_string(specularNr++); // transfer unsigned int to string
//			else if (name == "texture_normal")
//				number = std::to_string(normalNr++); // transfer unsigned int to string
//			else if (name == "texture_height")
//				number = std::to_string(heightNr++); // transfer unsigned int to string
//
//			// now set the sampler to the correct texture unit
//			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
//			// and finally bind the texture
//			glBindTexture(GL_TEXTURE_2D, textures[i].id);
//		}
//
//		// draw mesh
//		glBindVertexArray(VAO);
//		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
//		glBindVertexArray(0);
//
//		// always good practice to set everything back to defaults once configured.
//		glActiveTexture(GL_TEXTURE0);
//	}
//
//private:
//	// render data 
//	unsigned int VBO, EBO;
//
//	// initializes all the buffer objects/arrays
//	void setupMesh()
//	{
//		// create buffers/arrays
//		glGenVertexArrays(1, &VAO);
//		glGenBuffers(1, &VBO);
//		glGenBuffers(1, &EBO);
//
//		glBindVertexArray(VAO);
//		// load data into vertex buffers
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		// A great thing about structs is that their memory layout is sequential for all its items.
//		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
//		// again translates to 3/2 floats which translates to a byte array.
//		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//
//		// set the vertex attribute pointers
//		// vertex Positions
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//		// vertex normals
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
//		// vertex texture coords
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
//		// vertex tangent
//		glEnableVertexAttribArray(3);
//		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
//		// vertex bitangent
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
//		// ids
//		glEnableVertexAttribArray(5);
//		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
//
//		// weights
//		glEnableVertexAttribArray(6);
//		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
//		glBindVertexArray(0);
//	}
//};
//
//
//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
//
//class Model {
//private: 
//	Model(string const& path, bool gamma = false) : gammaCorrection(gamma) {
//		loadModel(path);
//	}
//	void Draw(Shader& shader) {
//		for (int i = 0; i < meshes.size(); i++)
//			meshes[i].Draw(shader);
//	}
//public:
//	//model data
//	vector<Mesh> meshes;
//	string directory;
//	vector<Texture> textures_loaded;
//	bool gammaCorrection;
//
//	void loadModel(string path) {
//		Assimp::Importer import;
//		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); //scene above root
//
//		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//		{
//			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
//			return;
//		}
//		directory = path.substr(0, path.find_last_of('/')); // save directory after checking everything works
//
//		processNode(scene->mRootNode, scene); //recursively check node by hirarchy till all nodes are checked and work 
//	}
//
//
//
//	void processNode(aiNode* node, const aiScene* scene) {
//		// process all the node's meshes (if any)
//		for (unsigned int i = 0; i < node->mNumMeshes; i++)
//		{
//			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//			meshes.push_back(processMesh(mesh, scene));
//		}
//		// then do the same for each of its children
//		for (unsigned int i = 0; i < node->mNumChildren; i++)
//		{
//			processNode(node->mChildren[i], scene);
//		}
//	}
//
//
//
//	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
//		vector<Vertex> vertices;
//		vector<unsigned int> indices;
//		vector<Texture> textures;
//
//		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//			Vertex vertex;
//			//retrieve vertex data
//			glm::vec3 vector;
//			vector.x = mesh->mVertices[i].x;
//			vector.y = mesh->mVertices[i].y;
//			vector.z = mesh->mVertices[i].z;
//			vertex.Position = vector; // get positions of vertex positions
//
//			if (mesh->HasNormals()) {
//				vector.x = mesh->mNormals[i].x;
//				vector.y = mesh->mNormals[i].y;
//				vector.z = mesh->mNormals[i].z;
//				vertex.Normal = vector; // get normals
//			}
//
//			if (mesh->mTextureCoords[0]) { // check if theres texture coordianates, save if there is
//                glm::vec2 vec;
//                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
//                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
//                vec.x = mesh->mTextureCoords[0][i].x; 
//                vec.y = mesh->mTextureCoords[0][i].y;
//                vertex.TexCoords = vec;
//                // tangent
//                vector.x = mesh->mTangents[i].x;
//                vector.y = mesh->mTangents[i].y;
//                vector.z = mesh->mTangents[i].z;
//                vertex.Tangent = vector;
//                // bitangent
//                vector.x = mesh->mBitangents[i].x;
//                vector.y = mesh->mBitangents[i].y;
//                vector.z = mesh->mBitangents[i].z;
//                vertex.Bitangent = vector;
//			}
//			else {
//				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//			}
//
//
//			vertices.push_back(vertex);
//		}
//		//process indices
//		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//			aiFace face = mesh->mFaces[i];
//			for (unsigned int j = 0; j < face.mNumIndices; j++) {
//				indices.push_back(face.mIndices[j]); //bring back set of vertices and index data for opengl to draw the mesh with
//			}
//		}
//		// process materials
//		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
//		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
//		// Same applies to other texture as the following list summarizes:
//		// diffuse: texture_diffuseN
//		// specular: texture_specularN
//		// normal: texture_normalN
//
//		// 1. diffuse maps
//		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//		// 2. specular maps
//		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//		// 3. normal maps
//		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
//		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//		// 4. height maps
//		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
//		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
//
//		// return a mesh object created from the extracted mesh data
//		return Mesh(vertices, indices, textures);
//	}
//
//
//
//	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
//	{
//		vector<Texture> textures;
//		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//		{
//			aiString str;
//			mat->GetTexture(type, i, &str);
//			bool skip = false;
//			for (unsigned int j = 0; j < textures_loaded.size(); j++)
//			{
//				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//				{
//					textures.push_back(textures_loaded[j]);
//					skip = true;
//					break;
//				}
//			}
//			if (!skip)
//			{   // if texture hasn't been loaded already, load it
//				Texture texture;
//				texture.id = TextureFromFile(str.C_Str(), directory);
//				texture.type = typeName;
//				texture.path = str.C_Str();
//				textures.push_back(texture);
//				textures_loaded.push_back(texture); // add to loaded textures
//			}
//		}
//		return textures;
//	}
//};
//
//
//
//
////texturefromfile body
//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
//{
//	string filename = string(path);
//	filename = directory + '/' + filename;
//
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}
//#endif







	//                                                     render loop    -    model loading
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//while (!glfwWindowShouldClose(window)) {
	//	float currentFrame = static_cast<float>(glfwGetTime());
	//	deltaTime = currentFrame - lastFrame;
	//	lastFrame = currentFrame;
	//	processInput(window);
	//	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	lightingShader.use();

	//	lightingShader.setVec3("viewPos", camera.Position);
	//			


	//	// directional light
	//	lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	//	lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
	//	lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
	//	lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	//	// point lights
	//	for (int i = 0; i < 4; i++) {
	//		std::string baseStr = "pointLights[" + std::to_string(i) + "]";

	//		lightingShader.setVec3(baseStr + ".position", pointLightPositions[i]);
	//		lightingShader.setVec3(baseStr + ".ambient", 0.3f, 0.3f, 0.3f);
	//		lightingShader.setVec3(baseStr + ".diffuse", 0.5f, 0.5f, 0.5f);
	//		lightingShader.setVec3(baseStr + ".specular", 1.0f, 1.0f, 1.0f);
	//		lightingShader.setFloat(baseStr + ".constant", 1.0f);
	//		lightingShader.setFloat(baseStr + ".linear", 0.09f);
	//		lightingShader.setFloat(baseStr + ".quadratic", 0.032f);
	//	}

	//	// spotLight
	//	lightingShader.setVec3("spotLight.position", camera.Position);
	//	lightingShader.setVec3("spotLight.direction", camera.Front);
	//	if (spotlightActive) {
	//		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	//		lightingShader.setVec3("spotLight.diffuse", 0.7f, 0.7f, 0.7f);
	//		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	//	}
	//	else { // turn flashlight off on button press
	//		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	//		lightingShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
	//		lightingShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
	//	}
	//		lightingShader.setFloat("spotLight.constant", 1.0f);
	//		lightingShader.setFloat("spotLight.linear", 0.09f);
	//		lightingShader.setFloat("spotLight.quadratic", 0.032f);
	//		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	//		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	//			




	//	lightingShader.setFloat("material.shininess", 32.0f);
	//	//binding different layers for different lightings
	//	lightingShader.setInt("material.diffuse", 0);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	//	lightingShader.setInt("material.specular", 1);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, specularMap);
	//	lightingShader.setInt("material.emission", 2);
	//	glActiveTexture(GL_TEXTURE2);
	//	glBindTexture(GL_TEXTURE_2D, emissionMap);



	//	//view and projection transforms
	//	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	//	glm::mat4 view = camera.GetViewMatrix();
	//	lightingShader.setMat4("projection", projection);
	//	lightingShader.setMat4("view", view);
	//			
	//	//world transforms
	//	glm::mat4 model = glm::mat4(1.0f);
	//	lightingShader.setMat4("model", model);









	//	//render model!
	//	//glm::mat4 model = glm::mat4(1.0f);
	//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // set transform to 0
	//	model = scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // scale to to 1
	//	lightingShader.setMat4("model", model); // pass matrices off shit
	//	backpack.Draw(lightingShader); // tell model to draw using shit matrices

	//	glDrawArrays(GL_TRIANGLES, 0, 36);


	//	glfwSetCursorPosCallback(window, mouse_callback);
	//	glfwSetScrollCallback(window, scroll_callback);
	//	
	//	glfwSetKeyCallback(window, key_callback);



	//	glfwSwapBuffers(window);
	//	glfwPollEvents();
	//}

//                                                             lighting boxes:
	//unsigned int lightVBO, cubeVAO;
	//glGenVertexArrays(1, &cubeVAO); // create 1 vertex array for cube vao
	//glGenBuffers(1, &lightVBO); // create buffer for vbo

	//glBindBuffer(GL_ARRAY_BUFFER, lightVBO); // bind buffer for vbo
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // set attributes

	//glBindVertexArray(cubeVAO);

	////position attribs
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	////normal attribs
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(1);
	////texture coords attribs
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//unsigned int diffuseMap = loadTexture("container2.png");
	//unsigned int specularMap = loadTexture("container2_specular.png");
	//unsigned int emissionMap = loadTexture("matrix.jpg");

	//unsigned int lightCubeVAO;
	//glGenVertexArrays(1, &lightCubeVAO);
	//glBindVertexArray(lightCubeVAO);

	//glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);


//                                                                                           skybox, framebuffers, chapter 4 stuff
//creating 4 blocks with 4 different shaders but same vertex shader:
	//Shader shaderRed("shader.vs", "default.frag");
	//Shader shaderGreen("shader.vs", "default.frag");
	//Shader shaderYellow ("shader.vs", "default.frag");
	//Shader shaderBlue("shader.vs", "default.frag");
	//unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.ID, "Matrices");
	//unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.ID, "Matrices");
	//unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.ID, "Matrices");
	//unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "Matrices");

	//glUniformBlockBinding(shaderRed.ID, uniformBlockIndexRed, 0);
	//glUniformBlockBinding(shaderGreen.ID, uniformBlockIndexGreen, 0);
	//glUniformBlockBinding(shaderBlue.ID, uniformBlockIndexBlue, 0);
	//glUniformBlockBinding(shaderYellow.ID, uniformBlockIndexYellow, 0);

	////create the ubos (uniform buffers) and bind them to point 1
	//unsigned int uboMatrices;
	//glGenBuffers(1, &uboMatrices);

	//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	//glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); //allocate memory for our buffers (2 mat4 buffers)
	//glBindBuffer(GL_UNIFORM_BUFFER, 0); // link the entire buffer to point 0

	//glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
	////fill the buffer

	//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	////we store the first half of the uniform buffer here (projection) since we only update it once, we will store hte second one in the render loop





	//// 1. create and bind a framebuffer 2. create the texture image we're gonna draw and keep data uninitialized 3. create renderbuffer for both depth and stencil buffers
	//// 4. attach renderbuffer to the framebuffer 5. if statement check to see if framebuffer successful complete 7. unbind the framebuffer 
	//// 8. create create VAO for the screen quad
	//unsigned int framebuffer;
	//glGenFramebuffers(1, &framebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//unsigned int textureColorbuffer;
	//glGenTextures(1, &textureColorbuffer);
	//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	//	std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	//}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//
	//Shader screenShader("normal.vs", "normal.frag");

	//unsigned int quadVAO, quadVBO;
	//glGenVertexArrays(1, &quadVAO);
	//glGenBuffers(1, &quadVBO);
	//glBindVertexArray(quadVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	//glBindVertexArray(0);


	////optimizing data
	////
 //   //unsigned int transparentVAO, transparentVBO;
 //   //glGenVertexArrays(1, &transparentVAO);
 //   //glGenBuffers(1, &transparentVBO);
 //   //glBindVertexArray(transparentVAO);
 //   //glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
 //   //glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
 //   //glEnableVertexAttribArray(0);
 //   //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
 //   //glEnableVertexAttribArray(1);
 //   //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
 //   //glBindVertexArray(0);


	//unsigned int cubeVAO, cubeVBO;
	//glGenVertexArrays(1, &cubeVAO);
	//glGenBuffers(1, &cubeVBO);
	//glBindVertexArray(cubeVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//// plane VAO
	//unsigned int planeVAO, planeVBO;
	//glGenVertexArrays(1, &planeVAO);
	//glGenBuffers(1, &planeVBO);
	//glBindVertexArray(planeVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glBindVertexArray(0);

	//// load textures
	//// -------------
	//unsigned int grassTexture = loadTexture("grass.png");
	//unsigned int cubeTexture = loadTexture("container.jpg");
	//unsigned int floorTexture = loadTexture("metal.png");
	//unsigned int windowTexture = loadTexture("window.png");

	////cubemap
	//// ----------

	//unsigned int skyboxVAO, skyboxVBO;
	//glGenVertexArrays(1, &skyboxVAO);
	//glGenBuffers(1, &skyboxVBO);
	//glBindVertexArray(skyboxVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glBindVertexArray(0);

	//vector<std::string> faces
	//{
	//	"skybox/right.jpg",
	//	"skybox/left.jpg",
	//	"skybox/top.jpg",
	//	"skybox/bottom.jpg",
	//	"skybox/front.jpg",
	//	"skybox/back.jpg"
	//};
	//unsigned int cubemapTexture = loadCubemap(faces);
	//// shader configuration
	//// --------------------
	//shader.use();
	//shader.setInt("texture1", 0);

	//skyboxShader.use();
	//skyboxShader.setInt("skybox", 0);

	//screenShader.use();
	//screenShader.setInt("screenTexture", 0);


		//// draw scene as normal
		//shader.use();
		//glm::mat4 model = glm::mat4(1.0f);
		//glm::mat4 view = camera.GetViewMatrix();
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		//shader.setMat4("model", model);
		//shader.setMat4("view", view);
		//shader.setMat4("projection", projection);
		//shader.setVec3("cameraPos", camera.Position);

		////
		//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
		////
		//// ^second part of uniform buffer we update the view here^
		//

		//// cubes
		//glBindVertexArray(cubeVAO);
		//shaderRed.use();
		// model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));	
		//shaderRed.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//
		//glBindVertexArray(cubeVAO);
		//shaderBlue.use();
		// model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));	
		//shaderRed.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//glBindVertexArray(cubeVAO);
		//shaderYellow.use();
		// model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));	
		//shaderRed.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//glBindVertexArray(cubeVAO);
		//shaderGreen.use();
		// model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));	
		//shaderRed.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);


		//// draw skybox as last
		//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		//skyboxShader.use();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		//skyboxShader.setMat4("view", view);
		//skyboxShader.setMat4("projection", projection);
		//// skybox cube
		//glBindVertexArray(skyboxVAO);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
		//glDepthFunc(GL_LESS); // set depth function back to default


		//// render
		//// ------
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

		//// set uniforms
		//shaderSingleColor.use();
		//glm::mat4 model = glm::mat4(1.0f);
		//glm::mat4 view = camera.GetViewMatrix();
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		//shaderSingleColor.setMat4("view", view);
		//shaderSingleColor.setMat4("projection", projection);

		//shader.use();
		//shader.setMat4("view", view);
		//shader.setMat4("projection", projection);

		//// draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
		//glStencilMask(0x00);
		//// floor
		//glBindVertexArray(planeVAO);
		//glBindTexture(GL_TEXTURE_2D, floorTexture);
		//shader.setMat4("model", glm::mat4(1.0f));
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glBindVertexArray(0);



		//// 1st. render pass, draw objects as normal, writing to the stencil buffer
		//// --------------------------------------------------------------------
		//glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass stencil
		//glStencilMask(0xFF); // enable writing to stencil buffer
		//// cubes
		//glBindVertexArray(cubeVAO);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, cubeTexture);
		//model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		//shader.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		//shader.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// //grass
		//std::map<float, glm::vec3> sorted;
		//for (unsigned int i = 0; i < transparents.size(); i++)
		//{
		//	float distance = glm::length(camera.Position - transparents[i]);
		//	sorted[distance] = transparents[i];
		//}
		//
		//glBindVertexArray(transparentVAO);
		//glBindTexture(GL_TEXTURE_2D, windowTexture);
		//for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		//{
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, it->second);
		//	shader.setMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 6);
		//}


		// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		// -----------------------------------------------------------------------------------------------------------------------------
		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //only those who are not equal to 1
		//glStencilMask(0x00); //disable writing to stencil buffer
		//glDisable(GL_DEPTH_TEST);
		//shaderSingleColor.use();
		//float scale = 1.1f;
		//// cubes
		//glBindVertexArray(cubeVAO);
		//glBindTexture(GL_TEXTURE_2D, cubeTexture);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		//model = glm::scale(model, glm::vec3(scale, scale, scale));
		//shaderSingleColor.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(scale, scale, scale));
		//shaderSingleColor.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
		//glStencilMask(0xFF);
		//glStencilFunc(GL_ALWAYS, 0, 0xFF);
		//glEnable(GL_DEPTH_TEST); // re enable depth test when we're done with outlining



//float skyboxVertices[] = {
//	// positions          
//	-1.0f,  1.0f, -1.0f,
//	-1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//
//	-1.0f, -1.0f,  1.0f,
//	-1.0f, -1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f,  1.0f,
//	-1.0f, -1.0f,  1.0f,
//
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//
//	-1.0f, -1.0f,  1.0f,
//	-1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f, -1.0f,  1.0f,
//	-1.0f, -1.0f,  1.0f,
//
//	-1.0f,  1.0f, -1.0f,
//	 1.0f,  1.0f, -1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	-1.0f,  1.0f,  1.0f,
//	-1.0f,  1.0f, -1.0f,
//
//	-1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f,  1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f,  1.0f,
//	 1.0f, -1.0f,  1.0f
//};
////vertex data
//float cubeVertices[] = {
//	// positions         
//	-0.5f, -0.5f, -0.5f,
//	 0.5f, -0.5f, -0.5f,
//	 0.5f,  0.5f, -0.5f,
//	 0.5f,  0.5f, -0.5f,
//	-0.5f,  0.5f, -0.5f,
//	-0.5f, -0.5f, -0.5f,
//
//	-0.5f, -0.5f,  0.5f,
//	 0.5f, -0.5f,  0.5f,
//	 0.5f,  0.5f,  0.5f,
//	 0.5f,  0.5f,  0.5f,
//	-0.5f,  0.5f,  0.5f,
//	-0.5f, -0.5f,  0.5f,
//
//	-0.5f,  0.5f,  0.5f,
//	-0.5f,  0.5f, -0.5f,
//	-0.5f, -0.5f, -0.5f,
//	-0.5f, -0.5f, -0.5f,
//	-0.5f, -0.5f,  0.5f,
//	-0.5f,  0.5f,  0.5f,
//
//	 0.5f,  0.5f,  0.5f,
//	 0.5f,  0.5f, -0.5f,
//	 0.5f, -0.5f, -0.5f,
//	 0.5f, -0.5f, -0.5f,
//	 0.5f, -0.5f,  0.5f,
//	 0.5f,  0.5f,  0.5f,
//
//	-0.5f, -0.5f, -0.5f,
//	 0.5f, -0.5f, -0.5f,
//	 0.5f, -0.5f,  0.5f,
//	 0.5f, -0.5f,  0.5f,
//	-0.5f, -0.5f,  0.5f,
//	-0.5f, -0.5f, -0.5f,
//
//	-0.5f,  0.5f, -0.5f,
//	 0.5f,  0.5f, -0.5f,
//	 0.5f,  0.5f,  0.5f,
//	 0.5f,  0.5f,  0.5f,
//	-0.5f,  0.5f,  0.5f,
//	-0.5f,  0.5f, -0.5f,
//};
//
//float planeVertices[] = {
//	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
//	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
//	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
//	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
//
//	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
//	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
//	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
//};
//float transparentVertices[] = {
//	// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
//	0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
//	0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
//	1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
//
//	0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
//	1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
//	1.0f,  0.5f,  0.0f,  1.0f,  0.0f
//};
//
//vector<glm::vec3> transparents
//{
//	glm::vec3(-1.5f, 0.0f, -0.48f),
//	glm::vec3(1.5f, 0.0f, 0.51f),
//	glm::vec3(0.0f, 0.0f, 0.7f),
//	glm::vec3(-0.3f, 0.0f, -2.3f),
//	glm::vec3(0.5f, 0.0f, -0.6f)
//};
//
//float quadVertices[] = {
//	// positions   // texCoords
//	-1.0f,  1.0f,  0.0f, 1.0f,
//	-1.0f, -1.0f,  0.0f, 0.0f,
//	 1.0f, -1.0f,  1.0f, 0.0f,
//
//	-1.0f,  1.0f,  0.0f, 1.0f,
//	 1.0f, -1.0f,  1.0f, 0.0f,
//	 1.0f,  1.0f,  1.0f, 1.0f
//};



	////glEnable(GL_DEPTH_TEST); // enable depth testing
	////glDepthFunc(GL_LESS); // set type of testing (less)
	////glEnable(GL_STENCIL_TEST); // enable stencil test
	////glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // enable stencil test function check if not equal
	////glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 1. if failed stencil test 2. if failed depth test 3. if succeeded depth test
	////                                                       misc
	//spotlightActive = false;
	////stbi_set_flip_vertically_on_load(true);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
	//glEnable(GL_STENCIL_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // enable blending
	//glEnable(GL_CULL_FACE);

	/*int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "maximum nr of vertex attributes supported: " << nrAttributes << std::endl;*/
	//Shader lightingShader("vertLight.vs", "fragLight.fss");
	//Shader lightCubeShader("vertLightCube.vs", "fragLightSource.fss");
	//Shader shader("shader.vs", "shader.frag");
	//Shader skyboxShader("skybox.vs", "skybox.frag");















//gl_PointSize: draws a point on screen, we set its size, if we set its size to the clip spaces z for example, it will get biggere the further we're from it
//gl_VertexID: an In variable for hte vertex shader that we can use to index which vertex we're working on
//gl_FragCoord: contains the window-relative coordinates of the current fragment
//gl_FrontFacing: figures out if face is front facing in face culling, we can use to draw different textures/stuff if fragment is front facing or not
//gl_FragDepth: variable that we can manually set depth with, but not as perfermant due to early depth testing
//      we can use detph<condition> we can specify a specific range to write to 
//interface blocks (out{} or in{} ): similar to a struct, only with in and out instead of types (accessed with "interface.value")
//uniform buffer objects: create with GL_UNIFORM_BUFFER, we layout them in a uniform block {}; for example can be used to store projection and view togehter
//-uniform block layout-: what if we want to know the size of variables in uniform shader? we have to take into account their size in bytes and offset from start of block
//      we can use the std140 layout, which explicitly states the memory layout for each variable type by standardizing their respective offsets 
//		the aligned offset of a variable must be equal to a multiple of its base alignment 
//		eg: 1.float - base alignment 4, 2.aligned offset 0, 3.vec3 - base alignment 16, 4.aligned offset 16, mat4 -base 16 aligned offset 32 (by column)... ...9.bool - base 4, offset 148
//		not the most efficient, but garuntees memo0ry stays organized
//using uniform buffers: we set them up similar to other buffers: gen with glGenBuffers, bind with glBindBuffer, then glBufferData with num of bytes in memory
//		multiple uniforms can sit on hte same binding if they share the same data in the binded buffer (for example 2 shaders have uniform Matrices, both binded to the same  bind
//		to set a uniform block to a specific bind/binding point, we use glUniformBlockBinding. example of setting a uniform to a bind/binding point:
//		unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");   
//		glUniformBlockBinding(shaderA.ID, lights_index, 2);
//		binding buffer object ot the same binding point:
//		glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock);
//		and we finish by adding the data with glBufferSubData
