#pragma once
#include <vector>
#include <GLFW/glfw3.h>

class Renderer {
public:
    void init(int width, int height);
    void render(float angleRotation);
    std::vector<unsigned char> getPixels();
    std::vector<unsigned char> getJpegData(int quality);
    void cleanup();
    GLFWwindow* getWindow() { return window; }

private:
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;
    
    // Double PBO pour lecture GPU asynchrone
    unsigned int pbo[2] = {0, 0};
    int pboIndex = 0;
    bool pboReady = false;
};