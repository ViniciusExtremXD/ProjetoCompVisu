# Projeto 1 – Processamento de Imagens

Universidade Presbiteriana Mackenzie – Ciência da Computação
Disciplina: Computação Visual

Aplicação em **C++26** construída com **SDL3** que atende aos requisitos do Projeto 1 da disciplina de Computação Visual. O software oferece carregamento, análise, equalização e salvamento de imagens, com interface gráfica em duas janelas e modo `--nogui` totalmente instrumentado.

## 📌 Integrantes

* Heitor Maciel - 10402559
* Vitor Pepe - 10339754
* Vinícius Magno - 10401365
* Kaiki Bellini Barbosa- 10402509

---

## 🎯 Objetivo

Desenvolver um software em **C++26** que realiza processamento avançado de imagens usando a biblioteca **SDL3** e suas extensões.

O programa recebe uma imagem como argumento de linha de comando, exibe a imagem em uma janela principal, mostra o histograma em uma janela secundária e permite operações de equalização e salvamento da imagem. Suporta tanto interface gráfica quanto modo headless (`--nogui`) para processamento automatizado.

---

## ✅ Requisitos do Projeto — Checklist

- [x] Carregamento de PNG / JPG / BMP com tratamento de erros (`SDL3_image`).
- [x] Conversão automática para escala de cinza com a fórmula 𝑌 = 0.2125R + 0.7154G + 0.0721B e detecção de imagens já monocromáticas.
- [x] Interface gráfica com duas janelas (principal e filha) alinhadas às especificações, botões com feedback visual e diálogos nativos.
- [x] Histograma exibido na janela secundária com classificação de média (escura/média/clara) e desvio padrão (baixo/médio/alto).
- [x] Equalização de histograma com alternância entre imagem equalizada e original sem recarregar a imagem.
- [x] Salvamento da imagem atual com a tecla `S`, gerando/atualizando `output_image.png`.
- [x] Documentação com instruções completas de compilação e execução.

---

## 🖼️ Funcionalidades Principais

### Interface Gráfica (GUI)
* **Duas janelas sincronizadas**:
  * **Principal** → adapta-se ao tamanho da imagem e mostra o resultado
  * **Secundária** → fixa, exibe histograma, estatísticas e botões (`Abrir`, `Equalizar/Original`, `Salvar`)

### Processamento de Imagens
* Carregamento de imagens nos formatos **PNG, JPG e BMP**
* Conversão segura para `SDL_PIXELFORMAT_RGBA8888` e escala de cinza
* Equalização via CDF (Cumulative Distribution Function)
* Classificação automática de intensidade e contraste
* Salvamento resiliente (PNG → BMP → JPG fallbacks)

### Modo `--nogui` (Headless)
* Processa imagens em ambientes sem janela
* Gera relatórios e gráficos automaticamente
* Mantém logs detalhados da inicialização SDL
* Cria diretórios estruturados com resultados completos

---

## ⚙️ Requisitos Técnicos

* **Linguagem:** C++26
* **Compilador:** g++ 15.1.0
* **Bibliotecas obrigatórias:**
  * SDL3
  * SDL_image
  * SDL_ttf

---

## 🔧 Instalação das Dependências

### Instalação do g++ 15.1.0 com suporte a C++26

#### Linux (Ubuntu/Debian)

