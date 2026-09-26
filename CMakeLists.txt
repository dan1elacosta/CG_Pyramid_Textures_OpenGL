# CMakeList.txt: proyecto de CMake para Pyramid_Textures_CG, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.10)

# Habilite Recarga activa para los compiladores de MSVC si se admiten.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Pyramid_Textures_CG")

# Agregue un origen al ejecutable de este proyecto.
add_executable (Pyramid_Textures_CG "Pyramid_Textures_CG.cpp" "Pyramid_Textures_CG.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Pyramid_Textures_CG PROPERTY CXX_STANDARD 20)
endif()

# TODO: Agregue pruebas y destinos de instalación si es necesario.
