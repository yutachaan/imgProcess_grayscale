#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void nearest_neighbor(unsigned char gray[], int width, int height);

int main(int argc, char *argv[]) {
  FILE *img;           // 元画像
  int width, height;   // 画像の横幅, 縦幅
  unsigned char *gray; // グレイスケール画像データ

  // ファイルを開いて画像データを読み込む
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);
  read_header(img, &width, &height);
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);
  fread(gray, sizeof(unsigned char), width * height, img);
  fclose(img);

  // 幾何学的歪みの除去
  nearest_neighbor(gray, width, height);

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

// ニアレストネイバー法による幾何学的歪みの除去(gray: 元画像データ, width: 画像の幅, height: 画像の高さ)
void nearest_neighbor(unsigned char gray[], int width, int height) {
  FILE *img_after;                          // 歪み除去後の画像ファイル
  unsigned char gray_after[width * height]; // 歪み除去後の画像データ

  int pos;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      pos = j * width + i;
      gray_after[pos] = gray[pos];
    }
  }

  // ファイルに書き込む
  if ((img_after = fopen("affine.pgm", "wb")) == NULL) exit(1);
  fprintf(img_after, "P5\n%d %d\n255\n", width, height);
  fwrite(gray_after, sizeof(unsigned char), width * height, img_after);
  fclose(img_after);
}
