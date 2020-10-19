#include <stdio.h>

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

void jump_header(FILE *img);
void output_img(RGB rgb[], char filepath[]);
unsigned char convert_to_gray(RGB rgb[], int mode);

int main(int argc, char *argv[]) {
  FILE *img;      // 元画像
  RGB rgb[65536]; // RGB画像用の配列

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc < 2) {
    printf("画像ファイルのパスを指定してください。\n");
    return -1;
  }
  else if (argc > 2) {
    printf("指定できる画像ファイルは1つです。\n");
    return -1;
  }

  // 第一引数で指定された画像ファイルを読み込みモードで開く
  img = fopen(argv[1], "rb");

  // ファイルを開けなかった場合プログラムを終了
  if (img == NULL) {
    printf("ファイルが開けませんでした。\n");
    return -1;
  }

  // ヘッダの読み飛ばし
  jump_header(img);

  // 画像データの読み込み
  fread(rgb, sizeof(RGB), 65536, img);

  // 画像ファイルを閉じる
  fclose(img);

  // カラー画像から赤を抽出して出力
  output_img(rgb, "images/red.pgm");

  // カラー画像から緑を抽出して出力
  output_img(rgb, "images/green.pgm");

  // カラー画像から青を抽出して出力
  output_img(rgb, "images/blue.pgm");

  // 赤・緑・青の平均値を求め、抽出
  output_img(rgb, "images/mean.pgm");

  // 輝度Yを求め、抽出
  output_img(rgb, "images/y.pgm");

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
  int i;                     // イテレータ用
  static int mode;           // 変換するモードを管理
  FILE *img_proc;            // 変換後の画像
  unsigned char gray[65536]; // グレイスケール変換後の画像データ

  // モードを更新
  mode++;

  // 書き込むファイルを開く
  img_proc = fopen(filepath, "wb");

  // ファイルを開なかった場合プログラムを終了
  if (img_proc == NULL) {
    printf("ファイルが開けませんでした。\n");
    return;
  }

  // ヘッダを書き込む
  fputs("P5\n256 256\n255\n", img_proc);

  // モードを指定して画素ごとにグレイスケールに変換
  for (i=0; i<65536; i++) {
    gray[i] = convert_to_gray(&rgb[i], mode);
  }

  // 画像データを書き込む
  fwrite(gray, sizeof(unsigned char), 65536, img_proc);

  // 画像ファイルを閉じる
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

  // 画素データを返す
  return gray;
}
