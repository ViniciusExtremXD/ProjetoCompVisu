#include "Histogram.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <SDL3_image/SDL_image.h>

namespace {
#if defined(SDL_IMAGE_VERSION_ATLEAST) && SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
bool SavePNG(SDL_Surface* surface, const char* path) {
    return IMG_SavePNG(surface, path);
}
#else
bool SavePNG(SDL_Surface* surface, const char* path) {
    return IMG_SavePNG(surface, path) == 0;
}
#endif

Uint32 ReadPixel(const Uint8* base, int index, Uint8 bytesPerPixel) {
    Uint32 value = 0;
    std::memcpy(&value, base + index * bytesPerPixel, bytesPerPixel);
    return value;
}

void ClearSurface(SDL_Surface* surface, SDL_Color color) {
    const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(surface->format);
    if (!fmt) return;
    Uint32 mapped = SDL_MapRGBA(fmt, nullptr, color.r, color.g, color.b, color.a);
    Uint8* base = static_cast<Uint8*>(surface->pixels);
    for (int y = 0; y < surface->h; ++y) {
        Uint8* row = base + y * surface->pitch;
        for (int x = 0; x < surface->w; ++x) {
            std::memcpy(row + x * fmt->bytes_per_pixel, &mapped, fmt->bytes_per_pixel);
        }
    }
}

// #########################################
//            FUNÇÃO: DESENHAR BARRAS DE HISTOGRAMA
// #########################################
void DrawBars(SDL_Renderer* renderer, int x, int y, int width, int height,
              const std::array<int, 256>& data, int maxVal, SDL_Color color) {
    if (maxVal <= 0) {
        return;
    }

    float barWidth = width / 256.0f;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < 256; ++i) {
        if (data[i] == 0) continue;
        float barHeight = (float)data[i] * height / maxVal;
        if (barHeight <= 0.0f) continue;
        SDL_FRect rect = {
            x + i * barWidth,
            y + (height - barHeight),
            barWidth,
            barHeight
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}
}

Histogram::Histogram() : mean(0), stdDev(0), maxValue(0), totalPixels(0) {
    data.fill(0);
}

void Histogram::calculate(SDL_Surface* image) {
    data.fill(0);
    mean = 0.0f;
    stdDev = 0.0f;
    maxValue = 0;
    totalPixels = 0;

    if (!image) {
        return;
    }

    if (!SDL_LockSurface(image)) {
        return;
    }

    const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(image->format);
    if (!fmt) {
        SDL_UnlockSurface(image);
        return;
    }

    int width = image->w;
    int height = image->h;
    totalPixels = width * height;

    const Uint8* base = static_cast<const Uint8*>(image->pixels);

    for (int y = 0; y < height; ++y) {
        const Uint8* row = base + y * image->pitch;
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = ReadPixel(row, x, fmt->bytes_per_pixel);
            Uint8 gray = 0, g = 0, b = 0, a = 0;
            SDL_GetRGBA(pixel, fmt, nullptr, &gray, &g, &b, &a);
            data[gray]++;
        }
    }

    SDL_UnlockSurface(image);

    if (totalPixels == 0) {
        return;
    }

    maxValue = *std::max_element(data.begin(), data.end());

    double sum = 0.0;
    for (int i = 0; i < 256; ++i) {
        sum += static_cast<double>(i) * static_cast<double>(data[i]);
    }
    mean = static_cast<float>(sum / static_cast<double>(totalPixels));

    double variance = 0.0;
    for (int i = 0; i < 256; ++i) {
        double diff = static_cast<double>(i) - static_cast<double>(mean);
        variance += diff * diff * static_cast<double>(data[i]);
    }
    variance /= static_cast<double>(totalPixels);
    stdDev = static_cast<float>(std::sqrt(variance));
}

// #########################################
//            FUNÇÃO: DESENHAR HISTOGRAMA BASE
// #########################################
void Histogram::draw(SDL_Renderer* renderer, int x, int y, int width, int height) {
    if (maxValue == 0) return;
    
    // Desenhar fundo
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_FRect backgroundRect = {(float)x, (float)y, (float)width, (float)height};
    SDL_RenderFillRect(renderer, &backgroundRect);
    
    DrawBars(renderer, x, y, width, height, data, maxValue, SDL_Color{100, 150, 200, 255});

    // Desenhar bordas
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_FRect borderRect = {(float)x, (float)y, (float)width, (float)height};
    SDL_RenderRect(renderer, &borderRect);
}

