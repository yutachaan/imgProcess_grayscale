#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void read_header(FILE *img, int *width, int *height);
void sobel_filter(unsigned char gray[], int width, int height);
void laplacian_filter(unsigned char gray[], int width, int height);

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

  // 元画像に水平方向と垂直のフィルタをそれぞれ適用(端以外)
  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 1; j++) {
      // 注目画素の座標を求める
      int pos = j * width + i;

      // 水平方向のフィルタを適用後の近傍画素の値を求める
      int neighbor[9];
      neighbor[1] = neighbor[4] = neighbor[7] = 0;
      neighbor[0] = gray[pos - width - 1] * (-1);
      neighbor[2] = gray[pos - width + 1];
      neighbor[3] = gray[pos - 1] * (-2);
      neighbor[5] = gray[pos + 1] * 2;
      neighbor[6] = gray[pos + width - 1] * (-1);
      neighbor[8] = gray[pos + width + 1];

      // 水平方向のフィルタ適用後の近傍画素の合計値を求める
      int sum_h = 0;
      for (int k = 0; k < 9; k++) sum_h += neighbor[k];

      // 垂直方向のフィルタを適用後の近傍画素の値を求める
      neighbor[3] = neighbor[4] = neighbor[5] = 0;
      neighbor[0] = gray[pos - width - 1] * (-1);
      neighbor[1] = gray[pos - width] * (-2);
      neighbor[2] = gray[pos - width + 1] * (-1);
      neighbor[6] = gray[pos + width - 1];
      neighbor[7] = gray[pos + width] * 2;
      neighbor[8] = gray[pos + width + 1];

      // 垂直方向のフィルタ適用後の近傍画素の合計値を求める
      int sum_v = 0;
      for (int k = 0; k < 9; k++) sum_v += neighbor[k];

      // Sabelフィルタを適用(255より大きい場合は255にする)
      if ((int)sqrt(pow(sum_h, 2) + pow(sum_v, 2)) > 255) sobel[pos] = 255;
      else sobel[pos] = (int)sqrt(pow(sum_h, 2) + pow(sum_v, 2));
    }
  }

  // ミラーリング
  for (int i = 0; i < width; i++) {
    int pos = (height - 1) * width + i;
    if (i == 0) {
      sobel[i] = sobel[i + width + 1];
      sobel[pos] = sobel[pos - width + 1];
    }
    else if (i == width - 1) {
      sobel[i] = sobel[i + width - 1];
      sobel[pos] = sobel[pos - width - 1];
    }
    else {
      sobel[i] = sobel[i + width];
      sobel[pos] = sobel[pos - width];
    }
  }
  for (int j = 1; j < height - 1; j++) {
    sobel[j * width] = sobel[j * width + 1];
    sobel[j * width + width - 1] = sobel[j * width + width - 2];
  }

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_sobel = fopen("sobel_airplane.pgm", "wb")) == NULL) exit(1);

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

  // 元画像にラプラシアンフィルタを適用
  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 1; j++) {
      // 注目画素の座標を求める
      int pos = j * width + i;

      // ラプラシアンフィルタ適用後の近傍画素の値を求める
      int neighbor[9];
      neighbor[0] = neighbor[2] = neighbor[6] = neighbor[8] = 0;
      neighbor[1] = gray[pos - width];
      neighbor[3] = gray[pos - 1];
      neighbor[4] = gray[pos] * (-4);
      neighbor[5] = gray[pos + 1];
      neighbor[7] = gray[pos + width];

      // ラプラシアンフィルタ適用後の近傍画素の合計値を求める
      int sum_lap = 0;
      for (int k = 0; k < 9; k++) sum_lap += neighbor[k];

      // 合計値の絶対値を新しい値とする(255より大きい場合は255にする)
      if (abs(sum_lap) > 255) lap[pos] = 255;
      else lap[pos] = abs(sum_lap);
    }
  }

  // ミラーリング
  for (int i = 0; i < width; i++) {
    int pos = (height - 1) * width + i;
    if (i == 0) {
      lap[i] = lap[i + width + 1];
      lap[pos] = lap[pos - width + 1];
    }
    else if (i == width - 1) {
      lap[i] = lap[i + width - 1];
      lap[pos] = lap[pos - width - 1];
    }
    else {
      lap[i] = lap[i + width];
      lap[pos] = lap[pos - width];
    }
  }
  for (int j = 1; j < height - 1; j++) {
    lap[j * width] = lap[j * width + 1];
    lap[j * width + width - 1] = lap[j * width + width - 2];
  }

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_lap = fopen("lap_airplane.pgm", "wb")) == NULL) exit(1);

  // ヘッダを書き込む
  fprintf(img_lap, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(lap, sizeof(unsigned char), width * height, img_lap);

  fclose(img_lap);
}
