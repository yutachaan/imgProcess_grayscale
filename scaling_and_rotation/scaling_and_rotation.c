#include <stdio.h>
#include <stdlib.h>

void read_header(FILE *img, int *width, int *height);

int main(int argc, char *argv[]) {
  FILE *img;           // 元画像
  int width;           // 画像の横幅
  int height;          // 画像の縦幅
  int img_size;        // 画像のサイズ
  unsigned char *gray; // グレイスケール変換後の画像データ

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

  // 横幅と縦幅から画像のサイズを決定
  img_size = width * height;

  // 配列を動的に確保(確保できなかった場合プログラムを終了)
  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * img_size)) == NULL) {
    printf("メモリが確保できませんでした。\n");
    exit(1);
  }

  // 画像データの読み込み
  fread(gray, sizeof(unsigned char), img_size, img);

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
