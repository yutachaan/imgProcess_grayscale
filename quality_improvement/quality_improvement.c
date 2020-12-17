#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

typedef struct {
  unsigned char h;
  unsigned char s;
  unsigned char v;
} HSV;

void read_header(FILE *img, int *width, int *height);
void improve_quality(RGB rgb[], unsigned char gray[], int width, int height);
void rgb2hsv(RGB rgb[], HSV hsv[]);
void hsv2rgb(HSV hsv[], RGB rgb[]);

int main(int argc, char *argv[]) {
  FILE *img_color;     // 低画質RGB画像
  FILE *img_high;      // グレイスケール画像
  int width, height;   // 画像の横幅、縦幅
  RGB *rgb;            // RGB画像用
  unsigned char *gray; // グレイスケール画像データ

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 3) exit(1);

  // 第一引数で指定された画像ファイル(低画質RGB画像)を開く(開けなかった場合プログラムを終了)
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
  FILE *img_color_high;         // 高画質RGB画像
  RGB rgb_high[width * height]; // 高画質RGB画像データ
  HSV hsv[width * height];      // HSV返還後の画像データ

  for (int i = 0; i < width * height; i++) {
    rgb2hsv(&rgb[i], &hsv[i]);      // 低画質RGB画像のデータをHSV画像に変換
    hsv[i].v = gray[i];             // 輝度は高画質グレイスケール画像のものを利用
    hsv2rgb(&hsv[i], &rgb_high[i]); // HSV画像のデータをRGB画像に変換して高画質化
  }

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_color_high = fopen("color_high.ppm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_color_high, "P6\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(rgb_high, sizeof(RGB), width * height, img_color_high);

  fclose(img_color_high);
}

// RGBをHSVに変換(rgb: RGB画像のデータ, hsv: HSV画像のデータ)
void rgb2hsv(RGB rgb[], HSV hsv[]) {
  // R, G, Bを0〜1の範囲に収める

  // Max, Minを求める

  // H, S, Vを求める
}

// HSVをRGBに変換(hsv: HSV画像のデータ, rgb: RGB画像のデータ)
void hsv2rgb(HSV hsv[], RGB rgb[]) {
  // H'を求める

  // fを求める

  // p, q, tを求める

  // R, G, Bを求める
}
