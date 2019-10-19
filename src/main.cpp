#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include <vector>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600


std::vector<GLfloat> vertices;
//reading shader source from files
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = ourColor;\n"
                                   "}\n";

//
void generate_square(GLfloat x, GLfloat y, GLfloat a){

    vertices.push_back(x-(a/2)); vertices.push_back(y-(a/2)); vertices.push_back(0.0f);
    vertices.push_back(x-(a/2)); vertices.push_back(y+(a/2)); vertices.push_back(0.0f);
    vertices.push_back(x+(a/2)); vertices.push_back(y-(a/2)); vertices.push_back(0.0f);

    vertices.push_back(x-(a/2)); vertices.push_back(y+(a/2)); vertices.push_back(0.0f);
    vertices.push_back(x+(a/2)); vertices.push_back(y-(a/2)); vertices.push_back(0.0f);
    vertices.push_back(x+(a/2)); vertices.push_back(y+(a/2)); vertices.push_back(0.0f);

}

void generate_carpet(GLfloat x, GLfloat y, GLfloat a, GLuint maxDepth, GLuint depth){
    if(depth < maxDepth){
        if(depth == 0) vertices.clear();
        generate_square(x, y, a);
        generate_square(x, y, a / 3);
        generate_carpet(x - a / 3, y - a / 3, a / 3, maxDepth, depth + 1);
        generate_carpet(x - a / 3, y, a / 3, maxDepth, depth + 1);
        generate_carpet(x - a / 3, y + a / 3, a / 3, maxDepth, depth + 1);
        generate_carpet(x + a / 3, y, a / 3, maxDepth, depth + 1);
        generate_carpet(x + a / 3, y + a / 3, a / 3, maxDepth, depth + 1);
        generate_carpet(x + a / 3, y - a / 3, a / 3, maxDepth, depth + 1);
        generate_carpet(x, y - a / 3, a / 3, maxDepth, depth + 1);
        generate_carpet(x, y + a / 3, a / 3, maxDepth, depth + 1);

    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void draw_carpet(GLuint VAO, GLuint shaderProgram, float* outerColor, float* innerColor){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //state-setting
    glClear(GL_COLOR_BUFFER_BIT); //state-using
    for(unsigned long long i = 0; i< vertices.size(); i+=6){
        GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        if(i%4 == 0){
            glUniform4f(vertexColorLocation, outerColor[0], outerColor[1], outerColor[2], outerColor[3]);
        }else{
            glUniform4f(vertexColorLocation, innerColor[0], innerColor[1], innerColor[2],innerColor[3]);
        }
        glDrawArrays(GL_TRIANGLES, i, 6);
    }

}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dywan Sierpińskiego", nullptr, nullptr);
    if (window == nullptr)
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
    //shaders compilation
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //fragment shader1
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) //checking if compilation was sucsessfull
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //linking shaders programs
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if(!success) //checking if compilation was sucsessfull
    {
        glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    int maxDepth = 1;
    float color1[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float color2[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glUseProgram(shaderProgram);
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);
        //render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Control window");
        ImGui::SliderInt("Stopien rekurencji", &maxDepth, 1, 8);
        ImGui::ColorEdit4("Outer color", color1);
        ImGui::ColorEdit4("Inner color", color2);
        ImGui::End();
        generate_carpet(0, -.2, 1.5, maxDepth, 0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        draw_carpet(VAO, shaderProgram, color1, color2);

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //events & swap buffer
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}