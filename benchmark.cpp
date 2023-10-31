//#include <time.h>
#include "util.h"
#include <vector>


#include "timsort.hpp"
#include "pdqsort.h"
#include "quadsort.hpp"
#include "tqsort.hpp"
//#include "quadsort.c"

#include <stdio.h>
#include <iostream>
#include <windows.h>



namespace std {
	int globalcomparecount;
	template <>
	struct std::less<char*> {
		bool operator()(const char* x1, const char* x2) const
		{
			//globalcomparecount++;
			return strcmp(x1, x2) < 0;
		}
	};

	template <>
	struct std::less<int*> {
		bool operator()(const int* x1, const int* x2) const
		{
			//globalcomparecount++;
			return *x1 < *x2;
		}
	};
	template <>
	struct std::less<int> {
		bool operator()(const int x1, const int x2) const
		{
			//globalcomparecount++;
			return x1 < x2;
		}
	};
	template <>
	struct std::greater<int*> {
		bool operator()(const int* x1, const int* x2) const
		{
			//globalcomparecount++;
			return *x2 - *x1;
		}
	};
}






//check array. array2 is correct
template<class T, class Compare>
bool check(T** array, T** array2, int m, int p, Compare compare) {
	int i = 0;
	bool b;
	for (int j = 0; j < p; j++) {
		while (i < m - 1) {
			//printf("%d ",i);
			b = compare(array[j][i], array2[j][i]) || compare(array2[j][i], array[j][i]);
			//printf("a");
			//printf("%d %d %d\n", array[j][i], array2[j][i], array[j][i+i]);
			if (b) {
				printf("%d %d\n", compare(array[j][i], array2[j][i]), compare(array2[j][i], array[j][i]));
				printf("found: ");
				std::cout << array[j][i];
				printf(",should be: ");
				std::cout << array2[j][i];
				printf(" run %d, element %d\n", j, i);
				//printf("failed %s %s %p ",,, array);
				//printf("%d %s %s %d %s",i,array[i],array[i+1],strcmp(array[i+1],array[i]),strcmp(array[i+1],array[i])<0?"true":"false");
				//printf("%s ",b?"true":"false");
				return false;
			}
			i++;
		}
	}
	//printf("check success\n");
	return true;
}



int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}


char*** allocatestringarray(size_t arraysize, size_t numarrays,int slength) {
	char*** array = (char***)malloc(numarrays * sizeof(char**));

	for (int i = 0; i < numarrays; i++) {
		array[i] = (char**)malloc(arraysize * sizeof(char*));
		array[i][0] = (char*)malloc(arraysize * (slength + 1) * sizeof(char));
		for (int j = 0; j < arraysize; j++) {
			array[i][j] = array[i][0] + j * (slength + 1);
		}
	}
	return array;
}
void deallocatestringarray(char*** arrays,size_t arraysize, size_t numarrays) {

	for (int i = 0; i < numarrays; i++) {
		free(arrays[i][0]);
		free(arrays[i]);
	}
	free(arrays);
}
void  makestringarrays(char*** arrays, size_t arraysize, size_t numarrays, float sortedchance, int slength, int repeats) {
	int r;
	unsigned long long b, d;
	double ratio=1;
	for (int i = 0; i < slength; i++) {
		ratio = ratio * 26;//maximum value for integers to be converted to string
	}
	ratio = ratio / arraysize * repeats;
	for (int i = 0; i < numarrays; i++) {
		srand(time(NULL) + i);
		for (int j = 0; j < arraysize; j++) {
			if ((double)rand() / RAND_MAX < sortedchance) {
				//generate sorted string
				b = j;
			}
			else {
				//generate random string
				b= rand() * arraysize / RAND_MAX;
			}
			//make string from integer
			b = (b / repeats); //integer division creates repeats
			b=b* ratio;
			for (int k= slength-1;k>=0;k--) {
				d = b % 26;
				//printf("a %d %d\n", b,d);
				arrays[i][j][k] = 97 + d;
				b = b / 26;
					
			}
			arrays[i][j][slength] = 0;//null terminate c string
		}
	}
}


int** allocatearrays(size_t arraysize, size_t numarrays) {
	int** arrays = (int**)malloc(numarrays * sizeof(int*));
	if (arrays == NULL) {
		return NULL;
	}
	for (int i = 0; i < numarrays; i++) {
		arrays[i] = (int*)malloc(arraysize * sizeof(int));
	}
	return arrays;
}
void deallocatearrays(int** arrays,size_t arraysize, size_t numarrays) {
	for (int i = 0; i < numarrays; i++) {
		free(arrays[i]);
	}
	free(arrays);
}

