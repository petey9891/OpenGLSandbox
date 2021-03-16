// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"

#define DEBUG true

#include "Debug.h"

//Animation speed
#define ANIMSTEP 0.5

#define CORES 8

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

GLFWwindow* InitWindow() {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return nullptr;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGLSandbox", NULL, NULL);
	if(window == nullptr) {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // Syncs with refresh rate

    // Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return nullptr;
	}

    printf("Using OpenGL Version: %s\n", glGetString(GL_VERSION) );

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return window;
}


int main( void )
{

    GLFWwindow* window = InitWindow();
    if (!window) {
        return -1;
    }

	// Clear the whole screen (front buffer)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    {
        Shader shader("../res/shaders/basic.shader");
        shader.bind();

        VertexBuffer verticesBuffer(vertices, 36 * sizeof(float));
        VertexBuffer vcoordsBuffer(vcoords, 24 * sizeof(float));


        VertexArray verticesArray;
        VertexBufferLayout verticiesLayout;
        verticiesLayout.addFloat(3);
        verticesArray.addBuffer(verticesBuffer, verticiesLayout);

        VertexArray vcoordsArray;
        VertexBufferLayout vcoordsLayout;
        vcoordsLayout.addFloat(2);
        vcoordsArray.addBuffer(vcoordsBuffer, vcoordsLayout);

        Renderer renderer;

		while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 ) {
            renderer.clear();
      
            t += 0.01f;

            shader.bind();
            // vcoordsBuffer.bind();
            shader.setUniform1f("time", t);
            shader.setUniform1f("age", float(t - updateTime));

            shader.setUniform1f("temperature", temperature);
            shader.setUniform1fv("thread", CORES, thread);

            renderer.drawArrays(verticesArray, shader);

            // GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 12));


            // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}