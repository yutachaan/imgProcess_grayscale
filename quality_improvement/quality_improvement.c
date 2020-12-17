#include <stdio.h>

#include <stdlib.h>

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

void read_header(FILE *img, int *width, int *height);

int main(int argc, char *argv[]) {
  FILE *img_color;     // 低画質RGB画像
  FILE *img_high;      // グレイスケール画像
  int width, height;   // 画像の横幅、縦幅
  RGB *rgb;            // RGB画像用
  unsigned char *gray; // グレイスケール画像データ

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 3) exit(1);

  // 第一引数で指定された画像ファイル(低画質カラー画像)を開く(開けなかった場合プログラムを終了)
  if ((img_color = fopen(argv[1], "rb")) == NULL) exit(1);

  read_header(img_color, &width, &height);

  // RGB画像用の配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((rgb = (RGB *)malloc(sizeof(RGB) * width * height)) == NULL) exit(1);

  // 低画質RGB画像データの読み込み
  fread(rgb, sizeof(RGB), width * height, img_color);

  fclose(img_color);

  // 第二引数で指定された画像ファイル(グレイスケール画像)を開く(開けなかった場合プログラムを終了)
  if ((img_high = fopen(argv[2], "rb")) == NULL) exit(1);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img_high);

  free(rgb);

  free(gray);

  return 0;
}

// ヘッダの情報を読み取る(img: 読み込む画像のファイルポインタ, width: 画像の横幅, height: 画像の縦幅)
void read_header(FILE *img, int *width, int *height) {
  char buf[256];

  int i;
  while (i < 3) {
    fgets(buf, sizeof(buf), img);

    // コメントの場合読み飛ばす
    if (buf[0] == '#') continue;

    // 画像の横幅と縦幅を読み取る
    if (i == 1) sscanf(buf, "%d %d", width, height);

    i++;
  }
}
