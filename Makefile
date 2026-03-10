# =============================================================================
# MAKEFILE PARA PROJETO DE PROCESSAMENTO DE IMAGENS - C++26
# =============================================================================
# 
# Universidade Pr# Banner de apresentação
banner:
	clear
	@echo "╔══════════════════════════════════════════════════════════════╗"
	@echo "║               PROJETO PROCESSAMENTO DE IMAGENS               ║"
	@echo "║                     C++26 com SDL3                          ║"
	@echo "╠══════════════════════════════════════════════════════════════╣"
	@echo "║ Universidade Presbiteriana Mackenzie                        ║"
	@echo "║ Computação Visual - $(shell date +'%Y')                                    ║"
	@echo "╚══════════════════════════════════════════════════════════════╝"na Mackenzie - Ciência da Computação
# Disciplina: Computação Visual
# 
# Compilação moderna para aplicação de processamento de imagens usando:
# - C++26 com GCC 15.1.0+
# - SDL3, SDL3_image, SDL3_ttf
# - Suporte para modo GUI e headless
# 
# Uso:
#   make                    # Compilar aplicação
#   make run               # Executar modo GUI
#   make test IMAGE=path   # Testar com imagem específica
#   make test-headless     # Testar modo sem GUI
#   make clean             # Limpar arquivos de build
#   make help              # Mostrar ajuda completa
# 
# =============================================================================

# =============================================================================
# CONFIGURAÇÕES DO COMPILADOR E FLAGS
# =============================================================================

# Compiladores
CXX = g++
CC = gcc

# Flags para C++26 com otimizações e avisos essenciais (sem conversions rigorosas)
CXXFLAGS = -std=c++26 -Wall -Wextra -g -O2 \
           -Wshadow -Wnon-virtual-dtor \
           -Wcast-align -Wunused -Woverloaded-virtual \
           -Wmisleading-indentation -Wduplicated-cond \
           -Wduplicated-branches -Wlogical-op \
           -Wnull-dereference -Wformat=2

# Flags para arquivos C
CFLAGS = -Wall -Wextra -g -O2

# Configurações SDL3 usando pkg-config
SDL_CFLAGS = $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf)
SDL_LIBS = $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf)

# Flags de linking adicionais para compatibilidade
LDFLAGS = -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags

# =============================================================================
# ESTRUTURA DE DIRETÓRIOS E ARQUIVOS
# =============================================================================

# Diretórios do projeto
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
ASSETS_DIR = assets
OUTPUT_DIR = output

# Localização de dependências externas
SYSTEM_INCLUDE_DIRS = -I/usr/local/include -I/usr/include/freetype2 -I/usr/include/libpng16
SYSTEM_LIB_DIRS = -L/usr/local/lib

