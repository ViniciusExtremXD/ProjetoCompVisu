#include "ImageProcessor.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <ctime>

namespace {
inline bool SavePNG(SDL_Surface* surface, const char* path) {
#if defined(SDL_IMAGE_VERSION_ATLEAST) && SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
    return IMG_SavePNG(surface, path);
#else
    return IMG_SavePNG(surface, path) == 0;
#endif
}

inline bool SaveJPG(SDL_Surface* surface, const char* path, int quality) {
#if defined(SDL_IMAGE_VERSION_ATLEAST) && SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
    return IMG_SaveJPG(surface, path, quality);
#else
    return IMG_SaveJPG(surface, path, quality) == 0;
#endif
}

inline bool SaveBMP(SDL_Surface* surface, const char* path) {
#if SDL_VERSION_ATLEAST(3, 0, 0)
    return SDL_SaveBMP(surface, path);
#else
    return SDL_SaveBMP(surface, path) == 0;
#endif
}

inline std::string GetImageError() {
#if defined(SDL_IMAGE_VERSION_ATLEAST)
#if SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
    const char* err = SDL_GetError();
#else
    const char* err = IMG_GetError();
#endif
#elif defined(IMG_GetError)
    const char* err = IMG_GetError();
#else
    const char* err = SDL_GetError();
#endif
    return err ? err : "";
}

inline bool EnsureLock(SDL_Surface* surface) {
    if (!surface) {
        SDL_SetError("Superfície nula");
        return false;
    }
    if (!SDL_LockSurface(surface)) {
        return false;
    }
    return true;
}

inline void UnlockIfLocked(SDL_Surface* surface) {
    if (surface) {
        SDL_UnlockSurface(surface);
    }
}

SDL_Surface* CreateGrayscaleSurface(SDL_Surface* source) {
    if (!source) {
        return nullptr;
    }

    SDL_Surface* gray = SDL_CreateSurface(source->w, source->h, SDL_PIXELFORMAT_RGBA8888);
    if (!gray) {
        return nullptr;
    }

    if (!EnsureLock(source)) {
        SDL_DestroySurface(gray);
        return nullptr;
    }
    if (!EnsureLock(gray)) {
        UnlockIfLocked(source);
        SDL_DestroySurface(gray);
        return nullptr;
    }

    const SDL_PixelFormatDetails* fmtSrc = SDL_GetPixelFormatDetails(source->format);
    const SDL_PixelFormatDetails* fmtDst = SDL_GetPixelFormatDetails(gray->format);
    if (!fmtSrc || !fmtDst) {
        UnlockIfLocked(gray);
        UnlockIfLocked(source);
        SDL_DestroySurface(gray);
        SDL_SetError("Formato de pixel desconhecido para conversão");
        return nullptr;
    }

    const Uint8* srcBase = static_cast<const Uint8*>(source->pixels);
    Uint8* dstBase = static_cast<Uint8*>(gray->pixels);

    for (int y = 0; y < source->h; ++y) {
        const Uint8* srcRow = srcBase + y * source->pitch;
        Uint8* dstRow = dstBase + y * gray->pitch;
        for (int x = 0; x < source->w; ++x) {
            Uint32 pixel = 0;
            std::memcpy(&pixel, srcRow + x * fmtSrc->bytes_per_pixel, fmtSrc->bytes_per_pixel);

            Uint8 r = 0, g = 0, b = 0, a = 0;
            SDL_GetRGBA(pixel, fmtSrc, nullptr, &r, &g, &b, &a);
            Uint8 grayValue = static_cast<Uint8>(0.2125f * r + 0.7154f * g + 0.0721f * b);

            Uint32 mapped = SDL_MapRGBA(fmtDst, nullptr, grayValue, grayValue, grayValue, a);
            std::memcpy(dstRow + x * fmtDst->bytes_per_pixel, &mapped, fmtDst->bytes_per_pixel);
        }
    }

    UnlockIfLocked(gray);
    UnlockIfLocked(source);
    return gray;
}
} // namespace

ImageProcessor::ImageProcessor() 
    : originalImage(nullptr), grayscaleImage(nullptr), 
      currentImage(nullptr), isEqualized(false) {
}

