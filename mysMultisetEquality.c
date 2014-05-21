#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SFMT.h"


#define P 2147483647		//!< 2^31 - 1 = 2147483647

const long NEW_LINE = '\n';	//!< '\n' = 10
sfmt_t sfmt; 				//!< SIMD-oriented Fast Mersenne Twister

/**
 * Initilizes the Mersenne Twister with some truely random numbers,
 * generated by RANDOM.ORG along with a user defined random number.
 * Exists the program if the paramter contains a non-base 10 symbol.
 *
 * @param[in]	seed	A character string only containing values 0-9
 */
void init_mt(char *seed);

/**
 * Works as follows:
 * 	1. Generates 2*80 + 2 random numbers
 *  2. Constructs 'a' such that it contains 2*80 random numbers in Fp
 *  3. Sets w[0] and w[1] to be random numbers in Fp
 *
 * @param[out]	a	List of 2*80 random numbers
 * @param[out]	w	The two w-values used in f(w)
 */
void setAW(unsigned long *a, unsigned long *w);

/**
 * Evaluates the univariate polynomial (w-h(x_1))(w-h(x_2))***(w-h(x_n)),
 * for both w[0] and w[1] and sets val[0] and val[1] accordingly 
 *
 * @param[in]	content		The content of a file (each line ends with '\n')
 * @param[in]	numChar		Number of characters in content
 * @param[in]	w			Variables for f(z), i.e. w[0] and w[1]
 * @param[in]	a			A list of random numbers in Fp of size 2*80
 */
void evalPoly(	const char *content,
				const long numChar,
				const unsigned long *w,
				const unsigned long *a,
				unsigned long *val);


/**
 * Prints 'Yes' if X and Y represent the same multiset of lines.
 * 'No' otherwise. Under the assumption that a line consists of
 * at most 80 characters, and each character is represented by
 * at most 16 bits, so each element in the multiset is represented
 * by at most b = 80*16 = 1280 bits and a multiset contains at most
 * n <= 2^24 lines this algorithm outputs correctly with probability
 * 1-n/2147483647
 *
 * @param[in]	argc	The number of command line arguments
 * @param[in]	argv	A pointer to an array of string where:
 *		There must be exactly 3 arguments where:
 *			-- The first argument is data-file-1 (X)
 *			-- The second argument is data-file-2 (Y)
 *			-- A user seed in base 10
 * @return Exit code 0 on success, 1 otherwise
 */
