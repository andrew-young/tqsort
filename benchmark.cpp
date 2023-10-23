//#include <time.h>
#include <vector>

#include "tqsort.hpp"
#include "timsort.hpp"
#include "pdqsort.h"
#include "quadsort.hpp"

#include <stdio.h>
#include <iostream>
#include <windows.h>


//#include "util.h"

template <class T>
struct CMPFUNC {
	int operator()(const T* x1, const T* x2) const
	{
		//globalcomparecount++;
		return *x1 - *x2;
	}
};
template <>
struct CMPFUNC<int> {
	int operator()(const int x1, const int x2) const
	{
		//globalcomparecount++;
		return x1 - x2;
	}
};
template <>
struct CMPFUNC<int*> {
	int operator()(const int *x1, const int *x2) const
	{
		//globalcomparecount++;
		return *x1 - *x2;
	}
};

template <>
struct CMPFUNC<char*> {
	int operator()(char* x1, char* x2) const
	{
		//globalcomparecount++;
		return strcmp(x1, x2);
	}
};
inline int compareint3(const int* num1, const int* num2) {
	return *num1 - *num2;

}

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




template<class T>
void printarray(T* array, int n = 20) {
	for (int i = 0; i < n; i++) {
		//printf("%d ", array[i]);
		std::cout << array[i];
		std::cout << " ";
		//printf("%d ", array[i]);
	}
	printf("\n");
}


//check array. array2 is correct
template<class T, class Compare>
bool check(T* array, T* array2, int m, int p, Compare compare) {
	int i = 0;
	bool b;
	for (int j = 0; j < p; j++) {
		while (i < m - 1) {
			//printf("%d ",i);
			b = compare(array[i + j * m], array2[i + j * m]) || compare(array2[i + j * m], array[i + j * m]);
			//printf("a");
			if (b) {
				printf("found: ");
				std::cout << array[i + j * m];
				printf(",should be: ");
				std::cout << array2[i + j * m];
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

void sortstring() {
	int arraysize = 100;
	int numarrays = 100000;
	int slength = 7;
	float sortedness = 0;
	//int n=20;

	char*** array = allocatestringarray(arraysize, numarrays, slength);
	makestringarrays(array,arraysize, numarrays, sortedness, slength, 4);
	std::vector<char*>* v = new std::vector<char*>[numarrays];
	struct timeval stop, start;


	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<char*>(array[i] , array[i] + arraysize);
		std::sort(array[i], array[i] + arraysize, std::less<char*>());
	}
	printf("start!\n");
	gettimeofday(&start, NULL);
	char** ptr,**tpr;
	for (int i = 0; i < numarrays; i++) {

		//gfx::timsort(v[i], std::less<char*>());
		tqsort(v[i].begin(), arraysize, std::less<char*>());
		//pdqsort < std::vector<char*>::iterator, std::less<char*>>(v[i].begin(), v[i].end(), std::less<char*>());
		//quadsort(v[i].begin(), arraysize, CMPFUNC<char*>());

	}
	gettimeofday(&stop, NULL);
	long long milliseconds = (stop.tv_sec - start.tv_sec) * 1000LL + (stop.tv_usec - start.tv_usec) / 1000; // calculate milliseconds
	double dif = milliseconds / (double)1000;
	printf("\n%f\n", dif);
	printf("comparecount %d \n", std::globalcomparecount);
	for (int i = 0; i <1 ; i++) {//numarrays
		if (check<char*>((char**)&v[i][0], array[i], arraysize, numarrays, std::less<char*>())) {
			
		}
		else {
			if (arraysize <= 1000) {
				printarray<char*>((char**)&v[i][0], arraysize);
				printarray<char*>(array[i], arraysize);
			}
		}
	}
	printf("\ndone\n");
	delete[]v;
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






void sortint() {
	int arraysize = 100;
	int numarrays = 100000;
	int sortedness = 0;
	
	int** array;
	std::vector<int>* v = new std::vector<int>[numarrays];
	//int* array = makeintarray(n, n);//m
	
	struct timeval stop, start;

	array = allocatearrays(arraysize, numarrays);
	makeintarrays(array,arraysize, numarrays, sortedness, 4);
	//printarray<int>(array[0], arraysize);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<int>(array[i], array[i] + arraysize);
		std::sort(array[i], array[i] + arraysize, std::less<int>());
	}

	
	printf("start!\n");
	gettimeofday(&start, NULL);
	int *ptr,*tpr;
	bool b=0;
	for (int i = 0; i < numarrays; i++) {
		ptr = (int*)&v[i][0];
		tpr = (int*)&v[i][arraysize];
		//gfx::timsort(v[i]);
		tqsort(v[i].begin(), arraysize, std::less<int>());

		//pdqsort(ptr, tpr, std::less<int>());
		//quadsort(v[i].begin(), arraysize, CMPFUNC<int>());
	}
	gettimeofday(&stop, NULL);
	double seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000; // calculate milliseconds
	//printf("\n%d",milliseconds);
	printf("\n%f\n", seconds);
	printf("bool %d\n", b);
	printf("comparecount %d \n", std::globalcomparecount);
	//check2<int>(array, m, p, compareint);
	if (check<int>((int*)&v[0][0],array[0], arraysize, 1, std::less<int>())) {
		printf("\nsuccess\n");
	}
	else {
		printf("\nfail\n");
		if (arraysize <= 1000) {
			printarray((int*)&v[0][0], arraysize);
			printarray<int>(array[0], arraysize);
		}
	}

	delete []v;
	
}

void benchmarkstring(char*** arrays, std::vector<char*>* v, int arraysize, int numarrays, float sortedness,int slength,float*total) {
	struct timeval stop, start;
	double seconds;

	printf("arraysize = %d, number of arrays = %d\n\n", arraysize, numarrays);

	makestringarrays(arrays, arraysize, numarrays, sortedness, slength,4);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
	}

	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		gfx::timsort(v[i]);
	}
	gettimeofday(&stop, NULL);
	seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
	printf("timsort - degree sorted %d%% - time %f\n", (int)(sortedness * 100), seconds);
	total[0] += seconds;
	//////////////
	makestringarrays(arrays, arraysize, numarrays, sortedness, slength,4);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
	}


	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		pdqsort(v[i].begin(), v[i].end(), std::less<char*>());
	}
	gettimeofday(&stop, NULL);
	seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
	printf("pdqsort - degree sorted %d%% - time %f\n", (int)(sortedness * 100), seconds);
	total[1] += seconds;
	////////////////
	makestringarrays(arrays, arraysize, numarrays, sortedness, slength,4);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<char*>(arrays[i], arrays[i] + arraysize);
	}

	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		tqsort(v[i].begin(), arraysize, std::less<char*>());
	}
	gettimeofday(&stop, NULL);
	seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
	printf("tqsort -  degree sorted %d%% - time %f\n\n",  (int)(sortedness * 100), seconds);
	total[2] += seconds;
}


void benchmarkint(int** arrays, std::vector<int>* v,int arraysize, int numarrays, float sortedness,float*total) {
	struct timeval stop, start;
	double seconds;
	printf("arraysize = %d, number of arrays = %d\n\n", arraysize, numarrays);

	makeintarrays(arrays, arraysize, numarrays, sortedness, 4);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
	}

	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		gfx::timsort(v[i]);
	}
	gettimeofday(&stop, NULL);
	seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
	printf("timsort - degree sorted %d%% - time %f\n", (int)(sortedness * 100), seconds);
	total[0] += seconds;
	//////////////
	makeintarrays(arrays, arraysize, numarrays, sortedness, 4);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
	}


	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		pdqsort(v[i].begin(), v[i].end(), std::less<int>());
	}
	gettimeofday(&stop, NULL);
	seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
	printf("pdqsort - degree sorted %d%% - time %f\n", (int)(sortedness * 100), seconds);
	total[1] += seconds;
	////////////////
	makeintarrays(arrays, arraysize, numarrays, sortedness, 4);
	for (int i = 0; i < numarrays; i++) {
		v[i] = std::vector<int>(arrays[i], arrays[i] + arraysize);
	}

	gettimeofday(&start, NULL);
	for (int i = 0; i < numarrays; i++) {
		tqsort(v[i].begin(), arraysize, std::less<int>());
	}
	gettimeofday(&stop, NULL);
	seconds = (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000;
	printf("tqsort - degree sorted %d%% - time %f\n\n",  (int) (sortedness*100), seconds);
	total[2] += seconds;
}

void benchmark() {


	////////
	//ints
	/////////

	int arraysize;
	int numarrays;
	int** arrays;
	std::vector<int>* v;
	float sortedness;
	float total[3] = { 0,0,0 };
	printf("ints\n\n");

	arraysize = 10000000;
	numarrays = 1;
	arrays = allocatearrays(arraysize, numarrays);
	v = new std::vector<int>[numarrays];
	sortedness = 0;
	benchmarkint(arrays, v,arraysize, numarrays, sortedness, total);

	sortedness = 0.8;
	benchmarkint(arrays, v, arraysize, numarrays, sortedness, total);

	sortedness = 0.95;
	benchmarkint(arrays, v, arraysize, numarrays, sortedness, total);
	deallocatearrays(arrays, arraysize, numarrays);
	delete[] v;

	arraysize = 100;
	numarrays = 100000;
	arrays = allocatearrays(arraysize, numarrays);
	v = new std::vector<int>[numarrays];

	sortedness = 0;
	benchmarkint(arrays, v, arraysize, numarrays, sortedness, total);

	sortedness = 0.9;
	benchmarkint(arrays, v, arraysize, numarrays, sortedness, total);
	deallocatearrays(arrays, arraysize, numarrays);
	delete[] v;



	////////
	//strings
	/////////

	char*** arraystring;
	std::vector<char*>* vstring;
	int slength=7;
	
	printf("strings - length %d\n\n", slength);

	arraysize = 10000000;
	numarrays = 1;
	arraystring = allocatestringarray(arraysize, numarrays,slength);
	vstring = new std::vector<char*>[numarrays];
	sortedness = 0;
	benchmarkstring(arraystring, vstring, arraysize, numarrays, sortedness, slength, total);

	sortedness = 0.9;
	benchmarkstring(arraystring, vstring, arraysize, numarrays, sortedness, slength, total);
	deallocatestringarray(arraystring, arraysize, numarrays);
	delete[] vstring;

	arraysize = 100;
	numarrays = 100000;
	arraystring = allocatestringarray(arraysize, numarrays, slength);
	vstring = new std::vector<char*>[numarrays];

	sortedness = 0;
	benchmarkstring(arraystring, vstring, arraysize, numarrays, sortedness, slength, total);

	sortedness = 0.9;
	benchmarkstring(arraystring, vstring, arraysize, numarrays, sortedness, slength, total);
	deallocatestringarray(arraystring, arraysize, numarrays);

	delete[] vstring;

	printf("timsort total - %f", total[0]);
	printf("pdfsort total - %f", total[1]);
	printf("tqsort total - %f", total[2]);
}
int main(){
	//sortint();
	//sortstring();
	benchmark();

	return 0;
}
