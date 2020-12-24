#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void remove_distortion(unsigned char gray[], int width1, int height1, int width2, int height2);

int main(int argc, char *argv[]) {
  FILE *img1, *img2;                    // 画像
  int width1, height1, width2, height2; // 画像の横幅, 縦幅
  unsigned char *gray1;                 // グレイスケール画像データ

  // 入力画像を読み込む
  if ((img1 = fopen(argv[1], "rb")) == NULL) exit(1);
  read_header(img1, &width1, &height1);
  if ((gray1 = (unsigned char *)malloc(sizeof(unsigned char) * width1 * height1)) == NULL) exit(1);
  fread(gray1, sizeof(unsigned char), width1 * height1, img1);
  fclose(img1);

  // 目的画像を読み込む
  if ((img2 = fopen(argv[2], "rb")) == NULL) exit(1);
  read_header(img2, &width2, &height2);
  fclose(img2);

  // 幾何学的歪みの除去
  remove_distortion(gray1, width1, height1, width2, height2);

  free(gray1);

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

// 幾何学的歪みの除去(gray: 元画像データ, width1, height1: 入力画像の幅と高さ, width2, height2: 目的画像の幅と高さ)
void remove_distortion(unsigned char gray[], int width1, int height1, int width2, int height2) {
  FILE *img_after;                            // 歪み除去後の画像ファイル
  unsigned char gray_after[width2 * height2]; // 歪み除去後の画像データ

  // 初期化
  for (int i = 0; i < width2; i++) {
    for (int j = 0; j < height2; j++) {
      gray_after[j * width2 + i] = 0;
    }
  }

  // アフィン変換
  int x, y;
  for (int i = 0; i < width1; i++) {
    for (int j = 0; j < height1; j++) {
      x = (int)(0.610322056 * i - 0.080224435 * j + 121.1989055);
      y = (int)(0.124733194 * i + 0.601439888 * j + 15.21494566);
      gray_after[y * width2 + x] = gray[j * width1 + i];
    }
  }

  // ファイルに書き込む
  if ((img_after = fopen("affine.pgm", "wb")) == NULL) exit(1);
  fprintf(img_after, "P5\n%d %d\n255\n", width2, height2);
  fwrite(gray_after, sizeof(unsigned char), width2 * height2, img_after);
  fclose(img_after);
}
