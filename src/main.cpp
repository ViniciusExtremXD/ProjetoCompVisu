#include <iostream>
#include <string>
#include <filesystem>
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include "GUI.h"
#include "Histogram.h"

namespace {
struct SDLInitResult {
    bool initialized{false};
    bool usedFallback{false};
    std::string driverName;
};

// #########################################
//            FUNÇÃO: LOGAR DRIVERS DE VÍDEO DISPONÍVEIS
// #########################################
void logVideoDrivers() {
    int count = SDL_GetNumVideoDrivers();
    if (count < 0) {
        std::cerr << "[SDL] Não foi possível listar video drivers: " << SDL_GetError() << std::endl;
        return;
    }
    std::cout << "[SDL] Drivers de vídeo disponíveis (" << count << "):";
    if (count == 0) {
        std::cout << " nenhum" << std::endl;
        return;
    }
    std::cout << std::endl;
    for (int i = 0; i < count; ++i) {
        const char* name = SDL_GetVideoDriver(i);
        std::cout << "  - " << (name ? name : "(desconhecido)") << std::endl;
    }
}

// #########################################
//            FUNÇÃO: INICIALIZAR SDL COM FALLBACK
// #########################################
SDLInitResult initializeSDLVideo(Uint32 flags) {
    SDLInitResult result;
    logVideoDrivers();

#if SDL_VERSION_ATLEAST(3, 0, 0)
    bool initOk = SDL_Init(flags);
#else
    bool initOk = SDL_Init(flags) == 0;
#endif
    if (initOk) {
        result.initialized = true;
        const char* driver = SDL_GetCurrentVideoDriver();
        result.driverName = driver ? driver : "(desconhecido)";
        std::cout << "[SDL] Inicialização bem-sucedida com driver '" << result.driverName << "'" << std::endl;
        return result;
    }

    std::string firstError = SDL_GetError();
    std::cerr << "[SDL] Falha ao inicializar subsistema de vídeo: "
              << (firstError.empty() ? "(erro não informado)" : firstError) << std::endl;

    SDL_Quit();

    // Fallback: tentar driver dummy para permitir modo --nogui
    std::cout << "[SDL] Tentando fallback com driver 'dummy'..." << std::endl;
#if defined(SDL_HINT_VIDEO_DRIVER)
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
#elif defined(SDL_HINT_VIDEODRIVER)
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "dummy");
#endif

#if SDL_VERSION_ATLEAST(3, 0, 0)
    bool fallbackOk = SDL_Init(flags);
#else
    bool fallbackOk = SDL_Init(flags) == 0;
#endif
    if (fallbackOk) {
        result.initialized = true;
        result.usedFallback = true;
        const char* driver = SDL_GetCurrentVideoDriver();
        result.driverName = driver ? driver : "dummy";
        std::cout << "[SDL] Inicialização usando driver de fallback '" << result.driverName << "'." << std::endl;
    } else {
        std::string fallbackError = SDL_GetError();
        std::cerr << "[SDL] Falha também no fallback: "
                  << (fallbackError.empty() ? "(erro não informado)" : fallbackError) << std::endl;
    }

    return result;
}
} // namespace

int main(int argc, char* argv[]) {
    // Suporta modo sem GUI: ./processador_imagens --nogui caminho
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " caminho_da_imagem.ext\n";
        std::cerr << "  ou: " << argv[0] << " --nogui caminho_da_imagem.ext" << std::endl;
        return 1;
    }

    bool noGui = false;
    std::string imagePath;
    if (std::string(argv[1]) == "--nogui") {
        if (argc != 3) {
            std::cerr << "Uso: " << argv[0] << " --nogui caminho_da_imagem.ext" << std::endl;
            return 1;
        }
        noGui = true;
        imagePath = argv[2];
    } else {
        imagePath = argv[1];
    }

    // Inicializar SDL (necessário para GUI e parte do pipeline)
    Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    SDLInitResult sdlInit = initializeSDLVideo(sdlFlags);
    if (!sdlInit.initialized) {
        std::cerr << "Erro crítico: SDL não pôde ser inicializado. Verifique drivers de vídeo ou execute com --nogui." << std::endl;
        return 1;
    }
    if (sdlInit.usedFallback) {
        std::cerr << "Aviso: driver '" << sdlInit.driverName
                  << "' não suporta janelas. Modo GUI indisponível nesta execução." << std::endl;
    }

    // SDL_image 3 não requer inicialização explícita; versões antigas ainda precisam
#if defined(SDL_IMAGE_VERSION_ATLEAST)
#if !SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::cerr << "Aviso: falha ao inicializar SDL_image: " << IMG_GetError() << std::endl;
        // Não retornamos; tentaremos prosseguir. IMG_Load/IMG_Save* podem falhar se suporte não estiver disponível.
    }
#endif
#elif defined(IMG_Init)
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::cerr << "Aviso: falha ao inicializar SDL_image: " << IMG_GetError() << std::endl;
    }
