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
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Vertex Shader: Aplica la cadena de matrices MVP (Projection * View * Model)
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
    TexCoord = aTexCoord; // Pasamos las coordenadas UV al Fragment Shader
}
)";

// Fragment Shader: Aplica la textura usando las coordenadas UV
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// Sampler2D es el uniform que representa el canal de la textura en la GPU
uniform sampler2D ourTexture;

void main()
{
    // Muestra (samplea) los colores de la textura en las coordenadas UV
    FragColor = texture(ourTexture, TexCoord);
}
)";

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Piramide 3D con EBO y Textura - OpenGL", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // Activar buffer de profundidad (Z-Buffer) para ocultar superficies traseras
    glEnable(GL_DEPTH_TEST);

    // Compilar Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compilar Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Enlazar Programa de Shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 1. DEFINICIÓN DE VÉRTICES (5 Vértices únicos para economizar memoria)
    // Estructura de cada fila: [X, Y, Z (Posicion),   U, V (Coordenadas de Textura)]
    float vertices[] = {
        // Posiciones            // Coordenadas UV
        -0.5f, -0.4f,  0.5f,     0.0f, 0.0f, // Vértice 0: Base Frontal-Izquierda
         0.5f, -0.4f,  0.5f,     1.0f, 0.0f, // Vértice 1: Base Frontal-Derecha
         0.5f, -0.4f, -0.5f,     1.0f, 1.0f, // Vértice 2: Base Trasera-Derecha
        -0.5f, -0.4f, -0.5f,     0.0f, 1.0f, // Vértice 3: Base Trasera-Izquierda
         0.0f,  0.6f,  0.0f,     0.5f, 1.0f  // Vértice 4: Cúspide de la Pirámide
    };

    // 2. DEFINICIÓN DE ÍNDICES (EBO: Conecta los 5 vértices para formar 6 triángulos)
    unsigned int indices[] = {
        0, 1, 2,   0, 2, 3, // Base cuadrada (2 triángulos)
        0, 1, 4,            // Cara Frontal
        1, 2, 4,            // Cara Derecha
        2, 3, 4,            // Cara Trasera
        3, 0, 4             // Cara Izquierda
    };

    // 3. CREACIÓN Y CONFIGURACIÓN DE BUFFERS (VAO, VBO, EBO)
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Seleccionamos el VAO para grabar las configuraciones de memoria
    glBindVertexArray(VAO);

    // Cargar datos al VBO (Vértices)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Cargar datos al EBO (Índices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Especificar Atributo 0: Posición (X, Y, Z) -> Salto (stride) de 5 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Especificar Atributo 1: Textura (U, V) -> Desfase (offset) de 3 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Desvincular VAO
    glBindVertexArray(0);

    // ---------------------------------------------------------------------------------
    // 4. CARGA Y CONFIGURACIÓN DE LA TEXTURA CON STB_IMAGE
    // ---------------------------------------------------------------------------------
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Configuración de envoltura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Configuración de filtrado
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Voltear verticalmente la imagen
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/TexturasEmojis.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        // Fuerza a OpenGL a leer la memoria byte a byte (evita crash en GPUs AMD)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Definir el formato de manera segura
        GLenum format = GL_RGB;
        if (nrChannels == 1)      format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        // Enviar la textura a la VRAM de la GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Textura cargada con exito: " << width << "x" << height << " (Canales: " << nrChannels << ")\n";
    }
    else
    {
        std::cout << "Error al cargar la textura. Verifica la ruta 'textures/TexturasEmojis.jpg'\n";
    }

    // Liberar la memoria RAM ocupada por la imagen procesada
    stbi_image_free(data);

    // Obtener ubicaciones uniformes en la GPU
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Activar y vincular la textura antes de dibujar
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Cámara temporal que gira alrededor para inspeccionar la pirámide
        float timeValue = (float)glfwGetTime();
        float radius = 3.0f;
        float camX = sin(timeValue) * radius;
        float camZ = cos(timeValue) * radius;

        // MATRICES MVP
        glm::mat4 model = glm::mat4(1.0f); // Pirámide fija en el centro del mundo
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 1.2f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Enviar matrices a la GPU
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Dibujar pirámide usando los 18 índices del EBO
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