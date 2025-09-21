#include "GUI.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>

// Implementação da classe Button
Button::Button(float x, float y, float w, float h, const std::string& text)
    : rect{x, y, w, h}, text(text), isHovered(false), isPressed(false), wasPressedLastFrame(false) {
    normalColor = {50, 100, 200, 255};     // Azul
    hoverColor = {100, 150, 255, 255};     // Azul claro
    pressedColor = {30, 60, 150, 255};     // Azul escuro
}

void Button::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_MOUSE_MOTION) {
        float mouseX = e.motion.x;
        float mouseY = e.motion.y;
        isHovered = isMouseInside(mouseX, mouseY);
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            float mouseX = e.button.x;
            float mouseY = e.button.y;
            if (isMouseInside(mouseX, mouseY)) {
                isPressed = true;
            }
        }
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            isPressed = false;
        }
    }
}

bool Button::isMouseInside(float mouseX, float mouseY) {
    return mouseX >= rect.x && mouseX <= rect.x + rect.w &&
           mouseY >= rect.y && mouseY <= rect.y + rect.h;
}

bool Button::wasClicked() {
    bool clicked = wasPressedLastFrame && !isPressed && isHovered;
    wasPressedLastFrame = isPressed;
    return clicked;
}

void Button::draw(SDL_Renderer* renderer, TTF_Font* font) {
    // Selecionar cor baseada no estado
    SDL_Color currentColor;
    if (isPressed) {
        currentColor = pressedColor;
    } else if (isHovered) {
        currentColor = hoverColor;
    } else {
        currentColor = normalColor;
    }
    
    // Desenhar retângulo do botão
    SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, 
                          currentColor.b, currentColor.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // Desenhar borda
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &rect);
    
    // Desenhar texto
    if (font) {
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), strlen(text.c_str()), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                int textW = textSurface->w;
                int textH = textSurface->h;
                SDL_FRect textRect = {
                    rect.x + (rect.w - textW) / 2,
                    rect.y + (rect.h - textH) / 2,
                    (float)textW,
                    (float)textH
                };
                SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_DestroySurface(textSurface);
        }
    }
}

// Implementação da classe GUI
GUI::GUI(const std::string& imagePath) 
    : mainWindow(nullptr), secondaryWindow(nullptr),
            mainRenderer(nullptr), secondaryRenderer(nullptr),
            imageTexture(nullptr), font(nullptr),
            mainWindowWidth(MAIN_WIDTH), mainWindowHeight(MAIN_HEIGHT),
            running(true), currentImagePath("") ,
            openButton(20, 430, 100, 40, "Abrir"),
            saveButton(280, 430, 100, 40, "Salvar"),
            equalizeButton(150, 430, 100, 40, "Equalizar") {
    
    // Carregar imagem
    if (!imageProcessor.loadImage(imagePath.c_str())) {
        throw std::runtime_error("Não foi possível carregar a imagem");
    }
    
    // Calcular histogramas iniciais (original e atual)
    originalHistogram.calculate(imageProcessor.getOriginalImage());
    histogram.calculate(imageProcessor.getCurrentImage());
    
    // Definir dimensões da janela principal baseadas na imagem
    mainWindowWidth = imageProcessor.getWidth();
    mainWindowHeight = imageProcessor.getHeight();
    
    // Obter informações do display
    const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());

    // Se não houver displayMode, usar fallback 800x600
    int displayW = 800, displayH = 600;
    if (displayMode) {
        displayW = displayMode->w;
        displayH = displayMode->h;
    }

    // Calcular posição centralizada para janela principal
    int mainX = (displayW - mainWindowWidth) / 2;
    int mainY = (displayH - mainWindowHeight) / 2;
    
    // Criar janela principal
    mainWindow = SDL_CreateWindow("Processamento de Imagens - Principal",
                                 mainWindowWidth, mainWindowHeight, 0);
    if (!mainWindow) {
        throw std::runtime_error("Erro ao criar janela principal");
    }
    
    SDL_SetWindowPosition(mainWindow, mainX, mainY);
    
    // Criar janela secundária (posicionada ao lado da principal) - criação direta
    int secondaryX = mainX + mainWindowWidth + 20;
    int secondaryY = mainY;

    secondaryWindow = SDL_CreateWindow("Processamento de Imagens - Histograma",
                                      SECONDARY_WIDTH, SECONDARY_HEIGHT, 0);
    if (!secondaryWindow) {
        SDL_DestroyWindow(mainWindow);
        throw std::runtime_error("Erro ao criar janela secundária");
    }
    // Tentar definir a janela secundária como filha da principal (pode falhar em alguns WMs)
    if (!SDL_SetWindowParent(secondaryWindow, mainWindow)) {
        std::cerr << "Aviso: não foi possível vincular a janela secundária: " << SDL_GetError() << std::endl;
    }
    SDL_SetWindowPosition(secondaryWindow, secondaryX, secondaryY);
    
    // Criar renderers
    mainRenderer = SDL_CreateRenderer(mainWindow, nullptr);
    secondaryRenderer = SDL_CreateRenderer(secondaryWindow, nullptr);
    
    if (!mainRenderer || !secondaryRenderer) {
        throw std::runtime_error("Erro ao criar renderers");
    }
    
    // Inicializar TTF
    if (TTF_Init() != 0) {
        std::cerr << "Aviso: TTF não pôde ser inicializado. Texto não será exibido." << std::endl;
    } else {
        font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 14);
        if (!font) {
            font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 14);
        }
    }
    
    // Criar textura inicial da imagem
    updateImageTexture();
}

