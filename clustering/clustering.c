#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define INPUTNO 2
#define MAXINPUTNO 420

typedef struct {
  int i;    // 行番号
  double d; // 距離
} distance_t;

void nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2);
void k_nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2, int k);
void k_means(double coord[][INPUTNO], int category[], int n, int k);
void read_data(char *fileloc, double coord[][INPUTNO], int category[], int *n);
void save_data(char filename[], double coord[][INPUTNO], int category[], int n);
int cmp_distance(const void * p, const void * q);

int main(int argc, char *argv[]) {
  int n1, n2, n3, n4;                 // 行数
  double coord1[MAXINPUTNO][INPUTNO]; // data01の座標
  int category1[MAXINPUTNO];          // data01のカテゴリ
  double coord2[MAXINPUTNO][INPUTNO]; // data02の座標
  int category2[MAXINPUTNO];          // data02のカテゴリ
  double coord3[MAXINPUTNO][INPUTNO]; // data03の座標
  int category3[MAXINPUTNO];          // data03のカテゴリ
  double coord4[MAXINPUTNO][INPUTNO]; // data04の座標
  int category4[MAXINPUTNO];          // data04のカテゴリ

  // <---------- データの読み込み・未分類データの出力 ---------->
  // data01(分類済みデータ)を読み込む
  read_data(argv[1], coord1, category1, &n1);

  // data02(未分類データ)を読み込む
  read_data(argv[2], coord2, category2, &n2);

  // data02(未分類)を保存する
  save_data("out/data02_before.csv", coord2, category2, n2);

  // data03(未分類データ)を読み込む
  read_data(argv[3], coord3, category3, &n3);

  // data03(未分類)を保存する
  save_data("out/data03_before.csv", coord3, category3, n3);

  // data04(未分類データ)を読み込む
  read_data(argv[4], coord4, category4, &n4);

  // data04(未分類)を保存する
  save_data("out/data04_before.csv", coord4, category4, n4);

  // <----------- 課題1 ---------->
  // 最近傍法
  nearest_neighbor(coord1, category1, n1, coord2, category2, n2);

  // data02(分類後)を保存する
  save_data("out/data02_after1.csv", coord2, category2, n2);

  // <---------- 課題2 ----------->
  // k-最近傍法(k: 奇数)
  k_nearest_neighbor(coord1, category1, n1, coord2, category2, n2, 5);

  // data02(分類後)を保存する
  save_data("out/data02_after2_odd.csv", coord2, category2, n2);

  // k-最近傍法(k: 偶数)
  k_nearest_neighbor(coord1, category1, n1, coord2, category2, n2, 6);

  // data02(分類後)を保存する
  save_data("out/data02_after2_even.csv", coord2, category2, n2);

  // <----------　課題3 ----------->
  // k-平均法
  // k_means(coord3, category3, n3, 2);

  // data03(分類後)を保存する
  // save_data("out/data03_after.csv", coord3, category3, n3);

  // k-平均法
  k_means(coord4, category4, n4, 5);

  // data03(分類後)を保存する
  save_data("out/data04_after.csv", coord4, category4, n4);

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

// k-最近傍法(coord1: 分類済みデータの座標, category1: 分類済みデータのカテゴリ, n1: 分類済みデータの行数,
//           coord2: 未分類データの座標, category2: 未分類データのカテゴリ, n2: 未分類データの行数, k: 何近傍か)
void k_nearest_neighbor(double coord1[][INPUTNO], int category1[], int n1, double coord2[][INPUTNO], int category2[], int n2, int k) {
  int num1, num2;     // 各クラスタに分類された数
  distance_t dis[n1];

  for (int i = 0; i < n2; i++) {
    num1 = num2 = 0;

    // ユークリッド距離を求めて構造体に代入
    for (int i2 = 0; i2 < n1; i2++) {
      dis[i2].i = i2;
      dis[i2].d = sqrt(pow(coord1[i2][0] - coord2[i][0], 2) + pow(coord1[i2][1] - coord2[i][1], 2));
    }

    // 構造体を距離でソート(昇順)
    qsort(dis, n1, sizeof(distance_t), cmp_distance);

    // 距離が短い順に上位k個について、各クラスタの数を求める
    for (int j = 0; j < k; j++) {
      if (category1[dis[j].i] == 1) num1++;
      else num2++;
    }

    // 数が多い方のクラスタに分類
    category2[i] = num1 > num2 ? 1 : 2;
  }
}

// k-平均法(coord: 座標, category: カテゴリ, n: 行数, k: クラスタ数)
void k_means(double coord[][INPUTNO], int category[], int n, int k) {
  distance_t dis[k];  // 各重心からの距離
  double num[k];      // 各クラスタに分類された数
  double sum[k][2];   // 各クラスタに分類された座標の合計値
  double g[k][2];     // 各クラスタの重心の座標
  double g_tmp[k][2]; // 各クラスタの重心の座標(一時保持)

  // 重心の初期化
  for (int j = 0; j < k; j++) {
    g_tmp[j][0] = coord[(n / k) * (j + 1) - 1][0];
    g_tmp[j][1] = coord[(n / k) * (j + 1) - 1][1];
  }

  int i;
  while (1) {
    // 重心が変化していなければ終了
    for (i = 0; i < k; i++) {
      if ((g[i][0] != g_tmp[i][0]) || g[i][1] != g_tmp[i][1]) break;
    }
    if (i == k) break;

    // 重心が変化している場合、重心を更新し処理を続行
    for (i = 0; i < k; i++) {
      g[i][0] = g_tmp[i][0];
      g[i][1] = g_tmp[i][1];
      num[i] = 1;
      sum[i][0] = 0;
      sum[i][1] = 0;
    }

    for (int j = 0; j < n; j++) {
      // 各重心からの距離を求める
      for (i = 0; i < k; i++) {
        dis[i].i = i;
        dis[i].d = sqrt(pow(g[i][0] - coord[j][0], 2) + pow(g[i][1] - coord[j][1], 2));
      }

      // 距離をソート(昇順)
      qsort(dis, k, sizeof(distance_t), cmp_distance);

      // 最も近い重心のクラスタに分類
      category[j] = dis[0].i + 1;
      // num[dis[0].i]++;
      sum[dis[0].i][0] += coord[j][0];
      sum[dis[0].i][1] += coord[j][1];

      if ((g[dis[0].i][0] != coord[j][0] || g[dis[0].i][1] != coord[j][1])) num[dis[0].i]++;
    }

    // 重心を求める
    for (i = 0; i < k; i++) {
      g_tmp[i][0] = sum[i][0] / num[i];
      g_tmp[i][1] = sum[i][1] / num[i];
    }
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
    fprintf(data, "%d\n", category[i]);    // クラスタを記述
  }

  fclose(data);
}

int cmp_distance(const void *p, const void *q) {
  return ((distance_t*)p)->d - ((distance_t*)q)->d;
}
