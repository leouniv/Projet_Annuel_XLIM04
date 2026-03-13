cmake_minimum_required(VERSION 3.10)

project(Rendu3D_Server)

set(CMAKE_CXX_STANDARD 17)

# Trouver les paquets
find_package(LibDataChannel CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)

# Créer l'exécutable avec main.cpp ET Renderer.cpp
add_executable(MonServeur main.cpp Renderer.cpp)

# Lier les librairies
target_link_libraries(MonServeur PRIVATE 
    LibDataChannel::LibDataChannel
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    OpenGL::GL
    JPEG::JPEG
)
