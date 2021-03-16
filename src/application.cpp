#include "led-matrix.h"

// Include EGL
#include <EGL/egl.h>
#include <GLES3/gl3.h>

// Include GLEW
// #include <GL/glew.h>

// Include GLFW
// #include <GLFW/glfw3.h>

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"

#define DEBUG true

#include "Debug.h"

//Resolution, three panels with 64x64 each.
#define W 192
#define H 64

//Animation speed
// #define ANIMSTEP 0.5

#define CORES 8

using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

using namespace rgb_matrix;
using namespace std;

float animstep = 0.01f;
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

// GLFWwindow* InitWindow() {
// 	// Initialise GLFW
// 	if (!glfwInit()) {
//         int errorCode;
//         const char* description;
//         while ((errorCode = glfwGetError(&description)) != GLFW_NO_ERROR) {
//             printf("Error code: %d\n", errorCode);
//             printf("Message: %s\n", description);
//         }
// 		fprintf( stderr, "Failed to initialize GLFW\n" );
// 		getchar();
// 		return nullptr;
// 	}


// 	glfwWindowHint(GLFW_SAMPLES, 4);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//     glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

//     // Open a window and create its OpenGL context
//     GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGLSandbox", NULL, NULL);
// 	if(window == nullptr) {
// 		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
// 		getchar();
// 		glfwTerminate();
// 		return nullptr;
// 	}
// 	glfwMakeContextCurrent(window);

//     glfwSwapInterval(1); // Syncs with refresh rate

//     // Initialize GLEW
// 	glewExperimental = true; // Needed for core profile
// 	if (glewInit() != GLEW_OK) {
// 		fprintf(stderr, "Failed to initialize GLEW\n");
// 		getchar();
// 		glfwTerminate();
// 		return nullptr;
// 	}

    // printf("Using OpenGL Version: %s\n", glGetString(GL_VERSION) );

// 	// Ensure we can capture the escape key being pressed below
// 	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

//     return window;
// }

static const EGLint configAttribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 8,

    EGL_SAMPLE_BUFFERS, 1,
    EGL_SAMPLES, 4,

    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE};

// Width and height of the desired framebuffer
static const EGLint pbufferAttribs[] = {
    EGL_WIDTH,
    W,
    EGL_HEIGHT,
    H,
    EGL_NONE,
};

static const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2,
                                        EGL_NONE};

