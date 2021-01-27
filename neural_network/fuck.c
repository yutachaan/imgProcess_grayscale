#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define INPUTNO 2
#define MAXINPUTNO 100

double f_step(double u);
double f_sigmoid(double u);
void initw(double w[INPUTNO + 1]);
double forward(double w[INPUTNO + 1], double e[INPUTNO]);
void getdata(double data_[][INPUTNO], int *mm, int *nn);
void savedata(double data_[][INPUTNO], double output[MAXINPUTNO], int *mm, int *nn);

int main() {
	double weight_[INPUTNO + 1];       // weights and threshold
	double output[MAXINPUTNO];
	int mm, nn;
	double data_[MAXINPUTNO][INPUTNO]; // dataset

	initw(weight_);
	getdata(data_, &mm, &nn);

	for (int i = 0; i < nn; i++) {
		output[i] = forward(weight_, data_[i]);
	}

	savedata(data_, output, &mm, &nn);

	return 0;
}

void initw(double weight_[INPUTNO + 1]) {
	weight_[0] = 1;
	weight_[1] = 1;
	weight_[2] = 0.5; // threshold
}

double forward(double weight_[INPUTNO + 1], double e[INPUTNO]) {
	double u = 0; // buffer
	double o;     // output value

	for (int i = 0; i < INPUTNO; i++) {
		u += e[i] * weight_[i];
	}
	u -= weight_[INPUTNO];

	o = f_step(u);
	// o = f_sigmoid(u);

	return o;
}

double f_step(double u) {
	if (u >= 0) return 1.0;
	else return 0.0;
}

double f_sigmoid(double u) {
	return 1.0 / (1.0 + exp(-u));
}

void getdata(double data_[][INPUTNO], int *mm_, int *nn_) {
	FILE *fin;
	char readline[256];
	int mm, nn;

	char temp_str;

	if ((fin = fopen("./data/data01.txt", "r")) == NULL) exit(1);

	nn = 0;
	while (fgets(readline, sizeof(readline), fin) != NULL) {
		char *token = strtok(readline, " ");
		mm = 0;
		while (token != NULL) {
			data_[nn][mm] = atof(token);
			token = strtok(NULL, " ");
			mm++;
		}
		nn++;
	}

	fclose(fin);

	*mm_ = mm;
	*nn_ = nn;
}

void savedata(double data_[][INPUTNO], double output[MAXINPUTNO], int *mm_, int *nn_) {
	FILE *fin;
	int nx;

	if ((fin = fopen("./data/data_saved.txt", "w")) == NULL) exit(1);

	fprintf(fin, "# x y z\n");
	for (int i = 0; i < *nn_; i++) {
		for (int j = 0; j < *mm_; j++) {
			fprintf(fin, "\t%.4f", data_[i][j]);
		}
		fprintf(fin, "\t%.4f\n", output[i]);
	}

	fclose(fin);
}