#endif
    // Modo sem GUI: apenas carregar/converter/salvar
    // #########################################
    //            FLUXO: MODO HEADLESS (--NOGUI)
    // #########################################
    if (noGui) {
        try {
            ImageProcessor ip;
            if (!ip.loadImage(imagePath.c_str())) {
                std::cerr << "Erro ao carregar imagem em modo --nogui" << std::endl;
                SDL_Quit();
                return 1;
            }
            // Garante que currentImage está definida (grayscale)
            if (!ip.getCurrentImage()) {
                std::cerr << "Imagem atual inválida após processamento" << std::endl;
                SDL_Quit();
                return 1;
            }
            if (!ip.saveImage("output_image.png")) {
                std::cerr << "Falha ao salvar output_image.png" << std::endl;
                SDL_Quit();
                return 1;
            }

            Histogram histogramCurrent;
            histogramCurrent.calculate(ip.getCurrentImage());
            Histogram histogramOriginal;
            histogramOriginal.calculate(ip.getOriginalImage());

            if (histogramCurrent.getTotalPixels() > 0) {
                std::cout << "Histograma (processada): média=" << histogramCurrent.getMean()
                          << " (" << histogramCurrent.getIntensityClassification()
                          << ") desvio=" << histogramCurrent.getStdDev()
                          << " (" << histogramCurrent.getContrastClassification() << ")" << std::endl;

                std::filesystem::path inputPath(imagePath);
                std::string baseName = inputPath.stem().string();
                if (baseName.empty()) {
                    baseName = "imagem";
                }

                std::filesystem::path outputDir = std::filesystem::path("output_" + baseName);
                std::error_code ec;
                std::filesystem::create_directories(outputDir, ec);
                if (ec) {
                    std::cerr << "Aviso: não foi possível criar diretório de saída '" << outputDir << "': " << ec.message() << std::endl;
                } else {
                    std::filesystem::path csvPath = outputDir / (baseName + "_histograma.csv");
                    if (histogramCurrent.saveCSV(csvPath.string())) {
                        std::cout << "Histograma salvo em: " << csvPath << std::endl;
                    } else {
                        std::cerr << "Aviso: falha ao salvar CSV do histograma em " << csvPath << std::endl;
                    }

                    std::filesystem::path summaryPath = outputDir / (baseName + "_stats.txt");
                    if (histogramCurrent.saveSummary(summaryPath.string())) {
                        std::cout << "Resumo estatístico salvo em: " << summaryPath << std::endl;
                    } else {
                        std::cerr << "Aviso: falha ao salvar resumo do histograma em " << summaryPath << std::endl;
                    }

                    std::filesystem::path plotPath = outputDir / (baseName + "_histograma.png");
                    if (histogramCurrent.savePlotImage(plotPath.string())) {
                        std::cout << "Imagem do histograma salva em: " << plotPath << std::endl;
                    } else {
                        std::cerr << "Aviso: falha ao renderizar o histograma em " << plotPath << std::endl;
                    }

                    std::filesystem::path pngPath("output_image.png");
                    if (std::filesystem::exists(pngPath)) {
                        std::filesystem::path copyPath = outputDir / (baseName + "_grayscale.png");
                        std::error_code copyEc;
                        std::filesystem::copy_file(pngPath, copyPath, std::filesystem::copy_options::overwrite_existing, copyEc);
                        if (!copyEc) {
                            std::cout << "Imagem em escala de cinza copiada para: " << copyPath << std::endl;
                        } else {
                            std::cerr << "Aviso: não foi possível copiar imagem processada: " << copyEc.message() << std::endl;
                        }
                    }

                    if (histogramOriginal.getTotalPixels() > 0) {
                        std::filesystem::path csvOriginal = outputDir / (baseName + "_original_histograma.csv");
                        if (histogramOriginal.saveCSV(csvOriginal.string())) {
                            std::cout << "Histograma original salvo em: " << csvOriginal << std::endl;
                        } else {
                            std::cerr << "Aviso: falha ao salvar CSV original em " << csvOriginal << std::endl;
                        }

                        std::filesystem::path summaryOriginal = outputDir / (baseName + "_original_stats.txt");
                        if (histogramOriginal.saveSummary(summaryOriginal.string())) {
                            std::cout << "Resumo original salvo em: " << summaryOriginal << std::endl;
                        } else {
                            std::cerr << "Aviso: falha ao salvar resumo original em " << summaryOriginal << std::endl;
                        }

                        std::filesystem::path plotOriginal = outputDir / (baseName + "_original_histograma.png");
                        if (histogramOriginal.savePlotImage(plotOriginal.string())) {
                            std::cout << "Imagem do histograma original salva em: " << plotOriginal << std::endl;
                        } else {
                            std::cerr << "Aviso: falha ao renderizar o histograma original em " << plotOriginal << std::endl;
                        }
                    } else {
                        std::cerr << "Aviso: não foi possível calcular histograma original da imagem." << std::endl;
                    }
                }
            } else {
                std::cerr << "Aviso: não foi possível calcular histograma da imagem processada." << std::endl;
            }
            std::cout << "Modo --nogui: imagem processada e salva em output_image.png" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Erro em modo --nogui: " << e.what() << std::endl;
            SDL_Quit();
            return 1;
        }
        // Finaliza SDL_image e SDL (apenas para versões antigas)
#if defined(SDL_IMAGE_VERSION_ATLEAST)
#if !SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
        IMG_Quit();
#endif
#elif defined(IMG_Quit)
        IMG_Quit();
#endif
        SDL_Quit();
        return 0;
    }

    if (sdlInit.usedFallback) {
        std::cerr << "Erro: modo GUI requer um driver de vídeo real. Feche aplicações em headless com --nogui." << std::endl;
        SDL_Quit();
        return 1;
    }

    // #########################################
    //            FLUXO: MODO GUI COMPLETO
    // #########################################
    // Criar e executar GUI
    try {
        GUI gui(imagePath);
        gui.run();
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        
        SDL_Quit();
        return 1;
    }

    // Limpar recursos das versões antigas do SDL_image
#if defined(SDL_IMAGE_VERSION_ATLEAST)
#if !SDL_IMAGE_VERSION_ATLEAST(3, 0, 0)
    IMG_Quit();
#endif
#elif defined(IMG_Quit)
    IMG_Quit();
#endif
    SDL_Quit();
    return 0;
}
