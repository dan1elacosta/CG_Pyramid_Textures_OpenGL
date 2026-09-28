cmake_minimum_required(VERSION 3.10)
project(Pyramid_Textures_CG C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar OpenGL en el sistema
find_package(OpenGL REQUIRED)

# Rutas de inclusión para las cabeceras (GLM, GLAD, GLFW)
include_directories(
    libraries/glfw-3.5.1/include
    libraries/glad/include
    libraries
)

# Incluir subdirectorio de GLFW
add_subdirectory(libraries/glfw-3.5.1)

# Código fuente de GLAD
set(GLAD_SOURCES libraries/glad/src/glad.c)

# Crear el ejecutable leyendo Pyramid_Textures_CG.cpp directamente en la raíz
add_executable(${PROJECT_NAME} Pyramid_Textures_CG.cpp ${GLAD_SOURCES})

# Enlazar librerías
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
)