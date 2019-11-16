#define _CRT_SECURE_NO_WARNINGS
#include<ctype.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<limits.h>
#include<stdlib.h>

#define BUFFSIZE 256
#define STRSIZE 50
#define SMALLSTRSIZE 5

char* print_char_to_16(unsigned char num)
{
	char* result = malloc(SMALLSTRSIZE);
	char alphabet[] = "0123456789ABCDEF";
	int rest1 = num % 16;
	int rest2 = num / 16;
	result[0] = alphabet[rest2];
	result[1] = alphabet[rest1];
	result[2] = ' ';
	result[3] = '\0';
	return result;
}
void countin_bytes(FILE* file, long* size, int* type) {
	unsigned char symb;
	while (!feof(file)) {
		symb = fgetc(file);
		if ((isprint(symb) == 0) && (symb != '\n') && (symb != '\r')) {
			if ((int)symb == 255)break;
			(*type) = 1;
			break;
		}
	}
	fseek(file, 0, 2);
	*size = ftell(file);
	fseek(file, 0, 0);
}
void write_discrepancy_txt(char* str1, char* str2, int byte_number, int line_number, char ch1, char ch2) {
	int size1 = strlen(str1), size2 = strlen(str2);
	char* buff3 = malloc(sizeof(char)*(max(size1, size2) + 1));
	for (int i = 0; i < min(size1, size2); i++) {
		if (str1[i] != str2[i]) {
			buff3[i] = '+';
		}
		else buff3[i] = ' ';
	}
	for (int i = min(size1, size2); i < max(size1, size2); i++) {
		buff3[i] = '+';
	}
	buff3[max(size1, size2)] = '\0';
	printf("Discrepancy  at byte  %d, at line %d ('%c', '%c')\n", byte_number, line_number, ch1, ch2);
	printf("%s\n%s\n%s\n", str1, buff3, str2);
	free(buff3);
}
void find_discrepancy_txt(char* str1, char* str2, long *byte_count, int *lines) {
	char ch1, ch2;
	int size1 = strlen(str1), size2 = strlen(str2), tmpstr = 0, tmp1 = size1, tmp2 = size2;
	for (int i = 0; i < min(size1, size2); i++) {
		if (str1[i] == '\n' || str2[i] == '\n') {
			tmpstr = i;
			(*lines)++;
			continue;
		}
		if (str1[i] != str2[i]) {
			ch1 = str1[i];
			ch2 = str2[i];
			break;
		}
		(*byte_count)++;
	}
	(*byte_count) += (*lines);
	char* buff1 = malloc(sizeof(char)*(size1 - tmpstr + 1));
	char* buff2 = malloc(sizeof(char)*(size2 - tmpstr + 1));
	for (int i = 0; i < size1 - tmpstr - 1; i++) {
		if (str1[i + tmpstr + 1] != '\n') 		buff1[i] = str1[i + tmpstr + 1];
		else {
			tmp1 = i;
			break;
		}
	}
	for (int i = 0; i < size2 - tmpstr - 1; i++) {
		if (str2[i + tmpstr + 1] != '\n')buff2[i] = str2[i + tmpstr + 1];
		else {
			tmp2 = i;
			break;
		}
	}
	buff1[min(tmp1, (size1 - tmpstr - 1))] = '\0';
	buff2[min(tmp2, (size2 - tmpstr - 1))] = '\0';
	write_discrepancy_txt(buff1, buff2, (*byte_count), (*lines), ch1, ch2);
}
int text_file_comprassion(FILE* File1, FILE* File2, long *size1, long *size2) {
	char* buff1 = malloc(sizeof(char)*BUFFSIZE + 1);
	char* buff2 = malloc(sizeof(char)*BUFFSIZE + 1);
	int cmp;
	long counter = 0, line_counter = 1;
	while (!feof(File1) && !feof(File2)) {
		size_t _size1 = fread(buff1, sizeof(char), BUFFSIZE, File1);
		size_t _size2 = fread(buff2, sizeof(char), BUFFSIZE, File2);
		buff1 = (realloc(buff1, _size1 + 1));
		buff2 = (realloc(buff2, _size2 + 1));
		buff1[_size1] = '\0';
		buff2[_size2] = '\0';
		cmp = strcmp(buff1, buff2);
		if (cmp == 0) {
			counter += BUFFSIZE;
			for (int i = 0; i < BUFFSIZE; i++) if (buff1[i] == '\n') line_counter++;
		}
		else {
			cmp = 1;
			find_discrepancy_txt(buff1, buff2, &counter, &line_counter);
			break;
		}
	}

	free(buff1);
	free(buff2);
	return cmp;
}
void write_discrepancy_bin(char* buff1, char* buff2, int byte_number, unsigned char ch1, unsigned char ch2, int size) {
	char buff3[49];
	int zero_counter = byte_number, count = 8;
	char* chr1 = NULL, *chr2 = NULL;
	chr1 = print_char_to_16(ch1);
	chr2 = print_char_to_16(ch2);
	printf("Discrepancy  at byte  %d (' %s', ' %s')\n", byte_number, chr1, chr2);
	byte_number -= byte_number % 16;
	size = 3 * size;
	for (int i = 0; i < min(size, 48) - 1; i++) {
		if (buff1[i] != buff2[i] || buff1[i + 1] != buff2[i + 1] && buff1[i] != ' ' && buff2[i] != ' ') {
			buff3[i] = '+';
			buff3[i + 1] = '+';
			i++;
		}
		else buff3[i] = ' ';
	}
	if (size < 48) {
		char tmp[] = " 00";
		strcat(buff1, "|");
		strcat(buff2, "|");
		for (int i = size + 1; i < 48; i += 3) {
			strcat(buff1, tmp);
			strcat(buff2, tmp);
		}
	}
	else 	size = min(strlen(buff1), strlen(buff2));
	buff3[size - 1] = '\0';
	printf("%0.8x  %s\n          %s\n%.8x  %s\n", byte_number, buff1, buff3, byte_number, buff2);
	free(chr1);
	free(chr2);
	free(buff3);
}
void find_discrepancy_bin(FILE* File1, FILE* File2, int byte_number, unsigned char* str1, unsigned char* str2, unsigned char chr1, unsigned char chr2, int size) {
	char* buff1 = NULL, *tmpstr = NULL, *buff2 = NULL;
	for (int i = 0; i < min(size, 16); i++) {
		tmpstr = print_char_to_16(str1[i]);
		if (buff1 == NULL)buff1 = tmpstr;
		else strncat(buff1, tmpstr, 3);
		tmpstr = print_char_to_16(str2[i]);
		if (buff2 == NULL)buff2 = tmpstr;
		else strncat(buff2, tmpstr, 3);
	}
	free(tmpstr);
	write_discrepancy_bin(buff1, buff2, byte_number, chr1, chr2, size);
}
int bin_file_comprassion(FILE* File1, FILE* File2, int size1, int size2) {
	short cmp = 0, flag = 0;
	int size = min(size1, size2);
	unsigned char ch1, ch2;
	unsigned char str1[17];
	unsigned char str2[17];
	int read1 = 0, read2 = 0;
	long byte_number = 0;
	while (!feof(File1) && !feof(File2) && flag == 0) {
		read1 = fread(str1, sizeof(char), 16, File1);
		read2 = fread(str2, sizeof(char), 16, File2);
		for (int i = 0; i < min(read1, read2); i++) {
			byte_number++;
			if (str1[i] != str2[i]) {
				ch1 = str1[i];
				ch2 = str2[i];
				cmp = 1;
				flag = 1;
			}
		}

	}

	if (cmp) {
		if (size > 15) {
			int step = min((byte_number - (byte_number % 16)), (size - (size % 16)));
			fseek(File1, step, 0);
			fseek(File2, step, 0);
		}
		else {
			fseek(File1, 0, 0);
			fseek(File2, 0, 0);
		}
		find_discrepancy_bin(File1, File2, byte_number, str1, str2, ch1, ch2, size);
	}
	return cmp;
}
int main(int argc, char* argv[]) {
	long size1 = 0, size2 = 0;
	int cmpr = 0, type1 = 0, type2 = 0;
	FILE* file1, *file2;
	file1 = fopen(argv[1], "rb");
	file2 = fopen(argv[2], "rb");
	countin_bytes(file1, &size1, &type1);
	countin_bytes(file2, &size2, &type2);
	if (size1 != size2) printf("Size mismatch: file1 is %d bytes, file 2 is %d bytes\n", size1, size2);
	else  printf("Size match: files is %d bytes\n", size2);
	if (type1 == 0 && type2 == 0) cmpr = text_file_comprassion(file1, file2, &size1, &size2);
	else cmpr = bin_file_comprassion(file1, file2, size1, size2);
	fclose(file1);
	fclose(file2);
	if (cmpr == 0) {
		printf("OK");
		return 0;
	}
	return 1;
}

