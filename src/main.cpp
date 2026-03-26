#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image.cpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader_s.h>
#include <model_s.h>
#include <quad_s.h>
#include <cube_s.h>
#include <table_s.h>

#include <memory>
#include <iostream>

void processInput(GLFWwindow *window, glm::mat4& view_dir, glm::mat4& view_loc, float& fov, float& n, float& f);
glm::mat4 makePerspectiveMatrix(float fov, float aspect_ratio, float n, float f);
void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height);

int main(int argc, char *argv[])
{
    std::cout << "Compiled Successfully" << std::endl;
    // input handling
    // --------------------
    const unsigned int SCR_WIDTH = atoi(argv[1]);
    const unsigned int SCR_HEIGHT = atoi(argv[2]);
    const unsigned int SCR_SHOT_WIDTH = atoi(argv[3]);
    const unsigned int SCR_SHOT_HEIGHT = atoi(argv[4]);
    const char *SCR_SHOT_PATH = argv[5];

    // setup
    // --------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // texture rendering buffer setup
    // --------------------
    unsigned int frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    unsigned int depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    unsigned int renderedTexture;
    glGenTextures(1, &renderedTexture);

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

    // post processing shader setup
    // --------------------
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    unsigned int post_shader = create_shader("src/post_vert.vs", "src/post_frag.fs");
    glUseProgram(post_shader);
    shader_set_int(post_shader, "scr_tex", 0);

    Quad screen_quad = Quad();

    // scene setup (only section (aside from render loop) you should be touching)
    // --------------------

    // shader generation
    unsigned int main_shader = create_shader("src/shader_vert.vs", "src/shader_frag.fs"); // you can name your shader files however you like

    // load models
    // -----------
    //load_model("models/teapot.obj");

    // rendering matricies setup
    // --------------------
    // create fov for perspective matrix calculation
    float perspective_fov = 0.785f; // modify for different zoom levels
    float n = 0.1f; // near plane z
    float f = 100.f; // far plane z

    // seperate view matrix components to allow for camera movement and rotation
    glm::mat4 view_dir = glm::mat4(1.0f);
    glm::mat4 view_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 model = glm::mat4(1.0f);

    // render loop
    // --------------------
    while (!glfwWindowShouldClose(window))
    {
        // input processing
        // --------------------
        processInput(window, view_dir, view_loc, perspective_fov, n, f);
        saveScreenshotToFile(SCR_SHOT_PATH, window, renderedTexture, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // toggle wireframe

        glActiveTexture(GL_TEXTURE0);

        // render to texture
        // --------------------
        glUseProgram(main_shader);
	glm::mat4 view = view_dir * view_loc;
	shader_set_mat4(main_shader, "view", (GLfloat *)&view);
	glm::mat4 perspective = makePerspectiveMatrix(perspective_fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, n, f);
	shader_set_mat4(main_shader, "perspective", (GLfloat *)&perspective);

        glViewport(0, 0, SCR_SHOT_WIDTH, SCR_SHOT_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	shader_set_mat4(main_shader, "model", (GLfloat *)&model);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // disable wireframe

        // render to screen
        // --------------------
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glUseProgram(post_shader);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screen_quad.render();

        // poll for inputs and display screen buffer
        // --------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    screen_quad.deallocate();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, glm::mat4& view_dir, glm::mat4& view_loc, float& fov, float& n, float& f)
{
    float spd = 0.0005f;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        spd *= 5;
    // camera movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, spd * glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, -spd * glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, spd * glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, -spd * glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, spd * glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        view_loc = glm::translate(view_loc, -spd * glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    // camera rotation
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, -glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, glm::vec3(view_dir[0][1], view_dir[1][1], view_dir[2][1]));
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, -glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, glm::vec3(view_dir[0][0], view_dir[1][0], view_dir[2][0]));
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        view_dir = glm::rotate(view_dir, spd, -glm::vec3(view_dir[0][2], view_dir[1][2], view_dir[2][2]));
    // camera zoom
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        fov += spd * 0.5f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        fov -= spd * 0.5f;
    // reset camera position
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        view_loc = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // reset camera rotation
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        view_dir = glm::mat4(1.0f);
    // reset camera fov
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        fov = 0.785f;
    // near and far plane distances
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        n -= spd;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        n += spd;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        f -= 10 * spd;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        f += 10 * spd;
}

// recreation of glm's perspective matrix with (l, r, b, t) inputs
glm::mat4 makePerspectiveMatrix(float n, float f, float l, float r, float b, float t) {
    glm::mat4 perspective = glm::mat4();
    // remember that glm does matrix[c][r] format
    perspective[0] = glm::vec4(2 * n / (r - l), 0.0f, 0.0f, 0.0f);
    perspective[1] = glm::vec4(0.0f, 2 * n / (t - b), 0.0f, 0.0f);
    perspective[2] = glm::vec4((r+l)/(r-l), (t+b)/(t-b), -(f+n)/(f-n), -1.0f);
    perspective[3] = glm::vec4(0.0f, 0.0f, -(2 * f * n)/(f-n), 0.0f);
    return perspective;
}

// recreation of glm's glm::perspective()
glm::mat4 makePerspectiveMatrix(float fov, float aspect_ratio, float n, float f) {
    float t = tan(fov * 0.5) * n;
    float r = t * aspect_ratio;
    return makePerspectiveMatrix(n, f, -r, r, -t, t);
}

void saveScreenshotToFile(std::string filename, GLFWwindow *window, int renderedTexture, int scr_width, int scr_height) {    
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        std::cout << "argsataerra" << std::endl;
        std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(scr_width * scr_height * 4 * sizeof(unsigned int));
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

        stbi_flip_vertically_on_write(true);
        int ret = stbi_write_png(filename.c_str(), scr_width, scr_height, 4, data.get(), scr_width * 4);
        if (ret == 0)
        {
            std::cout << "Failed to capture screenshot" << std::endl;
        }
    }
}
