#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>



//forward declare/prototype for window resizing
void framebuffer_size_callback(GLFWwindow* widnow, int WIDTH, int HEIGHT);
//forward declare input processing
void processInput(GLFWwindow* window);
//forward declae callback mouse and scroll function 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffsett, double yoffset);
//ttexture loader
unsigned int loadTexture(const char* path);

//camera position stuff
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool firstMove = true;

//delta time!!!!
float deltaTime = 0.0f;
float lastFrame = 0.0f;


glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // light source location





static const int WIDTH = 800, HEIGHT = 600;
int main() {
	//                                              window creation
	//initiating the glfw window
	//first parameter is a enum and second is its value
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	//creates a window OBJECT,    (asks for dimension, window name, monitor, sharing)
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "glLearn", NULL, NULL);
	//handles failed load
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//makes sure glad is initialized (glad takes care of the opengl functions)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//tells opengl what our widnow dimensions are
	glViewport(0, 0, 800, 600);
	//calls the resizing window function on reize/start
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	


	
	//                                                       misc
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	/*int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "maximum nr of vertex attributes supported: " << nrAttributes << std::endl;*/

	//vertex data
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
	};

	unsigned int indices[] = {
		0,1,3, //triangle 1
		1,2,3  //triangle 2
	};



	//          
	Shader lightingShader("vertLight.vs", "fragLight.fss");
	Shader lightCubeShader("vertLightCube.vs", "fragLightSource.fss");

	//                                                             lighting boxes:
	unsigned int lightVBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO); // create 1 vertex array for cube vao
	glGenBuffers(1, &lightVBO); // create buffer for vbo

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO); // bind buffer for vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // set attributes

	glBindVertexArray(cubeVAO);

	//position attribs
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal attribs
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coords attribs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	unsigned int emissionMap = loadTexture("matrix.jpg");

	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);




	//                                                            render loop - lighting
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime()); //per frame time logic
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//input processing + buffer clearing/rendering
		processInput(window);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		//lightPos = glm::vec3(sin(10 * static_cast<float>(glfwGetTime())/4), cos(10 * static_cast<float>(glfwGetTime()) / 4), sin(10 * static_cast<float>(glfwGetTime()) / 4));
		//lightPos.x = (sin(10 * static_cast<float>(glfwGetTime() / 2)));

		//set uniforms for cube
		lightingShader.use();
		//lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("viewPos", camera.Position);
		


		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.7f, 0.7f, 0.7f);
		lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point lights
		for (int i = 0; i < 4; i++) {
			std::string baseStr = "pointLights[" + std::to_string(i) + "]";

			lightingShader.setVec3(baseStr + ".position", pointLightPositions[i]);
			lightingShader.setVec3(baseStr + ".ambient", 0.4f, 0.7f, 0.7f);
			lightingShader.setVec3(baseStr + ".diffuse", 0.1f, 0.9f, 0.1f);
			lightingShader.setVec3(baseStr + ".specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat(baseStr + ".constant", 1.0f);
			lightingShader.setFloat(baseStr + ".linear", 0.09f);
			lightingShader.setFloat(baseStr + ".quadratic", 0.032f);
		}

		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 0.0f, 1.0f, 0.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09f);
		lightingShader.setFloat("spotLight.quadratic", 0.032f);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		
		
		//glm::vec3 lightColor;
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);

		glm::vec3 ambientColor = glm::vec3(0.2f);
		glm::vec3 diffuseColor = glm::vec3(0.5f);
		glm::vec3 specularColor = glm::vec3(1.0f);

		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor); // darken diffuse light a bit
		lightingShader.setVec3("light.specular", specularColor);
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.09f);
		lightingShader.setFloat("light.quadratic", 0.032f);

		lightingShader.setFloat("material.shininess", 32.0f);


		//binding different layers for different lightings
		lightingShader.setInt("material.diffuse", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		lightingShader.setInt("material.specular", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		lightingShader.setInt("material.emission", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);

		lightCubeShader.use(); // quicikly setting the color of the light cube
		lightCubeShader.setVec3("color", diffuseColor + glm::vec3(0.7f));
		lightingShader.use();

		//view and projection transforms
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		
		//world transforms
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);


		//drawing the 10 cubes
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			lightingShader.setMat4("model", model);
		
			//draw cube
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}




		// also draw the lamp object(s)
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		



		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	//														end
	//terminate resources on exit
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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