# Makefile para Projeto de Processamento de Imagens

# Detectar sistema operacional
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    EXE_EXT := .exe
    # Compilador Windows nativo
    CXX = g++
    # Bibliotecas Windows
    INCLUDES = -I"C:/SDL3/include" -Isrc
    LIBS = -L"C:/SDL3/lib" -lSDL3 -lSDL3_image -lSDL3_ttf -lm -mwindows
else
    DETECTED_OS := $(shell uname -s)
    EXE_EXT :=
    # Compilador Linux nativo
    CXX = g++
    # Bibliotecas Linux
    INCLUDES = -I/usr/local/include -I/usr/local/include/SDL3 -I/usr/include -I/usr/include/SDL3 -Isrc
    LIBS = -lSDL3 -lSDL3_image -lSDL3_ttf -lm
    
    # Cross compiler para Windows (se disponível)
    MINGW_CXX = x86_64-w64-mingw32-g++
    MINGW_INCLUDES = -I/usr/x86_64-w64-mingw32/include -I/usr/x86_64-w64-mingw32/include/SDL3 \
                     -Isrc -Itinyfiledialogs
    MINGW_LIBS = -L/usr/x86_64-w64-mingw32/lib -L/usr/lib/x86_64-w64-mingw32 -lSDL3 -lSDL3_image -lSDL3_ttf -lm -static-libgcc -static-libstdc++ -mwindows
endif

# Definições para tinyfiledialogs
ifeq ($(DETECTED_OS),Windows)
    CXXFLAGS += -D_WIN32
else
    CXXFLAGS += -D_UNIX
endif

CXXFLAGS = -std=c++20 -Wall -Wextra -O2

SRCDIR = src
OBJDIR = obj
BINDIR = bin

TARGET = $(BINDIR)/processador_imagens$(EXE_EXT)

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Criar diretórios se não existirem
$(shell mkdir -p $(OBJDIR) $(BINDIR))

all: native windows

# Alvos principais
native: $(TARGET)
windows: $(BINDIR)/processador_imagens.exe

# Compilação nativa (Linux/Windows)
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)
	@echo "Compilação nativa concluída! Executável: $(TARGET)"

# Compilação para Windows usando MinGW (apenas no Linux)
$(BINDIR)/processador_imagens.exe: $(SOURCES)
ifneq ($(DETECTED_OS),Windows)
	@if ! which $(MINGW_CXX) > /dev/null; then \
		echo "[make] Compilador MinGW ("$(MINGW_CXX)") não encontrado. Pulando build Windows."; \
	elif [ ! -d /usr/x86_64-w64-mingw32/include/SDL3 ]; then \
		echo "[make] Cabeçalhos SDL3 para MinGW não encontrados em /usr/x86_64-w64-mingw32/include/SDL3. Pulando build Windows."; \
	else \
		mkdir -p $(OBJDIR)/windows $(BINDIR); \
		$(MINGW_CXX) $(CXXFLAGS) -D_WIN32 $(MINGW_INCLUDES) $(SOURCES) -o $@ $(MINGW_LIBS); \
		echo "Compilação Windows concluída! Executável: $@"; \
		echo "Copie as DLLs necessárias para o diretório bin:"; \
		echo "  - SDL3.dll"; \
		echo "  - SDL3_image.dll"; \
		echo "  - SDL3_ttf.dll"; \
	fi
else
	@echo "Compilação cruzada não suportada no Windows"
endif

# Regras de compilação
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(TINYFDDIR)/%.c
	@mkdir -p $(OBJDIR)
ifeq ($(DETECTED_OS),Windows)
	$(CXX) $(CXXFLAGS) -D_WIN32 $(INCLUDES) -c $< -o $@
else
	$(CXX) $(CXXFLAGS) -D_UNIX $(INCLUDES) -c $< -o $@
endif

# Limpeza
clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
	@echo "Arquivos limpos!"

# Execução
run: $(TARGET)
	@echo "Execute com:"
	@echo "  Linux  : $(BINDIR)/processador_imagens [imagem]"
	@echo "  Windows: $(BINDIR)/processador_imagens.exe [imagem]"

# Instalar dependências no Ubuntu/Debian
deps-ubuntu:
	sudo apt update
	sudo apt install -y build-essential mingw-w64 libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

# Instalar dependências no Fedora
deps-fedora:
	sudo dnf install -y gcc-c++ mingw64-gcc-c++ sdl3-devel sdl3_image-devel sdl3_ttf-devel

# Instalar dependências no Arch Linux
deps-arch:
	sudo pacman -S --needed base-devel mingw-w64-gcc sdl3 sdl3_image sdl3_ttf

.PHONY: all clean run native windows deps-ubuntu deps-fedora deps-arch
