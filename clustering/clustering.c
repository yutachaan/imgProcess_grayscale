#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUTNO 2
#define MAXINPUTNO 500

void read_data(char *fileloc, double coord[][INPUTNO], double category[], int *n);
void save_data(char filename[], double coord[][INPUTNO], double category[], int n);

int main(int argc, char *argv[]) {
  int m = 2;                          // 列数
  int n, n2;                          // 行数
  double coord1[MAXINPUTNO][INPUTNO]; // data01の座標
  double category1[MAXINPUTNO];       // data01のカテゴリ
  double coord2[MAXINPUTNO][INPUTNO]; // data02の座標
  double category2[MAXINPUTNO];       // data02のカテゴリ

  m = 2;

  // data01(分類済みデータ)を読み込む
  read_data(argv[1], coord1, category1, &n);

  // data02(未分類データ)を読み込む
  read_data(argv[2], coord2, category2, &n2);

  // data02(未分類)を保存する
  save_data("data02_before.csv", coord2, category2, n2);

  // 最近傍法

  // data02(分類後)を保存する
  save_data("data02_after.csv", coord2, category2, n2);

  return 0;
}

// データを読み込む
void read_data(char *fileloc, double coord[][INPUTNO], double category[], int *n) {
  FILE *data;
  int m_, n_;    // 列、行
  char buf[256];

  // 指定されたファイルを開く
	if ((data = fopen(fileloc, "r")) == NULL) exit(1);

  n_ = 0;
  // 一行ずつ読み込む
  while (fgets(buf, sizeof(buf), data) != NULL) {
    char *token = strtok(buf, " ");            // 空白で区切った字句へのポインタ
    m_ = 0;

    // 一字句ずつ処理
    while (token != NULL) {
      if (m_ < 2) coord[n_][m_] = atof(token); // 最初の2つはdoubleに変換してcoordに代入
      else category[n_] = atof(token);         // 最後の1つはdoubleに変換してcategoryに代入

      // 次の字句へ移動
      token = strtok(NULL, " ");
      m_++;
    }

    // 行数をカウント
    n_++;
  }

  fclose(data);

  // 行数を返す
  *n = n_;
}

// データを保存
void save_data(char filename[], double coord[][INPUTNO], double category[], int n) {
  FILE *data;

  if ((data = fopen(filename, "w")) == NULL) exit(1);

  fprintf(data, "# x y z\n");

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 2; j++) {
      // 座標を記述
      fprintf(data, "\t%.4f", coord[i][j]);
    }
    // カテゴリを記述
    fprintf(data, "\t%.4f\n", category[i]);
  }

  fclose(data);
}
