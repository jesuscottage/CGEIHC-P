cmake_minimum_required(VERSION 3.20)
project(CGEIHC VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

# Assimp (solo GLTF y OBJ para minimizar tiempo de compilacion)
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.4.3
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

# Dear ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
)

FetchContent_MakeAvailable(glfw glm assimp imgui)

# ACTIVE_DIR — ruta absoluta a active/ en la raiz del repositorio
get_filename_component(REPO_ROOT "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)

# Fuentes del proyecto (GLOB recoge todos los .cpp en src/)
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Fuentes de GLAD (no entra en el GLOB porque esta en vendor/src/)
list(APPEND SOURCES src/vendor/src/glad.c)

# Fuentes de ImGui backend
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(CGEIHC ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(CGEIHC PRIVATE
    src/
    src/vendor/include
    src/vendor
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

option(PORTABLE "Build portable exe with relative paths" OFF)
if(NOT PORTABLE)
    target_compile_definitions(CGEIHC PRIVATE
        ACTIVE_DIR="${REPO_ROOT}/active/"
        SHADERS_DIR="${CMAKE_SOURCE_DIR}/shaders/"
        ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/"
    )
endif()

target_link_libraries(CGEIHC PRIVATE glfw glm::glm assimp)

# Crear directorio active/ si no existe
add_custom_command(TARGET CGEIHC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${REPO_ROOT}/active"
)

# Copiar DLL de Assimp junto al exe (Release genera DLL; Debug es estatico)
add_custom_command(TARGET CGEIHC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp>
        $<TARGET_FILE_DIR:CGEIHC>
)

# Copiar assets/ y shaders/ junto al exe
add_custom_command(TARGET CGEIHC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:CGEIHC>/assets
)
add_custom_command(TARGET CGEIHC POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:CGEIHC>/shaders
)