1. **GCC 15.1.0 (build do zero. Pode demorar!!!)**
   1. **Instalar dependências:**
        ```bash
        sudo apt install -y build-essential git make gawk flex bison libgmp-dev libmpfr-dev libmpc-dev python3 binutils perl libisl-dev libzstd-dev tar gzip bzip2
        ```
   2. **Forçar bash para GCC Build**
        ```bash
        export CONFIG_SHELL=/bin/bash
        ```
   3. **Baixar GCC 15.1.0**
        ```bash
        mkdir ~/gcc-15
        cd ~/gcc-15
        git clone https://gcc.gnu.org/git/gcc.git gcc-15-source
        cd gcc-15-source
        git checkout releases/gcc-15.1.0
        ./contrib/download_prerequisites
        ```
   4. **Configurar Build GCC**
        ```bash
        cd ~/gcc-15
        mkdir gcc-15-build
        cd gcc-15-build
        ../gcc-15-source/configure --prefix=/opt/gcc-15 --disable-multilib --enable-languages=c,c++
        ```
   5. **Buildar GCC (demora de minutos a horas)**
        ```bash
        make -j$(nproc)
        ```
   6. **Instalar GCC 15.1.0**
        ```bash
        sudo make install
        ```
   7. **Definir GCC/G++ 15.1.0 como default**
        ```bash
        sudo update-alternatives --install /usr/bin/gcc gcc /opt/gcc-15/bin/gcc 100
        sudo update-alternatives --install /usr/bin/g++ g++ /opt/gcc-15/bin/g++ 100
        ```

#### Windows

