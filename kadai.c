/* TODO:
   - output関係の関数の動作をまとめる
   - mallocで配列を確保するようにする
   - 指定された画像を変換するようにする
   - 画像サイズをファイルから取得する用にする
*/

#include <stdio.h>

typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} RGB;

void read_header(FILE *image);                        // ヘッダの読み飛ばし
void output_red(RGB rgb[]);                           // カラー画像の赤だけを取り出し出力
void output_green(RGB rgb[]);                         // カラー画像の緑だけを取り出し出力
void output_blue(RGB rgb[]);                          // カラー画像の青だけを取り出し出力
void output_mean(RGB rgb[]);                          // 赤・緑・青の平均を求め出力
void output_Y(RGB rgb[]);                             // 輝度Yを求め出力
unsigned char convert_to_gray(RGB rgb[], int mode);   // グレイスケールに変換

int main(void) {
  FILE *image;    // ファイルポインタ
  RGB rgb[65536]; // RGB画像用の配列

  // 画像データを開く
  image = fopen("../../imgdata/pepper.ppm", "rb");

  // ファイルを開けなかった場合プログラムを終了
  if (image == NULL) {
    printf("ファイルが開けませんでした。\n");
    return -1;
  }

  // ヘッダの読み飛ばし
  read_header(image);

  // 画像データの読み込み
  fread(rgb, sizeof(RGB), 65536, image);

  // 画像ファイルを閉じる
  fclose(image);

  // カラー画像の赤だけを取り出し、出力
  output_red(&rgb);

  // カラー画像の緑だけを取り出し、出力
  output_green(&rgb);

  // カラー画像の青だけを取り出し、出力
  output_blue(&rgb);

  // 赤・緑。青の平均を求め、出力
  output_mean(&rgb);

  // 輝度Yを求め、出力
  output_Y(&rgb);

  return 0;
}

void read_header(FILE *image) {
  char buf[256];       // ヘッダ読み込み時のバッファ
  int count_line = 3;  // コメントなしの場合のヘッダの行数

  // ヘッダの行数が0になるまで読み飛ばす
  while (count_line != 0) {
    fgets(buf, sizeof(buf), image);
    if (buf[0] != '#') count_line -= 1; // 読み込んだ行がコメントではない場合countを減らす
  }
}

void output_red(RGB rgb[]) {
  int i;
  FILE *image_red;           // ファイルポインタ
  unsigned char gray[65536]; // グレイスケール変換後のデータの保存

  // 書き込むファイルを開く
  image_red = fopen("images/red.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_red == NULL) {
    printf("ファイルが開けませんでした。\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", image_red);

  // グレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], 0);
  }

  // 画像データを書き出す
  fwrite(gray, sizeof(unsigned char), 65536, image_red);

  // 画像ファイルを閉じる
  fclose(image_red);
}

void output_green(RGB rgb[]) {
  int i;
  FILE *image_green;         // ファイルポインタ
  unsigned char gray[65536]; // グレイスケール変換後のデータの保存

  // 書き込むファイルを開く
  image_green = fopen("images/green.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_green == NULL) {
    printf("ファイルが開けませんでした。\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", image_green);

  // グレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], 1);
  }

  // 画像データを書き出す
  fwrite(gray, sizeof(unsigned char), 65536, image_green);

  // 画像ファイルを閉じる
  fclose(image_green);
}

void output_blue(RGB rgb[]) {
  int i;
  FILE *image_blue;          // ファイルポインタ
  unsigned char gray[65536]; // グレイスケール変換後のデータの保存

  // 書き込むファイルを開く
  image_blue = fopen("images/blue.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_blue == NULL) {
    printf("ファイルが開ませんでした。\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", image_blue);

  // グレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], 2);
  }

  // 画像データを書き出す
  fwrite(gray, sizeof(unsigned char), 65536, image_blue);

  // 画像ファイルを閉じる
  fclose(image_blue);
}

void output_mean(RGB rgb[]) {
  int i;
  FILE *image_mean;          // ファイルポインタ
  unsigned char gray[65536]; // グレイスケール変換後のデータの保存

  // 書き込むファイルを開く
  image_mean = fopen("images/mean.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_mean == NULL) {
    printf("ファイルが開ませんでした。\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", image_mean);

  // グレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], 3);
  }

  // 画像データを書き出す
  fwrite(gray, sizeof(unsigned char), 65536, image_mean);

  // 画像ファイルを閉じる
  fclose(image_mean);
}

void output_Y(RGB rgb[]) {
  int i;
  FILE *image_Y;             // ファイルポインタ
  unsigned char gray[65536]; // グレイスケール変換後のデータの保存

  // 書き込むファイルを開く
  image_Y = fopen("images/y.pgm", "wb");

  // ファイルを開けなかった場合プログラムを終了
  if (image_Y == NULL) {
    printf("ファイルが開ませんでした。\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", image_Y);

  // グレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], 4);
  }

  // 画像データを書き出す
  fwrite(gray, sizeof(unsigned char), 65536, image_Y);

  // 画像ファイルを閉じる
  fclose(image_Y);
}

unsigned char convert_to_gray(RGB rgb[], int mode) {
  unsigned char gray; // グレイスケール変換後の画素のデータ

  switch (mode) {
    case 0:
      // RGBの赤だけを抽出
      gray = rgb->red;
      break;
    case 1:
      // RGBの緑だけを抽出
      gray = rgb->green;
      break;
    case 2:
      // RGBの青だけを抽出
      gray = rgb->blue;
      break;
    case 3:
      // RGBのそれぞれの値の平均値を求める
      gray = (rgb->red + rgb->green + rgb->blue) / 3;
      break;
    case 4:
      // 輝度Yを求める
      gray = 0.299 * rgb->red + 0.587 * rgb->green + 0.114 * rgb->blue;
    default:
      break;
  }

  return gray;
}
