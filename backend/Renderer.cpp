#include "Renderer.hpp"
#include <iostream>
#include <jpeglib.h>
#include <cstddef>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>

// Types et constantes PBO (OpenGL 2.1+)
typedef ptrdiff_t GLsizeiptr_t;
typedef void (APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei, GLuint*);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr_t, const void*, GLenum);
typedef void* (APIENTRY* PFNGLMAPBUFFERPROC)(GLenum, GLenum);
typedef GLboolean (APIENTRY* PFNGLUNMAPBUFFERPROC)(GLenum);
typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*);

static PFNGLGENBUFFERSPROC glGenBuffersPtr = nullptr;
static PFNGLBINDBUFFERPROC glBindBufferPtr = nullptr;
static PFNGLBUFFERDATAPROC glBufferDataPtr = nullptr;
static PFNGLMAPBUFFERPROC glMapBufferPtr = nullptr;
static PFNGLUNMAPBUFFERPROC glUnmapBufferPtr = nullptr;
static PFNGLDELETEBUFFERSPROC glDeleteBuffersPtr = nullptr;

#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_STREAM_READ 0x88E1
#define GL_READ_ONLY 0x88B8

static bool loadPboExtensions() {
    glGenBuffersPtr = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBufferPtr = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferDataPtr = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glMapBufferPtr = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
    glUnmapBufferPtr = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
    glDeleteBuffersPtr = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
    return glGenBuffersPtr && glBindBufferPtr && glBufferDataPtr && 
           glMapBufferPtr && glUnmapBufferPtr && glDeleteBuffersPtr;
}

void Renderer::init(int w, int h) {
    this->width = w;
    this->height = h;

    if (!glfwInit()) return;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 
    
    window = glfwCreateWindow(width, height, "Serveur 3D Hidden", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);

    // Initialisation des PBOs
    if (loadPboExtensions()) {
        GLsizeiptr_t bufSize = width * height * 3;
        glGenBuffersPtr(2, pbo);
        for (int i = 0; i < 2; i++) {
            glBindBufferPtr(GL_PIXEL_PACK_BUFFER, pbo[i]);
            glBufferDataPtr(GL_PIXEL_PACK_BUFFER, bufSize, nullptr, GL_STREAM_READ);
        }
        glBindBufferPtr(GL_PIXEL_PACK_BUFFER, 0);
        std::cout << "[Systeme] PBO initialises (lecture GPU asynchrone)" << std::endl;
    } else {
        std::cout << "[Systeme] PBO non disponibles, fallback synchrone" << std::endl;
    }
}

void Renderer::render(float angleRotation) {
    if (!window) return;

    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float ratio = (float)width / (float)height;
    glFrustum(-ratio, ratio, -1.0, 1.0, 1.5, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(angleRotation * 57.29f, 1.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
        glColor3f(0.9f, 0.1f, 0.1f);
        glVertex3f(-1.0f, -1.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glColor3f(0.9f, 0.9f, 0.1f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glColor3f(0.1f, 0.9f, 0.1f);
        glVertex3f(-1.0f,  1.0f, -1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glColor3f(0.1f, 0.1f, 0.9f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glColor3f(0.9f, 0.1f, 0.9f);
        glVertex3f( 1.0f, -1.0f, -1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
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

// Encodage JPEG via libjpeg-turbo (avec SIMD)
static std::vector<unsigned char> encodeJpeg(const unsigned char* pixels, int w, int h, int quality) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    unsigned char* outBuffer = nullptr;
    unsigned long outSize = 0;
    jpeg_mem_dest(&cinfo, &outBuffer, &outSize);

    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = w * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
        const unsigned char* row = pixels + cinfo.next_scanline * row_stride;
        JSAMPROW rowPtr = const_cast<JSAMPROW>(row);
        jpeg_write_scanlines(&cinfo, &rowPtr, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    std::vector<unsigned char> result(outBuffer, outBuffer + outSize);
    free(outBuffer);
    return result;
}

std::vector<unsigned char> Renderer::getJpegData(int quality) {
    if (!window) return {};

    // PBO double-buffer : lancer la lecture async de la frame courante,
    // encoder la frame précédente (déjà prête dans l'autre PBO)
    if (pbo[0] != 0) {
        int current = pboIndex;
        int previous = (pboIndex + 1) % 2;

        // Lancer la lecture async de la frame courante dans pbo[current]
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glBindBufferPtr(GL_PIXEL_PACK_BUFFER, pbo[current]);
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // Lire les pixels de la frame précédente depuis pbo[previous]
        if (pboReady) {
            glBindBufferPtr(GL_PIXEL_PACK_BUFFER, pbo[previous]);
            auto* ptr = (unsigned char*)glMapBufferPtr(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
            std::vector<unsigned char> result;
            if (ptr) {
                result = encodeJpeg(ptr, width, height, quality);
                glUnmapBufferPtr(GL_PIXEL_PACK_BUFFER);
            }
            glBindBufferPtr(GL_PIXEL_PACK_BUFFER, 0);
            pboIndex = (pboIndex + 1) % 2;
            return result;
        }

        // Première frame : pas encore de données prêtes
        glBindBufferPtr(GL_PIXEL_PACK_BUFFER, 0);
        pboIndex = (pboIndex + 1) % 2;
        pboReady = true;
        return {};
    }

    // Fallback sans PBO
    auto pixels = getPixels();
    return encodeJpeg(pixels.data(), width, height, quality);
}

void Renderer::cleanup() {
    if (pbo[0] != 0 && glDeleteBuffersPtr) {
        glDeleteBuffersPtr(2, pbo);
    }
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}