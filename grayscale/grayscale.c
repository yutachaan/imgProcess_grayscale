#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGB;

void read_header(FILE *img, int *width, int *height);
void convert_img_to_gray(RGB rgb[], char filepath[], int img_size);
unsigned char convert_pixel_to_gray(RGB rgb[], int mode);

int main(int argc, char *argv[]) {
  FILE *img;    // 元画像
  int width;    // 画像の横幅
  int height;   // 画像の縦幅
  int img_size; // 画像のサイズ
  RGB *rgb;     // RGB画像用

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

  // ヘッダを読み取り、画像の横幅と縦幅を代入
  read_header(img, &width, &height);

  // 横幅と縦幅から画像のサイズを決定
  img_size = width * height;

  // RGB画像用の配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((rgb = (RGB *)malloc(sizeof(RGB) * img_size)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(rgb, sizeof(RGB), img_size, img);

  fclose(img);

  // カラー画像から赤を抽出して出力
  convert_img_to_gray(rgb, "red.pgm", img_size);

  // カラー画像から緑を抽出して出力
  convert_img_to_gray(rgb, "green.pgm", img_size);

  // カラー画像から青を抽出して出力
  convert_img_to_gray(rgb, "blue.pgm", img_size);

  // 赤・緑・青の平均値を求め、抽出
  convert_img_to_gray(rgb, "mean.pgm", img_size);

  // 輝度Yを求め、抽出
  convert_img_to_gray(rgb, "y.pgm", img_size);

  free(rgb);

  return 0;
}

// ヘッダの情報を読み取る(img: 読み込む画像のファイルポインタ, width: 画像の横幅保存用, height: 画像の縦幅保存用)
void read_header(FILE *img, int *width, int *height) {
  int i;
  char buf[256]; // ヘッダ読み込み時のバッファ

  while (i < 3) {
    // 1行読み取る
    fgets(buf, sizeof(buf), img);

    // コメントの場合読み飛ばす
    if (buf[0] == '#') continue;

    // 画像の横幅と縦幅を読み取る
    if (i == 1) sscanf(buf, "%d %d", width, height);

    // 次の行に移動
    i++;
  }
}

// 画像をグレイスケールに変換し出力(rgb: RGB画像のデータ, filepath: 保存する画像のパス)
void convert_img_to_gray(RGB rgb[], char filepath[], int img_size) {
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
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * img_size)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // モードを指定して画素ごとにグレイスケールに変換
  for (int i = 0; i < img_size; i++) {
    gray[i] = convert_pixel_to_gray(&rgb[i], mode);
  }

  // 画像データを書き込む
  fwrite(gray, sizeof(unsigned char), img_size, img_proc);

  free(gray);

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
