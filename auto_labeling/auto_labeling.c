#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DEPTH 255

void read_header(FILE *img, int *width, int *height);
void binarize(unsigned char gray[], int width, int height);

int main(int argc, char *argv[]) {
  FILE *img;           // 画像
  int width, height;   // 画像の横幅, 縦幅
  unsigned char *gray; // グレイスケール画像データ

  // 入力画像を読み込む
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);
  read_header(img, &width, &height);
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);
  fread(gray, sizeof(unsigned char), width * height, img);
  fclose(img);

  binarize(gray, width, height);

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

// 判別分析法を用いて画像を2値化(gray: 元画像のデータ, width: 画像幅, height: 画像高さ)
void binarize(unsigned char gray[], int width, int height) {
  int t_res, n1, n2, sum1, sum2, sum_square1, sum_square2; // グループ1と2の画素数・合計値・2乗の合計値
  double mean, var1, mean1, var2, mean2, var_w, var_b, s;  // 全体の平均、グループ1と2の分散・平均、クラス内分散、クラス間分散、分散比

  s = 0;

  // 分散比が最大となる閾値を求める
  for (int t = 1; t < 255; t++) {
    n1 = n2 = sum1 = sum2 = sum_square1 = sum_square2 = 0;

    // 画素数、合計値、2乗の合計値を求める
    for (int i = 0; i < width * height; i++) {
      if (gray[i] < t) {
        // グループ1
        n1++;
        sum1 += gray[i];
        sum_square1 += pow(gray[i], 2);
      }
      else {
        // グループ2
        n2++;
        sum2 += gray[i];
        sum_square2 += pow(gray[i], 2);
      }
    }

    // 平均、分散を求める
    mean1 = sum1 / n1;
    var1 = sum_square1 / n1 - pow(mean1, 2);
    mean2 = sum2 / n2;
    var2 = sum_square2 / n2 - pow(mean2, 2);
    mean = (sum1 + sum2) / (n1 + n2);

    // クラス内分散、クラス間分散を求める
    var_w = (n1 * pow(var1, 2) + n2 * pow(var2, 2)) / (n1 + n2);
    var_b = (n1 * pow(mean1 - mean, 2) + n2 * pow(mean2 - mean, 2)) / (n1 + n2);

    // 分散比と閾値の更新
    if (var_b / var_w > s) {
      s = var_b / var_w;
      t_res = t;
    }
  }

  // 2値化
  for (int i = 0; i < width * height; i++) {
    gray[i] = (gray[i] >= t_res) ? 255 : 0;
  }
}
