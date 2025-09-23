# Makefile para Projeto de Processamento de Imagens (SDL3 + image + ttf)

# ==========================
# DETECÇÃO DE SO / COMPILADOR
# ==========================
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

EXE_EXT :=
CXX ?= g++

# ==========================
# CONFIG GERAL
# ==========================
SRCDIR := src
TINYFDDIR := tinyfiledialogs
OBJDIR := obj
BINDIR := bin
TARGET := $(BINDIR)/processador_imagens$(EXE_EXT)

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
TINYFD_SOURCES := $(wildcard $(TINYFDDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TINYFD_OBJECTS := $(TINYFD_SOURCES:$(TINYFDDIR)/%.c=$(OBJDIR)/%.o)

# Flags base (não sobrescreva depois!)
CXXFLAGS := -std=c++20 -Wall -Wextra -O2

ifeq ($(DETECTED_OS),Windows)
    CXXFLAGS += -D_WIN32
else
    CXXFLAGS += -D_UNIX
endif

# ==========================
# SDL (Linux via pkg-config)
# ==========================
# Use pkg-config para descobrir cflags/libs das três libs
PKGCFG   ?= pkg-config
SDL_PKGS := sdl3 sdl3-image sdl3-ttf

ifeq ($(DETECTED_OS),Windows)
    # Caminhos Windows locais (ajuste conforme seu ambiente, se for compilar nativamente no Windows)
    INCLUDES := -I"C:/SDL3/include" -Isrc
    LIBS     := -L"C:/SDL3/lib" -lSDL3 -lSDL3_image -lSDL3_ttf -lm -mwindows
else
    # Linux/WSL2: use pkg-config (recomendado)
    # OBS: se instalou em /usr/local, garanta que o PKG_CONFIG_PATH contenha /usr/local/lib/pkgconfig
    # export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$$PKG_CONFIG_PATH
    INCLUDES := -Isrc -I$(TINYFDDIR) $(shell $(PKGCFG) --cflags $(SDL_PKGS))
    LIBS     := $(shell $(PKGCFG) --libs $(SDL_PKGS)) -lm
endif

# ==========================
# CROSS-COMPILATION MINGW (opcional no Linux)
# ==========================
MINGW_CXX      ?= x86_64-w64-mingw32-g++
MINGW_INCLUDES := -Isrc -I$(TINYFDDIR)
# OBS: para cross, é comum precisar dos .pc para o triplo alvo (mingw). Se não tiver,
# mantenha libs estáticas ou paths locais. Abaixo segue uma linha genérica:
MINGW_LIBS     := -lSDL3 -lSDL3_image -lSDL3_ttf -lm -static-libgcc -static-libstdc++ -mwindows

# ==========================
# DIRETÓRIOS
# ==========================
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# ==========================
# ALVOS
# ==========================
.PHONY: all clean run native windows deps-ubuntu deps-fedora deps-arch test install uninstall help

# Por padrão, compile apenas nativo; o alvo "windows" é opcional
all: native

native: $(TARGET)
	@echo "Compilação nativa concluída! Executável: $(TARGET)"

# Alvo para testar rapidamente com uma imagem de exemplo
test: $(TARGET)
	@echo "Procurando imagens de exemplo..."
	@if [ -f "obj/gato1.bmp" ]; then \
		echo "Testando com obj/gato1.bmp..."; \
		$(TARGET) obj/gato1.bmp; \
	elif [ -f "obj/1128858.bmp" ]; then \
		echo "Testando com obj/1128858.bmp..."; \
		$(TARGET) obj/1128858.bmp; \
	elif [ -f "obj/gato1.jpeg" ]; then \
		echo "Testando com obj/gato1.jpeg..."; \
		$(TARGET) obj/gato1.jpeg; \
	else \
		echo "Nenhuma imagem de teste encontrada em obj/"; \
		echo "Criando imagem de teste simples..."; \
		echo "Use: $(TARGET) caminho/para/sua/imagem.{png,jpg,bmp}"; \
		echo "Ou: $(TARGET) --nogui caminho/para/sua/imagem.{png,jpg,bmp}"; \
	fi

# Alvo para instalação local (opcional)
install: $(TARGET)
	@echo "Instalando $(TARGET) em /usr/local/bin..."
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "Instalação concluída! Use: processador_imagens [imagem]"

# Alvo para desinstalação
uninstall:
	@echo "Removendo processador_imagens de /usr/local/bin..."
	@sudo rm -f /usr/local/bin/processador_imagens
	@echo "Desinstalação concluída!"

# Alvo de ajuda
help:
	@echo "Makefile para Processador de Imagens (SDL3)"
	@echo ""
	@echo "Alvos disponíveis:"
	@echo "  all       - Compila o projeto (padrão: native)"
	@echo "  native    - Compila para o sistema atual"
	@echo "  windows   - Compilação cruzada para Windows (requer MinGW)"
	@echo "  test      - Compila e testa com imagem de exemplo"
	@echo "  run       - Mostra instruções de execução"
	@echo "  clean     - Remove arquivos objeto e executáveis"
	@echo "  install   - Instala o executável em /usr/local/bin"
	@echo "  uninstall - Remove o executável de /usr/local/bin"
	@echo "  help      - Mostra esta ajuda"
	@echo ""
	@echo "Dependências (escolha sua distribuição):"
	@echo "  deps-ubuntu - Instala dependências no Ubuntu/Debian"
	@echo "  deps-fedora - Instala dependências no Fedora"
	@echo "  deps-arch   - Instala dependências no Arch Linux"
	@echo ""
	@echo "Uso:"
	@echo "  make && ./bin/processador_imagens caminho/para/imagem.png"
	@echo "  make test  # Para teste rápido"

# Compilação cruzada para Windows usando MinGW (somente em Linux)
windows: $(SOURCES)
ifneq ($(DETECTED_OS),Windows)
	@if ! which $(MINGW_CXX) > /dev/null; then \
		echo "[make] Compilador MinGW ($(MINGW_CXX)) não encontrado. Pulando build Windows."; \
	else \
		mkdir -p $(OBJDIR)/windows $(BINDIR); \
		$(MINGW_CXX) $(CXXFLAGS) -D_WIN32 $(MINGW_INCLUDES) $(SOURCES) -o $(BINDIR)/processador_imagens.exe $(MINGW_LIBS); \
		echo "Compilação Windows concluída! Executável: $(BINDIR)/processador_imagens.exe"; \
		echo "Copie as DLLs necessárias para o diretório bin (se estiver linkando dinamicamente):"; \
		echo "  - SDL3.dll"; \
		echo "  - SDL3_image.dll"; \
		echo "  - SDL3_ttf.dll"; \
	fi
else
	@echo "Compilação cruzada não suportada no Windows"
endif

# Link final (nativo)
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

# Regra de compilação dos .cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Regra de compilação dos .c (tinyfiledialogs)
$(OBJDIR)/%.o: $(TINYFDDIR)/%.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
	@echo "Arquivos limpos!"

# Execução
run: $(TARGET)
	@echo "Execute com:"
	@echo "  Linux/WSL: $(BINDIR)/processador_imagens [imagem]"
	@echo "  Windows  : $(BINDIR)/processador_imagens.exe [imagem]"

# Dependências (atenção: no Ubuntu 24.04 os pacotes sdl3-dev podem não existir)
deps-ubuntu:
	sudo apt update
	# Toolchain + utilitários
	sudo apt install -y build-essential pkg-config cmake ninja-build
	# Codecs/formatos para SDL_image e TTF
	sudo apt install -y zlib1g-dev libpng-dev libjpeg-turbo8-dev libwebp-dev libtiff-dev libfreetype6-dev
	# Backends e gráficos comuns
	sudo apt install -y libasound2-dev libpulse-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libpipewire-0.3-dev libwayland-dev libdecor-0-dev
	@echo "Nota: SDL3/SDL3_image/SDL3_ttf foram instalados via build from source em /usr/local; não há libsdl3-dev oficiais no 24.04."

deps-fedora:
	sudo dnf install -y gcc-c++ pkgconf-pkg-config cmake ninja-build \
		zlib-devel libpng-devel libjpeg-turbo-devel libwebp-devel libtiff-devel freetype-devel

deps-arch:
	sudo pacman -S --needed base-devel pkgconf cmake ninja zlib libpng libjpeg-turbo libwebp libtiff freetype2
