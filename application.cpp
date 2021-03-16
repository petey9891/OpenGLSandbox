// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define CORES 8

#define ASSERT(x) if (!(x)) assert(false)

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLCheckError())

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLCheckError()
{
    while (GLenum error = glGetError())
    {
        
        std::cout << "[OpenGL Error] ";
          switch(error) {
              case GL_INVALID_ENUM :
                  std::cout << "GL_INVALID_ENUM : An unacceptable value is specified for an enumerated argument.";
                  break;
              case GL_INVALID_VALUE :
                  std::cout << "GL_INVALID_OPERATION : A numeric argument is out of range.";
                  break;
              case GL_INVALID_OPERATION :
                  std::cout << "GL_INVALID_OPERATION : The specified operation is not allowed in the current state.";
                  break;
              case GL_INVALID_FRAMEBUFFER_OPERATION :
                  std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION : The framebuffer object is not complete.";
                  break;
              case GL_OUT_OF_MEMORY :
                  std::cout << "GL_OUT_OF_MEMORY : There is not enough memory left to execute the command.";
                  break;
              case GL_STACK_UNDERFLOW :
                  std::cout << "GL_STACK_UNDERFLOW : An attempt has been made to perform an operation that would cause an internal stack to underflow.";
                  break;
              case GL_STACK_OVERFLOW :
                  std::cout << "GL_STACK_OVERFLOW : An attempt has been made to perform an operation that would cause an internal stack to overflow.";
                  break;
              default :
                  std::cout << "Unrecognized error" << error;
          }
          std::cout << std::endl;
          return false;
    }
    return true;
}


float temperature = 30.0f;
float thread[CORES];
float tmpTemperature = 40.0f;
float tmpThread[CORES];

float t = 0.0f;
float updateTime = -10.0f;

static const GLfloat vertices[] = {
    -1.0f,
    -1.0f,
    0.0f,
    -1.0f,
    1.0f,
    0.0f,

    -0.33333333333f,
    -1.0f,
    0.0f,
    -0.33333333333f,
    1.0f,
    0.0f,

    -0.33333333333f,
    -1.0f,
    0.0f,
    -0.33333333333f,
    1.0f,
    0.0f,

    0.33333333333f,
    -1.0f,
    0.0f,
    0.33333333333f,
    1.0f,
    0.0f,

    0.33333333333f,
    -1.0f,
    0.0f,
    0.33333333333f,
    1.0f,
    0.0f,

    1.0f,
    -1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
};

static const GLfloat vcoords[] = {
    0.0f,
    0.0f,
    -0.866f,
    0.5f,

    0.0f,
    -1.0f,
    -0.866f,
    -0.5f,

    0.0f,
    0.0f,
    0.0f,
    -1.0f,

    0.866f,
    0.5f,
    0.866f,
    -0.5f,

    0.0f,
    0.0f,
    0.866f,
    0.5f,

    -0.866f,
    0.5f,
    0.0f,
    1.0f,
};

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static struct ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    int type = -1;


    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = 0;
            else if (line.find("fragment") != std::string::npos)
                type = 1;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile status: " << result << std::endl;
    if ( result == GL_FALSE )
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout 
            << "Failed to compile "
            << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "shader"
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // create a shader program
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    GLint program_linked;

    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    std::cout << "Program link status: " << program_linked << std::endl;
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program, 1024, &log_length, message);
        std::cout << "Failed to link program" << std::endl;
        std::cout << message << std::endl;
    }

    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main( void )
{
	GLuint program, vert, frag, vbo, vbocoord;
	GLint posLoc, coordLoc, temperatureLoc, threadLoc, timeLoc, ageLoc, result;

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	// Open a window and create its OpenGL context
        GLFWwindow* window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

        std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Clear the whole screen (front buffer)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), vertices, GL_STATIC_DRAW));

	GLCall(glGenBuffers(1, &vbocoord));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbocoord));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vcoords, GL_STATIC_DRAW));

	// Get vertex attribute and uniform locations
	GLCall(posLoc = glGetAttribLocation(program, "pos"));
	GLCall(coordLoc = glGetAttribLocation(program, "coord"));
	GLCall(temperatureLoc = glGetUniformLocation(program, "temperature"));
	GLCall(threadLoc = glGetUniformLocation(program, "thread"));
	GLCall(timeLoc = glGetUniformLocation(program, "time"));
	GLCall(ageLoc = glGetUniformLocation(program, "age"));

	// Set our vertex data
	GLCall(glEnableVertexAttribArray(posLoc));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0));

	GLCall(glEnableVertexAttribArray(coordLoc));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbocoord));
	GLCall(glVertexAttribPointer(coordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0));

	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 ) {
        // Clear the screen
         glClear(GL_COLOR_BUFFER_BIT);

        // Draw
        t += 0.01f;

		GLCall(glUniform1f(timeLoc, t));
		GLCall(glUniform1f(ageLoc, float(t - updateTime)));

		GLCall(glUniform1f(temperatureLoc, temperature));
		GLCall(glUniform1fv(threadLoc, CORES, thread));
		GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 12));
        
        // Swap buffers
        GLCall(glfwSwapBuffers(window));

        // Poll
        GLCall(glfwPollEvents());
    }

	// Cleanup VBO
	glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vbocoord);
	glDeleteProgram(shader);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
