/*
 * Copyright (C) 2007 Robotics@Maryland
 * Copyright (C) 2007 Leo Singer <aronnax@umd.edu>
 * All rights reserved.
 *
 * Author: Leo Singer <aronnax@umd.edu>
 * File: fftw_on_mini_bench.cpp
 *
 * FFTW Benchmark for the Mac Mini
 *
 * This program simulates the processing load for performing FFTs on hydrophone data.
 * It compares two different transform planners from FFTW-3.
 *
 */


#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fftw3.h>

void bench_double_r2c_single(int N, int planner_flags, int trials1 = 10, int trials2 = 10000);
void bench_double_r2c_many(int N, int planner_flags, int howmany = 10, int trials1 = 10, int trials2 = 1000);
void fill_sonar_pulse(double *in_re, int N, double samprate, double amplitude, double freq, double phase, double snr);

int main (int argc, char * const argv[]) {
	int N = 1024;  //  Number of points in the data window
	int planner_flags = FFTW_PATIENT;
	std::cout << "FFTW Benchmark: " << N << " point transforms" << std::endl;
	bench_double_r2c_single(N, planner_flags);
	std::cout << std::endl << std::endl;
	bench_double_r2c_many(N, planner_flags);
    return 0;
}

void fill_sonar_pulse(double *in_re, int N, double samprate = 1.0e+6, double amplitude = 1, double freq = 30.0e+3, double phase = 0.2, double snr = 1000) {
	for (int i = 0 ; i < N ; i ++) {
		in_re[i] = amplitude * (cos(2 * M_PI * freq * i / samprate - phase) + (double) rand() / RAND_MAX / snr);
	}
}

void bench_double_r2c_single(int N, int planner_flags, int trials1, int trials2) {
	double *in_re;
	fftw_complex *out_c;
	fftw_plan p;
	timeval start, finish;
	double dt, sumt = 0;
	in_re = (double*) fftw_malloc(sizeof(double) * N);
	out_c = (fftw_complex*) fftw_malloc(sizeof(double) * N * 2);
	fill_sonar_pulse(in_re, N);
	p = fftw_plan_dft_r2c_1d(N,in_re,out_c,planner_flags);
	
	std::cout << "Single planned real to complex transform" << std::endl;
	std::cout << "Planner: " << std::endl;
	fftw_print_plan(p);
	
	std::cout << std::endl;
	
	for (int i = 0 ; i < trials1 ; i ++) {
		fill_sonar_pulse(in_re, N);
		gettimeofday(&start, NULL);
		for (int j = 0 ; j < trials2 ; j ++)
			fftw_execute(p);
		gettimeofday(&finish, NULL);
		dt = (finish.tv_sec+(finish.tv_usec/1000000.0)-(start.tv_sec+(start.tv_usec/1000000.0))) / trials2;
		sumt += dt;
		std::cout << "Trial " << i + 1 << " of " << trials1 << ": " << dt << std::endl ;
	}
	std::cout << "Average: " << sumt / trials1 << std::endl;
	
	fftw_free(in_re);
	fftw_free(out_c);
}

void bench_double_r2c_many(int N, int planner_flags, int howmany, int trials1, int trials2) {
	double *in_re;
	fftw_complex *out_c;
	fftw_plan p;
	timeval start, finish;
	double dt, sumt = 0;
	in_re = (double*) fftw_malloc(sizeof(double) * N * howmany);
	out_c = (fftw_complex*) fftw_malloc(sizeof(double) * N * 2 * howmany);
	fill_sonar_pulse(in_re, N * howmany);
	int dim_out = 2 * N;
	p = fftw_plan_many_dft_r2c(1, &N, howmany, in_re, &N, 1, N, out_c, &dim_out, 1, N, planner_flags);
	
	std::cout << "Multiple simultaneous planned real to complex transform" << std::endl;
	std::cout << howmany << " data vectors" << std::endl;
	std::cout << "Planner: " << std::endl;
	fftw_print_plan(p);
	
	std::cout << std::endl;
	
	for (int i = 0 ; i < trials1 ; i ++) {
		fill_sonar_pulse(in_re, N * howmany);
		gettimeofday(&start, NULL);
		for (int j = 0 ; j < trials2 ; j ++)
			fftw_execute(p);
		gettimeofday(&finish, NULL);
		dt = (finish.tv_sec+(finish.tv_usec/1000000.0)-(start.tv_sec+(start.tv_usec/1000000.0))) / trials2;
		sumt += dt;
		std::cout << "Trial " << i + 1 << " of " << trials1 << ": " << dt 
			<< "; " << dt / howmany << " per vector" << std::endl ;
	}
	std::cout << "Average: " << sumt / trials1 << "; " << sumt / trials1 / howmany << " per vector" << std::endl;
	
	fftw_free(in_re);
	fftw_free(out_c);
}
