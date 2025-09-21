# Processador de Imagens – Computação Visual (Proj1)

Aplicação em C++20 construída com SDL3 que atende aos requisitos do Projeto 1 da disciplina de Computação Visual da Universidade Presbiteriana Mackenzie. O software oferece carregamento, análise, equalização e salvamento de imagens, com interface gráfica em duas janelas e modo `--nogui` totalmente instrumentado.

## Requisitos do Projeto — Checklist

- [x] Carregamento de PNG / JPG / BMP com tratamento de erros (`SDL3_image`).
- [x] Conversão automática para escala de cinza com a fórmula 𝑌 = 0.2125R + 0.7154G + 0.0721B e detecção de imagens já monocromáticas.
- [x] Interface gráfica com duas janelas (principal e filha) alinhadas às especificações, botões com feedback visual e diálogos nativos.
- [x] Histograma exibido na janela secundária com classificação de média (escura/média/clara) e desvio padrão (baixo/médio/alto).
- [x] Equalização de histograma com alternância entre imagem equalizada e original sem recarregar a imagem.
- [x] Salvamento da imagem atual com a tecla `S`, gerando/atualizando `output_image.png`.
- [x] Documentação com instruções completas de compilação e execução.

## Funcionalidades Principais

- **Interface Gráfica (GUI)**: janela principal adapta-se ao tamanho da imagem e mostra o resultado; janela filha fixa exibe histograma, estatísticas e botões (`Abrir`, `Equalizar/Original`, `Salvar`).
- **Processamento**: conversão segura para `SDL_PIXELFORMAT_RGBA8888`, equalização via CDF, classificação de intensidade/contraste e salvamento resiliente (PNG → BMP → JPG).
- **Modo `--nogui`**: processa imagens em ambientes sem janela, gera relatórios e gráficos automáticos e mantém logs detalhados da inicialização SDL.

## Estrutura do Projeto

```
├── bin/                      # Executáveis gerados
├── obj/                      # Arquivos objeto e imagens de teste (*.bmp)
├── src/                      # Código-fonte principal
├── tinyfiledialogs/          # Biblioteca auxiliar (diálogos nativos)
├── scripts/                  # Scripts utilitários (ex.: test_nogui.sh)
└── output_<nome>/            # Diretórios criados no modo --nogui
```

## Dependências

Instale os pacotes equivalentes em sua distribuição/sistema:

```bash
# Ubuntu / Debian
sudo apt install build-essential libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

# Fedora
sudo dnf install gcc-c++ sdl3-devel sdl3_image-devel sdl3_ttf-devel

# Arch Linux
sudo pacman -S base-devel sdl3 sdl3_image sdl3_ttf
```

Em Windows, instale o toolchain MinGW-w64 e a distribuição oficial do SDL3.

## Compilação

```bash
git clone https://github.com/ViniciusExtremXD/ProjetoCompVisu.git
cd ProjetoCompVisu
make           # gera bin/processador_imagens
# Opcional (Linux): tentativa de build Windows usando MinGW
make windows   # requer x86_64-w64-mingw32-g++ e headers SDL3 para MinGW
# (Se os componentes não estiverem instalados, o Makefile apenas registra a ausência e prossegue.)
```

## Execução

### 1. Interface Gráfica (padrão)

```bash
./bin/processador_imagens caminho/para/imagem.png
```

Durante a inicialização, o programa lista os drivers de vídeo detectados pelo SDL e informa qual driver foi utilizado. Se nenhum driver gráfico real estiver disponível, a execução é abortada com orientações para usar `--nogui`.

Para depurar ausência de janelas:

- Assegure-se de executar **sem** `--nogui`.
- Verifique se o log não mostra fallback `dummy`; se mostrar, configure um driver suportado (`export SDL_VIDEODRIVER=x11`).
- Use `./bin/processador_imagens obj/gato1.bmp` como teste rápido (imagens de exemplo fornecidas).

### 2. Modo Headless (`--nogui`)

```bash
./bin/processador_imagens --nogui caminho/para/imagem.png
```

Saídas geradas automaticamente:

