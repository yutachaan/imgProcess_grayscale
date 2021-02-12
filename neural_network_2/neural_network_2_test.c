#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <float.h>

#define DATANO 8   // データ数
#define INPUTNO 3  // 入力数
#define HIDDENNO 3 // 隠れ層の数

void train(double input[][INPUTNO], double wh[][INPUTNO + 1], double hidden[][HIDDENNO], double wo[], double output[], double teacher[]);
void read_data(char *fileloc, double input[][INPUTNO], double teacher[]);
// void save_data(char filename[], double input[][INPUTNO], double teacher[], double output[], int n);
// void save_err(char filename[], double err_data[], int n);
double f_sigmoid(double x);
double d_sigmoid(double x);

int main(int argc, char *argv[]) {
  // <---------- データの読み込み ---------->
  double input[DATANO][INPUTNO]; // 入力
  double teacher[DATANO];        // 教師データ

  read_data(argv[1], input, teacher);

  // <---------- 各層の重みを0から1の乱数で初期化 ---------->
  double wh[HIDDENNO][INPUTNO + 1]; // 入力層から隠れ層の重み
  double wo[HIDDENNO + 1];          // 隠れ層から出力層の重み

  srand((unsigned)time(NULL));

  for (int j = 0; j < HIDDENNO; j++) {
    for (int k = 0; k < INPUTNO + 1; k++) {
      wh[j][k] = ((double)rand() / ((double)RAND_MAX + 1));
    }
  }
  for (int j = 0; j < HIDDENNO + 1; j++) {
    wo[j] = ((double)rand() / ((double)RAND_MAX + 1));
  }

  // // <---------- 学習 ---------->
  double hidden[DATANO][HIDDENNO]; // 隠れ層での出力値
  double output[DATANO];           // 出力層での出力値(学習結果)
  double epoch = 10000;            // 繰り返し回数
  // double limit = 0.01;             // 誤差がこの値に収束するまで繰り返す
  // double err = DBL_MAX;            // 誤差
  // double err_data[200];    // 誤差の変移
  // int count = 0;           // 繰り返し回数

  for (int i = 0; i < epoch; i++) {
  //   err = 0;
    train(input, wh, hidden, wo, output, teacher); // 学習
  //   err += pow((output[i] - teacher[i]), 2);                 // 誤差を計算
  //   err_data[count] = err;
  //   count++;
  }

  // // 学習済みデータの出力
  // save_data("out/data_after.csv", input, teacher, output, n);

  // // 誤差の変移の出力
  // save_err("out/err.csv", err_data, count);

  return 0;
}

// 学習(input: 入力, wh: 入力層から隠れ層の重み, hidden: 隠れ層での出力値, wo: 隠れ層から出力層の重み, output: 出力層での出力値, teacher: 教師データ)
void train(double input[][INPUTNO], double wh[][INPUTNO + 1], double hidden[][HIDDENNO], double wo[], double output[], double teacher[]) {
  double temp;         // 出力値の一時保存
  double alpha = 0.01; // 学習係数

  for (int i = 0; i < DATANO; i++) {
    // <---------- forward ---------->
    // 隠れ層での出力値を求める
    for (int j = 0; j < HIDDENNO; j++) {
      temp = 0;
      for (int k = 0; k < INPUTNO; k++) {
        temp += input[i][k] * wh[j][k];
      }
      temp -= wh[j][INPUTNO];
      hidden[i][j] = f_sigmoid(temp);
    }

    // 出力層での出力値を求める
    temp = 0;
    for (int j = 0; j < HIDDENNO; j++) {
      temp += hidden[i][j] * wo[j];
    }
    temp -= wo[HIDDENNO];
    output[i] = f_sigmoid(temp);

    // <----------- backward ---------->
    // 隠れ層から出力層の重みを更新
    for (int j = 0; j < HIDDENNO; j++) {
      wo[j] -= alpha * hidden[i][j] * d_sigmoid(output[i]) * (output[i] - teacher[i]);
    }
    wo[HIDDENNO] -= alpha * (-1) * d_sigmoid(output[i]) * (output[i] - teacher[i]);

    // 入力層から隠れ層の重みを更新
    for (int k = 0; k < INPUTNO; k++) {
      for (int j = 0; j < HIDDENNO; j++) {
        wh[j][k] -= alpha * input[i][k] * d_sigmoid(hidden[i][j]) * d_sigmoid(output[i]) * (output[i] - teacher[i]) * wo[j];
      }
    }
    for (int j = 0; j < HIDDENNO; j++) {
      wh[j][INPUTNO] -= alpha * (-1) * d_sigmoid(hidden[i][j]) * d_sigmoid(output[i]) * (output[i] - teacher[i]) * wo[j];
    }
  }
}

// データ読み込み(fileloc: 読み込むファイル, input: input保存用, teacher: 教師データ保存用)
void read_data(char *fileloc, double input[][INPUTNO], double teacher[]) {
  FILE *data;
  char buf[256];
  int m, n;      // 列番号, 行番号

  if ((data = fopen(fileloc, "r")) == NULL) exit(1);

  n = 0;
  while (fgets(buf, sizeof(buf), data) != NULL) {
    m = 0;
    char *token = strtok(buf, " ");

    while (token != NULL) {
      if (m < 3) input[n][m] = atof(token); // 最初の3つはinputに格納
      else teacher[n] = atof(token);        // 最後の1つはteacherに格納

      token = strtok(NULL, " ");
      m++;
    }

    n++;
  }

  fclose(data);
}

// // データを保存(filename: 保存するファイル名, input: input, teacher: 教師データ, output: 学習結果, n: 行数)
// void save_data(char filename[], double input[][INPUTNO], double teacher[], double output[], int n) {
//   FILE *data;

//   if ((data = fopen(filename, "w")) == NULL) exit(1);

//   for (int i = 0; i < n; i++) {
//     for (int j = 0; j < INPUTNO; j++) {
//       fprintf(data, "%d,", (int)input[i][j]);               // inputを記述
//     }
//     fprintf(data, "%d,%.4f\n", (int)teacher[i], output[i]); // 教師データと学習結果を記述
//   }

//   fclose(data);
// }

// // エラーの変移を保存(filaname: 保存するファイル名, err_data: エラーの変移, n: 行数)
// void save_err(char filename[], double err_data[], int n) {
//   FILE *data;

//   if ((data = fopen(filename, "w")) == NULL) exit(1);

//   for (int i = 0; i < n; i++) {
//     fprintf(data, "%d,%.4f\n", i, err_data[i]);
//   }

//   fclose(data);
// }

// シグモイド関数
double f_sigmoid(double x) {
  return 1 / (1 + exp(-x));
}

// シグモイド関数の微分
double d_sigmoid(double x) {
  return x * (1 - x);
}
