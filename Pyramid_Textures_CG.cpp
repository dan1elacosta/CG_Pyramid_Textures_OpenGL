//LLAMANDO A BIBLIOTECAS // ---------------------------------------------------------------
// 1. GLAD: Carga los punteros a las funciones de OpenGL según el driver de la GPU.
// DEBE ir antes que GLFW.
#include <glad/glad.h>

// 2. GLFW: Administra la ventana del sistema operativo, contexto de OpenGL y entradas (teclado/mouse).
#include <GLFW/glfw3.h>

// Biblioteca estándar de C++ para salida de errores e información en consola.
#include <iostream>

// 3. GLM: Biblioteca matemática para vectores (vec3, vec4) y matrices (mat4).
#include <glm/glm.hpp>

// 4. GLM Matrix Transform: Funciones para crear matrices de Traslación, Rotación, Escalado, Vista y Proyección.
#include <glm/gtc/matrix_transform.hpp>

// 5. GLM Type Ptr: Convierte las matrices de C++ en punteros compatibles con la GPU.
#include <glm/gtc/type_ptr.hpp>

// 6. STB Image: Carga imágenes desde disco a memoria para usarlas como texturas en OpenGL.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Callback para ajustar la resolución de dibujado cuando la ventana cambia de tamaño

// Variables globales para el control de la cámara
glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 3.0f); // Posición inicial de la cámara
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Dirección hacia donde mira
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Vector "arriba"

// Gestión del tiempo para un movimiento fluido (Delta Time)
float deltaTime = 0.0f; // Tiempo entre el fotograma actual y el anterior
float lastFrame = 0.0f;

// Callback para ajustar la resolución de dibujado cuando la ventana cambia de tamaño
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Procesar entrada de teclado (Movimiento con Flechas)
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime; // Ajustar velocidad de la cámara

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Shaders
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Piramide 3D - Camara Interactiva (Flechas)", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glEnable(GL_DEPTH_TEST);

    // Compilación de Shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Geometría de la Pirámide
    float vertices[] = {
        // Posiciones            // Coordenadas UV
        -0.5f, -0.4f,  0.5f,     0.0f, 0.0f,
         0.5f, -0.4f,  0.5f,     1.0f, 0.0f,
         0.5f, -0.4f, -0.5f,     1.0f, 1.0f,
        -0.5f, -0.4f, -0.5f,     0.0f, 1.0f,
         0.0f,  0.6f,  0.0f,     0.5f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,   0, 2, 3, // Base
        0, 1, 4,            // Cara Frontal
        1, 2, 4,            // Cara Derecha
        2, 3, 4,            // Cara Trasera
        3, 0, 4             // Cara Izquierda
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Carga de Textura
    //----------------------------------------------

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/TexturasEmojis.jpg", &width, &height, &nrChannels, 0);


    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLenum format = GL_RGB;
        if (nrChannels == 1)      format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Textura cargada con exito.\n";
    }
    else
    {
        std::cout << "Error al cargar la textura.\n";
    }

    //------------------------------------------------

    stbi_image_free(data);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window))
    {
        // Calcular tiempo de fotograma actual
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Procesar teclas presionales
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // MATRICES MVP DINÁMICAS
        glm::mat4 model = glm::mat4(1.0f);

        // Matriz View creada con la posición interactiva de la cámara
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}