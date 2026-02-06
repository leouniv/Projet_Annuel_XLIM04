#pragma once
#include <vector>
#include <GLFW/glfw3.h>

class Renderer {
public:
    // Initialise le contexte OpenGL (Invisible)
    void init(int width, int height);
    
    // Dessine le Cube 3D
    void render(float angleRotation);
    
    // Capture les pixels pour le réseau
    std::vector<unsigned char> getPixels();
    
    // Fermeture propre
    void cleanup();
    
    // Accès à la fenêtre pour la boucle principale
    GLFWwindow* getWindow() { return window; }

private:
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;
};