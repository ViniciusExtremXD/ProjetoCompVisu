#!/usr/bin/env bash
set -euo pipefail

# #########################################
#            SCRIPT: VALIDAR MODO --NOGUI
# #########################################

IMAGES=("obj/gato1.bmp" "obj/1128858.bmp")
BIN="./bin/processador_imagens"

if [[ ! -x "$BIN" ]]; then
  echo "[ERRO] Binário $BIN não encontrado. Execute 'make' antes." >&2
  exit 1
fi

status=0
for img in "${IMAGES[@]}"; do
  if [[ ! -f "$img" ]]; then
    echo "[AVISO] Imagem de teste '$img' inexistente. Pule este arquivo." >&2
    status=1
    continue
  fi
  name=$(basename "$img")
  base="${name%.*}"
  echo "[TESTE] Processando $img"
  if ! "$BIN" --nogui "$img" >"output_${base}/test.log" 2>&1; then
    echo "[ERRO] Execução falhou para $img (veja output_${base}/test.log)." >&2
    status=1
    continue
  fi
  for f in "output_${base}/${base}_histograma.csv" \
           "output_${base}/${base}_stats.txt" \
           "output_${base}/${base}_histograma.png" \
           "output_${base}/${base}_original_histograma.csv" \
           "output_${base}/${base}_original_stats.txt" \
           "output_${base}/${base}_original_histograma.png" \
           "output_${base}/${base}_grayscale.png"; do
    if [[ ! -s "$f" ]]; then
      echo "[ERRO] Arquivo esperado '$f' não foi gerado ou está vazio." >&2
      status=1
    fi
  done
  echo "[OK] Saídas geradas para $img"
  rm -f "output_${base}/test.log"
  # Mantemos os artefatos para inspeção manual
  touch "output_${base}/.validated"
  echo "" 
done
exit $status
