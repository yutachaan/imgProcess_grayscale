#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define INPUTNO 2
#define MAXINPUTNO 500

typedef struct {
  int i;    // 何行目か
  double d; // 距離
} distance_t;

void nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2);
void k_nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2, int k);
void read_data(char *fileloc, double coord[][INPUTNO], int category[], int *n);
void save_data(char filename[], double coord[][INPUTNO], int category[], int n);
int cmp(const void * p, const void * q);

int main(int argc, char *argv[]) {
  int n1, n2;                         // 行数
  double coord1[MAXINPUTNO][INPUTNO]; // data01の座標
  int category1[MAXINPUTNO];          // data01のカテゴリ
  double coord2[MAXINPUTNO][INPUTNO]; // data02の座標
  int category2[MAXINPUTNO];          // data02のカテゴリ

  // data01(分類済みデータ)を読み込む
  read_data(argv[1], coord1, category1, &n1);

  // data02(未分類データ)を読み込む
  read_data(argv[2], coord2, category2, &n2);

  // data02(未分類)を保存する
  save_data("out/data02_before.csv", coord2, category2, n2);

  // 最近傍法
  nearest_neighbor(coord1, category1, n1, coord2, category2, n2);

  // data02(分類後)を保存する
  save_data("out/data02_after1.csv", coord2, category2, n2);

  // k-最近傍法(k: 奇数)
  k_nearest_neighbor(coord1, category1, n1, coord2, category2, n2, 5);

  // data02(分類後)を保存する
  save_data("out/data02_after2_odd.csv", coord2, category2, n2);

  // k-最近傍法(k: 偶数)
  k_nearest_neighbor(coord1, category1, n1, coord2, category2, n2, 6);

  // data02(分類後)を保存する
  save_data("out/data02_after2_even.csv", coord2, category2, n2);

  return 0;
}

// 最近傍法(coord1: 分類済みデータの座標, category1: 分類済みデータのカテゴリ, n1: 分類済みデータの行数, coord2: 未分類データの座標, category2: 未分類データのカテゴリ, n2: 未分類データの行数)
void nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2) {
  double d;     // 最近傍のユークリッド距離
  double d_tmp; // ユークリッド距離

  for (int i = 0; i < n2; i++) {
    d = DBL_MAX;

    for (int i2 = 0; i2 < n1; i2++) {
      // ユークリッド距離を求める
      d_tmp = sqrt(pow(coord1[i2][0] - coord2[i][0], 2) + pow(coord1[i2][1] - coord2[i][1], 2));

      // dよりも短かった場合
      if (d_tmp < d) {
        d = d_tmp;                    // dを更新
        category2[i] = category1[i2]; // カテゴライズ
      }
    }
  }
}

// k-最近傍法(coord1: 分類済みデータの座標, category1: 分類済みデータのカテゴリ, n1: 分類済みデータの行数, coord2: 未分類データの座標, category2: 未分類データのカテゴリ, n2: 未分類データの行数, k: 何近傍か)
void k_nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2, int k) {
  int num1, num2;     // 各カテゴリに分類された数
  distance_t dis[n1];

  for (int i = 0; i < n2; i++) {
    num1 = num2 = 0;

    // ユークリッド距離を求めて構造体に代入
    for (int i2 = 0; i2 < n1; i2++) {
      dis[i2].i = i2;
      dis[i2].d = sqrt(pow(coord1[i2][0] - coord2[i][0], 2) + pow(coord1[i2][1] - coord2[i][1], 2));
    }

    // 構造体を距離でソート(昇順)
    qsort(dis, n1, sizeof(distance_t), cmp);

    // 距離が短い順に上位k個について、各カテゴリの数を求める
    for (int j = 0; j < k; j++) {
      if (category1[dis[j].i] == 1) num1++;
      else num2++;
    }

    // 数が多い方のカテゴリに分類
    category2[i] = num1 > num2 ? 1 : 2;
  }
}

// データを読み込む(fileloc: 読み込むファイル, coord: 座標保存用, category: カテゴリ保存用, n: 行数保存用)
void read_data(char *fileloc, double coord[][INPUTNO], int category[], int *n) {
  FILE *data;
  int mm, nn;
  char buf[256];

  if ((data = fopen(fileloc, "r")) == NULL) exit(1);

  nn = 0;
  // 一行ずつ読み込む
  while (fgets(buf, sizeof(buf), data) != NULL) {
    mm = 0;
    char *token = strtok(buf, " ");            // 空白で区切った最初の数字へのポインタ

    while (token != NULL) {
      if (mm < 2) coord[nn][mm] = atof(token); // 最初の2つはdoubleに変換してcoordに代入
      else category[nn] = atoi(token);         // 最後の1つはintに変換してcategoryに代入

      // 次の数字へ移動
      token = strtok(NULL, " ");
      mm++;
    }

    // 行数をカウント
    nn++;
  }

  fclose(data);

  // 行数を返す
  *n = nn;
}

// データを保存(filename: 保存するファイル名, coord: 座標, category: カテゴリー, n: 行数)
void save_data(char filename[], double coord[][INPUTNO], int category[], int n) {
  FILE *data;

  if ((data = fopen(filename, "w")) == NULL) exit(1);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 2; j++) {
      fprintf(data, "%.4f,", coord[i][j]); // 座標を記述
    }
    fprintf(data, "%d\n", category[i]);    // カテゴリを記述
  }

  fclose(data);
}

// 比較用の関数
int cmp(const void *p, const void *q) {
  return ((distance_t*)p)->d - ((distance_t*)q)->d;
}
