#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 度数をラジアンに変換するマクロ
#define RAD(ARC) ((ARC) * 3.14159 / 180)

void read_header(FILE *img, int *width, int *height);
void minimize(unsigned char gray[], int width, int height);
void enlarge(unsigned char gray[], int width, int height);
void rotate(unsigned char gray[], int width, int height);

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

  fclose(img);

  // 画像を縦・横それぞれ1/2倍に縮小
  minimize(gray, width, height);

  // 画像を縦・横それぞれ2倍に拡大
  enlarge(gray, width, height);

  // 画像を30度回転
  rotate(gray, width, height);

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

// 画像を縦・横それぞれ1/2倍に縮小(gray: 元画像のデータ, width: 元画像の横幅, height: 元画像の縦幅)
void minimize(unsigned char gray[], int width, int height) {
  FILE *img_mini;                                // 縮小後の画像
  int width_mini = width / 2;                    // 縮小後の横幅
  int height_mini = height / 2;                  // 縮小後の縦幅
  unsigned char mini[width_mini * height_mini];  // 縮小後の画像データ
  int pos;                                       // 平均操作法で用いる元画像の左上の画素の位置

  // 平均操作法
  for (int i = 0; i < width_mini; i++) {
    for (int j = 0; j < height_mini; j++) {
      // 位置を求める
      pos = 2 * (j * width + i);

      // 4画素の濃度平均を求めて代入
      mini[j * width_mini + i] = (gray[pos] + gray[pos + 1] + gray[pos + width] + gray[pos + width + 1]) / 4;
    }
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_mini = fopen("minimize_50p.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_mini, "P5\n%d %d\n255\n", width_mini, height_mini);

  // 画像データを書き込む
  fwrite(mini, sizeof(unsigned char), width_mini * height_mini, img_mini);

  fclose(img_mini);
}

// 画像を縦・横それぞれ2倍に拡大(gray: 元画像のデータ, width: 元画像の横幅, height: 元画像の縦幅)
void enlarge(unsigned char gray[], int width, int height) {
  FILE *img_big;                             // 拡大後の画像
  int width_big = width * 2;                 // 拡大後の横幅
  int height_big = height * 2;               // 拡大後の縦幅
  unsigned char big[width_big * height_big]; // 拡大後の画像データ
  int pos;                                   // 直線補間法で用いる元画像の左上の画素の位置
  int pos_big;                               // 直線補間法で用いる拡大画像の左上の画素の位置

  // 直線補間法
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      // 位置を求める
      pos = j * width + i;
      pos_big = 2 * (j * width_big + i);

      // 注目画素の値
      big[pos_big] = gray[pos];

      // 右に拡大する画素の値
      if (i != width - 1) {
        big[pos_big + 1] = (gray[pos] + gray[pos + 1]) / 2;
      }
      else {
        big[pos_big + 1] = (gray[pos] + gray[pos - 1]) / 2;
      }

      // 下に拡大する画素の値
      if (j != height - 1) {
        big[pos_big + width_big] = (gray[pos] + gray[pos + width]) / 2;
      }
      else {
        big[pos_big + width_big] = (gray[pos] + gray[pos - width]) / 2;
      }

      // 右下に拡大する画素の値
      if (i == width - 1 && j == height - 1) {
        big[pos_big + width_big + 1] = (gray[pos] + gray[pos - width - 1]) / 2;
      }
      else if (i == width - 1) {
        big[pos_big + width_big + 1] = (gray[pos] + gray[pos + width - 1]) / 2;
      }
      else if (j == height - 1) {
        big[pos_big + width_big + 1] = (gray[pos] + gray[pos - width + 1]) / 2;
      }
      else {
        big[pos_big + width_big + 1] = (gray[pos] + gray[pos + width + 1]) / 2;
      }
    }
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_big = fopen("enlarge_200p.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_big, "P5\n%d %d\n255\n", width_big, height_big);

  // 画像データを書き込む
  fwrite(big, sizeof(unsigned char), width_big * height_big, img_big);

  fclose(img_big);
}

// 画像を30度回転(gray: 元画像のデータ, width: 元画像の横幅, height: 元画像の縦幅)
void rotate(unsigned char gray[], int width, int height) {
  FILE *img_rot;                                           // 回転後の画像
  int width_rot  = width * (sin(RAD(30)) + cos(RAD(30)));  // 回転後の画像全体の横幅
  int height_rot = height * (sin(RAD(30)) + cos(RAD(30))); // 回転後の画像全体の縦幅
  unsigned char *rot;                                      // 回転後のグレイスケール画像データ
  int x_after, y_after;                                    // アフィン変換後の座標

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((rot = (unsigned char *)malloc(sizeof(unsigned char) * width_rot * height_rot)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // アフィン変換
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      // 回転+平行移動後のx座標とy座標を求める
      x_after = (int)(cos(RAD(30)) * i + sin(RAD(30)) * j);
      y_after = (int)(cos(RAD(30)) * j - sin(RAD(30)) * i) + (width * sin(RAD(30)));

      // 回転後の座標に回転前の座標のピクセルの値を代入
      rot[y_after * width_rot + x_after] = gray[j * width + i];
    }
  }

  // 書き込むファイルを開く(開なかった場合プログラムを終了)
  if ((img_rot = fopen("rotate_30deg.pgm", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを書き込む
  fprintf(img_rot, "P5\n%d %d\n255\n", width_rot, height_rot);

  // 画像データを書き込む
  fwrite(rot, sizeof(unsigned char), width_rot * height_rot, img_rot);

  free(rot);

  fclose(img_rot);
}