int main(int argc, char* argv[]) {
	EGLDisplay display;
	int major;
	int minor;
	int desiredWidth;
	int desiredHeight;

	// Setup EGL
	if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
		fprintf(stderr, "Failed to get EGL display! Error: %s\n", eglGetError());
		return EXIT_FAILURE;
	}

	if (eglInitialize(display, &major, &minor) == EGL_FALSE) {
		fprintf(stderr, "Failed to get EGL version! Error: %s\n", eglGetError());
		eglTerminate(display);
		return EXIT_FAILURE;
	}

	printf("Initialized EGL version: %d.%d\n", major, minor);

	// EGL Config
	EGLint numConfigs;
	EGLConfig config;
	if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
		fprintf(stderr, "Failed to get EGL Config! Error: %s\n", eglGetError());
		eglTerminate(display);
		return EXIT_FAILURE;
	}

	// EGL Surface
	EGLSurface surface = eglCreatePbufferSurface(display, config, pbufferAttribs);
	if (surface == EGL_NO_SURFACE) {
		fprintf(stderr, "Failed to create EGL surface! Error: %s\n", eglGetError());
		eglTerminate(display);
		return EXIT_FAILURE;
	}

	// Bind OpenGL API
	eglBindAPI(EGL_OPENGL_API);

	// EGL Context
	EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if (context == EGL_NO_CONTEXT) {
		fprintf(stderr, "Failed to create EGL context! Error: %s\n", eglGetError());
		eglDestroySurface(display, surface);
		eglTerminate(display);
		return EXIT_FAILURE;
	}

	eglMakeCurrent(display, surface, surface, context);

	desiredWidth = pbufferAttribs[1];
	desiredHeight = pbufferAttribs[3];

	glViewport(0, 0, desiredWidth, desiredHeight);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	printf("GL Viewport size: %dx%d\n", viewport[2], viewport[3]);


	if (desiredWidth != viewport[2] || desiredHeight != viewport[3]) {
		fprintf(stderr, "Error! the glViewport/glgetIntergerv are not working! EGL might be faulty!\n");
		return EXIT_FAILURE;
	}

	// Clear the whole screen (front buffer)
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    {
        Shader shader("../res/shaders/Circle.shader");
        shader.bind();

	    GLuint program, vert, frag, vbo, vbocoord;
    	GLint posLoc, coordLoc, temperatureLoc, threadLoc, timeLoc, ageLoc, loadingLoc, fadeLoc, result;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &vbocoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbocoord);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vcoords, GL_STATIC_DRAW);

        // Get vertex attribute and uniform locations
        GLCall(posLoc = glGetAttribLocation(shader.m_RendererID, "pos"));
        GLCall(coordLoc = glGetAttribLocation(shader.m_RendererID, "coord"));
        GLCall(temperatureLoc = glGetUniformLocation(shader.m_RendererID, "temperature"));
        GLCall(threadLoc = glGetUniformLocation(shader.m_RendererID, "thread"));
        GLCall(timeLoc = glGetUniformLocation(shader.m_RendererID, "time"));
        GLCall(ageLoc = glGetUniformLocation(shader.m_RendererID, "age"));
        GLCall(loadingLoc = glGetUniformLocation(shader.m_RendererID, "loading"));
        GLCall(fadeLoc = glGetUniformLocation(shader.m_RendererID, "fade"));

        // Set our vertex data
        GLCall(glEnableVertexAttribArray(posLoc));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCall(glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0));

        GLCall(glEnableVertexAttribArray(coordLoc));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbocoord));
        GLCall(glVertexAttribPointer(coordLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0));

        // VertexBuffer verticesBuffer(vertices, 36 * sizeof(float));
        // VertexBuffer vcoordsBuffer(vcoords, 24 * sizeof(float));


        // VertexArray verticesArray;
        // VertexBufferLayout verticiesLayout;
        // verticiesLayout.addFloat(3);
        // verticesArray.addBuffer(verticesBuffer, verticiesLayout);

        // VertexArray vcoordsArray;
        // VertexBufferLayout vcoordsLayout;
        // vcoordsLayout.addFloat(2);
        // vcoordsArray.addBuffer(vcoordsBuffer, vcoordsLayout);

        // LED Matrix Settings
        RGBMatrix::Options defaults;
        rgb_matrix::RuntimeOptions runtime;

        defaults.hardware_mapping = "adafruit-hat-pwm";
        defaults.led_rgb_sequence = "BGR";
        defaults.pwm_bits = 11;
        defaults.pwm_lsb_nanoseconds = 50;
        // defaults.panel_type = "FM6126A";
        defaults.rows = 64;
        defaults.cols = 192;
        defaults.chain_length = 1;
        defaults.parallel = 1;
        // defaults.brightness = 100; // 60 is a good brightness for downtime

        runtime.drop_privileges = 0;
        runtime.gpio_slowdown = 1;

        // Create matrix
        RGBMatrix* matrix = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults, &runtime);

        if (matrix == nullptr) {
            fprintf(stderr, "Error! Unable to create matrix!\n");
            return EXIT_FAILURE;
        }

        // Create canvas
        FrameCanvas* canvas = matrix->CreateFrameCanvas();

        // Buffer of pixels
	    unsigned char* buffer = (unsigned char*)malloc(W * H * 3);

        Renderer renderer;

		// while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 ) {
        GLCall(glUniform1f(loadingLoc, true));
        GLCall(glUniform1f(fadeLoc, 1.0f));

        float fadeLevel = 1.0f;


        bool loading = true;
        bool changingScene = false;


        while (true) {
            renderer.clear();
        
            // t += 0.01f;
            t += 0.03f;

		    GLCall(glUniform1f(timeLoc, t));
		    GLCall(glUniform1f(ageLoc, float(t - updateTime)));
		    GLCall(glUniform1f(temperatureLoc, temperature));
		    GLCall(glUniform1fv(threadLoc, CORES, thread));
		    glDrawArrays(GL_TRIANGLE_STRIP, 0, 12);


            if (loading) {
                if (t > 10) {
                    // Fade loading screen
                    GLCall(glUniform1f(fadeLoc, fadeLevel));
                    fadeLevel -= 0.03f;

                    if (fadeLevel <= 0.0f) {
                        loading = false;
                        GLCall(glUniform1f(loadingLoc, false));
                        GLCall(glUniform1f(fadeLoc, 0.0f));
                        
                        // Need to fade into new scene
                        changingScene = true;
                    }
                }
            }

            if (changingScene) {
                GLCall(glUniform1f(fadeLoc, fadeLevel));
                fadeLevel += 0.03f;

                if (fadeLevel >= 1.0f) {
                    GLCall(glUniform1f(fadeLoc, 1.0f));
                    changingScene = false;
                }                
            }

            // shader.bind();
            // shader.setUniform1f("time", t);
            // shader.setUniform1f("age", float(t - updateTime));

            // shader.setUniform1f("temperature", temperature);
            // // shader.setUniform1fv("thread", CORES, thread);
            // GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 12));
            // // renderer.drawArrays(verticesArray, shader);

            glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, buffer);

            for (int x = 0; x < W; x++) {
                for (int y = 0; y < H; y++) {
                    int index = 3*(x+y*W);
                    
                    // printf("rgb for pixel %d: rgb(%d, %d, %d)\n", x+y*W, buffer[index], buffer[index+1], buffer[index+2]);
                    // canvas->SetPixel(x, y, 255, 0, 255);
                    canvas->SetPixel(x, y, buffer[index], buffer[index+1], buffer[index+2]);
                }
            }
            canvas = matrix->SwapOnVSync(canvas);
        }

        free(buffer);
	    canvas->Clear();
    }

	// Close OpenGL window and terminate GLFW
	// glfwTerminate();

    eglDestroyContext(display, context);
	eglDestroySurface(display, surface);
    eglTerminate(display);

	return 0;
}