/*
参考: 京都大学OCW「出力処理」
https://ocw.kyoto-u.ac.jp/ja/09-faculty-of-engineering-jp/image-processing/pdf/dip_04.pdf
(閲覧日: 2020-10-28)
*/

#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void binarize(unsigned char gray[], int width, int height);
void dither(unsigned char gray[], int width, int height);

int main(int argc, char *argv[]) {
  FILE *img;           // 元画像
  int width, height;   // 画像の横幅, 縦幅
  unsigned char *gray; // グレイスケール画像データ

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 2) exit(1);

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);

  read_header(img, &width, &height);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img);

  fclose(img);

  binarize(gray, width, height);

  dither(gray, width, height);

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

// 固定しきい値法を用いて画像を2値化(gray: 元画像のデータ, width: 画像幅, height: 画像高さ)
void binarize(unsigned char gray[], int width, int height) {
  FILE *img_bin;                     // 2値化後の画像
  unsigned char bin[width * height]; // 2値化後の画像データ

  // 固定しきい値法
  for (int i = 0; i < width * height; i++) {
    bin[i] = (gray[i] >= 128) ? 255 : 0;
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_bin = fopen("binarize.pgm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_bin, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(bin, sizeof(unsigned char), width * height, img_bin);

  fclose(img_bin);
}

// Bayerマトリクスを用いてディザ画像を生成(gray: 元画像のデータ, width: 画像幅, height: 画像高さ)
void dither(unsigned char gray[], int width, int height) {
  FILE *img_dither;                         // ディザ画像
  unsigned char dither[width * height];     // ディザ画像データ
  int bayer[4][4] = {{ 15, 135,  45, 165},  //Bayerマトリクス
                     {195,  75, 225, 105},
                     { 60, 180,  30, 150},
                     {240, 120, 210,  90}};

  // 組織的ディザ法
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      dither[j * width + i] = (gray[j * width + i] >= bayer[i % 4][j % 4]) ? 255 : 0;
    }
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_dither = fopen("dither.pgm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_dither, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(dither, sizeof(unsigned char), width * height, img_dither);

  fclose(img_dither);
}
