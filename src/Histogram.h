#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <SDL3/SDL.h>
#include <array>
#include <string>

class Histogram {
private:
    std::array<int, 256> data;
    float mean;
   float stdDev;
    int maxValue;
    int totalPixels;
    
public:
    Histogram();
    
    void calculate(SDL_Surface* image);
    void draw(SDL_Renderer* renderer, int x, int y, int width, int height);
    void drawWithOverlay(SDL_Renderer* renderer, int x, int y, int width, int height,
                         const Histogram& overlay, SDL_Color overlayColor = {200, 200, 200, 160}) const;
    
    std::string getIntensityClassification() const;
    std::string getContrastClassification() const;
    
    float getMean() const { return mean; }
    float getStdDev() const { return stdDev; }
    const std::array<int, 256>& getData() const { return data; }
    int getTotalPixels() const { return totalPixels; }

    bool saveCSV(const std::string& path) const;
    bool saveSummary(const std::string& path) const;
    bool savePlotImage(const std::string& path, int width = 640, int height = 360) const;
};

#endif
