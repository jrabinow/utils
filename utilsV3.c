#include "utils.h"

/* THIS CODE IS A GENERIC LIBRARY. THE CODE FOCUSES ON PERFORMANCE
 * RATHER THAN READABILIY. GO READ utils.h INSTEAD FOR AN OVERVIEW
 * OF WHAT FUNCTIONS ARE AVAILABLE */

void *xmalloc(size_t size)
{
	void *ptr = malloc(size);

	if(ptr == NULL) {
		perror("Error allocating memory ");
		exit(EXIT_FAILURE);
	} else
		return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
	void *ptr = calloc(nmemb, size);

	if(ptr == NULL) {
		perror("Error allocating memory ");
		exit(EXIT_FAILURE);
	} else
		return ptr;
}

char *xstrdup(const char *str)
{
	char *newStr = strdup(str);

	if(newStr == NULL) {
		perror("Error allocating memory ");
		exit(EXIT_FAILURE);
	} else
		return newStr;
}

void *xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if(ptr == NULL) {
		perror("Error allocating memory ");
		exit(EXIT_FAILURE);
	} else
		return ptr;
}

int xopen(const char *path, int flags)
{
	int fd = open(path, flags);
	if(fd == -1) {
		perror("Error opening file ");
		exit(EXIT_FAILURE);
	} else
		return fd;
}

FILE *xfopen(const char *path, const char *mode)
{
	FILE *f = fopen(path, mode);
	if(f == (FILE*) NULL) {
		perror("Error opening file ");
		exit(EXIT_FAILURE);
	} else
		return f;
}

FILE *xfdopen(int fd, const char *mode)
{
	FILE *f = fdopen(fd, mode);
	if(f == (FILE*) NULL) {
		perror("Error opening file ");
		exit(EXIT_FAILURE);
	} else
		return f;
}

void str_tolower(char *str)
{
	for(; *str != '\0'; str++)
		if(*str >= 'A' && *str <= 'Z')
			*str = *str + 32;	// - 'A' + 'a'
}

void str_toupper(char *str)
{
	for(; *str != '\0'; str++)
		if(*str >= 'a' && *str <= 'z')
			*str = *str - 32;	// + 'A' - 'a'
}

bool startswith(const char *str, const char *prefix)
{
	while(*str == *prefix) {
		str++, prefix++;
		if(*str == '\0')
			break;
	}
	return *prefix == '\0';
}

bool endswith(const char *str, const char *suffix)
{
	int i, j;
	for(i = 0; str[i] != '\0'; i++);
	for(j = 0; suffix[j] != '\0'; j++);

	if(i < j)
		return false;

	for(; j >= 0; i--, j--)
		if(str[i] != suffix[j])
			return false;
	return true;
}

// xmalloc
char *extract(char *str, char start, char end)
{
	char *extracted = NULL;
	unsigned i;

	for(; *str != start && *str != '\0'; str++);

	if(*str != '\0') {
		for(str++, i = 0; str[i] != end && str[i] != '\0'; i++);
		if(str[i] != '\0' || end == '\0') {
			extracted = xmalloc(i);
			memcpy(extracted, str, i);
			extracted[i] = '\0';
		}
	}
	return extracted;
}

char *neg_strchr(char *s, int c)
{
	while(*s++ == c);
	if(*s == '\0')
		return (char*) NULL;
	else
		return s;
}

bool is_valid_int(char *str)
{
	int i;

	for(i = 0; str[i] != '\0'; i++)
		if( ! isdigit(str[i]))
			return false;
	return true;
}

