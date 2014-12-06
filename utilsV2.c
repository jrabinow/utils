#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
char *xstrdup(const char *str);
void *xrealloc(void *ptr, size_t size);

char *constStrConcat(const char *str1, const char *str2);
char *strconcat(char *str1, const char *str2);

char *readLine(FILE *file);
int splitStr(const char *str, char separator, char ***returnArray);


void *xmalloc(size_t size) {
	void *ptr = malloc(size);
	if(ptr == NULL) {
		perror("Error allocating memory ");
		exit(-1);
	} else
		return ptr;
}

void *xcalloc(size_t nmemb, size_t size) {
	void *ptr = calloc(nmemb, size);
	if(ptr == NULL) {
		perror("Error allocating memory ");
		exit(-1);
	} else
		return ptr;
}

char *xstrdup(const char *str) {
	char *newStr = strdup(str);
	if(newStr == NULL) {
		perror("Error allocating memory ");
		exit(-1);
	} else
		return newStr;
}

void *xrealloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);
	if(ptr == NULL) {
		perror("Error allocating memory ");
		exit(-1);
	} else
		return ptr;
}

char *itoa(int n, char *buffer) {
	char *ptr = buffer;
	int log;

	if(n < 0) {
		*ptr++ = '-';
		n = 0 - n;
	}
	
	for(log = n; log != 0; log /= 10)
		ptr++;
	
	for(*ptr = '\0'; n != 0; n /= 10)
		*--ptr = n % 10 + '0';

	return buffer;
}

// xmalloc, strlen
char *constStrConcat(const char *str1, const char *str2) {
	char *newStr = NULL;
	newStr = (char*) xmalloc(strlen(str1)+strlen(str2)+1);
	strcpy(newStr, str1);
	strcpy(newStr+strlen(str1), str2);
	return newStr;
}

// xrealloc, strlen
char *strconcat(char *str1, const char *str2) {
	str1 = (char*) xrealloc(str1, strlen(str1)+strlen(str2)+1);
	strcpy(str1+strlen(str1), str2);
	return str1;
}

// xmalloc, xrealloc
char *readLine(FILE *file) {
	int i = 0, currentSize = 32;
	char c, *str = xmalloc(32);

	while((c = fgetc(file)) != EOF && c != '\n') {
		if(i == currentSize)
			str = xrealloc(str, currentSize <<= 1);
		str[i++] = c;
	}
	
	if(i == currentSize)
	       	str = xrealloc(str, currentSize += 1);
	else {
		if(c == EOF && i == 0)
			str[i++] = EOF;
		str = realloc(str, i+1);	// freeing space -> no need to check for null
	}
	str[i] = '\0';
	return str;
}

// xmalloc, xcalloc, xstrdup
int splitStr(const char *str, char separator, char ***returnArray) {
	int i, count = 1;
	for(; str[0] == separator && str[0] != '\0'; str++);	// skip over separators at start of string
	for(i = 0; str[i] != '\0'; i++)
		count += (str[i] == separator && str[i+1] != separator);
	if(count == 1) {
		*returnArray = (char**) xmalloc(sizeof(char*));
		(*returnArray)[0] = xstrdup(str);
		return 1;
	}
	
	*returnArray = (char**) xcalloc(count, sizeof(char*));

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			if(i == 0) 	// skip over multiple consecutive separators
				str++;
			else { 
				(*returnArray)[count] = (char*) xmalloc(i+1);
				strncpy((*returnArray)[count], str, i);
				(*returnArray)[count++][i] = '\0';
				str += i+1;
			}
			i = -1;
		}
	}
	return count+1;
}

#include <termios.h>
#define turn_echoing_off() {\
	struct termios t;\
	tcgetattr(0, &t);\
	t.c_lflag &= ~ECHO;\
	tcsetattr(0, TCSANOW, &t);\
}

#define turn_echoing_on() {\
	struct termios t;\
	tcgetattr(0, &t);\
	t.c_lflag |= ECHO;\
	tcsetattr(0, TCSANOW, &t);\
}

int main(int argc, char **argv)
{
	char *line;
	puts("Hello World !");

	printf("Enter password: ");
	turn_echoing_off();
	line = readLine(stdin);
	turn_echoing_on();
	
	putchar('\n');
	printf("Here is your password: '%s'.\n", line);
	free(line);
	
	return 0;
}
