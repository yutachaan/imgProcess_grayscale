#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUTNO 2
#define MAXINPUTNO 500

void read_data(double data[][INPUTNO], double data_category[], int *m_, int *n_);
void savedata(double data[][INPUTNO], double output[MAXINPUTNO], int *m_, int *n_);

int main() {
		int m_, n_;
		double data[MAXINPUTNO][INPUTNO]; // データセット
		double data_category[MAXINPUTNO]; // 分類済みデータのカテゴリ
		double output[MAXINPUTNO];        // 未分類データのクラスタリング後のカテゴリ

		read_data(data, data_category, &m_, &n_);

		// ここをクラスタリング処理に変える
		for (int i = 0; i < n_; i++) {
			output[i] = data_category[i];
		}

		savedata(data, output, &m_, &n_);

	return 0;
}

// データを読み込む
void read_data(double data[][INPUTNO], double data_category[], int *m_, int *n_) {
	FILE *fin;
	int m, n;      // 列数、行数
	char buf[256];

	if ((fin = fopen("resources/data_01.txt", "r")) == NULL) exit(1);

	n = 0;
	while (fgets(buf, sizeof(buf), fin) != NULL) {
		char *token = strtok(buf, " ");        // 空白で区切った字句へのポインタ
		m = 0;

		// 字句が見つかった時
		while (token != NULL) {
			if (m < 2) data[n][m] = atof(token); // 最初の2つの字句はdoubleへ変換してdataに代入
			else data_category[n] = atof(token); // 最後の1つの字句はdoubleへ変換してdata_categoryに代入
			token = strtok(NULL, " ");           // 次の字句へ移動
			m++;
		}

		n++;
	}

	fclose(fin);

	*m_ = 2;
	*n_ = n;
}

// クラスタリング後のデータを保存
void savedata(double data[][INPUTNO], double output[MAXINPUTNO], int *m_, int *n_) {
	FILE *fin;
	int m, n;

	if ((fin = fopen("data_saved.txt", "w")) == NULL) exit(1);

	fprintf(fin, "# x y z\n");

	// データとカテゴリを出力
	for (int i = 0; i < *n_; i++) {
		for (int j = 0; j < *m_; j++)	{
			fprintf(fin, "\t%.4f", data[i][j]);
		}
		fprintf(fin, "\t%.4f\n", output[i]);
	}

	fclose(fin);
}
