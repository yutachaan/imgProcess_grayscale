/* 参考: モノづくりC言語塾 「C言語 qsort関数の使い方【構造体データも並べ替えができる】」
   https://monozukuri-c.com/langc-funclist-qsort/
   (閲覧日: 2020-10-27)
*/

#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void median_filter(unsigned char gray[], int width, int height);
int cmpnum(const void * n1, const void * n2);

int main(int argc, char *argv[]) {
  FILE *img;            // 元画像
  int width, height;    // 画像の横幅, 縦幅, 最大階調値
  unsigned char *gray;  // グレイスケール画像データ

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

  // メディアインフィルタを用いてノイズ除去
  median_filter(gray, width, height);

  free(gray);

  fclose(img);

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

// メディアインフィルタを用いてノイズ除去(gray: 元画像のデータ, width: 画像幅, height: 画像高さ)
void median_filter(unsigned char gray[], int width, int height) {
  FILE *img_denoise;                          // ノイズ除去後のファイル
  unsigned char gray_denoise[width * height]; // ノイズ除去後の画像データ
  int neighbor[9];                            // 近傍画素の値

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((img_denoise = fopen("denoise.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // 元画像にメディアンフィルタを適用してノイズ除去
  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 1; j++) {
      // 近傍がその値を代入
      neighbor[0] = gray[i - width - 1];
      neighbor[1] = gray[i - width];
      neighbor[2] = gray[i - width + 1];
      neighbor[3] = gray[i - 1];
      neighbor[4] = gray[i];
      neighbor[5] = gray[i + 1];
      neighbor[6] = gray[i + width - 1];
      neighbor[7] = gray[i + width];
      neighbor[8] = gray[i + width + 1];

      // ソート
      qsort(neighbor, sizeof(neighbor) / sizeof(neighbor[0]), sizeof(int), cmpnum);
    }
  }

  // ヘッダを書き込む
  fprintf(img_denoise, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(gray_denoise, sizeof(unsigned char), width * height, img_denoise);

  fclose(img_denoise);
}

// 並べ替え基準を示す関数(昇順)
int cmpnum(const void * n1, const void * n2)
{
	if (*(int *)n1 > *(int *)n2) return 1;
	else if (*(int *)n1 < *(int *)n2) return -1;
	else return 0;
}
