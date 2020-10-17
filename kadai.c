#include <stdio.h>

// RGBの構造体型
typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} RGB;

void read_header(FILE *image); // ヘッダの読み飛ばし
void pick_red(RGB *rgb);       // カラー画像の赤だけを取り出し出力
void pick_green(RGB *rgb);     // カラー画像の緑だけを取り出し出力
void pick_blue(RGB *rgb);      // カラー画像の青だけを取り出し出力

int main(void) {
  FILE *image;                // ファイルポインタ
  RGB rgb[65536];             // RGB画像用の配列

  // 画像データを開く
  image = fopen("../../imgdata/pepper.ppm", "rb");

  // ファイルを開けなかった場合プログラムを終了
  if (image == NULL) {
    printf("file load error\n");
    return -1;
  }

  // ヘッダの読み飛ばし
  read_header(image);

  // 画像データの読み込み
  fread(rgb, sizeof(RGB), 65536, image);

  // 画像ファイルを閉じる
  fclose(image);

  // カラー画像の赤だけを取り出し、出力
  pick_red(&rgb);

  // カラー画像の緑だけを取り出し、出力
  pick_green(&rgb);

  // カラー画像の青だけを取り出し、出力
  pick_blue(&rgb);

  return 0;
}

void read_header(FILE *image) {
  char buf[256];  // ヘッダ読み込み時のバッファ
  int count = 3;  // コメントなしの場合のヘッダの行数

  // ヘッダの行数が0になるまで読み飛ばす
  while (count != 0) {
    fgets(buf, sizeof(buf), image);
    if (buf[0] != '#') count -= 1;   // 読み込んだ行がコメントではない場合countを減らす
  }
}

void pick_red(RGB *rgb) {
  FILE *image_red;
  int i;

  // 書き込むファイルを開く
  image_red = fopen("images/red.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_red == NULL) {
    printf("file load error\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n", image_red);       // 形式
  fputs("256 256\n", image_red);  // 画像サイズ
  fputs("255\n", image_red);      // 最大輝度値

  // 画像データを書き込む
  for (i=0; i<65536; i++) {
    fwrite(&rgb[i].red, sizeof(unsigned char), 1, image_red);
  }

  // 画像ファイルを閉じる
  fclose(image_red);
}

void pick_green(RGB *rgb) {
  FILE *image_green;
  int i;

  // 書き込むファイルを開く
  image_green = fopen("images/green.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_green == NULL) {
    printf("file load error\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n", image_green);       // 形式
  fputs("256 256\n", image_green);  // 画像サイズ
  fputs("255\n", image_green);      // 最大輝度値

  // 画像データを書き込む
  for (i=0; i<65536; i++) {
    fwrite(&rgb[i].green, sizeof(unsigned char), 1, image_green);
  }

  // 画像ファイルを閉じる
  fclose(image_green);
}

void pick_blue(RGB *rgb) {
  FILE *image_blue;
  int i;

  // 書き込むファイルを開く
  image_blue = fopen("images/blue.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_blue == NULL) {
    printf("file load error\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n", image_blue);       // 形式
  fputs("256 256\n", image_blue);  // 画像サイズ
  fputs("255\n", image_blue);      // 最大輝度値

  // 画像データを書き込む
  for (i=0; i<65536; i++) {
    fwrite(&rgb[i].blue, sizeof(unsigned char), 1, image_blue);
  }

  // 画像ファイルを閉じる
  fclose(image_blue);
}