int** makeintarrays(int** arrays,size_t arraysize, size_t numarrays, float sortedchance, int repeats) {
	double r;
	int d;
	for (int i = 0; i < numarrays; i++) {
		if (arrays[i] == NULL) {
			return NULL;
		}
		srand(time(NULL) + i);
		for (int j = 0; j < arraysize; j++) {
			r = (double)(rand()) / RAND_MAX;
			
			if (r < sortedchance) {
				//sorted int
				d = j;
			}
			else {
				//random int
				d = (double)(rand()) / RAND_MAX * arraysize;
				
			}
			arrays[i][j] = (d / repeats);
		}
			
	}
	return arrays;
}







void benchmarkstring(char*** arrays, std::vector<char*>* v, int arraysize, int numarrays, std::vector<float> sortedness,int slength,float*total) {
	
	
	struct timeval stop, start;
	double sec;
	std::vector<std::vector<double>>seconds(sortedness.size());

	for (int i = 0; i < sortedness.size(); i++)
	{
		seconds[i] = std::vector<double>(4);
		makestringarrays(arrays, arraysize, numarrays, sortedness[i], slength, 4);
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
		}

		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			gfx::timsort(v[i]);
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][0] = sec;
		total[0] += sec;
		//////////////
		//makestringarrays(arrays, arraysize, numarrays, sortedness, slength,4);
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
		}


		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			pdqsort(v[i].begin(), v[i].end(), std::less<char*>());
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][1] = sec;
		total[1] += sec;
		////////////////
		//makestringarrays(arrays, arraysize, numarrays, sortedness, slength,4);
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
		}

		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			tqsort((char**)&(v[i][0]), arraysize, std::less<char*>());
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][2] = sec;
		total[2] += sec;

		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
		}
		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			quadsort((char**)&(v[i][0]), arraysize, CMPFUNC<char*>());//
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][3] = sec;
		total[3] += sec;
	}
	printf("arraysize = %d, number of arrays = %d, Time in seconds\n", arraysize, numarrays);
	printf("---------------------\nAlgorithm |");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf(" Sortedness %%%d |", (int)(sortedness[i] * 100));
	}
	printf("\n|---|");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("--- |", (int)(sortedness[i] * 100));
	}
	printf("\n| timsort  | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][0] );
	}
	printf("\n| pdqsort  | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][1] );
	}
	printf("\n| tqsort   | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][2] );
	}
	printf("\n| quadsort | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][3] );
	}
	printf("\n\n");
}


void benchmarkint(int** arrays, std::vector<int>* v, int arraysize, int numarrays, std::vector<float> sortedness, float* total) {

	
	struct timeval stop, start;
	double sec;
	std::vector<std::vector<double>>seconds(sortedness.size());
	

	for (int i = 0 ; i < sortedness.size(); i++)
	{
		seconds[i] = std::vector<double>(4);
		makeintarrays(arrays, arraysize, numarrays, sortedness[i], 4);
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
		}

		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			gfx::timsort(v[i]);
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][0] = sec;
		total[0] += sec;

		//////////////
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
		}
		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			pdqsort(v[i].begin(), v[i].end(), std::less<int>());
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][1] = sec;
		total[1] += sec;

		////////////////
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
		}
		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			tqsort((int*)&(v[i][0]), arraysize, std::less<int>());
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][2] = sec;
		total[2] += sec;

		///////////////////////
		for (int i = 0; i < numarrays; i++) {
			v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
		}
		gettimeofday(&start, NULL);
		for (int i = 0; i < numarrays; i++) {
			quadsort((int*)&(v[i][0]), arraysize, CMPFUNC<int*>());
		}
		gettimeofday(&stop, NULL);
		sec = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
		seconds[i][3] = sec;
		total[3] += sec;
	}

	printf("arraysize = %d, number of arrays = %d, Time in seconds\n", arraysize, numarrays);
	printf("---------------------\n| Algorithm |");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("Sortedness %%%d |", (int)(sortedness[i] * 100));
	}
	printf("\n|---|");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("--- |", (int)(sortedness[i] * 100));
	}
	printf("\n| timsort  | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][0] );
	}
	printf("\n| pdqsort  | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][1] );
	}
	printf("\n| tqsort   | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][2] );
	}
	printf("\n| quadsort | ");
	for (int i = 0; i < sortedness.size(); i++)
	{
		printf("%.3f |", seconds[i][3] );
	}
	printf("\n\n");
	
}

