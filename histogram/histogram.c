#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height, int *maxdepth);
void output_table(unsigned char gray[], int freq[], int img_size, int maxdepth);
void smooth_histogram(int freq[], int img_size, int maxdepth);

int main(int argc, char *argv[]) {
  FILE *img;                   // 元画像
  int width, height, maxdepth; // 画像の横幅, 縦幅, 最大階調値
  int img_size;                // 画像サイズ
  unsigned char *gray;         // グレイスケール画像データ
  int *freq;                   // 頻度を保存するための配列

  // コマンドライン引数の数が適切でない場合プログラムを終了
  if (argc != 2) exit(1);

  // 第一引数で指定された画像ファイルを開く(開けなかった場合プログラムを終了)
  if ((img = fopen(argv[1], "rb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // ヘッダを読み取り、画像の横幅と縦幅と最大階調値を代入
  read_header(img, &width, &height, &maxdepth);

  img_size = width * height;

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * img_size)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), img_size, img);

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((freq = (int *)malloc(sizeof(int) * maxdepth)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 濃度度数分布表を出力
  output_table(gray, freq, img_size, maxdepth);

  // ヒストグラム平滑化
  smooth_histogram(freq, img_size, maxdepth);

  free(freq);

  free(gray);

  fclose(img);

  return 0;
}

// ヘッダの情報を読み取る(img: 読み込む画像のファイルポインタ, width: 画像の横幅保存用, height: 画像の縦幅保存用, maxdepth: 最大階調値)
void read_header(FILE *img, int *width, int *height, int *maxdepth) {
  int i;
  char buf[256]; // ヘッダ読み込み時のバッファ

  while (i < 3) {
    fgets(buf, sizeof(buf), img);

    // コメントの場合読み飛ばす
    if (buf[0] == '#') continue;

    // 画像の横幅と縦幅を読み取る
    if (i == 1) sscanf(buf, "%d %d", width, height);

    // 画像の最大階調値を読み取る
    if (i == 2) sscanf(buf, "%d", maxdepth);

    i++;
  }
}

// 濃度度数分布表を出力(gray: 画像のデータ, freq: 頻度を保存する配列, img_size: 画像のサイズ, maxdepth: 最大階調値)
void output_table(unsigned char gray[], int freq[], int img_size, int maxdepth) {
  FILE *table; // 度数分布表を出力するファイル

  // 各濃度値の頻度を記録
  for (int i = 0; i < img_size; i++) {
    freq[gray[i]]++;
  }

  // 書き込むファイルを開く(開けなかった場合プログラムを終了)
  if ((table = fopen("freq_table.csv", "wb")) == NULL) {
    printf("ファイルが開けませんでした。\n");
    exit(1);
  }

  // 濃度度数分布表をcsvファイルに出力
  for (int i = 0; i <= maxdepth; i++) {
    fprintf(table, "%d,%d\n", i, freq[i]);
  }

  fclose(table);
}

// ヒストグラム平滑化(freq: 頻度, img_size: 画像サイズ, maxdepth: 最大階調値)
void smooth_histogram(int freq[], int img_size, int maxdepth) {
  int depth_goal = 64;                   // 目標階調数
  int freq_goal = img_size / depth_goal; // 頻度目標値
  int cum = 0;                           // 蓄積頻度
  int cur_thick[maxdepth];               // 補正濃度の配列

  // 補正濃度の配列の値を全て0で初期化
  for (int i = 0; i <= maxdepth; i++) {
    cur_thick[i] = 0;
  }

  // 濃度変換表作成
  for (int i = 0; i <= maxdepth; i++) {
    // 「目標頻度と蓄積頻度の差の絶対値」と「"目標頻度と蓄積頻度の差"と現在の頻度の差の絶対値」を比較
    if (abs(freq_goal - cum) < abs(freq_goal - cum - freq[i])) {
      cum = 0;                                                       // 蓄積頻度を0にする
      cur_thick[i] = (i ? cur_thick[i - 1] : 0) + 1;                 // 補正濃度を1加算(iが0の場合は前の補正濃度の値を0とする)
      if (cur_thick[i] >= depth_goal) cur_thick[i] = depth_goal - 1; // 補正濃度が目標階調数以上の場合の処理
    }
    else {
      cur_thick[i] = i ? cur_thick[i - 1] : 0;                       // 補正濃度の値を現在の補正濃度の値にする
    }
    cum += freq[i];                                                  // 蓄積頻度に現在の濃度の頻度を加算
  }
}
