#include <Windows.h>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderPair {
	std::string vertex;
	std::string fragment;
};

static ShaderPair ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream sstring[2];

	enum class ShaderType
	{
		NONE = -1, VERTEX, FRAGMENT
	};

	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			sstring[(int)type] << line << std::endl;
		}
	}
	return { sstring[0].str(), sstring[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& Shader) {
	unsigned int shader = glCreateShader(type);
	const char* str = Shader.c_str();
	glShaderSource(shader, 1, &str, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) malloc(sizeof(char) * length);
		glGetShaderInfoLog(shader, length, &length, message);

		OutputDebugString(message);
		OutputDebugString("\n");
		glDeleteShader(shader);
		return 0;
	}

	OutputDebugString("Shader compilation OK\n");
	return shader;
}

static unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader) {

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}


int __cdecl WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// initialize GLEW
	if (glewInit() != GLEW_OK)
		return -1;


	GLuint buffer;
	GLdouble verts[] = { -0.5 , -0.5, -0.5 , 0.5 , 0.5 , 0.5 , 0.5, -0.5 };
	unsigned int indices[] = {0, 1, 2, 2, 3, 0};
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), 0);

	unsigned int ib;
	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	ShaderPair basicShaderSrc = ParseShader("shaders/Basic.shader");

	unsigned int shader = CreateShader(basicShaderSrc.vertex, basicShaderSrc.fragment);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}