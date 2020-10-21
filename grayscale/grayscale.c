#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

void jump_header(FILE *img);
void output_img(RGB rgb[], char filepath[]);
unsigned char convert_to_gray(RGB rgb[], int mode);

int main(int argc, char *argv[]) {
  FILE *img; // 元画像
  RGB *rgb;  // RGB画像用

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc < 2) {
    printf("画像ファイルのパスを指定してください。\n");
    exit(1);
  }
  else if (argc > 2) {
    printf("指定できる画像ファイルは1つです。\n");
    exit(1);
  }

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  jump_header(img);

  // RGB画像用の配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((rgb = (RGB *)malloc(sizeof(RGB) * 65536)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(rgb, sizeof(RGB), 65536, img);

  fclose(img);

  // カラー画像から赤を抽出して出力
  output_img(rgb, "red.pgm");

  // カラー画像から緑を抽出して出力
  output_img(rgb, "green.pgm");

  // カラー画像から青を抽出して出力
  output_img(rgb, "blue.pgm");

  // 赤・緑・青の平均値を求め、抽出
  output_img(rgb, "mean.pgm");

  // 輝度Yを求め、抽出
  output_img(rgb, "y.pgm");

  free(rgb);

  return 0;
}

// ヘッダを読み飛ばす(img: 読み飛ばす画像のファイルポインタ)
void jump_header(FILE *img) {
  char buf[256];       // ヘッダ読み込み時のバッファ
  int count_line = 3;  // コメントなしの場合のヘッダの行数

  // ヘッダの行数が0になるまで読み飛ばす
  while (count_line != 0) {
    fgets(buf, sizeof(buf), img);       // 1行読み飛ばす
    if (buf[0] != '#') count_line -= 1; // 読み込んだ行がコメントではない場合countを減らす
  }
}

// 画像をグレイスケールに変換し出力(rgb: RGB画像のデータ, filepath: 保存する画像のパス)
void output_img(RGB rgb[], char filepath[]) {
  int i;
  static int mode;     // 変換するモードを管理
  FILE *img_proc;      // 変換後の画像
  unsigned char *gray; // グレイスケール変換後の画像データ

  // モードを更新
  mode++;

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_proc = fopen(filepath, "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", img_proc);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * 65536)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // モードを指定して画素ごとにグレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], mode);
  }

  // 画像データを書き込む
  fwrite(gray, sizeof(unsigned char), 65536, img_proc);

  free(gray);

  fclose(img_proc);
}

// 画素をグレイスケールに変換(rgb: 変換する画素, mode: 変換するモード)
unsigned char convert_to_gray(RGB rgb[], int mode) {
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