- `output_image.png`: resultado em escala de cinza (sobrescrito a cada execução, conforme o enunciado).
- Diretório `output_<nome-da-imagem>/` contendo:
  - `<nome>_grayscale.png`: imagem processada em escala de cinza.
  - `<nome>_histograma.csv`: contagens do histograma da imagem atual (após processamento).
  - `<nome>_stats.txt`: média, desvio padrão e respectivas classificações da imagem atual.
  - `<nome>_histograma.png`: renderização do histograma atual.
  - `<nome>_original_histograma.csv`: contagens do histograma da imagem original.
  - `<nome>_original_stats.txt`: média/desvio da imagem original.
  - `<nome>_original_histograma.png`: renderização comparativa da imagem original.

## Uso da Interface

1. **Abrir imagem**: botão "Abrir" ou argumento na linha de comando.
2. **Equalizar**: botão "Equalizar" alterna entre equalização e imagem original (texto muda para "Original").
3. **Salvar**: botão "Salvar" ou tecla `S` grava `output_image.png` na raiz; a GUI segue o mesmo fluxo do modo headless para fallbacks de formato.
4. **Histograma**: atualizado em tempo real; a janela exibe a curva atual sobreposta ao histograma original e mostra médias/desvios para ambos.

### Status do desenvolvimento da GUI

- ✅ Janela principal ajustada ao tamanho da imagem e centralizada.
- ✅ Janela secundária fixa com overlay do histograma original vs. processado.
- ✅ Botões `Abrir`, `Equalizar/Original`, `Salvar` operacionais e com feedback visual.
- ⏳ Pendências recomendadas: refinamento visual (legendas/cores), integração opcional com `tinyfiledialogs` e mais testes manuais.

### Conjunto de comandos de validação

```bash
# 1. Compilar do zero
make clean && make

# 2. Validar GUI (exige backend gráfico)
./bin/processador_imagens obj/gato1.bmp

# 3. Validar modo headless manualmente
./bin/processador_imagens --nogui obj/gato1.bmp
./bin/processador_imagens --nogui obj/1128858.bmp

# 4. Rodar suíte automática do modo headless
./scripts/test_nogui.sh
```

Os relatórios gerados permanecem em `output_<nome>/` para inspeção posterior.

## Logs e Diagnóstico

- A inicialização SDL mostra drivers disponíveis (ex.: `wayland`, `x11`, `kmsdrm`, `dummy`).
- Em caso de falha, o programa tenta um fallback `dummy` apenas para permitir `--nogui` e orienta o usuário.
- O pipeline de salvamento exibe mensagens claras caso PNG/BMP/JPG falhem.
- Script auxiliar `scripts/test_nogui.sh` valida automaticamente a geração de CSV/PNG/stats para as imagens de exemplo.

## Problemas Conhecidos / Dicas

- Se o texto não aparecer na GUI, assegure-se de ter fontes TTF padrão (`Liberation Sans` ou `Arial`).
- Em ambientes sem suporte gráfico, utilize `--nogui` ou exporte `SDL_VIDEODRIVER` para um driver suportado.
- Recrie os diretórios `output_<nome>` ao executar novos testes; evite versionar os resultados gerados automaticamente.
- A compilação Windows (`make windows`) é opcional: o Makefile detecta se MinGW + SDL3 estão disponíveis e, caso não estejam, apenas registra a ausência.

## Próximos Passos Sugeridos

- Adicionar filtros extras (borramento, nitidez, detecção de bordas).
- Automatizar testes com imagens de referência.
- Internacionalização da interface e mensagens.
- Empacotar build Windows com as DLLs necessárias quando o toolchain estiver pronto.

## Status Atual vs. Pendências

| Item | Situação | Observações |
|------|----------|-------------|
| Requisitos obrigatórios do enunciado | ✅ Concluídos | GUI, histograma, equalização, salvamento e modo CLI implementados |
| Documentação | ✅ Atualizada | README revisado com comandos e fluxo de testes |
| Testes automatizados | 🔄 Parcial | `scripts/test_nogui.sh` cobre modo headless; GUI ainda depende de teste manual |
| Build Windows | 🔄 Opcional | Makefile detecta ausência de MinGW/SDL3 e orienta instalação |
| Refinos finais | 🔄 Em aberto | Melhorias visuais na GUI e integração `tinyfiledialogs` ainda planejadas |

Boa avaliação e bons experimentos! :)