char *itoa(int n, char *buffer)
{
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

int hexatoi(const char *hex)
{
	register int size;
	register unsigned res, pow;

	for(size = 0; hex[size] != '\0'; size++);

	for(pow = 1, res = 0, size--; size >= 0; pow <<= 4, size--)
		res += pow * (hex[size] >= 'a' ? hex[size] - 87 : (hex[size] >= 'A' ? hex[size] - 55 : hex[size] - '0'));
	return res;
}

// xmalloc, strlen
char *const_append(const char *str1, const char *str2)
{
	char *newStr = (char*) NULL;
	size_t len1, len2;

	len1 = strlen(str1);
	len2 = strlen(str2);
	newStr = (char*) xmalloc(len1 + len2 + 1);
	memcpy(newStr, str1, len1);
	memcpy(newStr + len1, str2, len2 + 1);
	return newStr;
}

// xrealloc, strlen
char *append(char *str1, const char *str2)
{
	size_t len1, len2;

	len1 = strlen(str1);
	len2 = strlen(str2);
	str1 = (char*) xrealloc(str1, len1 + len2 + 1);
	memcpy(str1 + len1, str2, len2 + 1);
	return str1;
}

void *initialize_vector(void *dest, const void *src, size_t size, size_t nmemb)
{
	int i;

	memcpy(dest, src, size);
	for(i = 1; i << 1 <= nmemb; i <<= 1)
		memcpy(dest + i * size, dest, i * size);
	memcpy(dest + i * size, dest, (nmemb - i) * size);
	return dest;
}

// xmalloc, xrealloc
char *readLine(FILE *stream)
{
	register unsigned i = 0;
	unsigned currentSize = 32;
	register char c;
	char *str = (char*) xmalloc(32);

#ifdef _POSIX_THREAD_SAFE_FUNCTIONS		// locking and getc_unlocked functions
	flockfile(stream);
	while((c = getc_unlocked(stream)) != EOF && c != '\n') {
		if(i == currentSize)
			str = (char*) xrealloc(str, currentSize <<= 1);
		str[i++] = c;
	}
	funlockfile(stream);
#else
	while((c = getc(stream)) != EOF && c != '\n') {
		if(i == currentSize)
			str = (char*) xrealloc(str, currentSize <<= 1);
		str[i++] = c;
	}
#endif
	if(i == currentSize)
		str = (char*) xrealloc(str, currentSize += 1);
	else if(c == EOF && i == 0) {
		free(str);
		return (char*) NULL;
	} else
		str = realloc(str, i+1);	// freeing space -> no need to check for null
	str[i] = '\0';
	return str;
}

// xmalloc, xrealloc
char *read_file_descriptor(int fd)
{
	unsigned currentSize = 32;
	ssize_t i = 0;
	char *str = (char*) xmalloc(32);

	/* read (currentSize - i) chars at a time, double currentSize and increment
	 * i by the number of characters read and repeat until no more characters
	 * are available */
	do {
		i += read(fd, str + i, currentSize - i);	
		if(i == currentSize)
			str = xrealloc(str, currentSize <<= 1);
	} while(i << 1 == currentSize);

	if(i == -1 || str[0] == 4) {	// 4 == EOT (End Of Transmission, for sockets),
		// see 'ascii' manpage for more details
		free(str);
		return (char*) NULL;
	} else {
		/* remove all non-printable characters from end of string
		 * see 'isprint' manpage for more details */
		while(! isprint(str[i-1]))
			i--;
		/* allocate precisely as much memory (not a single byte more)
		 * as is needed to contain the data */
		if(i == currentSize)
			str = (char*) xrealloc(str, currentSize += 1);
		else	// freeing space -> no need to check for NULL
			str = realloc(str, i+1);
		str[i] = '\0';
	}
	return str;
}

// xmalloc
// returnArray is set to NULL if all chars in str are separator
// returnArray and all elements in returnArray are dynamically allocated -> free them all when done
size_t split_str(const char *str, const char separator, char ***returnArray)
{
	register int i;
	size_t count = 1;

	// COMMENT NEXT 6 LINES TO NOT SKIP CONSECUTIVE separator
	// CHARS AT START OF str
	while(*str == separator)
		str++;
	if(*str == '\0') {
		*returnArray = (char**) NULL;
		return 0;
	}

	for(i = 0; str[i] != '\0'; i++)
		//count += (str[i] == separator);
		count += (str[i] == separator && str[i+1] != separator && str[i+1] != '\0');
	// REPLACE PREVIOUS LINE WITH ABOVE COMMENTED LINE
	// TO NOT SKIP OVER CONSECUTIVE SEPARATORS

	*returnArray = (char**) xmalloc(count * sizeof(char*));

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			// COMMENT NEXT 3 LINES TO NOT SKIP OVER CONSECUTIVE SEPARATORS
			if(i == 0)
				str++;
			else {
				(*returnArray)[count] = (char*) xmalloc(i+1);
				memcpy((*returnArray)[count], str, i);
				(*returnArray)[count++][i] = '\0';
				str += i+1;
			}	// COMMENT THIS LINE TO NOT SKIP OVER CONSECUTIVE SEPARATORS
			i = -1;
		}
	}
	if(i != 0) {
		(*returnArray)[count] = (char*) xmalloc(i+1);
		strcpy((*returnArray)[count++], str);
	}
	return count;
}

