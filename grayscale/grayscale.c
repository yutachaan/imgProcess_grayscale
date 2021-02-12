#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

void read_header(FILE *img, int *width, int *height);
void convert_img_to_gray(RGB rgb[], char filepath[], int width, int height);
unsigned char convert_pixel_to_gray(RGB rgb[], int mode);

int main(int argc, char *argv[]) {
  FILE *img;         // 元画像
  int width, height; // 画像の横幅，縦幅
  RGB *rgb;          // RGB画像用

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 2) exit(1);

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);

  read_header(img, &width, &height);

  // RGB画像用の配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((rgb = (RGB *)malloc(sizeof(RGB) * width * height)) == NULL) exit(1);

  // 画像データの読み込み
  fread(rgb, sizeof(RGB), width * height, img);

  fclose(img);

  // カラー画像から赤を抽出して出力
  convert_img_to_gray(rgb, "red.pgm", width, height);

  // カラー画像から緑を抽出して出力
  convert_img_to_gray(rgb, "green.pgm", width, height);

  // カラー画像から青を抽出して出力
  convert_img_to_gray(rgb, "blue.pgm", width, height);

  // 赤・緑・青の平均値を求め，抽出
  convert_img_to_gray(rgb, "mean.pgm", width, height);

  // 輝度Yを求め，抽出
  convert_img_to_gray(rgb, "y.pgm", width, height);

  free(rgb);

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

// 画像をグレイスケールに変換し出力(rgb: RGB画像のデータ, filepath: 保存する画像のパス, width: 画像の横幅, height: 画像の高さ)
void convert_img_to_gray(RGB rgb[], char filepath[], int width, int height) {
  FILE *img_proc;                     // 変換後の画像
  unsigned char gray[width * height]; // グレイスケール変換後の画像データ
  static int mode;                    // 変換するモード

  // モードを更新
  mode++;

  // モードを指定して画素ごとにグレイスケールに変換
  for (int i = 0; i < width * height; i++) {
    gray[i] = convert_pixel_to_gray(&rgb[i], mode);
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_proc = fopen(filepath, "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_proc, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(gray, sizeof(unsigned char), width * height, img_proc);

  fclose(img_proc);
}

// 画素をグレイスケールに変換(rgb: 変換する画素, mode: 変換するモード)
unsigned char convert_pixel_to_gray(RGB rgb[], int mode) {
  unsigned char gray; // グレイスケール変換後の画素データ

  switch (mode) {
    case 1:
      // 赤だけを抽出
      gray = rgb->r;
      break;
    case 2:
      // 緑だけを抽出
      gray = rgb->g;
      break;
    case 3:
      // 青だけを抽出
      gray = rgb->b;
      break;
    case 4:
      // 赤・緑・青の平均値を求める
      gray = (rgb->r + rgb->g + rgb->b) / 3;
      break;
    case 5:
      // 輝度Yを求める
      gray = 0.299 * rgb->r + 0.587 * rgb->g + 0.114 * rgb->b;
      break;
    default:
      break;
  }

  return gray;
}
