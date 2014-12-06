#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MALLOC 12
#define ERR_FILE 9

char *constStrConcat(const char *str1, const char *str2) {
	char *newStr = NULL;
	if((newStr = (char*) malloc(strlen(str1)+strlen(str2)+1)) != NULL) {
		strcpy(newStr, str1);
		strcpy(newStr+strlen(str1), str2);
	}
	return newStr;
}

char *strconcat(char *str1, const char *str2) {
	if((str1 = (char*) realloc(str1, strlen(str1)+strlen(str2)+1)) != NULL) {
		strcpy(str1+strlen(str1), str2);
		return str1;
	} else
		return NULL;
}

char *readLine(FILE *file) {
	char c, *str;
	int i = 0, currentSize = 20;
	if((str = malloc(20)) == NULL)
		exit(-1);
	while((c = fgetc(file)) != EOF && c != '\n') {
		if(i == currentSize && (str = realloc(str, currentSize += 20)) == NULL)
			exit(-1);
		str[i++] = c;
	}
	if(i == currentSize && (str = realloc(str, currentSize += 1)) == NULL)
		exit(-1);
	str[i] = '\0';
	return str;
}

int splitStr(const char *str, char separator, char ***returnArray) {
	int i, count = 1;
	for(; str[0] == separator && str[0] != '\0'; str++);	// skip over separators at start of string
	for(i = 0; str[i] != '\0'; i++)
		count += (str[i] == separator && str[i+1] != separator);
	if(count == 1) {
		if((*returnArray = (char**) malloc(sizeof(char*))) == NULL || ((*returnArray)[0] = strdup(str)) == NULL)
			exit(-1);
		return 1;
	}
	
	if((*returnArray = (char**) calloc(count, sizeof(char*))) == NULL)
		exit(-1);

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			if(i == 0) 	// skip over multiple consecutive separators
				str++;
			else { 
				if(((*returnArray)[count] = (char*) malloc(i+1)) == NULL)
					exit(-1);
				strncpy((*returnArray)[count], str, i);
				(*returnArray)[count++][i] = '\0';
				str += i+1;
			}
			i = -1;
		}
	}
	return count+1;
}

void quit(int retValue) {
	switch(retValue) {
		case ERR_FILE:
			perror("Error opening file ");
			break;
		case ERR_MALLOC:
			perror("Error allocating memory ");
			break;
		default:
			fprintf(stderr, "Exiting now...\n");
	}
	exit(retValue);
}

int main() {
	printf("Hello World!\n");
	return 0;
}