ImageProcessor::~ImageProcessor() {
    clearSurfaces();
}

void ImageProcessor::clearSurfaces() {
    if (currentImage && currentImage != grayscaleImage && currentImage != originalImage) {
        SDL_DestroySurface(currentImage);
    }
    currentImage = nullptr;

    if (grayscaleImage) {
        SDL_DestroySurface(grayscaleImage);
        grayscaleImage = nullptr;
    }

    if (originalImage) {
        SDL_DestroySurface(originalImage);
        originalImage = nullptr;
    }
}

bool ImageProcessor::loadImage(const char* path) {
    SDL_Surface* loaded = IMG_Load(path);
    if (!loaded) {
        std::cerr << "Erro ao carregar imagem: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Surface* converted = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA8888);
    SDL_DestroySurface(loaded);

    if (!converted) {
        std::cerr << "Erro ao converter formato da imagem: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Surface* grayCandidate = nullptr;
    if (isGrayscale(converted)) {
        grayCandidate = SDL_DuplicateSurface(converted);
    } else {
        grayCandidate = CreateGrayscaleSurface(converted);
    }

    if (!grayCandidate) {
        std::cerr << "Erro ao gerar imagem em escala de cinza: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(converted);
        return false;
    }

    clearSurfaces();
    originalImage = converted;
    grayscaleImage = grayCandidate;
    currentImage = grayscaleImage;
    isEqualized = false;

    return true;
}

bool ImageProcessor::isGrayscale(SDL_Surface* surface) {
    if (!surface) return false;
    
    if (!SDL_LockSurface(surface)) {
        std::cerr << "Aviso: não foi possível travar superfície para verificação de escala de cinza: "
                  << SDL_GetError() << std::endl;
        return false;
    }

    const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(surface->format);
    if (!fmt) {
        SDL_UnlockSurface(surface);
        return false;
    }

    const Uint8* base = static_cast<const Uint8*>(surface->pixels);
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; ++y) {
        const Uint8* row = base + y * surface->pitch;
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = 0;
            std::memcpy(&pixel, row + x * fmt->bytes_per_pixel, fmt->bytes_per_pixel);
            Uint8 r = 0, g = 0, b = 0, a = 0;
            SDL_GetRGBA(pixel, fmt, nullptr, &r, &g, &b, &a);
            if (r != g || g != b) {
                SDL_UnlockSurface(surface);
                return false;
            }
        }
    }

    SDL_UnlockSurface(surface);
    return true;
}

