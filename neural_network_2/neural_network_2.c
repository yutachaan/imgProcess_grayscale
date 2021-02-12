#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>

#define INPUTNO 3
#define MAXINPUTNO 10
#define HIDDENNO 3
#define LIMIT 0.01
#define ALPHA 0.1

double f_sigmoid(double u);
void read_data(char *fileloc, double input[][INPUTNO], double teacher[], int *n);
void save_data(char filename[], double input[][INPUTNO], double teacher[], double output[], int n);
void save_err(char filename[], double err_data[], int n);
double forward(double input[][INPUTNO], double wh[][INPUTNO + 1], double wo[], double hi[], int i);
void olearn(double teacher, double wo[], double hi[], double output);
void hlearn(double input[][INPUTNO], double teacher, double wh[][INPUTNO + 1], double wo[], double hi[], double output, int i);

int main(int argc, char *argv[]) {
  int n;                             // 行数
  double input[MAXINPUTNO][INPUTNO]; // インプット
  double teacher[MAXINPUTNO];        // 教師データ
  double output[MAXINPUTNO];         // 学習結果

  // <---------- データの読み込み ---------->
  read_data(argv[1], input, teacher, &n);

  // <---------- 各層の重みを乱数で初期化 ---------->
  double wh[HIDDENNO][INPUTNO + 1]; // 隠れ層
  double wo[HIDDENNO + 1];          // 出力層

  srand((unsigned int)time(NULL));

  for (int i = 0; i < HIDDENNO; i++) {
    for (int j = 0; j <= INPUTNO; j++) {
      wh[i][j] = ((double)rand() / ((double)RAND_MAX + 1));
    }
  }
  for (int i = 0; i <= HIDDENNO; i++) {
    wo[i] = ((double)rand() / ((double)RAND_MAX + 1));
  }

  // <---------- 学習 ---------->
  double hidden[HIDDENNO]; // 隠れ層での値
  double err = DBL_MAX;    // 誤差
  double err_data[200];    // 誤差の変移
  int count = 0;           // 繰り返し回数

  while (count < 230) {
    err = 0;
    for (int i = 0; i < n; i++) {
      output[i] = forward(input, wh, wo, hidden, i);           // 出力値を計算
      olearn(teacher[i], wo, hidden, output[i]);               // 出力層の重みを更新
      hlearn(input, teacher[i], wh, wo, hidden, output[i], i); // 隠れ層の重みを更新
      err += pow((output[i] - teacher[i]), 2);                 // 誤差を計算
    }
    err_data[count] = err;
    count++;
  }

  // 学習済みデータの出力
  save_data("out/data_after.csv", input, teacher, output, n);

  // 誤差の変移の出力
  save_err("out/err.csv", err_data, count);

  return 0;
}

// 出力値を計算(input: input, wh: 隠れ層の重み, wo: 出力層の重み, hi: 隠れ層での値, i: 何行目か)
double forward(double input[][INPUTNO], double wh[][INPUTNO + 1], double wo[], double hi[], int i) {
  double u, o; // 入力に重みを掛けた値(u: 隠れ層, o: 出力層)

  // 隠れ層の値を求める
  for (int j = 0; j < HIDDENNO; j++) {
    u = 0;
    for (int k = 0; k < INPUTNO; k++) u += input[i][k] * wh[j][k];
    u -= wh[j][INPUTNO];
    hi[j] = f_sigmoid(u);
  }

  // 出力層の値を求めて返却
  o = 0;
  for (int j = 0; j < HIDDENNO; j++) o += hi[j] * wo[j];
  o -= wo[HIDDENNO];
  return f_sigmoid(o);
}

// 出力層の重みの更新(teacher: 教師データ, wo: 出力層の重み, hi: 隠れ層の値, output: 出力値)
void olearn(double teacher, double wo[], double hi[], double output) {
  double d = (output - teacher) * output * (1 - output);

  // 重みを更新
  for (int i = 0; i < HIDDENNO; i++) {
    wo[i] -= ALPHA * d * hi[i];
  }

  // 閾値を更新
  wo[HIDDENNO] -= ALPHA * d * (-1);
}

// 隠れ層の重みを更新(input: input, teacher: 教師データ, wh: 隠れ層の重み, wo: 出力層の重み, hi: 隠れ層の値, output: 出力値, i: 何行目か)
void hlearn(double input[][INPUTNO], double teacher, double wh[][INPUTNO + 1], double wo[], double hi[], double output, int i) {
  double d = (teacher - output) * output * (1 - output);

  for (int j = 0; j < HIDDENNO; j++) {
    // 重みを更新
    for (int k = 0; k < INPUTNO; k++) {
      wh[j][k] += ALPHA * hi[j] * (1 - hi[j]) * wo[j] * d * input[i][k];
    }

    // 閾値を更新
    wh[j][INPUTNO] += ALPHA * hi[j] * (1 - hi[j]) * wo[j] * d * (-1);
  }
}

// データ読み込み(fileloc: 読み込むファイル, input: input保存用, teacher: 教師データ保存用, n: 行数保存用)
void read_data(char *fileloc, double input[][INPUTNO], double teacher[], int *n) {
  FILE *data;
  int mm, nn;
  char buf[256];

  if ((data = fopen(fileloc, "r")) == NULL) exit(1);

  nn = 0;
  while (fgets(buf, sizeof(buf), data) != NULL) {
    mm = 0;
    char *token = strtok(buf, " ");

    while (token != NULL) {
      if (mm < 3) input[nn][mm] = atof(token); // 最初の3つはinputに格納
      else teacher[nn] = atof(token);          // 最後の1つはteacherに格納

      token = strtok(NULL, " ");
      mm++;
    }

    nn++;
  }

  fclose(data);

  *n = nn;
}

// データを保存(filename: 保存するファイル名, input: input, teacher: 教師データ, output: 学習結果, n: 行数)
void save_data(char filename[], double input[][INPUTNO], double teacher[], double output[], int n) {
  FILE *data;

  if ((data = fopen(filename, "w")) == NULL) exit(1);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < INPUTNO; j++) {
      fprintf(data, "%d,", (int)input[i][j]);               // inputを記述
    }
    fprintf(data, "%d,%.4f\n", (int)teacher[i], output[i]); // 教師データと学習結果を記述
  }

  fclose(data);
}

// エラーの変移を保存(filaname: 保存するファイル名, err_data: エラーの変移, n: 行数)
void save_err(char filename[], double err_data[], int n) {
  FILE *data;

  if ((data = fopen(filename, "w")) == NULL) exit(1);

  for (int i = 0; i < n; i++) {
    fprintf(data, "%d,%.4f\n", i, err_data[i]);
  }

  fclose(data);
}

// シグモイド関数
double f_sigmoid(double u) {
  return 1 / (1 + exp(-u));
}