// xmalloc
// returnArray is set to NULL if all chars in str are separator
// returnArray is dynamically allocated -> free when done
size_t split_str_lite(char *str, const char separator, char ***returnArray)
{
	register int i;
	size_t count = 1;

	// COMMENT NEXT 6 LINES TO NOT SKIP CONSECUTIVE separator
	// CHARS AT START OF str
	while(*str == separator)
		str++;
	if(*str == '\0') {
		*returnArray = (char**) NULL;
		return 0;
	}

	for(i = 0; str[i] != '\0'; i++)
		//count += (str[i] == separator);
		count += (str[i] == separator && str[i+1] != separator && str[i+1] != '\0');
	// REPLACE PREVIOUS LINE WITH ABOVE COMMENTED LINE
	// TO NOT SKIP OVER CONSECUTIVE SEPARATORS

	*returnArray = (char**) xmalloc(count * sizeof(char*));

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			// COMMENT NEXT 3 LINES TO NOT SKIP OVER CONSECUTIVE SEPARATORS
			if(i == 0)
				str++;
			else {
				(*returnArray)[count] = str;
				(*returnArray)[count++][i] = '\0';
				str += i+1;
			}	// COMMENT THIS LINE TO NOT SKIP OVER CONSECUTIVE SEPARATORS
			i = -1;
		}
	}
	if(i != 0)
		(*returnArray)[count] = str;
	return count;
}
#include <sys/stat.h>
int is_dir(char *path)
{
	struct stat buf;

	if(stat(path, &buf) != 0) {
		perror("Error statting file ");
		return -1;
	} else
		return S_ISDIR(buf.st_mode);
}

#ifdef	_WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif
// xmalloc
char *make_path(const char *old_path, const char *dir_name)
{
	char *new_path = (char*) NULL;
	int len1, len2;

	len1 = strlen(old_path);
	len2 = strlen(dir_name);
	new_path = (char*) xmalloc(len1 + len2 + 2);
	memcpy(new_path, old_path, len1);
	new_path[len1] = FILE_SEPARATOR;
	memcpy(new_path + len1 + 1, dir_name, len2 + 1);
	return new_path;
}
#undef FILE_SEPARATOR

#include <dirent.h>
#include <errno.h>
void dirwalk(const char *path)
{
	DIR *dir;
	struct dirent *entry;
	char *newPath;

	dir = opendir(path);
	if(dir == (DIR*) NULL)
		fprintf(stderr, "Error opening directory '%s' : %s.\n", path, strerror(errno));
	else {
		// iterate over linked list. When we have examined all files in directory, readdir() returns NULL
		while((entry = readdir(dir)) != (struct dirent*)NULL) {
			if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				newPath = make_path(path, entry->d_name);
#ifdef _WIN32
				if(is_dir(newPath))
#else
					if(entry->d_type == DT_DIR)	// windows sucks, this line works on unix-based OSs only
#endif
						dirwalk(newPath);
				free(newPath);
			}
		}
		closedir(dir);
	}
}

void register_signal_handler(int signum, void (*sighandler)(int))
{
	struct sigaction new_sigaction;

	memset(&new_sigaction, 0, sizeof(struct sigaction));
	new_sigaction.sa_handler = sighandler;
	if(sigaction(signum, &new_sigaction, NULL) != 0) {
		perror("Error registering signal handler ");
		exit(EXIT_FAILURE);
	}
}

// Don't bother trying to run the following on windows. You're wasting your time
#ifndef _WIN32

// Color management
static ColorEnv __session__;

void set_color_env(Color c, Color bgc, Font f) {
	printf("\x1B[%d;%dm", c + 30, f);
	__session__.c = c;
	__session__.f = f;
	printf("\x1B[%dm", bgc + 40);
	__session__.bgc = bgc;
}


void print_string(char *str, Color c, Color bgc, Font f) {
	ColorEnv temp = __session__;
	set_color_env(c, bgc, f);
	printf("%s", str);
	set_color_env(temp.c, temp.bgc, temp.f);
}

// input character display
void turn_echoing_off(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
	term.c_lflag &= ~ECHO;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
}

void turn_echoing_on(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
	term.c_lflag |= ECHO;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
}

// program reads input without user having to press enter
void instant_getchar(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
	term.c_lflag &= ~ICANON;
	term.c_cc[VMIN] = 1;	
	term.c_cc[VTIME] = 0;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
}

void normal_getchar(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
	term.c_lflag |= ICANON;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
	}
}

#endif

