#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void labeling(unsigned char gray[], unsigned char lab[], int pos, int width, int height, int label);

int main(int argc, char *argv[]) {
  FILE *img;           // 画像
  FILE *img_lab;       // ラベリング処理後の画像
  int width, height;   // 画像の横幅, 縦幅
  unsigned char *gray; // グレイスケール画像データ

  // 入力画像を読み込む
  if ((img = fopen(argv[1], "rb")) == NULL) exit(1);
  read_header(img, &width, &height);
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) exit(1);
  fread(gray, sizeof(unsigned char), width * height, img);
  fclose(img);

  // 画像を2値化
  for (int i = 0; i < width * height; i++) {
    gray[i] = (gray[i] >= 128) ? 255 : 0;
  }

  // ラベリング処理後の画像データの初期化
  unsigned char gray_lab[width * height];
  for (int i = 0; i < width * height; i++) {
    gray_lab[i] = 0;
  }

  // ラベリング処理
  int pos, label;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      pos = j * width + i;
      if (gray[pos] == 255 && gray_lab[pos] == 0) {
        label++;
        labeling(gray, gray_lab, pos, width, height, label);
      }
    }
  }

  // ファイルに書き込む
  if ((img_lab = fopen("labeling.pgm", "wb")) == NULL) exit(1);
  fprintf(img_lab, "P5\n%d %d\n255\n", width, height);
  fwrite(gray_lab, sizeof(unsigned char), width * height, img_lab);
  fclose(img_lab);

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

// ラベリング(gray: 2値化済みのグレイスケール画像のデータ, gray_lab: ラベリング後の画像データ, pos: 座標)
void labeling(unsigned char gray[], unsigned char gray_lab[], int pos, int width, int height, int label) {
  if (gray[pos + 1] == 255) {
    labeling(gray, gray_lab, pos + 1, width, height, label);
  }
  else {
    gray_lab[pos] = label;
  }

  if (gray[pos + width - 1] == 255) {
    labeling(gray, gray_lab, pos + width - 1, width, height, label);
  }
  else {
    gray_lab[pos] = label;
  }

  if (gray[pos + width] == 255) {
    labeling(gray, gray_lab, pos + width, width, height, label);
  }
  else {
    gray_lab[pos] = label;
  }

  if (gray[pos + width + 1] == 255) {
    labeling(gray, gray_lab, pos + width + 1, width, height, label);
  }
  else {
    gray_lab[pos] = label;
  }

  printf("%d, %d\n", label, pos);
}
