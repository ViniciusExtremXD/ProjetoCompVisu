#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_dialog.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <string>
#include "ImageProcessor.h"
#include "Histogram.h"

class Button {
private:
    SDL_FRect rect;
    std::string text;
    SDL_Color normalColor;
    SDL_Color hoverColor;
    SDL_Color pressedColor;
    bool isHovered;
    bool isPressed;
    bool wasPressedLastFrame;

public:
    Button(float x = 0, float y = 0, float w = 0, float h = 0, const std::string& text = "");
    void handleEvent(const SDL_Event& e);
    bool isMouseInside(float mouseX, float mouseY);
    bool wasClicked();
    void draw(SDL_Renderer* renderer, TTF_Font* font);
    void setText(const std::string& newText) { text = newText; }
};

// Constantes para as dimensões das janelas
const int MAIN_WIDTH = 800;
const int MAIN_HEIGHT = 600;
const int SECONDARY_WIDTH = 400;
const int SECONDARY_HEIGHT = 520;

class GUI {
private:
    SDL_Window* mainWindow;
    SDL_Window* secondaryWindow;
    SDL_Renderer* mainRenderer;
    SDL_Renderer* secondaryRenderer;
    SDL_Texture* imageTexture;
    TTF_Font* font;
    int mainWindowWidth;
    int mainWindowHeight;
    bool running;
    std::string currentImagePath;
    std::string selectedFile;  // For file dialog callbacks
    Button openButton, saveButton, equalizeButton;
    Histogram histogram;
    Histogram originalHistogram;
    ImageProcessor imageProcessor;
    
    void updateImageTexture();
    void render();
    void drawText(SDL_Renderer* renderer, const std::string& text, 
                 int x, int y, SDL_Color color);
    
    // Callback handlers for file dialogs (SDL uses SDL_DialogFileCallback)
    static void fileOpenCallback(void* userdata, const char* const* files, int filter);
    static void fileSaveCallback(void* userdata, const char* const* files, int filter);

public:
    GUI(const std::string& imagePath);
    ~GUI();
    void run();
    bool init();
    void cleanup();
    void handleEvents();
};

#endif // GUI_H
