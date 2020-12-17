#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

#define MAX_DEPTH 255

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

typedef struct {
  double h;
  double s;
  double v;
} HSV;

void read_header(FILE *img, int *width, int *height);
void improve_quality(RGB rgb[], unsigned char gray[], int width, int height);
void rgb2hsv(RGB rgb, HSV hsv[]);
void hsv2rgb(HSV hsv, RGB rgb[]);

int main(int argc, char *argv[]) {
  FILE *img_color;     // 低画質RGB画像
  FILE *img_high;      // グレイスケール画像
  RGB *rgb;            // RGB画像データ
  unsigned char *gray; // グレイスケール画像データ
  int width, height;   // 画像の横幅、縦幅

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 3) exit(1);

  // 第一引数で指定された低画質RGB画像を開く
  if ((img_color = fopen(argv[1], "rb")) == NULL) exit(1);

  read_header(img_color, &width, &height);

  // RGB画像用の配列を動的に確保
  if ((rgb = (RGB *)malloc(sizeof(RGB) * width * height)) == NULL) exit(1);

  // 低画質RGB画像データの読み込み
  fread(rgb, sizeof(RGB), width * height, img_color);

  fclose(img_color);

  // 第二引数で指定されたグレイスケール画像を開く
  if ((img_high = fopen(argv[2], "rb")) == NULL) exit(1);

  read_header(img_color, &width, &height);

  // グレイスケール画像用の配列を動的に確保
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);

  // グレイスケール画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img_high);

  fclose(img_high);

  improve_quality(rgb, gray, width, height);

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

// HSVによる画質の改善(rgb: RGB画像のデータ, gray: グレイスケール画像のデータ, width: 画像の幅, height: 画像の高さ)
void improve_quality(RGB rgb[], unsigned char gray[], int width, int height){
  FILE *img_color_high;    // 高画質RGB画像
  HSV hsv[width * height]; // HSV変換後の画像データ

  for (int i = 0; i < width * height; i++) {
    rgb2hsv(rgb[i], &hsv[i]);               // 低画質RGB画像のデータをHSV画像に変換
    hsv[i].v = gray[i] / (double)MAX_DEPTH; // 輝度は高画質グレイスケール画像のものを利用
    hsv2rgb(hsv[i], &rgb[i]);               // HSV画像のデータをRGB画像に変換して高画質化
  }

  // 書き込み
  if ((img_color_high = fopen("color_high.ppm", "wb")) == NULL) exit(1);
  fprintf(img_color_high, "P6\n%d %d\n255\n", width, height);
  fwrite(rgb, sizeof(RGB), width * height, img_color_high);
  fclose(img_color_high);
}

// RGBをHSVに変換(rgb: RGB画像のデータ, hsv: HSV画像のデータ)
void rgb2hsv(RGB rgb, HSV hsv[]) {
  // R, G, Bを0〜1の範囲に変換
  double r = rgb.r / (double)MAX_DEPTH;
  double g = rgb.g / (double)MAX_DEPTH;
  double b = rgb.b / (double)MAX_DEPTH;

  double max = MAX(r, MAX(g, b));
  double min = MIN(r, MIN(g, b));

  // Hを求める
  if (max == min) {
    hsv->h = DBL_MAX;
  }
  else if (max == r) {
    hsv->h = 60 * (g - b) / (max - min);
  }
  else if (max == g) {
    hsv->h = 60 * (b - r) / (max - min) + 120;
  }
  else {
    hsv->h = 60 * (r - g) / (max - min) + 240;
  }

  if (hsv->h < 0) {
    hsv->h += 360;
  }

  // Sを求める
  if (max == 0) {
    hsv->s = 0;
  }
  else {
    hsv->s = (max - min) / max;
  }
}

// HSVをRGBに変換(hsv: HSV画像のデータ, rgb: RGB画像のデータ)
void hsv2rgb(HSV hsv, RGB rgb[]) {
  double h2, f, p, q, t, r, g, b;

  // H', f, p, q, tを求める
  h2 = ((int)hsv.h / 60) % 6;
  f = (hsv.h / 60) - h2;
  p = hsv.v * (1 - hsv.s);
  q = hsv.v * (1 - hsv.s * f);
  t = hsv.v * (1 - (1 - f) * hsv.s);

  // R, G, Bを求める
  switch ((int)h2) {
    case 0:
      r = hsv.v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = hsv.v;
      b = p;
      break;
    case 2:
      r = p;
      g = hsv.v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = hsv.v;
      break;
    case 4:
      r = t;
      g = p;
      b = hsv.v;
      break;
    case 5:
      r = hsv.v;
      g = p;
      b = q;
      break;
    default:
      r = g = b = hsv.v;
      break;
  }

  // 最大階調値倍して代入
  rgb->r = (unsigned char)(r * MAX_DEPTH);
  rgb->g = (unsigned char)(g * MAX_DEPTH);
  rgb->b = (unsigned char)(b * MAX_DEPTH);
}
