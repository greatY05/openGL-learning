#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"

//forward declare/prototype for window resizing
void framebuffer_size_callback(GLFWwindow* widnow, int width, int height);



//handles viewport resizing on window resizing
void framebuffer_size_callback(GLFWwindow* widnow, int width, int height) {
	glViewport(0, 0, width, height);

}

//processes inputs
void processInput(GLFWwindow* window) {
	//give the window we're working on and hte key
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		//turns "should we close the window?" to true, therefore killing the while loop in main on the next iteration
		glfwSetWindowShouldClose(window, true);
	}
}

//vertex shader source code saved in a c style string
const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";


const char* fragmentShaderSource = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
//"uniform vec4 ourColor;\n"
"void main() {\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";



int main() {
	//                                              window creation
	//initiating the glfw window
	//first parameter is a enum and second is its value
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//creates a window OBJECT,    (asks for dimension, window name, monitor, sharing)
	GLFWwindow* window = glfwCreateWindow(800, 600, "glLearn", NULL, NULL);
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


	


	/*int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "maximum nr of vertex attributes supported: " << nrAttributes << std::endl;*/

	//vertex data
	float vertices[] = {
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};



	float verticesL[] = {
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	unsigned int indices[] = {
		0,1,3, //triangle 1
		1,2,3  //triangle 2
	};
	//                                                         vbo
	//vertrex buffer object - glGenBuffer generates a buffer bts and brings back a reference to them
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	//binds to object buffer we created above to our VBO buffer type
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//copies user defined data to our current buffer. takes buffer type, size, data, and managing mode
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	////enables vertex attributes so line above works, since its disabled by default
	//glEnableVertexAttribArray(0);

	//                                                         vao
	unsigned int VAO;
	glGenVertexArrays(1, &VAO); //binds created vao object to VAO var
	//bind vertex array object
	glBindVertexArray(VAO);
	//copy our vertices array into a buffer for opengl
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//set our vertex attribute pointers
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//                                                         ebo
	// we use EBO (element array buffer) to avoid redundancy and vertex duplication
	
	unsigned int EBO;
	glGenBuffers(1, &EBO); // create buffer EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //GL_ELEMENT_ARRAY_BUFFER for ebo (element  buffer object)
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // type buffer, size, data, type of usage (static)


	//RELATION BETWEEN VBO-VAO-EBO:
	//vbo - a buffer, raw vertex data in the gpu memory, ensures faster access
	//vao - a config object, remembers vertex data organization and how to interpet it
	//ebo - uses indexing to reuse vertex. instead of defining verticies multiple times, this creates primitives
	



	//                                                       shaders
	//vertex shader
	////using glCreateShader to create a shader we want (here a vertex shader)
	//unsigned int vertexShader;
	//vertexShader = glCreateShader(GL_VERTEX_SHADER);
	////attaching our source code from above Main() and compiling the shader
	////glShaderSource - gets the shader, the element in it, hte source code, and total length of it
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);

	////fragment shader
	////similar to vertex, we create a buffer, create a type buffer fragment onto it, assign it the source code and compilei t
	//unsigned int fragmentShader;
	//fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//glCompileShader(fragmentShader);
	/////debugging to see if theres any compile time errors
	//int success;
	//char infoLog[512];
	//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	//if (!success) {
	//	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	//}

	////                                          creating the shader program
	//unsigned int shaderProgram;
	//shaderProgram = glCreateProgram();

	
	//float timeValue = glfwGetTime();
	//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // finds the location of "ourColor" form the fragment shader
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); //sets the value of the shader (ourColor in tthis case)


	//attach fragment, vertex and link them with linker
	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);
	//glLinkProgram(shaderProgram);
	////compile time checking for errors:
	//glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // querys the shader about object parameters (link status of the shader program)
	//if (!success) {
	//	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog); //returns info log about shader,able to check compile errors
	//}
	////sets shader as the current program to draw on
	////glUseProgram(shaderProgram);
	////deleting the shaders now that we linked them
	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);
	// sets out how to interpet the data of the vertex data as so:
	// 1. index (0), 2. size specifier (3 as in vec3), 3. type (float), 4. normalized (no), 
	// 5. space between attributes (3f since 3 point array of float), 6. offset from beginning (0 times size of type)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//VERTEX - first step, processes vertexes individually, responsible for coordination and passes attributes to fragment shader
	//FRAGMENT - handles colors of pixels, runs once for each pixel drawn (not hidden ones)
	//shader program - links vertex and fragment, thus building amnd completing the pipeline




	//                                                     image
	

	

	//creating id for the image
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1); // takes the amount of images to generate and where to store them (int array type)
	glActiveTexture(GL_TEXTURE1); // activate texture unit 0 
	glBindTexture(GL_TEXTURE_2D, texture1); //bind texture


	//wrapping/filtering options

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0); // takes image location, width, height and color channels
	if (data) {
		//generate image data:
		//parameters: 1. texture target (channel 2d), 2. mipmap level (0), 3. store texture format (with rgb values here)
		// 4-5. wid-hi 6. (0 always, legacy thingy) 7-8. data type of source image (pass current stoired values) 9. image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
	{
		
		std::cout << "failed to load texture " << std::endl;
	}


	stbi_image_free(data); //free image memory now that its generated
	stbi_set_flip_vertically_on_load(true);
	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "failed to load texture " << std::endl;
	}



	stbi_image_free(data); //free image memory now that its generated

	

	




	//					                  				render loop
	 
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	Shader ourShader("vertex.vs", "fragment.fss");

	ourShader.use();
	ourShader.setInt("texture2", 1);

	float offset = 0.5f;
	
	//starts the loop, while we dont need to close it - run
	while (!glfwWindowShouldClose(window)) {
		//calls process inputs function
		processInput(window);
		//clear colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//draw the triangle/use ou shader program
		//glUseProgram(shaderProgram);
		//glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//ourShader.setFloat("xOffset", offset);
		//glBindVertexArray(VAO);

		//pulsing green color
		//float timeValue = glfwGetTime();
		//float greenValue = sin(timeValue) / 2.0f + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // location of ourColor
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		//elemement drawing method
	    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //drawing mode, number of indices, type, starting index

		//array drawing method

		//glDrawArrays(GL_TRIANGLES, 0, 3); // drawing mode, starting index, amount of vertices


		//swaps front and back buffer to prevent artifacts
		glfwSwapBuffers(window);
		//checks if any events happened, and calls them if so
		glfwPollEvents();
	}

	// STATE MACHINE:
	// glBindBuffer and glUseProgram dont immidately draw, but set the internal state
	// after that, commands like glDrawArray use whatever state said functions chan


	//														end
	//terminate resources on exit
	glfwTerminate();
	return 0;
}