int main(int argc, char *argv[]) {
	FILE *fp1, *fp2;			//!< file pointers
	long fSize;					//!< file size (in bytes)
	long numChar;				//!< number of chars in file
	char *buffer;				//!< buffer with the whole file
	unsigned long a[2*80];		//!< mapping from 2^16 to Fp
	unsigned long w[2];			//!< used to evaulate polynomial
	unsigned long f1[2], f2[2];	//!< fingerprint of file 1 and 2
	
	/* Initialize Mersenne Twister */
	init_mt(argv[3]);
	
	/* Initialize a and w */
	setAW(a, w);

	/* Open first file */
	if((fp1 = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Cannot open data-file-1\n");
		exit(1);
	}
	
	/* Get the file size */
	fseek(fp1, 0, SEEK_END);
	fSize = ftell(fp1);
	numChar = fSize/sizeof(char);
	rewind(fp1);
	
	/* Read the whole file into buffer */
	buffer = (char *) malloc(sizeof(char)*(fSize));
	if(fread(buffer, sizeof(char), fSize, fp1) != fSize) {
		fprintf(stderr, "Could not read data-file-1\n");
		exit(1);
	}
	
	/* Close data files (We are done with them!) */
	fclose(fp1);
	
	/* Fingerprint of data-file-1 */
	evalPoly(buffer, numChar, w, a, f1);
	
	/* Open second file */
	if((fp2 = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "Cannot open data-file-2\n");
		exit(1);
	}
	/* Read the whole file into (previus) buffer */
	if(fread(buffer, sizeof(char), fSize, fp2) != fSize) {
		fprintf(stderr, "Could not read data-file-2\n");
		exit(1);
	}
	/* Close the file */
	fclose(fp2);

	/* Fingerprint of data-file-2 */
	evalPoly(buffer, numChar, w, a, f2);
	
	/*
	 * We have the following cases:
	 * 	- f1[0] - f2[0] == 0 and f1[1] - f2[1] != 0: 1st was false positive 
	 *  - f1[0] - f2[0] != 0 and f1[1] - f2[1] == 0: 2nd was false positive
	 *  - f1[0] - f2[0] != 0 and f1[1] - f2[1] != 0: They are distinct
	 *  - f1[0] - f2[0] == 0 and f1[1] - f2[1] == 0: They are probably equal
	 */
	if(((f1[0] - f2[0]) == 0) && ((f1[1] - f2[1]) == 0)) {
		printf("Yes"); //<- With prob. 1-2^-12
	} else {
		printf("No");
	}
	
	/* Cleanup */
	free(buffer);

	return 0;
}

inline void init_mt(char *seed) {
	char *pEnd; //!< Used to check if user seed is parsable
	/* Seed from 
	 * http://www.random.org/integers/?num=9&min=1&max=1000000000&col=9&base=16&format=plain&rnd=date.2014-05-01
	 */
	// Seed the MT using 9 random integers and 1 from user input
	uint32_t init[10] =	{	0x012d661f, 0x399ba1e1, 0x01d033e9,
							0x24ab21da, 0x3a1b142b, 0x21109a50,
							0x20d005a4, 0x35215de9, 0x091c98f1};
	// Parse user input
	init[9] = strtol(seed, &pEnd, 10);
	// Check if all was parsed
	if(pEnd[0] != '\0') {
		fprintf(stderr, "Could not convert user seed to an integer ('%c' is not in base 10)\n", pEnd[0]);
		exit(1);
	}
	// Init SFMT
	sfmt_init_by_array(&sfmt, init, 10);
}

void setAW(unsigned long *a, unsigned long *w) {
	int min = sfmt_get_min_array_size32(&sfmt); //<- Min. size for rand
	int nRandom = 162; //<- 2*80 for the 'a' array and 2 for w
	if(min > 162) {
		nRandom = min;
	}
	uint32_t *rand = malloc(sizeof(uint32_t)*nRandom);
	
	/* Fill the array with 32-bit random uints */
	sfmt_fill_array32(&sfmt, rand, nRandom);

	/* Map the random integers to our field */
	for(int i = 0; i < 2*80; i++) {
		a[i] = rand[i] % P;
	}
	/* And set the w-values */
	w[0] = rand[2*80] % P;
	w[1] = rand[2*80 + 1] % P;
}

inline void evalPoly(	const char *content,
						const long numChar,
						const unsigned long *w,
						const unsigned long *a,
						unsigned long *val) {
	unsigned long h[2] = {0, 0};	//!< Accumulated value of h
	unsigned long tmp;				//!< Concatanation of c1 and c2
	int i, c = 0;					//!< Index variable
	unsigned long val0 = 1, val1 = 1; //<- Tmp. value of f1(w) and f2(w)
	
	/* Example string 0x6D7973 (mys in ASCII) */
	while (c < numChar) {
		/* Calclulate h(X), i.e. a line */
		for(/*  */; content[c] != '\n'; c++) {
			/* tmp = 0x6D00\n */
			tmp = (content[c] & 0xFF) << 8;
			if(content[c + 1] != '\n') {
				/* tmp = 0x6D79 */
				c++;
				tmp |= (content[c] & 0xFF);
			}
			h[0] += (a[i    ] * tmp) % P;
			h[1] += (a[i + 1] * tmp) % P;
		}
		/* Calculate f(X) (partial) */
		val0 = (abs(w[0]-h[0]) * val0) % P;
		val1 = (abs(w[1]-h[1]) * val1) % P;
		/* Reset counters */
		h[0] = 0;
		h[1] = 0;
		i = 0;
		/* Skip the new line just seen */
		c++; 
	}
	val[0] = val0;	//<- We are done, update the values
	val[1] = val1;
}