void ImageProcessor::convertToGrayscale() {
    if (!originalImage) return;

    SDL_Surface* newGray = CreateGrayscaleSurface(originalImage);
    if (!newGray) {
        std::cerr << "Erro ao converter para escala de cinza: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Surface* previousGray = grayscaleImage;
    SDL_Surface* previousCurrent = currentImage;

    if (previousCurrent && previousCurrent != previousGray && previousCurrent != originalImage) {
        SDL_DestroySurface(previousCurrent);
    }

    if (previousGray) {
        SDL_DestroySurface(previousGray);
    }

    grayscaleImage = newGray;
    currentImage = grayscaleImage;
    isEqualized = false;
}

void ImageProcessor::equalizeHistogram() {
    if (!grayscaleImage) return;
    
    int width = grayscaleImage->w;
    int height = grayscaleImage->h;
    int totalPixels = width * height;
    
    // Calcular histograma
    int histogram[256] = {0};
    
    if (!SDL_LockSurface(grayscaleImage)) {
        std::cerr << "Erro ao bloquear superfície para equalização: " << SDL_GetError() << std::endl;
        return;
    }

    const SDL_PixelFormatDetails* fmtGray = SDL_GetPixelFormatDetails(grayscaleImage->format);
    if (!fmtGray) {
        SDL_UnlockSurface(grayscaleImage);
        std::cerr << "Formato de pixel desconhecido para equalização" << std::endl;
        return;
    }

    const Uint8* grayBase = static_cast<const Uint8*>(grayscaleImage->pixels);

    for (int y = 0; y < height; ++y) {
        const Uint8* row = grayBase + y * grayscaleImage->pitch;
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = 0;
            std::memcpy(&pixel, row + x * fmtGray->bytes_per_pixel, fmtGray->bytes_per_pixel);
            Uint8 gray = 0, dumpG = 0, dumpB = 0, dumpA = 0;
            SDL_GetRGBA(pixel, fmtGray, nullptr, &gray, &dumpG, &dumpB, &dumpA);
            histogram[gray]++;
        }
    }
    
    SDL_UnlockSurface(grayscaleImage);
    
    // Calcular CDF (Função de Distribuição Acumulada)
    int cdf[256] = {0};
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + histogram[i];
    }
    
    // Criar mapeamento de equalização
    int mapping[256];
    for (int i = 0; i < 256; i++) {
        mapping[i] = (int)((float)cdf[i] * 255.0f / totalPixels + 0.5f);
    }
    
    // Criar nova imagem equalizada
    if (currentImage && currentImage != grayscaleImage) {
        SDL_DestroySurface(currentImage);
    }
    
    currentImage = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
    if (!currentImage) return;
    
    if (!SDL_LockSurface(grayscaleImage)) {
        std::cerr << "Erro ao bloquear superfície para aplicar equalização: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(currentImage);
        currentImage = grayscaleImage;
        return;
    }

    if (!SDL_LockSurface(currentImage)) {
        SDL_UnlockSurface(grayscaleImage);
        std::cerr << "Erro ao bloquear superfície destino da equalização: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(currentImage);
        currentImage = grayscaleImage;
        return;
    }

    const SDL_PixelFormatDetails* fmtSrc = SDL_GetPixelFormatDetails(grayscaleImage->format);
    const SDL_PixelFormatDetails* fmtDst = SDL_GetPixelFormatDetails(currentImage->format);
    if (!fmtSrc || !fmtDst) {
        SDL_UnlockSurface(currentImage);
        SDL_UnlockSurface(grayscaleImage);
        std::cerr << "Formato de pixel desconhecido durante equalização" << std::endl;
        SDL_DestroySurface(currentImage);
        currentImage = grayscaleImage;
        return;
    }

    const Uint8* srcBase = static_cast<const Uint8*>(grayscaleImage->pixels);
    Uint8* dstBase = static_cast<Uint8*>(currentImage->pixels);

    for (int y = 0; y < height; ++y) {
        const Uint8* srcRow = srcBase + y * grayscaleImage->pitch;
        Uint8* dstRow = dstBase + y * currentImage->pitch;
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = 0;
            std::memcpy(&pixel, srcRow + x * fmtSrc->bytes_per_pixel, fmtSrc->bytes_per_pixel);
            Uint8 gray = 0, g = 0, b = 0, a = 0;
            SDL_GetRGBA(pixel, fmtSrc, nullptr, &gray, &g, &b, &a);

            Uint8 newGray = static_cast<Uint8>(mapping[gray]);
            Uint32 mapped = SDL_MapRGBA(fmtDst, nullptr, newGray, newGray, newGray, a);
            std::memcpy(dstRow + x * fmtDst->bytes_per_pixel, &mapped, fmtDst->bytes_per_pixel);
        }
    }
    
    SDL_UnlockSurface(currentImage);
    SDL_UnlockSurface(grayscaleImage);
    
    isEqualized = true;
}

void ImageProcessor::restoreOriginal() {
    if (currentImage && currentImage != grayscaleImage) {
        SDL_DestroySurface(currentImage);
    }
    currentImage = grayscaleImage;
    isEqualized = false;
}

