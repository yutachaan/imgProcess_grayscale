#include <stdio.h>
#include <stdlib.h>

#define WIDTH 256
#define HEIGHT 256

void skip_header(FILE *img);
int labeling(unsigned char gray[WIDTH][HEIGHT], unsigned char gray_lab[WIDTH][HEIGHT], int x, int y, int label);

int main(int argc, char *argv[]) {
  FILE *img;                         // 画像
  FILE *img_lab;                     // ラベリング処理後の画像
  unsigned char gray[WIDTH][HEIGHT]; // グレイスケール画像データ

  // 入力画像を読み込む
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);
  skip_header(img);
  fread(gray, sizeof(unsigned char), WIDTH * HEIGHT, img);
  fclose(img);

  // 画像を2値化
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      gray[i][j] = (gray[i][j] >= 128) ? 255 : 0;
    }
  }

  // ラベリング処理後の画像データの初期化
  unsigned char gray_lab[WIDTH][HEIGHT] = {0};

  // ラベリング処理
  int label = 1;
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      if (gray[i][j] == 0 && gray_lab[i][j] != 0) {
        labeling(gray, gray_lab, i, j, label);
        label++;
        printf("%d\n", label);
      }
    }
  }

  // ファイルに書き込む
  if ((img_lab = fopen("labeling.pgm", "wb")) == NULL) exit(1);
  fprintf(img_lab, "P5\n%d %d\n255\n", WIDTH, HEIGHT);
  fwrite(gray_lab, sizeof(unsigned char), WIDTH * HEIGHT, img_lab);
  fclose(img_lab);

  return 0;
}

// ヘッダをスキップ(img: 画像のファイルポインタ)
void skip_header(FILE *img) {
  char buf[256];
  int i;

  while (i < 3) {
    fgets(buf, sizeof(buf), img);
    if (buf[0] == '#') continue;
    i++;
  }
}

// ラベリング(gray: 2値化済みのグレイスケール画像のデータ, gray_lab: ラベリング後の画像データ, x: x座標, y: y座標, label: ラベル値)
int labeling(unsigned char gray[WIDTH][HEIGHT], unsigned char gray_lab[WIDTH][HEIGHT], int x, int y, int label) {
  if (gray_lab[x][y] != 0 || x < 0 || x > WIDTH || y < 0 || y > HEIGHT) return 1;
  gray_lab[x][y] = label;
  return (labeling(gray, gray_lab, x - 1, y - 1, label) + labeling(gray, gray_lab, x,     y - 1, label) +
          labeling(gray, gray_lab, x + 1, y - 1, label) + labeling(gray, gray_lab, x - 1, y,     label) +
          labeling(gray, gray_lab, x + 1, y,     label) + labeling(gray, gray_lab, x - 1, y + 1, label) +
          labeling(gray, gray_lab, x,     y + 1, label) + labeling(gray, gray_lab, x + 1, y + 1, label));
}
