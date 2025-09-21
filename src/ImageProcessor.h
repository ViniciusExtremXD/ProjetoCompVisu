#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <SDL3/SDL.h>
#include <vector>
#include <memory>

class ImageProcessor {
private:
    SDL_Surface* originalImage;
    SDL_Surface* grayscaleImage;
    SDL_Surface* currentImage;
    bool isEqualized;

    void clearSurfaces();

public:
    ImageProcessor();
    ~ImageProcessor();
    
    // Carregamento e conversão
    bool loadImage(const char* path);
    bool isGrayscale(SDL_Surface* surface);
    void convertToGrayscale();
    
    // Equalização de histograma
    void equalizeHistogram();
    void restoreOriginal();
    bool getIsEqualized() const { return isEqualized; }
    
    // Salvar imagem
    bool saveImage(const char* path);
    
    // Getters
    SDL_Surface* getCurrentImage() const { return currentImage; }
    SDL_Surface* getOriginalImage() const { return originalImage; }
    int getWidth() const { return currentImage ? currentImage->w : 0; }
    int getHeight() const { return currentImage ? currentImage->h : 0; }
};

#endif
