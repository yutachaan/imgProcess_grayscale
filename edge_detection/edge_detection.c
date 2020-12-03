#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void sobel_filter(unsigned char gray[], int width, int height);
void laplacian_filter(unsigned char gray[], int width, int height);

int main(int argc, char *argv[]) {
  FILE *img;              // 元画像
  int width, height;      // 画像の横幅, 縦幅
  unsigned char *gray;    // グレイスケール画像データ

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

  sobel_filter(gray, width, height);

  laplacian_filter(gray, width, height);

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

// Sobelフィルタ(gray: 元画像のデータ, width: 画像の幅, height: 画像の高さ)
void sobel_filter(unsigned char gray[], int width, int height) {
  FILE *img_sobel;                     // Sobelフィルタ適用後の画像ファイル
  unsigned char sobel[width * height]; // Sobelフィルタ適用後の画像データ

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_sobel = fopen("sobel.pgm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_sobel, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(sobel, sizeof(unsigned char), width * height, img_sobel);

  fclose(img_sobel);
}

// Laplacianフィルタ(gray: 元画像のデータ, width: 画像の幅, height: 画像の高さ)
void laplacian_filter(unsigned char gray[], int width, int height) {
  FILE *img_lap;                     // Laplacianフィルタ適用後の画像ファイル
  unsigned char lap[width * height]; // Laplacianフィルタ適用後の画像データ

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_lap = fopen("lap.pgm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_lap, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(lap, sizeof(unsigned char), width * height, img_lap);

  fclose(img_lap);
}
