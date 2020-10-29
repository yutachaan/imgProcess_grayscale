/* 参考: モノづくりC言語塾 「C言語 qsort関数の使い方【構造体データも並べ替えができる】」
   https://monozukuri-c.com/langc-funclist-qsort/
   (閲覧日: 2020-10-27)
*/

#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void median_filter(unsigned char gray[], unsigned char denoise[], int width, int height);
void laplacian_filter(unsigned char denoise[], int width, int height);
int cmpnum(const void * n1, const void * n2);

int main(int argc, char *argv[]) {
  FILE *img;              // 元画像
  int width, height;      // 画像の横幅, 縦幅, 最大階調値
  unsigned char *gray;    // グレイスケール画像データ
  unsigned char *denoise; // ノイズ除去後の画像データ

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 2) exit(1);

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを読み取り、画像の横幅と縦幅を代入
  read_header(img, &width, &height);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img);

  fclose(img);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((denoise = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // メディアインフィルタを用いてノイズ除去
  median_filter(gray, denoise, width, height);

  free(gray);

  // 8近傍ラプラシアンフフィルタを用いて鮮鋭化
  laplacian_filter(denoise, width, height);

  free(denoise);

  return 0;
}

// ヘッダの情報を読み取る(img: 読み込む画像のファイルポインタ, width: 画像の横幅, height: 画像の縦幅)
void read_header(FILE *img, int *width, int *height) {
  int i;
  char buf[256];

  while (i < 3) {
    fgets(buf, sizeof(buf), img);

    // コメントの場合読み飛ばす
    if (buf[0] == '#') continue;

    // 画像の横幅と縦幅を読み取る
    if (i == 1) sscanf(buf, "%d %d", width, height);

    i++;
  }
}

// メディアインフィルタ(gray: 元画像のデータ, denoise: ノイズ除去後の画像データ, width: 画像の幅, height: 画像の高さ)
void median_filter(unsigned char gray[], unsigned char denoise[], int width, int height) {
  FILE *img_denoise; // ノイズ除去後のファイル
  int neighbor[9];   // 近傍画素の値
  int pos;           // 注目画素の座標

  // 元画像にメディアンフィルタを適用してノイズ除去
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      // 注目画素の座標を求める
      pos = j * width + i;

      // 端の画素はそのままの値を使用
      if (i == 0 || i == width - 1 || j == 0 || j == height - 1) {
        denoise[pos] = gray[pos];
        continue;
      }

      // 近傍画素の値を代入
      neighbor[0] = gray[pos - width - 1];
      neighbor[1] = gray[pos - width];
      neighbor[2] = gray[pos - width + 1];
      neighbor[3] = gray[pos - 1];
      neighbor[4] = gray[pos];
      neighbor[5] = gray[pos + 1];
      neighbor[6] = gray[pos + width - 1];
      neighbor[7] = gray[pos + width];
      neighbor[8] = gray[pos + width + 1];

      // 近傍画素の値をソート
      qsort(neighbor, sizeof(neighbor) / sizeof(neighbor[0]), sizeof(int), cmpnum);

      // 近傍画素の中央値を新しい値として使用
      denoise[pos] = neighbor[4];
    }
  }

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_denoise = fopen("denoise.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_denoise, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(denoise, sizeof(unsigned char), width * height, img_denoise);

  fclose(img_denoise);
}

// 8近傍ラプラシアンフィルタ(denoise: ノイズ除去後の画像データ, width: 画像幅, height: 画像高さ)
void laplacian_filter(unsigned char denoise[], int width, int height) {
  FILE *img_sharp;                     // 鮮鋭化後のファイル
  unsigned char sharp[width * height]; // 鮮鋭化後の画像データ
  int neighbor[9];                     // 近傍画素の値
  int pos;                             // 注目画素の座標
  int laplacian_sum;                   // ラプラシアンフィルタ適用後の近傍画素の値の合計

  // 元画像に8近傍ラプラシアンフィルタを適用して鮮鋭化
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      // 注目画素の座標を求める
      pos = j * width + i;

      // 端の画素はそのままの値を使用
      if (i == 0 || i == width - 1 || j == 0 || j == height - 1) {
        sharp[pos] = denoise[pos];
        continue;
      }

      laplacian_sum = 0;

      // 近傍画素にラプラシアンフィルタを適用した値を代入
      neighbor[0] = denoise[pos - width - 1];
      neighbor[1] = denoise[pos - width];
      neighbor[2] = denoise[pos - width + 1];
      neighbor[3] = denoise[pos - 1];
      neighbor[4] = denoise[pos] * (-8);
      neighbor[5] = denoise[pos + 1];
      neighbor[6] = denoise[pos + width - 1];
      neighbor[7] = denoise[pos + width];
      neighbor[8] = denoise[pos + width + 1];

      // ラプラシアンフィルタ適用後の近傍画素の合計値を求める
      for (int k = 0; k < 9; k++) {
        laplacian_sum += neighbor[k];
      }

      // 元の値から合計値を引いたものを新しい値とする(0未満の場合は0, 255より大きい場合は255にする)
      if ((denoise[pos] - laplacian_sum) < 0) {
        sharp[pos] = 0;
      }
      else if ((denoise[pos] - laplacian_sum) > 255) {
        sharp[pos] = 255;
      }
      else {
        sharp[pos] = denoise[pos] - laplacian_sum;
      }
    }
  }

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_sharp = fopen("sharp.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_sharp, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(sharp, sizeof(unsigned char), width * height, img_sharp);

  fclose(img_sharp);
}

// 並べ替え基準を示す関数(昇順)
int cmpnum(const void * n1, const void * n2)
{
	if (*(int *)n1 > *(int *)n2) return 1;
	else if (*(int *)n1 < *(int *)n2) return -1;
	else return 0;
}
