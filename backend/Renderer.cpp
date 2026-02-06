#include "Renderer.hpp"
#include <iostream>

void Renderer::init(int w, int h) {
    this->width = w;
    this->height = h;

    if (!glfwInit()) return;

    // --- MODIFICATION CLE : Fenêtre Invisible ---
    // Le serveur calcule en arrière-plan, pas besoin d'afficher sur l'écran serveur
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 
    
    window = glfwCreateWindow(width, height, "Serveur 3D Hidden", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    // Activation de la profondeur (Z-Buffer) pour la 3D
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(float angleRotation) {
    if (!window) return;

    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    // Fond gris foncé professionnel
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection Perspective (Réalisme 3D)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio = (float)width / (float)height;
    glFrustum(-ratio, ratio, -1.0, 1.0, 1.5, 20.0);

    // Caméra / Modèle
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f); // Recul caméra
    glRotatef(angleRotation * 57.29f, 1.0f, 1.0f, 0.0f); // Rotation X+Y

    // Dessin du Cube
    glBegin(GL_QUADS);
        // Face Avant (Rouge)
        glColor3f(0.9f, 0.1f, 0.1f);
        glVertex3f(-1.0f, -1.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        // Face Arrière (Jaune)
        glColor3f(0.9f, 0.9f, 0.1f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Haut (Vert)
        glColor3f(0.1f, 0.9f, 0.1f);
        glVertex3f(-1.0f,  1.0f, -1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        // Bas (Bleu)
        glColor3f(0.1f, 0.1f, 0.9f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        // Droite (Magenta)
        glColor3f(0.9f, 0.1f, 0.9f);
        glVertex3f( 1.0f, -1.0f, -1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Gauche (Cyan)
        glColor3f(0.1f, 0.9f, 0.9f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

std::vector<unsigned char> Renderer::getPixels() {
    if (!window) return {};
    std::vector<unsigned char> pixels(width * height * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    return pixels;
}

void Renderer::cleanup() {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}