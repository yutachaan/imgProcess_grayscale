#include <stdio.h>

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} RGB;

void read_header(FILE *fp);

int main(void) {
  FILE *image;                    /* ファイルポインタ */
  RGB rgb[65536];                 /* RGB画像用の配列 */
  unsigned char gray[65536];      /* グレースケール画像用の配列 */
  char buf[256];

  /* 画像データを開く */
  image = fopen("../../imgdata/pepper.ppm", "rb");

  /* 中身がない場合はプログラムを終了 */
  if (image == NULL) {
    printf("file load error");
    return -1;
  }

  /* ヘッダ部の読み込み */
  read_header(image);

  /* 画像データの読み込み */
  fread(rgb, sizeof(RGB), 65536, image);

  /* 画像ファイルを閉じる */
  fclose(image);

  return 0;
}

/* ヘッダの読み飛ばし */
void read_header(FILE *fp) {
  char buf[256];                   /* ヘッダ読み込み時のバッファ */
  int count = 3;                   /* コメントなしの場合のヘッダの行数 */

  /* ヘッダの行数が0になるまで読み飛ばす */
  while (count != 0) {
    fgets(buf, sizeof(buf), fp);
    if (buf[0] != '#') count -= 1; /* 読み込んだ行がコメントではない場合countを減らす */
  }
}
