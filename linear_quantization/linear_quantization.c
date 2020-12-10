#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height, int *max_grad);
void linear_quantization(unsigned char gray[], int width, int height, int max_grad);

int main(int argc, char *argv[]) {
  FILE *img;                   // 元画像
  int width, height, max_grad; // 画像の横幅, 縦幅, 最大階調値
  unsigned char *gray;         // グレイスケール画像データ

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 2) exit(1);

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);

  read_header(img, &width, &height, &max_grad);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img);

  fclose(img);

  linear_quantization(gray, width, height, max_grad);

  free(gray);

  return 0;
}

// ヘッダの情報を読み取る(img: 読み込む画像のファイルポインタ, width: 画像の横幅, height: 画像の縦幅, max_grad: 最大階調値)
void read_header(FILE *img, int *width, int *height, int *max_grad) {
  char buf[256];

  int i;
  while (i < 3) {
    fgets(buf, sizeof(buf), img);

    // コメントの場合読み飛ばす
    if (buf[0] == '#') continue;

    // 画像の横幅と縦幅を読み取る
    if (i == 1) sscanf(buf, "%d %d", width, height);

    // 画像の最大階調値を読み取る
    if (i == 2) sscanf(buf, "%d", max_grad);

    i++;
  }
}

// 線形量子化(gray: 元画像のデータ, width: 画像の幅, height: 画像の高さ, max_grad: 最大階調値)
void linear_quantization(unsigned char gray[], int width, int height, int max_grad) {
  FILE *img_quantize;                     // 線形量子化後の画像ファイル
  unsigned char quantize[width * height]; // 線形量子化後の画像データ
  int after_max_grad = 16;                // 線形量子化後の最大階調値

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_quantize = fopen("quantize_lenna.pgm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_quantize, "P5\n%d %d\n%d\n", width, height, after_max_grad);

  // 画像データを書き込む
  fwrite(quantize, sizeof(unsigned char), width * height, img_quantize);

  fclose(img_quantize);
}

