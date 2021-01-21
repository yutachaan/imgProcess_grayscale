#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define GNUPLOT_PATH "C:/gnuplot/bin/gnuplot.exe"

#define INPUTNO 2       // Number of inputs
#define MAXINPUTNO 500  // Maximum size of data
#define N 256

void plot_data(void);
void getdata(double data_[][INPUTNO], double data_category[], int *mm, int *nn);
void savedata(double data_[][INPUTNO], double output[MAXINPUTNO], int *mm, int *nn);


int main() {
		int ii, jj;
		int mm, nn;
		double data_[MAXINPUTNO][INPUTNO]; // dataset
		double data_category[MAXINPUTNO];  // data category
		double output[MAXINPUTNO];         // output value*

		getdata(data_, data_category, &mm, &nn);

		printf("Number of data:%d\n", nn);
		printf("------------------------\n");
		printf("Input data        |  output\n", nn);

		// Computation should be done here
		for (ii = 0; ii<nn; ii++) {
			printf("%d:", ii);
			for (jj = 0; jj<INPUTNO; jj++)	printf("%lf ", data_[ii][jj]);
			output[ii] = data_category[ii];
			printf(" | %lf\n", output[ii]);
		}

		savedata(data_, output, &mm, &nn);
		plot_data();

	return 0;
}

void savedata(double data_[][INPUTNO], double output[MAXINPUTNO], int *mm_, int *nn_) {
	int mm, nn;
	int nx;

	FILE *fin;
	char  filename[] = "./data/data_saved.txt";
	mm = (*mm_);
	nn = (*nn_);
	if ((fin = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Can't open file\n");
	}
	else {
		fprintf(fin, "#\tx\ty\tz\n");
		for (int ii = 0; ii < nn; ii++) {
			for (int jj = 0; jj < mm; jj++)	fprintf(fin, "\t%.4f", data_[ii][jj]);
			fprintf(fin, "\t%.4f\n", output[ii]);
		}
		fclose(fin);
	}

	return;
}

void plot_data() {
	FILE *gp;
	// Call Gnuplot
	if ((gp = _popen(GNUPLOT_PATH, "w")) == NULL) {   //Use pipe to boot gnuplot
		fprintf(stderr, "Not Found %s.", GNUPLOT_PATH);
		exit(EXIT_FAILURE);
	}
	fprintf(gp, "set term wxt  0\n");
	fprintf(gp, "reset\n");
	fprintf(gp, "unset label\n");
	fprintf(gp, "unset key\n");
	fprintf(gp, "unset grid\n");
	fprintf(gp, "show grid\n");
	fprintf(gp, "set title \"Data\"\n");
	fprintf(gp, "set xlabel \"x\"\n");
	fprintf(gp, "set ylabel \"y\"\n");
	fprintf(gp, "set terminal wxt size 1000, 1000 font \"Verdana,10\"\n");
	fprintf(gp, "set size ratio - 1\n");
	fprintf(gp, "set palette rgbformulae 22, 13, -31\n");
	fprintf(gp, "set pm3d map # no interpolation\n");
	fprintf(gp, "set term wxt  1\n");
	fprintf(gp, "splot 'data/data_saved.txt' using 1:2:3 with points palette pointsize 1 pointtype 7\n");

	fflush(gp);            //Flush the buffer
	system("pause");
	fprintf(gp, "exit\n"); //End the Gnu plot
	_pclose(gp);

	return;
}



void getdata(double data_[][INPUTNO], double data_category[], int *mm_, int *nn_) {
	int mm, nn;
	char  filename[] = "./data/data_01.txt";
	char readline[N] = { '\0' };

	char temp_str;
	FILE *fin;
	char delim[] = " ";

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Can't open file\n");
	}

	nn = 0;
	while (fgets(readline, N, fin) != NULL) {
		char * token = strtok(readline, delim);
		mm = 0;
		while (token != NULL) {
			if (mm<2) {
				data_[nn][mm] = atof(token);
				//printf("%3.3g ", data_[nn][mm]);
			}
			else {
				data_category[nn] = atof(token);
				//printf(" category=%3.3g\n ", data_category[nn]);
			}
			token = strtok(NULL, delim);
			mm++;
		}
		nn++;
	}
	fclose(fin);
	mm = 2;
	(*mm_) = mm;
	(*nn_) = nn;

	return;
}