void benchmark() {


	////////
	//ints
	/////////
	
	int arraysize;
	int numarrays;
	int** arrays;
	std::vector<int>* v;
	std::vector<float> sortedness;
	float total[4] = { 0,0,0,0 };
	
	printf("integers\n-------------\n");

	arraysize = 10000000;
	numarrays = 1;
	arrays = allocatearrays(arraysize, numarrays);
	v = new std::vector<int>[numarrays];
	sortedness = {0., 0.8, 0.95};
	benchmarkint(arrays, v, arraysize, numarrays, sortedness, total);

	deallocatearrays(arrays, arraysize, numarrays);
	delete[] v;

	arraysize = 100;
	numarrays = 100000;
	arrays = allocatearrays(arraysize, numarrays);
	v = new std::vector<int>[numarrays];

	sortedness = { 0., 0.9 };
	benchmarkint(arrays, v, arraysize, numarrays, sortedness, total);

	delete[] v;

	

	////////
	//strings
	/////////
	
	char*** arraystring;
	std::vector<char*>* vstring;
	int slength=7;
	
	printf("\nstrings - length %d\n----------------\n", slength);

	arraysize = 10000000;
	numarrays = 1;
	arraystring = allocatestringarray(arraysize, numarrays,slength);
	vstring = new std::vector<char*>[numarrays];
	sortedness = { 0., 0.9 };
	benchmarkstring(arraystring, vstring, arraysize, numarrays, sortedness, slength, total);

	deallocatestringarray(arraystring, arraysize, numarrays);
	delete[] vstring;

	arraysize = 100;
	numarrays = 100000;
	arraystring = allocatestringarray(arraysize, numarrays, slength);
	vstring = new std::vector<char*>[numarrays];

	sortedness = { 0., 0.9 };
	benchmarkstring(arraystring, vstring, arraysize, numarrays, sortedness, slength, total);

	deallocatestringarray(arraystring, arraysize, numarrays);

	delete[] vstring;
	
	printf("timsort total - %f\n", total[0]);
	printf("pdfsort total - %f\n", total[1]);
	printf("tqsort total - %f\n", total[2]);
	printf("quadsort total - %f\n", total[3]);

	
}

void sortstring() {
	size_t arraysize = 64;
	int numarrays = 100000;
	int slength = 7;
	float sortedness = 0.0;
	//int n=20;

	char*** array = allocatestringarray(arraysize, numarrays, slength);
	makestringarrays(array, arraysize, numarrays, sortedness, slength, 4);
	std::vector<char*>* v = new std::vector<char*>[numarrays];
	struct timeval stop, start;
	char** ptr, ** tpr;
	printf("start string!\n\n");

	char**swap= (char**)malloc(arraysize * sizeof(char*));
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<char*>(array[i], array[i] + arraysize);
		//std::sort((char**)&v[i][0], (char**)&v[i][arraysize/2], std::less<char*>());
		//std::sort((char**)&v[i][arraysize/2], (char**)&v[i][arraysize], std::less<char*>());
		//memcpy(swap, (char**)&v[i][0], (arraysize / 2) * sizeof(char**));
	}

	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		ptr = (char**)&v[i][0];
		//memcpy(swap, (char**)&v[i][0], (arraysize/2) * sizeof(char**));
		//std::move((char**)&v[i][arraysize / 2], (char**)&v[i][arraysize] , swap);
		//tq_sort::forward_merge(swap, arraysize / 2, (char**)&v[i][arraysize / 2], arraysize/2, (char**)&v[i][0], std::less<char*>());
		//tq_sort::forward_merge(ptr, arraysize / 2, (char**)&v[i][arraysize / 2], arraysize/2,swap, std::less<char*>());
		//memcpy(swap, (char**)&v[i][0], (arraysize) * sizeof(char**));
		//tq_sort::partial_forward_merge2((char**)&v[i][0],  arraysize/2, (char**)&v[i][arraysize / 2], arraysize/2, swap, std::less<char*>());
		//tq_sort::merge((char**)&v[i][0], arraysize / 2, (char**)&v[i][arraysize / 2], arraysize / 2, swap, std::less<char*>());
		//tq_sort::cross_merge(swap, (char**)&v[i][0], arraysize / 2, arraysize / 2, std::less<char*>());
		//memcpy( (char**)&v[i][0], swap, (arraysize) * sizeof(char**));
		//tq_sort::tqparity_merge(swap, ptr, arraysize / 2, arraysize / 2, std::less<char*>());
