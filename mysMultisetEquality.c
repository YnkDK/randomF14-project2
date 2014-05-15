#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mt64.h"

// 2^31 - 1 = 2147483647
#define P 2147483647
#define mt() (genrand64_int64() % 2147483647)

int main(int argc, char *argv[]) {
	FILE *fp1, *fp2;
	long fSize;
	char *buffer1, *buffer2;
	unsigned long tmp = 0;
	unsigned long h = 0, f1 = 1, f2 = 1, w;
	const char NEW_LINE = '\n';
	char c1, c2;
	
	bool *isGen = malloc(sizeof(bool)*0xFFFF);
	unsigned long *hashtable = malloc(sizeof(unsigned long)*0xFFFF);
	
	/* INIT MT */
	/* Seed from RANDOM.ORG */
    unsigned long long init[10]={440657579ULL, 925861038ULL, 964351785ULL, 862964423ULL, 757649787ULL, 930764738ULL, 984819959ULL, 826081103ULL, 74519538ULL, 57745628ULL}, length=10;
    init_by_array64(init, length);
    
    /* Choose a w */
    w = mt();
    
	/* open first file */
	if((fp1 = fopen(argv[1], "rb")) == NULL) {
		printf("Cannot open first file.\n");
		exit(1);
	}

	/* open second file */
	if((fp2 = fopen(argv [2], "rb")) == NULL) {
		printf("Cannot open second file.\n");
		exit(1);
	}
	
	/* Get the file size (Assume they are equal) */
	fseek(fp1, 0, SEEK_END);
	fSize = ftell(fp1);
	rewind(fp1);
	
	/* Read both files to maximize the I/O performance */
	buffer1 = (char *) malloc(sizeof(char)*fSize);
	fread(buffer1, 1, fSize, fp1);
	
	buffer2 = (char *) malloc(sizeof(char)*fSize);
	fread(buffer2, 1, fSize, fp2);
	
	/* Close data files (We are done with them!) */
	fclose(fp1);
	fclose(fp2);
	
	/* Start with file 1 */
	int i;
	for(i = 0; i <= fSize/sizeof(char)+1; i += 2) {
		c1 = buffer1[i];
		c2 = buffer1[i+1];
		if(c1 != NEW_LINE && c2 != NEW_LINE) {
			// Extract the next 16 bits
			tmp = (((buffer1[i] & 0xFF)) << 8) | (buffer1[i+1] & 0xFF);
			// Calculate the next clause a_i*x_i
			if(!isGen[tmp]) {
				isGen[tmp] = true;
				
				hashtable[tmp] = (tmp * mt()) % P;
			}
			
			h += hashtable[tmp];
		} else {
			if(c2 == NEW_LINE) {
				// Take the last 8 bits
				tmp = (buffer1[i] & 0xFF);
				if(!isGen[tmp]) {
					isGen[tmp] = true;
					hashtable[tmp] = (tmp * mt()) % P;
				}								
				h += hashtable[tmp];
			} else {
				// c2 is the first character in the next element
				i--;
			}
			// h(x) is now fully calculated
			
			f1 = ((w-h) * f1) % P;
			
			h = 0;
			continue;
		}
	}
	
	/* Go to file 2 */
	for(i = 0; i <= fSize/sizeof(char)+1; i += 2) {
		c1 = buffer2[i];
		c2 = buffer2[i+1];
		if(c1 != NEW_LINE && c2 != NEW_LINE) {
			// Extract the next 16 bits
			tmp = (((buffer2[i] & 0xFF)) << 8) | (buffer2[i+1] & 0xFF);
			// Calculate the next clause a_i*x_i
			if(!isGen[tmp]) {
				isGen[tmp] = true;
				
				hashtable[tmp] = (tmp * mt()) % P;
			}
			
			h += hashtable[tmp];
		} else {
			if(c2 == NEW_LINE) {
				// Take the last 8 bits
				tmp = (buffer2[i] & 0xFF);
				if(!isGen[tmp]) {
					isGen[tmp] = true;
					hashtable[tmp] = (tmp * mt()) % P;
				}								
				h += hashtable[tmp];
			} else {
				// c2 is the first character in the next element
				i--;
			}
			// h(x) is now fully calculated
			
			f2 = ((w-h) * f2) % P;
			
			h = 0;
			continue;
		}
	}
	

	
	/* Cleanup */
	free(buffer1);
	free(buffer2);

	printf((f1-f2==0) ? "Yes" : "No");
	return 0;
}
