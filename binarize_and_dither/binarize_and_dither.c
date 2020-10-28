#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void binarize(unsigned char gray[], int width, int height);

int main(int argc, char *argv[]) {
  FILE *img;           // 元画像
  int width, height;   // 画像の横幅, 縦幅
  unsigned char *gray; // グレイスケール画像データ

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 2) exit(1);

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを読み取り、画像の横幅と縦幅を取得
  read_header(img, &width, &height);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img);

  // 固定しきい値法を用いて画像を2値化
  binarize(gray, width, height);

  fclose(img);

  free(gray);

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

// 固定しきい値法を用いて画像を2値化(gray: 元画像のデータ, width: 画像幅, height: 画像高さ)
void binarize(unsigned char gray[], int width, int height) {
  FILE *img_binarize; // 2値化後の画像
  unsigned char gray_binarize[width * height]; // 2値化後の画像データ

  // 固定しきい値法
  for (int i = 0; i < width * height; i++) {
    gray_binarize[i] = (gray[i] >= 128) ? 255 : 0;
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_binarize = fopen("binarize.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_binarize, "P5\n%d %d\n255\n", width, height);

  // 画像データを書き込む
  fwrite(gray_binarize, sizeof(unsigned char), width * height, img_binarize);

  fclose(img_binarize);
}