// 
		//gfx::timsort(v[i], std::less<char*>());
		tqsort(ptr, arraysize, std::less<char*>());
		//tqsort(v[i].begin(), arraysize, std::less<char*>());
		//pdqsort < std::vector<char*>::iterator, std::less<char*>>(v[i].begin(), v[i].end(), std::less<char*>());
		//quadsort(ptr, arraysize, CMPFUNC<char*>());
	}
	gettimeofday(&stop, NULL);

	for (int i = 0; i < numarrays; i++) {
		std::sort(array[i], array[i] + arraysize, std::less<char*>());
		
	}
	
	long long milliseconds = (stop.tv_sec - start.tv_sec) * 1000LL + (stop.tv_usec - start.tv_usec) / 1000; // calculate milliseconds
	double dif = milliseconds / (double)1000;
	printf("\n%f\n", dif);
	printf("comparecount %d \n", std::globalcomparecount);
	if (check<char*>((char***)&v[0], array, arraysize, numarrays, std::less<char*>())) {

	}
	else {
		if (arraysize <= 1000) {
			printarray<char*>((char**)&v[0][0], arraysize);
			printarray<char*>(array[0], arraysize);
		}
	}
	printf("\ndone\n");
	delete[]v;
}


void sortint() {
	size_t arraysize = 64;
	int numarrays = 100000;
	int sortedness = 0.0;

	int** array;
	//int* swap = (int*)malloc(arraysize * sizeof(int));
	std::vector<int>* v = new std::vector<int>[numarrays];
	//int* array = makeintarray(n, n);//m

	struct timeval stop, start;

	array = allocatearrays(arraysize, numarrays);
	makeintarrays(array, arraysize, numarrays, sortedness, 4);
	//printarray<int>(array[0], arraysize);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<int>(array[i], array[i] + arraysize);
		//std::sort((int*)&v[i][0], (int*)&v[i][arraysize / 2], std::less<int>());
		//std::sort((int*)&v[i][arraysize / 2], (int*)&v[i][arraysize], std::less<int>());
		//printarray<int>(array[0], arraysize);
		std::sort(array[i], array[i] + arraysize, std::less<int>());
		//printarray<int>(array[0], arraysize);
	}


	printf("start!\n\n");
	gettimeofday(&start, NULL);
	int* ptr, * tpr;
	bool b = 0;
	for (int i = 0; i < numarrays; i++) {
		ptr = (int*)&v[i][0];
		tpr = (int*)( & v[i][0]) + arraysize;
		//tq_sort::partial_forward_merge2((int*)&v[i][0],  arraysize/2, (int*)&v[i][arraysize / 2], arraysize/2, swap, std::less<int>());
		//tq_sort::merge((int*)&v[i][0], arraysize / 2, (int*)&v[i][arraysize / 2], arraysize / 2, swap, std::less<int>());
		//tq_sort::cross_merge(swap, (int*)&v[i][0], arraysize / 2, arraysize / 2, std::less<int>());


		//gfx::timsort(v[i]);
		//tqsort(ptr, arraysize, std::less<int>());
		tqsort(v[i].begin(), arraysize, std::less<int>());
		//pdqsort_branchless(ptr, tpr, std::less<int>());
		//quadsort((int*)&v[i][0], arraysize, CMPFUNC<int*>());
	}
	gettimeofday(&stop, NULL);
	double seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000; // calculate milliseconds
	//printf("\n%d",milliseconds);
	printf("\n%f\n", seconds);
	printf("bool %d\n", b);
	printf("comparecount %d \n", std::globalcomparecount);
	//check2<int>(array, m, p, compareint);
	if (check<int>((int**)&v[0], array, arraysize, 1, std::less<int>())) {
		printf("\nsuccess\n");
	}
	else {
		printf("\nfail\n");
		if (arraysize <= 1000) {
			printarray((int*)&v[0][0], arraysize);
			printarray<int>(array[0], arraysize);
		}
	}

	delete[]v;

}

int main(){
	//sortint();
	//sortstring();
	benchmark();

	return 0;
}
