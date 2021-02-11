#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INPUTNO 3
#define MAXINPUTNO 8

double f_sigmoid(double u);
void read_data(char *fileloc, double input[][INPUTNO], int teacher[], int *n);
void save_data(char filename[], double input[][INPUTNO], int teacher[], double output[], int n);

int main(int argc, char *argv[]) {
  int n;
  double input[MAXINPUTNO][INPUTNO];
  int teacher[MAXINPUTNO];
  double output[MAXINPUTNO];

  // データの読み込み
  read_data(argv[1], input, teacher, &n);

  // 学習済みデータの出力
  save_data("out/data_after.csv", input, teacher, output, n);
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