1. Instalar o **MSYS2**: [https://www.msys2.org](https://www.msys2.org)
2. Atualizar pacotes:
   ```bash
   pacman -Syu
   ```
3. Instalar g++ 15:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
4. Verificar versão:
   ```bash
   g++ --version
   ```

### Instalação do SDL3 e Extensões

#### Linux (Ubuntu 24.04 / WSL2)

1. **Instale as ferramentas básicas**
   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake git pkg-config ninja-build
   ```

2. **Instale dependências para SDL_image e SDL_ttf**
   ```bash
   sudo apt install -y zlib1g-dev libpng-dev libjpeg-turbo8-dev \
     libwebp-dev libtiff-dev libfreetype6-dev \
     libasound2-dev libpulse-dev libx11-dev libxext-dev \
     libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev \
     libxss-dev libxkbcommon-dev libdrm-dev libgbm-dev \
     libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
     libdbus-1-dev libibus-1.0-dev libudev-dev \
     libpipewire-0.3-dev libwayland-dev libdecor-0-dev
   ```

3. **Compile e instale SDL3, SDL3_image e SDL3_ttf**
   ```bash
   cd ~/repos

   # SDL3
   git clone https://github.com/libsdl-org/SDL.git sdl3
   cd sdl3
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSDL_TESTS=OFF
   cmake --build build -j
   sudo cmake --install build --prefix /usr/local
   cd ..

   # SDL3_image
   git clone https://github.com/libsdl-org/SDL_image.git sdl3_image
   cd sdl3_image
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSDLIMAGE_SAMPLES=OFF
   cmake --build build -j
   sudo cmake --install build --prefix /usr/local
   cd ..

   # SDL3_ttf
   git clone https://github.com/libsdl-org/SDL_ttf.git sdl3_ttf
   cd sdl3_ttf
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSDLTTF_INSTALL_PKGCONFIG=ON
   cmake --build build -j
   sudo cmake --install build --prefix /usr/local
   cd ..
   ```

4. **Configure o pkg-config**
   Adicione ao `~/.bashrc`:
   ```bash
   export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
   ```
   e rode:
   ```bash
   source ~/.bashrc
   ```

#### Windows (MinGW-w64)

1. Instale **MinGW-w64** ou use **WSL2** (recomendado).
2. Baixe e compile as bibliotecas:
   * [SDL3](https://github.com/libsdl-org/SDL)
   * [SDL_image](https://github.com/libsdl-org/SDL_image)
   * [SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
3. Ajuste o `Makefile` para apontar para os diretórios `include` e `lib` do SDL.
4. Compile:
   ```bash
   mingw32-make
   ```
5. Copie as DLLs necessárias (`SDL3.dll`, `SDL3_image.dll`, `SDL3_ttf.dll`) para a pasta `bin/`.

#### Instalação Alternativa (Distribuições Linux)

```bash
# Ubuntu / Debian
sudo apt install build-essential libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

# Fedora
sudo dnf install gcc-c++ sdl3-devel sdl3_image-devel sdl3_ttf-devel

# Arch Linux
sudo pacman -S base-devel sdl3 sdl3_image sdl3_ttf
```

---

## 📂 Estrutura do Projeto

```
ProjetoCompVisu/
├── assets/                   # Imagens de teste
├── bin/                      # Executáveis gerados
├── build/                    # Arquivos objeto (.o) e executável principal
├── include/                  # Arquivos de cabeçalho (.h/.hpp)
│   ├── GUI.h
│   ├── Histogram.h
│   ├── ImageProcessor.h
│   └── tinyfiledialogs.h
├── obj/                      # Arquivos objeto e imagens de exemplo
├── output_<nome>/            # Diretórios criados no modo --nogui
├── scripts/                  # Scripts utilitários (ex.: test_nogui.sh)
├── src/                      # Código-fonte principal (.cpp)
│   ├── GUI.cpp
│   ├── Histogram.cpp
│   ├── ImageProcessor.cpp
│   └── main.cpp
├── tinyfiledialogs/          # Biblioteca auxiliar (diálogos nativos)
├── Makefile                  # Automação da compilação
└── README.md                 # Documentação
```

---

## 🔧 Compilação

```bash
git clone https://github.com/ViniciusExtremXD/ProjetoCompVisu.git
cd ProjetoCompVisu
make           # gera build/main

# Opcional (Linux): tentativa de build Windows usando MinGW
make windows   # requer x86_64-w64-mingw32-g++ e headers SDL3 para MinGW
```

### Comandos de Build Adicionais

```bash
make clean     # limpa arquivos objeto e executáveis
make run       # compila e executa com imagem de teste
```

---

## ⌨️ Execução

### 1. Interface Gráfica (padrão)

```bash
./build/main caminho/para/imagem.png
```

**Exemplo de teste rápido:**
```bash
./build/main assets/teste1.png
```

Durante a inicialização, o programa lista os drivers de vídeo detectados pelo SDL e informa qual driver foi utilizado. Se nenhum driver gráfico real estiver disponível, a execução é abortada com orientações para usar `--nogui`.

### 2. Modo Headless (`--nogui`)

```bash
./build/main --nogui caminho/para/imagem.png
```

**Saídas geradas automaticamente:**
- `output_image.png`: resultado em escala de cinza (sobrescrito a cada execução)
- Diretório `output_<nome-da-imagem>/` contendo:
  - `<nome>_grayscale.png`: imagem processada em escala de cinza
  - `<nome>_histograma.csv`: contagens do histograma da imagem atual
  - `<nome>_stats.txt`: média, desvio padrão e classificações da imagem atual
  - `<nome>_histograma.png`: renderização do histograma atual
  - `<nome>_original_histograma.csv`: contagens do histograma da imagem original
  - `<nome>_original_stats.txt`: média/desvio da imagem original
  - `<nome>_original_histograma.png`: renderização comparativa da imagem original

---

## 🖱️ Uso da Interface

### Controles da GUI

1. **Abrir imagem**: botão "Abrir" ou argumento na linha de comando
2. **Equalizar**: botão "Equalizar" alterna entre equalização e imagem original (texto muda para "Original")
3. **Salvar**: botão "Salvar" ou tecla `S` grava `output_image.png` na raiz
4. **Histograma**: atualizado em tempo real; a janela exibe a curva atual sobreposta ao histograma original

---

## 📚 Referências

* Instalação GCC:
  * [Moving to C++26: How to Build and Set Up GCC 15.1 on Ubuntu](https://medium.com/@xersendo/moving-to-c-26-how-to-build-and-set-up-gcc-15-1-on-ubuntu-f52cc9173fa0)
* SDL3 Documentation: [https://wiki.libsdl.org/SDL3/](https://wiki.libsdl.org/SDL3/)
* Computação Visual - Processamento de Imagens
