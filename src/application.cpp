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

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

     float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    {
        // Move to another scope because we are creating these vertex arrays on the stack instead of heap
        // Runs into an infinite loop when exiting since glGetError throws an error when there is not a valid context
        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        IndexBuffer ib(indices, 6);

        VertexBufferLayout layout;
        layout.addFloat(2);

        va.addBuffer(vb, layout);

        Shader shader("../res/shaders/basic.shader");
        shader.bind();
        

        float r = 0.0f;
        float increment = 0.05f;

        Renderer renderer;

		while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 ) {
            renderer.clear();

            shader.bind();
            shader.setUniform4f("u_Color", r, 0.6f, 0.8f, 1.0f);


            renderer.draw(va, ib, shader);

            // increment r
            if (r < 0.0f || r > 1.0f)
                increment *= -1.0;
            r += increment;

                        // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}