GUI::~GUI() {
    if (imageTexture) SDL_DestroyTexture(imageTexture);
    if (font) TTF_CloseFont(font);
    if (secondaryRenderer) SDL_DestroyRenderer(secondaryRenderer);
    if (mainRenderer) SDL_DestroyRenderer(mainRenderer);
    if (secondaryWindow) SDL_DestroyWindow(secondaryWindow);
    if (mainWindow) SDL_DestroyWindow(mainWindow);
    TTF_Quit();
}

void GUI::run() {
    while (running) {
        handleEvents();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void GUI::fileOpenCallback(void* userdata, const char* const* files, int filter) {
    GUI* gui = static_cast<GUI*>(userdata);
    (void)filter;
    if (files && files[0]) {
        gui->selectedFile = files[0];
    }
}

void GUI::fileSaveCallback(void* userdata, const char* const* files, int filter) {
    GUI* gui = static_cast<GUI*>(userdata);
    (void)filter;
    if (files && files[0]) {
        gui->selectedFile = files[0];
    }
}

void GUI::handleEvents() {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        // Processar eventos para os botões (apenas eventos da janela secundária)
        if (SDL_GetWindowFromID(event.window.windowID) == secondaryWindow) {
            equalizeButton.handleEvent(event);
            openButton.handleEvent(event);
            saveButton.handleEvent(event);
        }
        
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
                
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(mainWindow)) {
                    running = false;
                }
                break;
                
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_S) {
                    imageProcessor.saveImage("output_image.png");
                }
                break;
        }
        
        // Verificar cliques nos botões
        if (openButton.wasClicked()) {
            selectedFile = "";
            
            SDL_DialogFileFilter filters[] = {
                {"Arquivos de imagem", "*.png;*.jpg"},
                {NULL, NULL}
            };

            SDL_ShowOpenFileDialog(GUI::fileOpenCallback, this, mainWindow, filters, 1, currentImagePath.empty() ? NULL : currentImagePath.c_str(), false);

            if (!selectedFile.empty()) {
                if (imageProcessor.loadImage(selectedFile.c_str())) {
                    currentImagePath = selectedFile;
                    originalHistogram.calculate(imageProcessor.getOriginalImage());
                    histogram.calculate(imageProcessor.getCurrentImage());
                    updateImageTexture();
                    equalizeButton.setText("Equalizar");  // Reset estado
                } else {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                        "Erro",
                        "Não foi possível carregar a imagem selecionada.",
                        mainWindow);
                }
            }
        }

        if (equalizeButton.wasClicked()) {
            if (imageProcessor.getIsEqualized()) {
                imageProcessor.restoreOriginal();
                equalizeButton.setText("Equalizar");
            } else {
                imageProcessor.equalizeHistogram();
                equalizeButton.setText("Original");
            }
            histogram.calculate(imageProcessor.getCurrentImage());
            updateImageTexture();
        }
        
        if (saveButton.wasClicked()) {
            selectedFile = "";
            
            SDL_DialogFileFilter filters[] = {
                {"Arquivos PNG", "*.png"},
                {"Arquivos JPEG", "*.jpg;*.jpeg"},
                {"Arquivos BMP", "*.bmp"},
                {NULL, NULL}
            };

            SDL_ShowSaveFileDialog(GUI::fileSaveCallback, this, mainWindow, filters, 3, currentImagePath.empty() ? NULL : currentImagePath.c_str());
                
            if (!selectedFile.empty()) {
                std::string path = selectedFile;
                // Garantir extensão
                if (!path.ends_with(".png") && !path.ends_with(".jpg") && 
                    !path.ends_with(".jpeg") && !path.ends_with(".bmp")) {
                    path += ".png";  // Extensão padrão
                }
                
                if (!imageProcessor.saveImage(path.c_str())) {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                        "Erro",
                        "Não foi possível salvar a imagem. Verifique as permissões da pasta.",
                        mainWindow);
                }
            }
            }
        }
    }

