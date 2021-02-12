#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 256
#define HEIGHT 256

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

void skip_header(FILE *img);
int labeling(unsigned char gray[][HEIGHT], unsigned char gray_lab[][HEIGHT], int x, int y, int label);
int labeling_color(unsigned char gray[][HEIGHT], RGB rgb_lab[][HEIGHT], int x, int y, int r, int g, int b);

int main(int argc, char *argv[]) {
  FILE *img;                             // 画像
  FILE *img_lab;                         // ラベリング処理後のグレイスケール画像
  FILE *img_lab_color;                   // ラベリング処理後のカラー画像
  unsigned char gray[WIDTH][HEIGHT];     // グレイスケール画像データ
  unsigned char gray_lab[WIDTH][HEIGHT]; // ラベリング処理後のグレイスケール画像データ
  RGB rgb_lab[WIDTH][HEIGHT];            // ラベリング処理後のカラー画像データ

  // 入力画像を読み込む
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);
  skip_header(img);
  fread(gray, sizeof(unsigned char), WIDTH * HEIGHT, img);
  fclose(img);

  // 画像を2値化&ラベリング処理後の画像データを初期化
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      gray[i][j] = (gray[i][j] >= 100) ? 255 : 0;
      gray_lab[i][j] = rgb_lab[i][j].r = rgb_lab[i][j].g = rgb_lab[i][j].b = 0;
    }
  }

  // ラベリング処理(グレイスケール画像)
  int label;
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      // 背景でなく，まだラベルが設定されていない場合
      if (gray[i][j] == 0 && gray_lab[i][j] == 0) {
        label+=5;
        labeling(gray, gray_lab, i, j, label);
      }
    }
  }

  // ラベリング処理(カラー画像)
  srand((unsigned int)time(NULL));
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      // 背景でなく，まだラベルが設定されていない場合
      if (gray[i][j] == 0 && rgb_lab[i][j].r == 0) {
        labeling_color(gray, rgb_lab, i, j, 1 + rand() % 255, 1 + rand() % 255, 1 + rand() % 255);
      }
    }
  }

  // ファイルに書き込む
  if ((img_lab = fopen("labeling.pgm", "wb")) == NULL) exit(1);
  fprintf(img_lab, "P5\n%d %d\n255\n", WIDTH, HEIGHT);
  fwrite(gray_lab, sizeof(unsigned char), WIDTH * HEIGHT, img_lab);
  fclose(img_lab);

  if ((img_lab_color = fopen("labeling_color.ppm", "wb")) == NULL) exit(1);
  fprintf(img_lab_color, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
  fwrite(rgb_lab, sizeof(RGB), WIDTH * HEIGHT, img_lab_color);
  fclose(img_lab_color);

  return 0;
}

// ヘッダーをスキップ(img: 画像のファイルポインタ)
void skip_header(FILE *img) {
  char buf[256];

  int i = 0;
  while (i < 3) {
    fgets(buf, sizeof(buf), img);
    if (buf[0] == '#') continue;
    i++;
  }
}

// ラベリング(gray: 2値化済みのグレイスケール画像のデータ, gray_lab: ラベリング後の画像データ, x, y: 座標, label: ラベル値)
int labeling(unsigned char gray[][HEIGHT], unsigned char gray_lab[][HEIGHT], int x, int y, int label) {
  // 枠外の場合，背景の場合，既にラベルが設定されている場合は終了
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || gray[x][y] == 255 || gray_lab[x][y] != 0) return 1;

  gray_lab[x][y] = label;

  // 8近傍に対して再帰
  return (labeling(gray, gray_lab, x - 1, y - 1, label) + labeling(gray, gray_lab, x,     y - 1, label) +
          labeling(gray, gray_lab, x + 1, y - 1, label) + labeling(gray, gray_lab, x - 1, y,     label) +
          labeling(gray, gray_lab, x + 1, y,     label) + labeling(gray, gray_lab, x - 1, y + 1, label) +
          labeling(gray, gray_lab, x,     y + 1, label) + labeling(gray, gray_lab, x + 1, y + 1, label));
}

// カラーラベリング(gray: 2値化済みのグレイスケール画像のデータ, gray_lab: ラベリング後の画像データ, x, y: 座標, r, g, b: ラベル値)
int labeling_color(unsigned char gray[][HEIGHT], RGB rgb_lab[][HEIGHT], int x, int y, int r, int g, int b) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || gray[x][y] == 255 || rgb_lab[x][y].r != 0) return 1;

  rgb_lab[x][y].r = r;
  rgb_lab[x][y].g = g;
  rgb_lab[x][y].b = b;

  return (labeling_color(gray, rgb_lab, x - 1, y - 1, r, g, b) + labeling_color(gray, rgb_lab, x,     y - 1, r, g, b) +
          labeling_color(gray, rgb_lab, x + 1, y - 1, r, g, b) + labeling_color(gray, rgb_lab, x - 1, y,     r, g, b) +
          labeling_color(gray, rgb_lab, x + 1, y,     r, g, b) + labeling_color(gray, rgb_lab, x - 1, y + 1, r, g, b) +
          labeling_color(gray, rgb_lab, x,     y + 1, r, g, b) + labeling_color(gray, rgb_lab, x + 1, y + 1, r, g, b));
}