// #########################################
//            FUNÇÃO: DESENHAR HISTOGRAMA COMPARATIVO
// #########################################
void Histogram::drawWithOverlay(SDL_Renderer* renderer, int x, int y, int width, int height,
                                const Histogram& overlay, SDL_Color overlayColor) const {
    int combinedMax = std::max(maxValue, overlay.maxValue);
    if (combinedMax == 0) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_FRect backgroundRect = {(float)x, (float)y, (float)width, (float)height};
    SDL_RenderFillRect(renderer, &backgroundRect);

    DrawBars(renderer, x, y, width, height, overlay.data, combinedMax, overlayColor);
    DrawBars(renderer, x, y, width, height, data, combinedMax, SDL_Color{100, 150, 200, 255});

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_FRect borderRect = {(float)x, (float)y, (float)width, (float)height};
    SDL_RenderRect(renderer, &borderRect);
}

std::string Histogram::getIntensityClassification() const {
    if (mean < 85) return "escura";
    else if (mean < 170) return "media";
    else return "clara";
}

std::string Histogram::getContrastClassification() const {
    if (stdDev < 30) return "baixo";
    else if (stdDev < 60) return "medio";
    else return "alto";
}

bool Histogram::saveCSV(const std::string& path) const {
    if (totalPixels == 0) {
        return false;
    }

    std::ofstream out(path);
    if (!out.is_open()) {
        return false;
    }

    out << "intensidade,contagem\n";
    for (int i = 0; i < 256; ++i) {
        out << i << ',' << data[i] << '\n';
    }
    return true;
}

bool Histogram::saveSummary(const std::string& path) const {
    if (totalPixels == 0) {
        return false;
    }

    std::ofstream out(path);
    if (!out.is_open()) {
        return false;
    }

    out << "Total de pixels: " << totalPixels << '\n';
    out << std::fixed << std::setprecision(2);
    out << "Média de intensidade: " << mean << " (" << getIntensityClassification() << ")\n";
    out << "Desvio padrão: " << stdDev << " (" << getContrastClassification() << ")\n";
    return true;
}

// #########################################
//            FUNÇÃO: EXPORTAR HISTOGRAMA COMO PNG
// #########################################
bool Histogram::savePlotImage(const std::string& path, int width, int height) const {
    if (totalPixels == 0 || maxValue == 0 || width <= 0 || height <= 0) {
        return false;
    }

    SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) {
        return false;
    }

    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
        return false;
    }

    ClearSurface(surface, SDL_Color{20, 20, 20, 255});

    int paddingBottom = std::max(10, height / 12);
    int paddingTop = std::max(6, height / 18);
    int plotHeight = std::max(1, height - paddingBottom - paddingTop);
    int axisY = height - paddingBottom;

    const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(surface->format);
    if (!fmt) {
        SDL_UnlockSurface(surface);
        SDL_DestroySurface(surface);
        return false;
    }

    auto putPixel = [&](int px, int py, SDL_Color color) {
        if (px < 0 || px >= width || py < 0 || py >= height) return;
        Uint32 mapped = SDL_MapRGBA(fmt, nullptr, color.r, color.g, color.b, color.a);
        Uint8* row = static_cast<Uint8*>(surface->pixels) + py * surface->pitch;
        std::memcpy(row + px * fmt->bytes_per_pixel, &mapped, fmt->bytes_per_pixel);
    };

    for (int x = 0; x < width; ++x) {
        putPixel(x, axisY, SDL_Color{200, 200, 200, 255});
    }
    for (int y = paddingTop; y < height - paddingBottom; ++y) {
        putPixel(0, y, SDL_Color{200, 200, 200, 255});
    }

    double barWidth = static_cast<double>(width) / 256.0;
    double scale = static_cast<double>(plotHeight) / static_cast<double>(maxValue);

    for (int i = 0; i < 256; ++i) {
        int barHeight = static_cast<int>(std::round(static_cast<double>(data[i]) * scale));
        if (barHeight <= 0) continue;
        if (barHeight > plotHeight) barHeight = plotHeight;

        int startX = static_cast<int>(std::floor(i * barWidth));
        int endX = static_cast<int>(std::floor((i + 1) * barWidth));
        if (endX <= startX) endX = startX + 1;
        if (endX > width) endX = width;

        for (int x = startX; x < endX; ++x) {
            for (int y = axisY - 1; y >= axisY - barHeight && y >= 0; --y) {
                putPixel(x, y, SDL_Color{90, 170, 255, 255});
            }
        }
    }

    SDL_UnlockSurface(surface);

    bool saved = SavePNG(surface, path.c_str());
    SDL_DestroySurface(surface);
    return saved;
}
