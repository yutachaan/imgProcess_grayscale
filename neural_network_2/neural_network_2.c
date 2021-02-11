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
void read_data(char *fileloc, double input[][INPUTNO], int teacher[], int *n);
void save_data(char filename[], double input[][INPUTNO], int teacher[], double output[], int n);
double forward(double e[], double wh[][INPUTNO + 1], double wo[], double hi[]);
void olearn(int teacher, double wo[], double hi[], double output);

int main(int argc, char *argv[]) {
  int n;                             // 行数
  double input[MAXINPUTNO][INPUTNO]; // インプット
  int teacher[MAXINPUTNO];           // 教師データ
  double output[MAXINPUTNO];         // 学習結果

  // <---------- データの読み込み ---------->
  read_data(argv[1], input, teacher, &n);

  // <---------- 各層の重みを乱数で初期化 ---------->
  double wh[HIDDENNO][INPUTNO + 1]; // 隠れ層
  double wo[HIDDENNO + 1];          // 出力層

  srand((unsigned int)time(NULL));

  for (int i = 0; i < HIDDENNO; i++) {
    for (int j = 0; j <= INPUTNO; j++) {
      wh[i][j] = rand() / (double)RAND_MAX;
    }
  }

  for (int i = 0; i <= HIDDENNO; i++) {
    wo[i] = rand() / (double)RAND_MAX;
  }

  // <---------- 学習 ---------->
  double hidden[HIDDENNO]; // 隠れ層での値
  double err = DBL_MAX;    // 誤差
  while (err > LIMIT) {
    err = 0;
    for (int i = 0; i < n; i++) {
      output[i] = forward(input[i], wh, wo, hidden); // 出力値を計算
      olearn(teacher[i], wo, hidden, output[i]);     // 出力層の重みを更新
    }
  }

  // 学習済みデータの出力
  save_data("out/data_after.csv", input, teacher, output, n);

  return 0;
}

// 出力値を計算(data: 1行文のインプット, wh: 隠れ層の重み, wo: 出力層の重み, hi: 隠れ層での値)
double forward(double data[], double wh[][INPUTNO + 1], double wo[], double hi[]) {
  double u, o; // 入力に重みを掛けた値(u: 隠れ層, o: 出力層)

  // 隠れ層の値を求める
  for (int i = 0; i < HIDDENNO; i++) {
    u = 0;
    for (int j = 0; j < INPUTNO; j++) u += data[j] * wh[i][j];
    u -= wh[i][INPUTNO];
    hi[i] = f_sigmoid(u);
  }

  // 出力層の値を求めて返却
  o = 0;
  for (int i = 0; i < HIDDENNO; i++) o += hi[i] * wo[i];
  o -= wo[HIDDENNO];
  return f_sigmoid(o);
}

// 出力層の重みの更新(teacher: 教師データ, wo: 出力層の重み, hi: 隠れ層の値, output: 出力値)
void olearn(int teacher, double wo[], double hi[], double output) {
  // 重みを更新
  for (int i = 0; i < HIDDENNO; i++) {
    wo[i] += ALPHA * (teacher - output) * output * (1 - output) * hi[i];
  }

  // 閾値を更新
  wo[HIDDENNO] += ALPHA * (teacher - output) * output * (1 - output) * (-1);
}

// データ読み込み(fileloc: 読み込むファイル, input: input保存用, teacher: 教師データ保存用, n: 行数保存用)
void read_data(char *fileloc, double input[][INPUTNO], int teacher[], int *n) {
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
      else teacher[nn] = atoi(token);          // 最後の1つはteacherに格納

      token = strtok(NULL, " ");
      mm++;
    }

    nn++;
  }

  fclose(data);

  *n = nn;
}

// データを保存(filename: 保存するファイル名, input: input, teacher: 教師データ, output: 学習結果, n: 行数)
void save_data(char filename[], double input[][INPUTNO], int teacher[], double output[], int n) {
  FILE *data;

  if ((data = fopen(filename, "w")) == NULL) exit(1);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 3; j++) {
      fprintf(data, "%d,", (int)input[i][j]);          // inputを記述
    }
    fprintf(data, "%d,%.4f\n", teacher[i], output[i]); // 教師データと学習結果を記述
  }

  fclose(data);
}

// シグモイド関数
double f_sigmoid(double u) {
  return 1.0 / (1.0 + exp(-u));
}