# Arquivos fonte
SOURCES_CPP = $(wildcard $(SRC_DIR)/*.cpp)
SOURCES_C = $(wildcard $(SRC_DIR)/*.c)
OBJECTS_CPP = $(SOURCES_CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJECTS_C = $(SOURCES_C:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
# Adicionar tinyfiledialogs.c diretamente como objeto
TINYFD_OBJECT = $(BUILD_DIR)/tinyfiledialogs.o
ALL_OBJECTS = $(OBJECTS_CPP) $(OBJECTS_C) $(TINYFD_OBJECT)

# Executável principal
TARGET = $(BUILD_DIR)/main

# Arquivo de imagem de teste padrão
DEFAULT_TEST_IMAGE = $(ASSETS_DIR)/teste1.png

# =============================================================================
# TARGETS PRINCIPAIS
# =============================================================================

# Target padrão: compilar aplicação
all: banner $(TARGET)
	clear
	@echo
	@echo "✅ Build concluído com sucesso!"
	@echo "   Executável: $(TARGET)"
	@echo "   Para testar: make run ou make test IMAGE=caminho/imagem.png"

# Compilar executável principal
$(TARGET): $(ALL_OBJECTS) | $(BUILD_DIR)
	clear
	@echo "🔗 Linkando executável..."
	$(CXX) $(ALL_OBJECTS) -o $@ $(SYSTEM_INCLUDE_DIRS) $(SYSTEM_LIB_DIRS) $(SDL_LIBS) $(LDFLAGS)
	@echo "   Executável criado: $@"

# =============================================================================
# COMPILAÇÃO DE ARQUIVOS OBJETO
# =============================================================================

# Compilar arquivos C++ (.cpp)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	clear
	@echo "🔨 Compilando C++: $<"
	$(CXX) $(CXXFLAGS) $(SYSTEM_INCLUDE_DIRS) -I$(INCLUDE_DIR) $(SDL_CFLAGS) -c $< -o $@

# Compilar arquivos C (.c)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	clear
	@echo "🔨 Compilando C: $<"
	$(CC) $(CFLAGS) $(SYSTEM_INCLUDE_DIRS) -I$(INCLUDE_DIR) $(SDL_CFLAGS) -c $< -o $@

# Compilar tinyfiledialogs.c especificamente do diretório include
$(BUILD_DIR)/tinyfiledialogs.o: $(INCLUDE_DIR)/tinyfiledialogs.c | $(BUILD_DIR)
	clear
	@echo "🔨 Compilando biblioteca externa: $<"
	$(CC) $(CFLAGS) $(SYSTEM_INCLUDE_DIRS) -I$(INCLUDE_DIR) -c $< -o $@

# Criar diretório de build se não existir
$(BUILD_DIR):
	clear
	@echo "📁 Criando diretório de build..."
	mkdir -p $(BUILD_DIR)

# =============================================================================
# TARGETS DE EXECUÇÃO E TESTE
# =============================================================================

# Executar aplicação em modo GUI (sem argumentos)
run: $(TARGET)
	clear
	@echo "🚀 Executando aplicação em modo GUI..."
	@if [ -f "$(DEFAULT_TEST_IMAGE)" ]; then \
		echo "   Carregando imagem padrão: $(DEFAULT_TEST_IMAGE)"; \
		./$(TARGET) $(DEFAULT_TEST_IMAGE); \
	else \
		echo "   Executando sem imagem (será solicitada via interface)"; \
		./$(TARGET); \
	fi

# Testar com imagem específica
# Uso: make test IMAGE=caminho/para/imagem.png [NOGUI=1]
test: $(TARGET)
	clear
	@if [ -z "$(IMAGE)" ]; then \
		echo "❌ Erro: Especifique uma imagem para teste"; \
		echo "   Uso: make test IMAGE=caminho/para/imagem.png"; \
		echo "   Exemplo: make test IMAGE=$(DEFAULT_TEST_IMAGE)"; \
		echo "   Modo headless: make test IMAGE=imagem.png NOGUI=1"; \
		exit 1; \
	elif [ ! -f "$(IMAGE)" ]; then \
		echo "❌ Erro: Arquivo não encontrado: $(IMAGE)"; \
		exit 1; \
	elif [ "$(NOGUI)" = "1" ]; then \
		echo "🤖 Testando modo headless com: $(IMAGE)"; \
		./$(TARGET) --nogui $(IMAGE); \
	else \
		echo "🖼️  Testando modo GUI com: $(IMAGE)"; \
		./$(TARGET) $(IMAGE); \
	fi

# Testar modo headless com imagem padrão
test-headless: $(TARGET)
	clear
	@if [ ! -f "$(DEFAULT_TEST_IMAGE)" ]; then \
		echo "❌ Erro: Imagem de teste não encontrada: $(DEFAULT_TEST_IMAGE)"; \
		exit 1; \
	fi
	@echo "🤖 Testando modo headless com imagem padrão..."
	./$(TARGET) --nogui $(DEFAULT_TEST_IMAGE)

# Executar com debugger
debug: $(TARGET)
	clear
	@echo "🐛 Executando com GDB..."
	@if [ -f "$(DEFAULT_TEST_IMAGE)" ]; then \
		gdb --args ./$(TARGET) $(DEFAULT_TEST_IMAGE); \
	else \
		gdb ./$(TARGET); \
	fi

# Executar análise de memória com Valgrind
valgrind: $(TARGET)
	clear
	@echo "🔍 Executando análise de memória..."
	@if [ -f "$(DEFAULT_TEST_IMAGE)" ]; then \
		valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) --nogui $(DEFAULT_TEST_IMAGE); \
	else \
		echo "❌ Imagem de teste não encontrada para Valgrind"; \
	fi

# =============================================================================
# LIMPEZA E MANUTENÇÃO
# =============================================================================

# Limpar arquivos de build
clean:
	clear
	@echo "🧹 Limpando arquivos de build..."
	rm -rf $(BUILD_DIR)
	@echo "   Diretório $(BUILD_DIR) removido"

# Limpeza completa (incluindo arquivos de output)
clean-all: clean
	clear
	@echo "🧹 Limpeza completa..."
	rm -rf $(OUTPUT_DIR)
	rm -f output_*.png *.bmp *.jpg
	find . -name "output_*" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "   Arquivos de output removidos"

# =============================================================================
# INFORMAÇÕES E DIAGNÓSTICOS
# =============================================================================

# Mostrar informações do sistema
info:
	clear
	@echo "ℹ️  Informações do sistema:"
	@echo "   Compilador C++: $(shell $(CXX) --version | head -1)"
	@echo "   Compilador C: $(shell $(CC) --version | head -1)"
	@echo "   Flags C++: $(CXXFLAGS)"
	@echo "   SDL3 encontrado: $(shell pkg-config --exists sdl3 && echo 'Sim' || echo 'Não')"
	@echo "   SDL3_image encontrado: $(shell pkg-config --exists sdl3-image && echo 'Sim' || echo 'Não')"
	@echo "   SDL3_ttf encontrado: $(shell pkg-config --exists sdl3-ttf && echo 'Sim' || echo 'Não')"
	@echo "   Diretório atual: $(shell pwd)"
	@echo "   Arquivos fonte C++: $(words $(SOURCES_CPP)) arquivos"
	@echo "   Arquivos fonte C: $(words $(SOURCES_C)) arquivos"

# Verificar dependências
check-deps:
	clear
	@echo "🔍 Verificando dependências..."
	@pkg-config --exists sdl3 || (echo "❌ SDL3 não encontrado" && exit 1)
	@pkg-config --exists sdl3-image || (echo "❌ SDL3_image não encontrado" && exit 1)
	@pkg-config --exists sdl3-ttf || (echo "❌ SDL3_ttf não encontrado" && exit 1)
	@command -v $(CXX) >/dev/null || (echo "❌ $(CXX) não encontrado" && exit 1)
	@echo "✅ Todas as dependências encontradas"

# Banner de apresentação
banner:
	@echo "╔══════════════════════════════════════════════════════════════╗"
	@echo "║               PROJETO PROCESSAMENTO DE IMAGENS               ║"
	@echo "║                     C++26 com SDL3                           ║"
	@echo "╠══════════════════════════════════════════════════════════════╣"
	@echo "║ Universidade Presbiteriana Mackenzie                         ║"
	@echo "║ Computação Visual - $(shell date +'%Y')                      ║"
	@echo "╚══════════════════════════════════════════════════════════════╝"

# Ajuda completa
help:
	clear
	@echo "📖 AJUDA - MAKEFILE PROCESSAMENTO DE IMAGENS"
	@echo
	@echo "🎯 TARGETS PRINCIPAIS:"
	@echo "   make              - Compilar aplicação"
	@echo "   make all          - Compilar com banner"
	@echo "   make run          - Executar modo GUI"
	@echo "   make clean        - Limpar build"
	@echo
	@echo "🧪 TARGETS DE TESTE:"
	@echo "   make test IMAGE=arquivo.png    - Testar com imagem específica"
	@echo "   make test IMAGE=arquivo.png NOGUI=1  - Testar modo headless"
	@echo "   make test-headless             - Testar headless com imagem padrão"
	@echo
	@echo "🛠️  TARGETS DE DEBUG:"
	@echo "   make debug        - Executar com GDB"
	@echo "   make valgrind     - Análise de memória"
	@echo
	@echo "ℹ️  TARGETS DE INFORMAÇÃO:"
	@echo "   make info         - Informações do sistema"
	@echo "   make check-deps   - Verificar dependências"
	@echo "   make help         - Esta ajuda"
	@echo
	@echo "🧹 TARGETS DE LIMPEZA:"
	@echo "   make clean        - Limpar arquivos de build"
	@echo "   make clean-all    - Limpeza completa"
	@echo
	@echo "📁 ESTRUTURA:"
	@echo "   $(SRC_DIR)/          - Código fonte (.cpp, .c)"
	@echo "   $(INCLUDE_DIR)/      - Headers (.h, .hpp)"
	@echo "   $(BUILD_DIR)/        - Arquivos compilados"
	@echo "   $(ASSETS_DIR)/       - Imagens de teste"

# =============================================================================
# TARGETS ESPECIAIS
# =============================================================================

# Targets que não correspondem a arquivos
.PHONY: all run test test-headless debug valgrind clean clean-all info check-deps banner help

# Configurações especiais do Make
.DEFAULT_GOAL := all
.SILENT: