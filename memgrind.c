#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "mymalloc.c"

//A: malloc() 1 byte and immediately free it - do this 150 times
void A(){
	char* ptr = NULL;
	int count = 0;

	while (count < 150){
		ptr = (char*)malloc(1);
		free(ptr);

		 count ++;
	}


}
/*malloc() 1 byte, store the pointer in an array - do this 150 times.
 Once you've malloc()ed 50 byte chunks, then free() the 50 1 byte pointers one by one.
*/
void bHelper(){

	char* ptrArray[150];
	int x = 0;
	int y = 0;

	while (x < 50) {
		ptrArray[x] = (char*)malloc(1);
		++x;
	}

	while (y < 50) {
		free(ptrArray[y]);
		++y;
	}
}

void B() {
	int count=0;
	while (count < 3) {
		bHelper();
		count++;
	}
}


/*C: Randomly choose between a 1 byte malloc() or free()ing a 1 byte pointer
> do this until you have allocated 50 times
- Keep track of each operation so that you eventually malloc() 50 bytes, in total
> if you have already allocated 50 times, disregard the random and just free() on each
 iteration
- Keep track of each operation so that you eventually free() all pointers
 > don't allow a free() if you have no pointers to free()
*/

void freeNodes(char** ptr) {
	int z = 0;

	while(z < 50){
		if(ptr[z] != NULL){
			free(ptr[z]);
			ptr[z] = NULL;
		}
		++z;
  }
}


void C(){

	int x = 0;
	int y = 0;
	int mallocs = 0;

	char* ptrArray[50];

	while (x <50) {
		ptrArray[x] = NULL;
		++x;
	}

	while(mallocs < 50){

		if(rand() %2 ==0){ //flip a coin to either malloc or free

			if (rand() % 2 ==0) { //flip another coin to start at the beginning or end of the array to look for a pointer to free
				for(y = 0 ; y < 50; ++y){

					if(ptrArray[y] != NULL){
						free(ptrArray[y]);
						ptrArray[y] = NULL;
						break;
					}
				}
			}
			else {
				for(y = 49 ; y >= 0; --y){
					if(ptrArray[y] != NULL){
						free(ptrArray[y]);
						ptrArray[y] = NULL;
						break;
					}
				}
			}
		}
		else{

			char* ptr = (char*)malloc(1);

			ptrArray[mallocs] = ptr;

			++mallocs;

		}
		freeNodes(ptrArray);
	}

}

/*
D: Randomly choose between a randomly-sized malloc() or free()ing a pointer – do this many
times (see below)
- Keep track of each malloc so that all mallocs do not exceed your total memory capacity
- Keep track of each operation so that you eventually malloc() 50 times
- Keep track of each operation so that you eventually free() all pointers
- Choose a random allocation size between 1 and 64 bytes
*/
void D() {
int x = 0;
int y = 0;
int z = 0;
int mallocs = 0;
int memTaken=0;

char* ptrArray[50];

while (x <50) {
	ptrArray[x] = NULL;
	++x;
}

while(mallocs < 50){

	if (memTaken > 4096) { //if you took up too much memory, free
		while(z < 50){ //free the rest of the pointers that weren't done in the main loop
			if(ptrArray[z] != NULL){
				free(ptrArray[z]);
				ptrArray[z] = NULL;
			}
			++z;
	  }
	}

	if(rand() %2 ==0){ //flip a coin to either malloc or free

		if (rand() % 2 ==0) { //flip another coin to start at the beginning or end of the array to look for a pointer to free
			for(y = 0 ; y < 50; ++y){

				if(ptrArray[y] != NULL){
					free(ptrArray[y]);
					ptrArray[y] = NULL;
					break;
				}
			}
		}
		else {
			for(y = 49 ; y >= 0; --y){
				if(ptrArray[y] != NULL){
					free(ptrArray[y]);
					ptrArray[y] = NULL;
					break;
				}
			}
		}
	}
	else{
		int currMemory = (rand()%(1-64)+1);
		char* ptr = (char*)malloc(currMemory);

		ptrArray[mallocs] = ptr;
		memTaken += currMemory;

		++mallocs;

	}
}

  freeNodes(ptrArray);

}



/*E: malloc the cache's exact capacity: 128 28-byte chars (since metadata is 4 chars)
		-then free every other pointer
		-put data back in there again
		-then free it all
*/
void E(){

	int i = 0;
	char* ptrArray[128];


	for(i = 0; i < 128; ++i){
		char* ptr = (char*)malloc(28);
		ptrArray[i] = ptr;

	}
	for(i = 0; i < 128; i=i+2){
		free (ptrArray[i]);

	}
	for (i=0; i < 128; i=i+2) {
		char* ptr = (char*)malloc(28);
		ptrArray[i] = ptr;

	}

	for(i = 0; i < 128; i++){
		free (ptrArray[i]);

	}

}

//F: put actual data in the memory
void F(){

	char* wordPointer = malloc(8);
	wordPointer[0] = 'A';
	wordPointer[1] = 'L';
	wordPointer[2] = 'L';
	wordPointer[3] = 'I';
	wordPointer[4] = 'S';
	wordPointer[5] = 'O';
	wordPointer[6] = 'N';
	wordPointer[7] = '\0';
	free(wordPointer);

}

int main(){

	 double timeA = 0;
	 double timeB = 0;
	 double timeC = 0;
	 double timeD = 0;
	 double timeE = 0;
	 double timeF = 0;
	 int x = 0;

	 double timeAarr[100];
	 double timeBarr[100];
	 double timeCarr[100];
	 double timeDarr[100];
	 double timeEarr[100];
	 double timeFarr[100];

	struct timeval start;
	struct timeval end;
	struct timeval time;

	gettimeofday(&time, NULL);
	suseconds_t systime = time.tv_usec;
	srand(systime);

	for(x = 0; x < 100; x++){
		gettimeofday(&start, NULL);
		A();
		gettimeofday(&end, NULL);
		timeAarr[x] = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		timeA += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	}

	for(x = 0; x < 100; x++)
	{
		gettimeofday(&start, NULL);
		B();
		gettimeofday(&end, NULL);
		timeBarr[x] = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		timeB += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	}


	for(x = 0; x < 100; x++)	{
		gettimeofday(&start, NULL);
		C();
		gettimeofday(&end, NULL);
		timeCarr[x] = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		timeC += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	}

	for(x = 0; x < 100; x++)	{
		gettimeofday(&start, NULL);
		D();
		gettimeofday(&end, NULL);
		timeDarr[x] = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		timeD += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	}

	for(x = 0; x < 100; x++){
		gettimeofday(&start, NULL);
		E();
		gettimeofday(&end, NULL);
		timeEarr[x] = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		timeE += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	}

	for(x = 0; x < 100; x++)	{
		gettimeofday(&start, NULL);
		F();
		gettimeofday(&end, NULL);
		timeFarr[x] = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		timeF += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	}

	timeA = timeA/100.0;
	timeB = timeB/100.0;
	timeC = timeC/100.0;
	timeD = timeD/100.0;
	timeE = timeE/100.0;
	timeF = timeF/100.0;

	printf("Microseconds for workload A: %f\n", timeA);
	printf("Microseconds for workload B: %f\n", timeB);
	printf("Microseconds for workload C: %f\n", timeC);
	printf("Microseconds for workload D: %f\n", timeD);
	printf("Microseconds for workload E: %f\n", timeE);
	printf("Microseconds for workload F: %f\n", timeF);

	return 0;
}