// #########################################
//            FUNÇÃO: RENDERIZAR TELAS DA GUI
// #########################################
void GUI::render() {
    // Renderizar janela principal
    SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mainRenderer);
    
    if (imageTexture) {
        SDL_RenderTexture(mainRenderer, imageTexture, nullptr, nullptr);
    }
    
    SDL_RenderPresent(mainRenderer);
    
    // Renderizar janela secundária
    SDL_SetRenderDrawColor(secondaryRenderer, 30, 30, 30, 255);
    SDL_RenderClear(secondaryRenderer);
    
    // Desenhar histograma
    histogram.drawWithOverlay(secondaryRenderer, 50, 50, 300, 200, originalHistogram,
                              SDL_Color{180, 180, 180, 180});
    
    // Desenhar informações do histograma
    if (font) {
        std::stringstream ss;
        ss << "Atual média: " << (int)histogram.getMean() 
           << " (" << histogram.getIntensityClassification() << ")";
        drawText(secondaryRenderer, ss.str(), 50, 270, {255, 255, 255, 255});
        
        ss.str("");
        ss << "Atual desvio: " << (int)histogram.getStdDev() 
           << " (" << histogram.getContrastClassification() << ")";
        drawText(secondaryRenderer, ss.str(), 50, 300, {255, 255, 255, 255});

        ss.str("");
        ss << "Original média: " << (int)originalHistogram.getMean()
           << " (" << originalHistogram.getIntensityClassification() << ")";
        drawText(secondaryRenderer, ss.str(), 50, 330, {200, 200, 200, 255});

        ss.str("");
        ss << "Original desvio: " << (int)originalHistogram.getStdDev()
           << " (" << originalHistogram.getContrastClassification() << ")";
        drawText(secondaryRenderer, ss.str(), 50, 360, {200, 200, 200, 255});
        
        drawText(secondaryRenderer, "Pressione 'S' para salvar", 50, 390, {200, 200, 200, 255});
    }
    
        // Desenhar botões
    openButton.draw(secondaryRenderer, font);
    equalizeButton.draw(secondaryRenderer, font);
    saveButton.draw(secondaryRenderer, font);    SDL_RenderPresent(secondaryRenderer);
}

void GUI::updateImageTexture() {
    if (imageTexture) {
        SDL_DestroyTexture(imageTexture);
    }
    
    SDL_Surface* currentImage = imageProcessor.getCurrentImage();
    if (currentImage) {
        imageTexture = SDL_CreateTextureFromSurface(mainRenderer, currentImage);
    }
}

void GUI::drawText(SDL_Renderer* renderer, const std::string& text, 
                   int x, int y, SDL_Color color) {
    if (!font) return;
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), strlen(text.c_str()), color);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_FRect destRect = {(float)x, (float)y, 
                                 (float)textSurface->w, (float)textSurface->h};
            SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }
}