bool ImageProcessor::saveImage(const char* path) {
    if (!currentImage) {
        std::cerr << "Erro: nenhuma imagem para salvar" << std::endl;
        return false;
    }
    // Tentar salvar como PNG via SDL_image primeiro (se disponível)
    bool pngSaved = SavePNG(currentImage, path);
    if (pngSaved) {
        std::cout << "Imagem salva com sucesso em: " << path << std::endl;
        return true;
    } else {
        std::string pngError = GetImageError();
        if (pngError.empty()) pngError = SDL_GetError();
        if (pngError.empty()) pngError = "Erro desconhecido ao salvar PNG";
        std::cerr << "Aviso: falha ao salvar PNG: " << pngError << std::endl;
    }

    // Tentar BMP com SDL core (deveria sempre estar disponível)
    auto replace_extension = [](const std::string& p, const std::string& newExt){
        std::string s = p;
        // find last path separator
        size_t sep = s.find_last_of("/\\");
        size_t dot = s.find_last_of('.');
        if (dot == std::string::npos || (sep != std::string::npos && dot < sep)) {
            return s + newExt;
        }
        return s.substr(0, dot) + newExt;
    };

    std::string bmpPath = replace_extension(path, ".bmp");
    // Diagnostics: print surface info before saving
    const char* fmtName = SDL_GetPixelFormatName(currentImage->format);
    const SDL_PixelFormatDetails* pfd = SDL_GetPixelFormatDetails(currentImage->format);
    std::cerr << "[DEBUG] currentImage: w=" << currentImage->w << " h=" << currentImage->h
              << " format=" << (fmtName ? fmtName : "unknown") << " depth=" << (pfd ? pfd->bits_per_pixel : 0)
              << " pitch=" << currentImage->pitch << " pixels=" << (void*)currentImage->pixels << std::endl;

    // Quick filesystem write test: try opening the target path for writing
    FILE* testf = fopen(bmpPath.c_str(), "wb");
    if (!testf) {
        std::cerr << "[DEBUG] Não foi possível abrir caminho de saída para escrita: " << bmpPath << " (" << strerror(errno) << ")" << std::endl;
    } else {
        unsigned char b = 0;
        if (fwrite(&b, 1, 1, testf) != 1) {
            std::cerr << "[DEBUG] Erro ao escrever teste no arquivo: " << bmpPath << " (" << strerror(errno) << ")" << std::endl;
        }
        fclose(testf);
        remove(bmpPath.c_str());
    }

    // Convert to save-friendly RGB24 surface (some save routines expect no alpha)
    SDL_Surface* saveSurface = SDL_ConvertSurface(currentImage, SDL_PIXELFORMAT_RGB24);
    if (!saveSurface) {
        std::cerr << "Aviso: falha ao converter superfície para formato de salvamento: " << SDL_GetError() << std::endl;
    } else {
        if (SaveBMP(saveSurface, bmpPath.c_str())) {
            std::cout << "Imagem salva com sucesso em BMP: " << bmpPath << std::endl;
            SDL_DestroySurface(saveSurface);
            return true;
        } else {
            std::string bmpErr = SDL_GetError();
            if (bmpErr.empty()) bmpErr = "Erro desconhecido ao salvar BMP";
            std::cerr << "Falha ao salvar BMP: " << bmpErr << std::endl;
        }

        // Try saving to /tmp to rule out path/permission issues
        std::string tmpPath = std::string("/tmp/processador_output_") + std::to_string((long)time(nullptr)) + ".bmp";
        if (SaveBMP(saveSurface, tmpPath.c_str())) {
            std::cout << "Imagem salva com sucesso em BMP (tmp): " << tmpPath << std::endl;
            SDL_DestroySurface(saveSurface);
            return true;
        } else {
            std::string tmpErr = SDL_GetError();
            if (tmpErr.empty()) tmpErr = "Erro desconhecido ao salvar BMP em /tmp";
            std::cerr << "Falha ao salvar BMP em /tmp: " << tmpErr << std::endl;
        }

        SDL_DestroySurface(saveSurface);
    }

    // Tentar JPG via SDL_image (se disponível)
    std::string jpgPath = replace_extension(path, ".jpg");
    if (SaveJPG(currentImage, jpgPath.c_str(), 95)) {
        std::cout << "Imagem salva com sucesso em JPG: " << jpgPath << std::endl;
        return true;
    } else {
        std::string jpgErr = GetImageError();
        if (jpgErr.empty()) jpgErr = SDL_GetError();
        if (jpgErr.empty()) jpgErr = "Erro desconhecido ao salvar JPG";
        std::cerr << "Falha ao salvar JPG: " << jpgErr << std::endl;
    }

    std::cerr << "Erro: todas as tentativas de salvamento falharam" << std::endl;
    return false;
}
