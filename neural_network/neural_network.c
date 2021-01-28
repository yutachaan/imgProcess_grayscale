#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INPUTNO 2
#define MAXINPUTNO 350
#define HIDDENNO 2

double f_step(double u);
void read_data(char *fileloc, double coord[][INPUTNO], int *n);
void save_data(char filename[], double coord[][INPUTNO], double output[], int n);
void simple_perceptron(double coord[][INPUTNO], double output[], double weight[], int n);
void multilayer_perceptron(double coord[][INPUTNO], double output[], double weight_h[][INPUTNO + 1], double weight_o[], int n);

int main(int argc, char *argv[]) {
  int n1, n2, n3;                     // 行数
  double coord1[MAXINPUTNO][INPUTNO]; // data01の座標
  double output1[MAXINPUTNO];         // data01の出力
  double coord2[MAXINPUTNO][INPUTNO]; // data02の座標
  double output2[MAXINPUTNO];         // data02の出力
  double coord3[MAXINPUTNO][INPUTNO]; // data03の座標
  double output3[MAXINPUTNO];         // data03の出力

  // <---------- データの読み込み・未分類データの出力 ---------->
  // data01を読み込む
  read_data(argv[1], coord1, &n1);

  // data01を保存する
  for (int i = 0; i < n1; i++) output1[i] = 0;
  save_data("out/data01_before.csv", coord1, output1, n1);

  // data02を読み込む
  read_data(argv[2], coord2, &n2);

  // data02を保存する
  for (int i = 0; i < n2; i++) output2[i] = 0;
  save_data("out/data02_before.csv", coord2, output2, n2);

  // data03を読み込む
  read_data(argv[3], coord3, &n3);

  // data03を保存する
  for (int i = 0; i < n3; i++) output3[i] = 0;
  save_data("out/data03_before.csv", coord3, output3, n3);

  // <----------- 課題1(OR分類器) ---------->
  double weight_or[INPUTNO + 1] = {1, 1, 0.5};

  // data01
  simple_perceptron(coord1, output1, weight_or, n1);
  save_data("out/data01_or.csv", coord1, output1, n1);

  // data02
  simple_perceptron(coord2, output2, weight_or, n2);
  save_data("out/data02_or.csv", coord2, output2, n2);

  // data03
  simple_perceptron(coord3, output3, weight_or, n3);
  save_data("out/data03_or.csv", coord3, output3, n3);

  // <----------- 課題1(AND分類器) ---------->
  double weight_and[INPUTNO + 1] = {1, 1, 1.3};

  // data01
  simple_perceptron(coord1, output1, weight_and, n1);
  save_data("out/data01_and.csv", coord1, output1, n1);

  // data02
  simple_perceptron(coord2, output2, weight_and, n2);
  save_data("out/data02_and.csv", coord2, output2, n2);

  // data03
  simple_perceptron(coord3, output3, weight_and, n3);
  save_data("out/data03_and.csv", coord3, output3, n3);

  // <----------- 課題2(XOR分類器) ---------->
  double weight_xor_h[HIDDENNO][INPUTNO + 1] = {{1, 1, 0.5}, {1, 1, 1.3}};
  double weight_xor_o[HIDDENNO + 1] = {1, -1, 0.5};

  // data01
  multilayer_perceptron(coord1, output1, weight_xor_h, weight_xor_o, n1);
  save_data("out/data01_xor.csv", coord1, output1, n1);

  // data02
  multilayer_perceptron(coord2, output2, weight_xor_h, weight_xor_o, n2);
  save_data("out/data02_xor.csv", coord2, output2, n2);

  // data03
  multilayer_perceptron(coord3, output3, weight_xor_h, weight_xor_o, n3);
  save_data("out/data03_xor.csv", coord3, output3, n3);

  return 0;
}

// 単純パーセプトロン(coord: 座標, output: 出力保存用, weight: 重み, n: 行数)
void simple_perceptron(double coord[][INPUTNO], double output[], double weight[], int n) {
  double u; // 入力に重みを掛けて閾値を引いた値

  for (int i = 0; i < n; i++) {
    u = 0;

    // uを求める
    for (int j = 0; j < INPUTNO; j++) {
      u += coord[i][j] * weight[j];
    }
    u -= weight[INPUTNO];

    // uを活性化関数に渡して出力
    output[i] = f_step(u);
  }
}

// 多層パーセプトロン(coord: 座標, output: 出力保存用, weight_h: 隠れ層での重み, weight_o: 出力層での重み, n: 行数)
void multilayer_perceptron(double coord[][INPUTNO], double output[], double weight_h[][INPUTNO + 1], double weight_o[], int n){
  double u, o;             // 入力に重みを掛けて閾値を引いた値(u: 隠れ層, o: 出力層)
  double hidden[HIDDENNO]; // それぞれの隠れ層でのuの値


  for (int i = 0; i < n; i++) {
    // 初期化
    for (int j = 0; j < HIDDENNO; j++) hidden[j] = 0;

    // 隠れ層の計算
    for (int j = 0; j < HIDDENNO; j++) {
      u = 0;

      // uを求める
      for (int k = 0; k < INPUTNO; k++) {
        u += coord[i][k] * weight_h[j][k];
      }
      u -= weight_h[j][INPUTNO];

      // uを活性化関数に渡して格納
      hidden[j] = f_step(u);
    }

    // 出力層の計算
    o = 0;
    for (int j = 0; j < HIDDENNO; j++) {
      o += hidden[j] * weight_o[j];
    }
    o -= weight_o[HIDDENNO];

    // oを活性化関数に渡して出力
    output[i] = f_step(o);
  }
}

// データを読み込む(fileloc: 読み込むファイル, coord: 座標保存用, n: 行数保存用)
void read_data(char *fileloc, double coord[][INPUTNO], int *n) {
  FILE *data;
  int mm, nn;
  char buf[256];

  if ((data = fopen(fileloc, "r")) == NULL) exit(1);

  nn = 0;
  while (fgets(buf, sizeof(buf), data) != NULL) {
    char *token = strtok(buf, " "); // 空白で区切った最初の数字へのポインタ

    mm = 0;
    while (token != NULL) {
      coord[nn][mm] = atof(token); // 最初の2つはdoubleに変換してcoordに代入
      token = strtok(NULL, " ");   // 次の数字へ移動
      mm++;
    }

    nn++;
  }

  fclose(data);

  *n = nn;
}

// データを保存(filename: 保存するファイル名, coord: 座標, output: 出力, n: 行数)
void save_data(char filename[], double coord[][INPUTNO], double output[], int n) {
  FILE *data;

  if ((data = fopen(filename, "w")) == NULL) exit(1);

  for (int i = 0; i < n; i++) {
    fprintf(data, "%.4f,%.4f,%.4f\n", coord[i][0], coord[i][1], output[i]);
  }

  fclose(data);
}

// ステップ関数
double f_step(double u) {
	if (u >= 0) return 1.0;
	else return 0;
}
