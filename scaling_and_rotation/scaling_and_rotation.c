#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);
void minimize(unsigned char gray[], char filepath[], int width, int height);
void enlarge(unsigned char gray[], char filepath[], int width, int height);

int main(int argc, char *argv[]) {
  FILE *img;           // 元画像
  int width, height;   // 画像の横幅, 縦幅
  unsigned char *gray; // グレイスケール画像データ

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

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * (width * height))) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), width * height, img);

  // 画像を縦・横それぞれ1/2倍に縮小
  minimize(gray, "minimize_50p.pgm", width, height);

  // 画像を縦・横それぞれ2倍に拡大
  enlarge(gray, "enlarge_200p.pgm", width, height);

  free(gray);

  fclose(img);

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

void minimize(unsigned char gray[], char filepath[], int width, int height) {
  int i, j;
  FILE *img_mini;               // 縮小後の画像
  unsigned char *gray_mini;     // 縮小後のグレイスケール画像データ
  int width_mini = width / 2;   // 縮小後の横幅
  int height_mini = height / 2; // 縮小後の縦幅
  int pos;                      // 平均操作法で用いる元画像の左上の画素の位置

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_mini = fopen(filepath, "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_mini, "P5\n%d %d\n255\n", width_mini, height_mini);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray_mini = (unsigned char *)malloc(sizeof(unsigned char) * (width_mini * height_mini))) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 平均操作法
  for (i = 0; i < width_mini; i++) {
    for (j = 0; j < height_mini; j++) {
      // 位置を求める
      pos = 2 * (j * width + i);

      // 4画素の濃度平均を求めて代入
      gray_mini[j * width_mini + i] = (gray[pos] + gray[pos + 1] + gray[pos + width] + gray[pos + width + 1]) / 4;
    }
  }

  // 画像データを書き込む
  fwrite(gray_mini, sizeof(unsigned char), width_mini * height_mini, img_mini);

  free(gray_mini);

  fclose(img_mini);
}

void enlarge(unsigned char gray[], char filepath[], int width, int height) {
  int i, j;
  FILE *img_big;               // 拡大後の画像
  unsigned char *gray_big;     // 拡大後のグレイスケール画像データ
  int width_big = width * 2;   // 拡大後の横幅
  int height_big = height * 2; // 拡大後の縦幅
  int pos;                     // 直線補間法で用いる元画像の左上の画素の位置
  int pos_big;                 // 直線補間法で用いる拡大画像の左上の画素の位置

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_big = fopen(filepath, "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_big, "P5\n%d %d\n255\n", width_big, height_big);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray_big = (unsigned char *)malloc(sizeof(unsigned char) * (width_big * height_big))) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 直線補間法(右端、下端以外)
  for (i = 0; i < width - 1; i++) {
    for (j = 0; j < height - 1; j++) {
      // 位置を求める
      pos = j * width + i;
      pos_big = 2 * (j * width_big + i);

      // 新画素を隣接画素の直線近似により決定
      gray_big[pos_big] = gray[pos];
      gray_big[pos_big + 1] = (gray[pos] + gray[pos + 1]) / 2;
      gray_big[pos_big + width_big] = (gray[pos] + gray[pos + width]) / 2;
      gray_big[pos_big + width_big + 1] = (gray[pos] + gray[pos + width + 1]) / 2;
    }
  }

  // 直線補間法(右端)
  i = width - 1;
  for (j = 0; j < height - 1; j++) {
    pos = j * width + i;
    pos_big = 2 * (j * width_big + i);

    gray_big[pos_big] = gray[pos];
    gray_big[pos_big + 1] = (gray[pos] + gray[pos - 1]) / 2;
    gray_big[pos_big + width_big] = (gray[pos] + gray[pos + width]) / 2;
    gray_big[pos_big + width_big + 1] = (gray[pos] + gray[pos + width - 1]) / 2;
  }

  // 直線補間法(下端)
  j = height - 1;
  for (i = 0; i < width - 1; i++) {
    pos = j * width + i;
    pos_big = 2 * (j * width_big + i);

    gray_big[pos_big] = gray[pos];
    gray_big[pos_big + 1] = (gray[pos] + gray[pos + 1]) / 2;
    gray_big[pos_big + width_big] = (gray[pos] + gray[pos - width]) / 2;
    gray_big[pos_big + width_big + 1] = (gray[pos] + gray[pos - width + 1]) / 2;
  }

  // 直線補間法(右下端)
  i = width - 1;
  j = height - 1;
  {
    pos = j * width + i;
    pos_big = 2 * (j * width_big + i);

    gray_big[pos_big] = gray[pos];
    gray_big[pos_big + 1] = (gray[pos] + gray[pos - 1]) / 2;
    gray_big[pos_big + width_big] = (gray[pos] + gray[pos - width]) / 2;
    gray_big[pos_big + width_big + 1] = (gray[pos] + gray[pos - width - 1]) / 2;
  }

  // 画像データを書き込む
  fwrite(gray_big, sizeof(unsigned char), width_big * height_big, img_big);

  free(gray_big);

  fclose(img_big);
}
