/* utils.c
 * Copyright (C) Julien Rabinow <jnr305@nyu.edu>
 *
 * utils.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * utils.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"

/* -------------------- MEMORY MANAGEMENT -------------------- */
#ifdef MANAGE_MEM
static void *heap_bottom = (void*) NULL, *heap_top = (void*) NULL;
#ifdef USING_VALGRIND
static void *__initial_alloc__ = (void*) NULL;

static void __clean_initial_alloc(void)
{
	free(__initial_alloc__);
}
#endif /* #ifdef USING_VALGRIND */

void init_alloc(void)
{
	int count = 0;
	do {
		heap_bottom = heap_top = malloc(1);
		if(unlikely(heap_bottom == (void*) NULL))
			switch(errno) {
				case ENOMEM:
					log_message(LOG_ERROR, "Error allocating memory: %s", strerror(errno));
					if(count++ < MAX_RETRIES_ALLOC) {
						log_message(LOG_ERROR, "Retrying in 100ms");
						usleep(100);
					} else {
						log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_ALLOC);
						exit(EXIT_FAILURE);
					}
					break;
				default:
					log_message(LOG_FATAL, "Error allocating memory: %s", strerror(errno));
					exit(EXIT_FAILURE);
			}
	} while(unlikely(heap_bottom == (void*) NULL));
#ifdef USING_VALGRIND
	__initial_alloc__ = heap_bottom;
	if(atexit(&__clean_initial_alloc)) {
		log_message(LOG_FATAL, "Error registering cleanup function");
		exit(EXIT_FAILURE);
	}
#endif /* #ifdef USING_VALGRIND */
}

BOOL_TYPE is_allocated(const void *ptr)
{
	return heap_bottom <= ptr && ptr <= heap_top;
}

void xfree(void *ptr)
{
	if(heap_bottom <= ptr && ptr <= heap_top)
		free(ptr);
}
#endif /* #ifdef MANAGE_MEM */

/* -------------------- ERROR HANDLING -------------------- */
#if defined(ENABLE_ERROR_HANDLING) || defined(INTERNAL_ERROR_HANDLING)

void *xmalloc(size_t size)
{
	void *ptr = (void*) NULL;
	int count = 0;

	do {
		ptr = malloc(size);
		if(likely(ptr != (void*) NULL) || size == 0)
			break;
#ifdef __unix__
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error allocating memory: %s", strerror(errno));
				if(count++ < MAX_RETRIES_ALLOC) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix__ */
				log_message(LOG_FATAL, "Error allocating memory: %s", strerror(errno));
				exit(EXIT_FAILURE);
#ifdef __unix__
		}
#endif /* #ifdef __unix__ */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if(ptr < heap_bottom)
		heap_bottom = ptr;
	if(ptr > heap_top)
		heap_top = ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
	void *ptr = (void*) NULL;
	int count = 0;

	do {
		ptr = calloc(nmemb, size);
		if(likely(ptr != (void*) NULL) || size == 0 || nmemb == 0)
			break;
#ifdef __unix__
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error allocating memory: %s", strerror(errno));
				if(count++ < MAX_RETRIES_ALLOC) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix__ */
				log_message(LOG_FATAL, "Error allocating memory: %s", strerror(errno));
				exit(EXIT_FAILURE);
#ifdef __unix__
		}
#endif /* #ifdef __unix__ */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if(ptr < heap_bottom)
		heap_bottom = ptr;
	if(ptr > heap_top)
		heap_top = ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

char *xstrdup(const char *str)
{
	char *ptr = (char*) NULL;
	int count = 0;

	do {
		ptr = strdup(str);
		if(likely(ptr != (char*) NULL))
			break;
#ifdef __unix__
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error allocating memory: %s", strerror(errno));
				if(count++ < MAX_RETRIES_ALLOC) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix__ */
				log_message(LOG_FATAL, "Error allocating memory: %s", strerror(errno));
				exit(EXIT_FAILURE);
#ifdef __unix__
		}
#endif /* #ifdef __unix__ */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if((void*) ptr < heap_bottom)
		heap_bottom = (void*) ptr;
	if((void*) ptr > heap_top)
		heap_top = (void*) ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
	int count = 0;

	do {
		ptr = realloc(ptr, size);
		if(likely(ptr != (void*) NULL) || size == 0)
			break;
#ifdef __unix__
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error allocating memory: %s", strerror(errno));
				if(count++ < MAX_RETRIES_ALLOC) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix__ */
				log_message(LOG_FATAL, "Error allocating memory: %s", strerror(errno));
				exit(EXIT_FAILURE);
#ifdef __unix__
		}
#endif /* #ifdef __unix__ */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if(ptr < heap_bottom)
		heap_bottom = ptr;
	if(ptr > heap_top)
		heap_top = ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

FILE *xfopen(const char *path, const char *mode)
{
	FILE *f = (FILE*) NULL;
	int count = 0;

	do {
		f = fopen(path, mode);
		if(f != (FILE*) NULL)
			break;
#ifdef __unix__
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error opening file: %s", strerror(errno));
				if(count++ < MAX_RETRIES_OPEN) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			case EINTR:
				log_message(LOG_ERROR, "Error opening file: %s\nRetrying", strerror(errno));
				if(count++ >= MAX_RETRIES_OPEN) {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix__ */
				log_message(LOG_FATAL, "Error opening file: %s", strerror(errno));
				exit(EXIT_FAILURE);
#ifdef __unix__
		}
#endif /* #ifdef __unix__ */
	} while(BOOL_TRUE);

	return f;
}

FILE *xfdopen(int fd, const char *mode)
{
	FILE *f = (FILE*) NULL;
	int count = 0;

	do {
		f = fdopen(fd, mode);
		if(f != (FILE*) NULL)
			break;
#ifdef __unix__
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error opening file: %s", strerror(errno));
				if(count++ < MAX_RETRIES_OPEN) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			case EINTR:
				log_message(LOG_ERROR, "Error opening file: %s\nRetrying", strerror(errno));
				if(count++ >= MAX_RETRIES_OPEN) {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix__ */
				log_message(LOG_FATAL, "Error opening file: %s", strerror(errno));
				exit(EXIT_FAILURE);
#ifdef __unix__
		}
#endif /* #ifdef __unix__ */
	} while(BOOL_TRUE);

	return f;
}

#ifdef __unix__

int xopen(const char *path, int flags, ...)
{
	int fd, count = 0, mode = 0;
	va_list ap;

#if defined(__linux__) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 1, 0))
	if(flags & O_CREAT || flags & O_TMPFILE)
#else
	if(flags & O_CREAT)
#endif /* #if defined(__linux__) && LINUX_VERSION_CODE <= KERNEL_VERSION(3, 1, 0) */
	{
		va_start(ap, flags);
#ifdef __APPLE__
		mode = (mode_t) va_arg(ap, int);
#else
		mode = va_arg(ap, mode_t);
#endif /* #ifdef __APPLE__ */
		va_end(ap);
	}
	do {
		fd = open(path, flags, mode);
		if(fd != -1)
			break;
		switch(errno) {
			case ENOMEM:
				log_message(LOG_ERROR, "Error opening file: %s", strerror(errno));
				if(count++ < MAX_RETRIES_OPEN) {
					log_message(LOG_ERROR, "Retrying in 100ms");
					usleep(100);
				} else {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
#if defined(_GNU_SOURCE) && defined(__linux__)
			case EINVAL:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
				flags &= ~O_DIRECT;
#pragma GCC diagnostic pop
#endif /* #if defined(_GNU_SOURCE) && defined(__linux__) */
			case EINTR:
				log_message(LOG_ERROR, "Error opening file: %s\nRetrying", strerror(errno));
				if(count++ >= MAX_RETRIES_OPEN) {
					log_message(LOG_FATAL, "Giving up after %d tries", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			default:
				log_message(LOG_FATAL, "Error opening file: %s", strerror(errno));
				exit(EXIT_FAILURE);
		}
	} while(BOOL_TRUE);

	return fd;
}
#endif /* ifdef __unix__ */

#endif /* #if defined(ENABLE_ERROR_HANDLING) || defined(INTERNAL_ERROR_HANDLING) */

/* -------------------- STRING MANIPULATION -------------------- */
#ifdef ENABLE_STRING_MANIPULATION

BOOL_TYPE is_valid_int(const char *str)
{
	if( ! isdigit(*str) && *str != '-')
		return BOOL_FALSE;

	for(str++; *str != '\0'; str++)
		if( ! isdigit(*str))
			return BOOL_FALSE;
	return BOOL_TRUE;
}

BOOL_TYPE is_valid_float(const char *str)
{
	int period = BOOL_FALSE;

	if(*str == '.')
		period = BOOL_TRUE;
	else if( ! isdigit(*str) && *str != '-')
		return BOOL_FALSE;

	for(str++; *str != '\0'; str++)
		if(*str == '.' && period == BOOL_FALSE)
			period = BOOL_TRUE;
		else if( ! isdigit(*str))
			return BOOL_FALSE;
	return BOOL_TRUE;
}

BOOL_TYPE is_valid_hex(const char *str)
{
	size_t i;

	for(i = 0; str[i] != '\0'; i++)
		if( ! isxdigit(str[i]))
			return BOOL_FALSE;
	return BOOL_TRUE;
}

BOOL_TYPE startswith(const char *str, const char *prefix)
{
	while(*str == *prefix) {
		str++, prefix++;
		if(*str == '\0')
			break;
	}
	return *prefix == '\0';
}

BOOL_TYPE endswith(const char *str, const char *suffix)
{
	int i, j;
	for(i = 0; str[i] != '\0'; i++);
	for(j = 0; suffix[j] != '\0'; j++);

	if(i < j)
		return BOOL_FALSE;

	for(; j >= 0; i--, j--)
		if(str[i] != suffix[j])
			return BOOL_FALSE;
	return BOOL_TRUE;
}

void str_tolower(char *str)
{
	for(; *str != '\0'; str++)
		if(*str >= 'A' && *str <= 'Z')
			*str = *str + 32;	/* - 'A' + 'a' */
}

void str_toupper(char *str)
{
	for(; *str != '\0'; str++)
		if(*str >= 'a' && *str <= 'z')
			*str = *str - 32;	/* + 'A' - 'a' */
}

#if (! defined(__linux__)) && (! defined(BSD)) && (! defined(__MACH__))
char *stpcpy(char *dest, const char *src)
{
	size_t len = strlen(src);

	memcpy(dest, src, len + 1);

	return dest + 1;
}
#endif /* #if (! defined(__linux__)) && (! defined(BSD)) && (! defined(__MACH__)) */

#ifndef __linux__
char *strchrnul(const char *s, int c)
{
	const char *ptr = strchr(s, c);

	return (char*) (ptr != (char*) NULL ? ptr : ptr + strlen(s));
}
#endif /* #ifndef __linux__ */

#ifdef C99
char *va_const_append(const char *str, va_list ap)
{
	char *new_str = (char*) NULL, *ptr = (char*) NULL;
	va_list aq;
	size_t len = strlen(str) + 1;

	va_copy(aq, ap);
	while((ptr = va_arg(ap, char*)) != (char*) NULL)
		len += strlen(ptr);
#ifdef INTERNAL_ERROR_HANDLING
	ptr = new_str = (char*) xmalloc(len * sizeof(char));
#else
	ptr = new_str = (char*) malloc(len * sizeof(char));
	if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		ptr = stpcpy(new_str, str);
		while((str = va_arg(aq, char*)) != (char*) NULL)
			ptr = stpcpy(ptr, str);
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */

	return new_str;
}

#undef const_append
char *const_append(const char *str, ...)
{
	va_list ap;
	char *new_str = (char*) NULL;

	va_start(ap, str);
	new_str = va_const_append(str, ap);
	va_end(ap);

	return new_str;
}
#define const_append(...)		const_append(__VA_ARGS__, (char*) NULL)

#undef append
char *append(char *str, ...)
{
	char *ptr = (char*) NULL, *new_str;
	va_list ap;
	size_t orig_len, len = 0;

	new_str = str;
	len = new_str == (char*) NULL ? 0 : strlen(new_str);
	orig_len = len;
	va_start(ap, str);
#if defined(MANAGE_MEM) && defined(C99)
	if( ! is_allocated(str))
		return va_const_append(str, ap);
#endif /* #if defined(MANAGE_MEM) && defined(C99) */
	while((ptr = va_arg(ap, char*)) != (char*) NULL)
		len += strlen(ptr);
	len++;
	va_end(ap);
#ifdef INTERNAL_ERROR_HANDLING
	new_str = (char*) xrealloc(new_str, len * sizeof(char));
#else
	new_str = (char*) realloc(new_str, len * sizeof(char));
	if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		va_start(ap, str);
		ptr = new_str + orig_len;
		/* avoid overflow by doing the calculation in 2 steps */
		while((str = va_arg(ap, char*)) != (char*) NULL)
			ptr = stpcpy(ptr, str);
		va_end(ap);
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */

	return new_str;
}
#define append(...)	append(__VA_ARGS__, (char*) NULL)
#endif /* #ifdef C99 */

char *extract(const char *str, char start, char end)
{
	char *extracted = (char*) NULL;
	unsigned i;

	for(; *str != start && *str != '\0'; str++);

	if(*str != '\0') {
		for(str++, i = 0; str[i] != end && str[i] != '\0'; i++);
		if(str[i] != '\0' || end == '\0') {
#ifdef INTERNAL_ERROR_HANDLING
			extracted = (char*) xmalloc(i + 1);
#else
			extracted = (char*) malloc(i + 1);
			if(extracted != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
				memcpy(extracted, str, i);
				extracted[i] = '\0';
#ifndef INTERNAL_ERROR_HANDLING
			}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
		}
	}
	return extracted;
}

char *trim(const char *str)
{
	const char *start_str, *end_str;
	char *ptr;
	size_t len;

	for(start_str = str; isspace(*start_str); start_str++);
	for(end_str = start_str; ! isspace(*end_str) && *end_str != '\0'; end_str++);

	for(ptr = (char*) end_str; *ptr != '\0'; ptr++)
		if(isspace(*ptr) && ! isspace(*(ptr - 1)))
			end_str = ptr;
	len = (size_t) (end_str - start_str);
#ifdef INTERNAL_ERROR_HANDLING
	ptr = (char*) xmalloc(len + 1);
#else
	ptr = (char*) malloc(len + 1);
	if(ptr != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		memcpy(ptr, start_str, len);
		ptr[len] = '\0';
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */

	return ptr;
}

char *insert(const char *str, char c, size_t pos)
{
	char *new_str = (char*) NULL;
	size_t len = strlen(str);

	if(pos <= len) {
#ifdef INTERNAL_ERROR_HANDLING
		new_str = (char*) xmalloc(len + 2);
#else
		new_str = (char*) malloc(len + 2);
		if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
			memcpy(new_str, str, pos);
			new_str[pos] = c;
			memcpy(new_str + pos + 1, str + pos, len - pos + 1);
#ifndef INTERNAL_ERROR_HANDLING
		}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	}
	return new_str;
}

char *insert_str(const char *str, const char *ins, size_t pos)
{
	size_t len1, len2;
	char *new_str = (char*) NULL;

	len1 = strlen(str);
	if(pos <= len1) {
		len2 = strlen(ins);
#ifdef INTERNAL_ERROR_HANDLING
		new_str = (char*) xmalloc(len1 + len2 + 1);
#else
		new_str = (char*) malloc(len1 + len2 + 1);
		if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
			memcpy(new_str, str, pos);
			memcpy(new_str + pos, ins, len2);
			memcpy(new_str + pos + len2, str + pos, len1 - pos + 1);
#ifndef INTERNAL_ERROR_HANDLING
		}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	}
	return new_str;
}

char *erase(const char *str, size_t pos)
{
	char *new_str = (char*) NULL;
	size_t len = strlen(str);

	if(pos < len) {
#ifdef INTERNAL_ERROR_HANDLING
		new_str = (char*) xmalloc(len);
#else
		new_str = (char*) malloc(len);
		if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
			memcpy(new_str, str, pos);
			memcpy(new_str + pos, str + pos + 1, len - pos);
#ifndef INTERNAL_ERROR_HANDLING
		}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	}
	return new_str;
}

char *erase_str(const char *str, size_t pos, size_t len)
{
	char *new_str = (char*) NULL;
	size_t len2 = strlen(str), new_len = len2 - len + 1;

	if(pos + len <= len2) {
#ifdef INTERNAL_ERROR_HANDLING
		new_str = (char*) xmalloc(new_len);
#else
		new_str = (char*) malloc(new_len);
		if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
			memcpy(new_str, str, pos);
			memcpy(new_str + pos, str + pos + len, new_len - pos);
#ifndef INTERNAL_ERROR_HANDLING
		}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */

	}
	return new_str;
}

char *replace_str(const char *haystack, const char *needle, const char *replacement)
{
	char *new_str = (char*) NULL;
	char *ptr = strstr(haystack, needle);
	size_t len_replacement = strlen(replacement), len_needle = strlen(needle),
		len_haystack = strlen(haystack);

	if(ptr != (char*) NULL) {
#ifdef INTERNAL_ERROR_HANDLING
		new_str = xmalloc(len_haystack - len_needle + len_replacement + 1);
#else
		new_str = malloc(len_haystack - len_needle + len_replacement + 1);
		if(new_str != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
			memcpy(new_str, haystack, ptr - haystack);
			memcpy((byte*)((intptr_t) new_str + (intptr_t) ptr -
						(intptr_t) haystack), replacement, len_replacement);
			memcpy((byte*)((intptr_t) new_str + (intptr_t) ptr -
						(intptr_t) haystack) + len_replacement, ptr + len_needle,
					len_haystack - (intptr_t) ptr + (intptr_t) haystack - len_needle);
#ifndef INTERNAL_ERROR_HANDLING
		}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	}
	return new_str;
}

const char *rev_strpbrk(const char *str, const char *accept)
{
	const char *ptr = (const char*) NULL, *iter = str;

	for(iter = str; *iter != '\0'; iter++)
		if(strchr(accept, *iter))
			ptr = iter;
	return ptr;
}

char *neg_strchr(const char *s, int c)
{
	while(*s++ == c);
	return (char*) (*s == '\0' ? NULL : s);
}

unsigned count_characters(const char *str, char c)
{
	unsigned count;

	for(count = 0; *str != '\0'; count += *str++ == c)
		;
	return count;
}

/* return_array is set to NULL if all chars in str are separator
 * return_array and all elements in return_array are dynamically allocated -> free them all when done */
size_t split_str(const char *str, char separator, char ***return_array)
{
	int i;
	size_t count = 1;

	/* COMMENT NEXT 6 LINES TO NOT SKIP CONSECUTIVE separator
	 * CHARS AT START OF str */
	while(*str == separator)
		str++;
	if(*str == '\0') {
		*return_array = (char**) NULL;
		return 0;
	}

	for(i = 0; str[i] != '\0'; i++)
		/*count += (str[i] == separator); */
		count += (str[i] == separator && str[i+1] != separator && str[i+1] != '\0');
	/* REPLACE PREVIOUS LINE WITH ABOVE COMMENTED LINE
	 * TO NOT SKIP OVER CONSECUTIVE SEPARATORS */

#ifdef INTERNAL_ERROR_HANDLING
	*return_array = (char**) xmalloc(count * sizeof(char*));
#else
	*return_array = (char**) malloc(count * sizeof(char*));
	if(unlikely(*return_array == (char**) NULL))
		return 0;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			/* COMMENT NEXT 3 LINES TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			if(i == 0)
				str++;
			else {
#ifdef INTERNAL_ERROR_HANDLING
				(*return_array)[count] = (char*) xmalloc(i + 1);
#else
				(*return_array)[count] = (char*) malloc(i + 1);
				if(unlikely((*return_array)[count] == (char*) NULL)) {
					for(; count > 0; count--)
						free((*return_array)[count - 1]);
					free(*return_array);
					*return_array = (char**) NULL;
					return 0;
				}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
				memcpy((*return_array)[count], str, i);
				(*return_array)[count++][i] = '\0';
				str += i+1;
			/* COMMENT THE NEXT LINE TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			}
			i = -1;
		}
	}
	if(i != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		(*return_array)[count] = (char*) xmalloc(i + 1);
#else
		(*return_array)[count] = (char*) malloc(i + 1);
		if(unlikely((*return_array)[count] == (char*) NULL)) {
			for(; count > 0; count--)
				free((*return_array)[count - 1]);
			free(*return_array);
			*return_array = (char**) NULL;
			return 0;
		}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		strcpy((*return_array)[count++], str);
	}
	return count;
}

/* return_array is set to NULL if all chars in str are separator
 * return_array is dynamically allocated -> free when done */
size_t split_str_lite(char *str, char separator, char ***return_array)
{
	int i;
	size_t count = 1;

	/* COMMENT NEXT 6 LINES TO NOT SKIP CONSECUTIVE separator
	 * CHARS AT START OF str */
	while(*str == separator)
		str++;
	if(*str == '\0') {
		*return_array = (char**) NULL;
		return 0;
	}

	for(i = 0; str[i] != '\0'; i++)
		/*count += (str[i] == separator); */
		count += (str[i] == separator && str[i+1] != separator && str[i+1] != '\0');
	/* REPLACE PREVIOUS LINE WITH ABOVE COMMENTED LINE
	 * TO NOT SKIP OVER CONSECUTIVE SEPARATORS */

#ifdef INTERNAL_ERROR_HANDLING
	*return_array = (char**) xmalloc(count * sizeof(char*));
#else
	*return_array = (char**) malloc(count * sizeof(char*));
	if(unlikely(*return_array == (char**) NULL))
		return 0;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			/* COMMENT NEXT 3 LINES TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			if(i == 0)
				str++;
			else {
				(*return_array)[count] = str;
				(*return_array)[count++][i] = '\0';
				str += i+1;
			}	/* COMMENT THIS LINE TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			i = -1;
		}
	}
	if(i != 0)
		(*return_array)[count++] = str;
	return count;
}

char* str_join(int str_array_size, char **str_array, char *separator)
{
	int total_len = 0, i;
	size_t separator_len;
	char *str = (char *) NULL, *start;

	separator_len = strlen(separator);
	for(i = 0; i < str_array_size; i++)
		total_len += strlen(str_array[i]);

#ifdef INTERNAL_ERROR_HANDLING
	str = (char *) xmalloc((total_len + separator_len *
				(str_array_size - 1) + 1) * sizeof(char));
#else
	str = (char *) malloc((total_len + separator_len *
				(str_array_size - 1) + 1) * sizeof(char));
	if(unlikely(str == (char *) NULL))
		return (char *) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	start = str;
	for(i = 0; i < str_array_size - 1; i++) {
		start = stpcpy(start, str_array[i]);
		memcpy(start, separator, separator_len);
		start += separator_len;
	}
	strcpy(start, str_array[i]);
	return str;
}
#endif /* ifdef ENABLE_STRING_MANIPULATION */

/*void delete_stringbuffer(StringBuffer sb)
{
	size_t i;

	for(i = 0; i < sb->size; i++)
		free(sb->data[i]);
	free(sb->data);
	free(sb);
} */

/* -------------------- Reading data -------------------- */
#ifdef ENABLE_READ_DATA

char *read_line(FILE *stream)
{
	unsigned i = 0;
	unsigned current_size = 32;
	char c;
#ifdef INTERNAL_ERROR_HANDLING
	char *str = (char*) xmalloc(current_size);
#else
	char *str = (char*) malloc(current_size);
	if(unlikely(str == (char*) NULL))
		return (char*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

#if defined(_POSIX_THREAD_SAFE_FUNCTIONS) && (_POSIX_C_SOURCE >= 1 || \
		defined(_XOPEN_SOURCE) || defined(POSIX_SOURCE) || \
		defined(_BSD_SOURCE) || defined(_SVID_SOURCE))
	/* locking and getc_unlocked functions */
	flockfile(stream);
	while((c = getc_unlocked(stream)) != EOF && c != '\n')
#else
		while((c = getc(stream)) != EOF && c != '\n')
#endif /* #if defined(_POSIX_THREAD_SAFE_FUNCTIONS) && (_POSIX_C_SOURCE >= 1 || \
	  defined(_XOPEN_SOURCE) || defined(POSIX_SOURCE) || \
	  defined(_BSD_SOURCE) || defined(_SVID_SOURCE)) */
		{
			if(i == current_size) {
#ifdef INTERNAL_ERROR_HANDLING
				str = (char*) xrealloc(str, current_size <<= 1);
#else
				str = (char*) realloc(str, current_size <<= 1);
				if(unlikely(str == (char*) NULL))
					return (char*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
			}
			str[i++] = c;
		}
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS) && (_POSIX_C_SOURCE >= 1 || \
		defined(_XOPEN_SOURCE) || defined(POSIX_SOURCE) || \
		defined(_BSD_SOURCE) || defined(_SVID_SOURCE))
	funlockfile(stream);
#endif /* #if defined(_POSIX_THREAD_SAFE_FUNCTIONS) && (_POSIX_C_SOURCE >= 1 || \
	  defined(_XOPEN_SOURCE) || defined(POSIX_SOURCE) || \
	  defined(_BSD_SOURCE) || defined(_SVID_SOURCE)) */
	if(i == current_size) {
#ifdef INTERNAL_ERROR_HANDLING
		str = (char*) xrealloc(str, current_size += 1);
#else
		str = (char*) realloc(str, current_size += 1);
		if(unlikely(str == (char*) NULL))
			return (char*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	} else if(unlikely(c == EOF && i == 0)) {
		free(str);
		return (char*) NULL;
	} else {
#ifdef INTERNAL_ERROR_HANDLING
		str = xrealloc(str, i + 1);
#else
		str = realloc(str, i + 1);
		if(unlikely(str == (char*) NULL))
			return (char*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	str[i] = '\0';
	return str;
}

char *read_fd_str(int fd)
{
	ssize_t n;
	char *res = (char*) read_file_descriptor(fd, &n);
	res[n] = '\0';
	return res;
}

byte *read_file_descriptor(int fd, ssize_t *n)
{
	unsigned current_size = 32;
	ssize_t i = 0, ret = 0;
#ifdef INTERNAL_ERROR_HANDLING
	byte *mem = (byte*) xmalloc(current_size);
#else
	byte *mem = (byte*) malloc(current_size);
	if(unlikely(mem == (byte*) NULL))
		return (byte*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	/* read (current_size - i) chars at a time, double current_size and increment
	 * i by the number of characters read and repeat until no more characters
	 * are available */
	do {
		ret = read(fd, mem + i, current_size - i);
		if(unlikely(ret <= 0))
			break;
		i += ret;
		if(i == (ssize_t) current_size) {
#ifdef INTERNAL_ERROR_HANDLING
			mem = (byte*) xrealloc(mem, current_size <<= 1);
#else
			mem = (byte*) realloc(mem, current_size <<= 1);
			if(unlikely(mem == (byte*) NULL))
				return (byte*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		}
	} while(i << 1 == (ssize_t) current_size);

	if(ret == -1) {
		free(mem);
		return (byte*) NULL;
	} else {
		/* allocate precisely as much memory (not a single byte more)
		 * as is needed to contain the data */
#ifdef INTERNAL_ERROR_HANDLING
		if(i == (ssize_t) current_size)
			mem = (byte*) xrealloc(mem, current_size += 1);
		else
			mem = (byte*) xrealloc(mem, i + 1);
#else
		if(i == (ssize_t) current_size)
			mem = (byte*) realloc(mem, current_size += 1);
		else
			mem = (byte*) realloc(mem, i + 1);
		if(unlikely(mem == (byte*) NULL))
			return (byte*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	*n = i;
	return mem;
}

char *read_file_str(const char *path)
{
	ssize_t n;
	char *res = (char*) read_file(path, &n);

	res[n] = '\0';
	return res;
}

byte *read_file(const char *path, ssize_t *n)
{
	byte *ptr;
	int fd;

#ifdef INTERNAL_ERROR_HANDLING
#if defined(__linux__) && defined(_GNU_SOURCE)
	fd = xopen(path, O_RDONLY | O_CLOEXEC);
#else
	fd = xopen(path, O_RDONLY);
	if(fcntl(fd, F_SETFD, FD_CLOEXEC) != 0) {
		log_message(LOG_FATAL, "Failed setting FD_CLOEXEC flag on file descriptor: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
#endif /* #if defined(__linux__) && defined(_GNU_SOURCE) */
#else
#if defined(__linux__) && defined(_GNU_SOURCE)
	fd = open(path, O_RDONLY | O_CLOEXEC);
#else
	fd = open(path, O_RDONLY);
	if(fcntl(fd, F_SETFD, FD_CLOEXEC) != 0)
		log_message(LOG_ERROR, "Failed setting FD_CLOEXEC flag on file descriptor: %s", strerror(errno));
#endif /* #if defined(__linux__) && defined(_GNU_SOURCE) */
	if(unlikely(fd == -1))
		return (byte*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	ptr = read_file_descriptor(fd, n);
	close(fd);

	return ptr;
}
#endif /* #ifdef ENABLE_READ_DATA */



/* -------------------- DATA STRUCTURES -------------------- */
#ifdef ENABLE_DATASTRUCTS

/* ----- Double-linked list ----- */
DLinkedList new_dlinkedlist(void)
{
#ifdef INTERNAL_ERROR_HANDLING
	DLinkedList dl = (DLinkedList) xmalloc(sizeof(__datastruct__));
#else
	DLinkedList dl = (DLinkedList) malloc(sizeof(__datastruct__));
	if(likely(dl != (DLinkedList) NULL))
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		dl->in = dl->out = (__datastruct_elem__*) NULL;
	return dl;
}

void delete_dlinkedlist(DLinkedList dl, void (*__del__)(void*))
{
	__datastruct_elem__ *e = dl->in, *next;;
	if(__del__ != (void(*)(void*)) NULL) {
		while(e != (__datastruct_elem__*) NULL) {
			__del__(e->data);
			next = e->next;
			free(e);
			e = next;
		}
		e = dl->out;
		while(e != (__datastruct_elem__*) NULL) {
			__del__(e->data);
			next = e->next;
			free(e);
			e = next;
		}
	} else {
		while(e != (__datastruct_elem__*) NULL) {
			next = e->next;
			free(e);
			e = next;
		}
		e = dl->out;
		while(e != (__datastruct_elem__*) NULL) {
			next = e->next;
			free(e);
			e = next;
		}
	}
}

void *dll_add(DLinkedList dl, void *data)
{
#ifdef INTERNAL_ERROR_HANDLING
	__datastruct_elem__ *e = (__datastruct_elem__*) xmalloc(sizeof(__datastruct_elem__));
#else
	__datastruct_elem__ *e = (__datastruct_elem__*) malloc(sizeof(__datastruct_elem__));
	if(likely(e != (__datastruct_elem__*) NULL)) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		e->data = data;
		e->next = dl->out;
		dl->out = e;
		return data;
#ifndef INTERNAL_ERROR_HANDLING
	} else
		return (void*) NULL;
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
}

void *dll_remove(DLinkedList dl)
{
	void *data = (void*) NULL;
	__datastruct_elem__ *e = dl->out;

	if(e != (__datastruct_elem__*) NULL) {
		dl->out = e->next;
		free(e);
	}
	return data;
}

void dll_rewind(DLinkedList dl)
{
	__datastruct_elem__ *e = dl->in;

	while(e != (__datastruct_elem__*) NULL) {
		dl->in = e->next;
		e->next = dl->out;
		dl->out = e;
	}
}

void *dll_iterate(DLinkedList dl)
{
	void *data = (void*) NULL;
	__datastruct_elem__ *e = dl->out;

	if(e != (__datastruct_elem__*) NULL) {
		data = e->data;
		dl->out = e->next;
		e->next = dl->in;
		dl->in = e;
	}
	return data;
}

void *dll_rev_iterate(DLinkedList dl)
{
	void *data = (void*) NULL;
	__datastruct_elem__ *e = dl->in;

	if(dl->out != (__datastruct_elem__*) NULL)
		data = dl->out->data;
	if(e != (__datastruct_elem__*) NULL) {
		dl->in = e->next;
		e->next = dl->out;
		dl->out = e;
	}
	return data;
}

void *dll_head(DLinkedList dl)
{
	return dl->out != (__datastruct_elem__*) NULL ? dl->out->data : (void*) NULL;
}

DLinkedList dll_tail(DLinkedList dl)
{
#ifdef INTERNAL_ERROR_HANDLING
	DLinkedList tl = (DLinkedList) xmalloc(sizeof(__datastruct__));
#else
	DLinkedList tl = (DLinkedList) malloc(sizeof(__datastruct__));
	if(likely(tl != (DLinkedList) NULL)) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		tl->in = dl->out;
		if(tl->in != (__datastruct_elem__*) NULL)
			tl->out = tl->in->next;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	return tl;
}

DLinkedList dll_copy_interator(DLinkedList dl)
{
#ifdef INTERNAL_ERROR_HANDLING
	DLinkedList new_dl = (DLinkedList) xmalloc(sizeof(__datastruct__));
#else
	DLinkedList new_dl = (DLinkedList) malloc(sizeof(__datastruct__));
	if(likely(new_dl != (DLinkedList) NULL)) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		new_dl->in = dl->in;
		new_dl->out = dl->out;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	return new_dl;
}
#if 0
/* TODO: finish implementing */
DLinkedList clone_dll(DLinkedList dl, void *(*__clonefunc__)(void*))
{
	failwith("function not yet implemented");
	__datastruct_elem__ *e, *e_copy;
#ifdef INTERNAL_ERROR_HANDLING
	DLinkedList new_dl = (DLinkedList) xmalloc(sizeof(__datastruct__));
#else
	DLinkedList new_dl = (DLinkedList) malloc(sizeof(__datastruct__));
	if(likely(new_dl != (DLinkedList) NULL)) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		e = dl->in;
		while(e != (__datastruct_elem__*) NULL) {
#ifdef INTERNAL_ERROR_HANDLING
			e_copy = (__datastruct_elem__*) xmalloc(sizeof(__datastruct_elem__));
#else
			e_copy = (__datastruct_elem__*) malloc(sizeof(__datastruct_elem__));
			if(likely(e_copy != (__datastruct_elem__*) NULL)) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
				e_copy->data = __clonefunc__(e->data);
				e->next = dl->out;
				dl->out = e;
				return data;
#ifndef INTERNAL_ERROR_HANDLING
			} else
				return (void*) NULL;
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
		}
#ifndef INTERNAL_ERROR_HANDLING
	}
	return new_dl;
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
}

/* TODO: implement*/
DLinkedList dll_join(DLinkedList dl1, DLinkedList dl2)
{
	__datastruct_elem__ *e1, *e2;
	failwith("function not yet implemented");
}

/* TODO: implement*/
DLinkedList dll_filter(DLinkedList dl, BOOL_TYPE (*__filterfunc__)(void*))
{
	failwith("function not yet implemented");

}
#endif /* #if 0 */

void *dll_map(DLinkedList dl, void *(*__mapfunc__)(void *data, void *arg), void *arg)
{
	__datastruct_elem__ *e = dl->out;

	while(e != (__datastruct_elem__*) NULL) {
		arg = __mapfunc__(e->data, arg);
		e = e->next;
	}
	return arg;
}

#if 0
/* TODO: implement*/
void dll_sort(DLinkedList dl, int (*__cmp__)(void *d1, void *d2))
{
	failwith("function not yet implemented");
}
#endif /* #if 0 */

/* ----- Stack ----- */
void delete_stack(Stack s, void (*__del__)(void*))
{
	Stack temp;

	if(__del__ != (void(*)(void*)) NULL)
		while(s != (Stack) NULL) {
			__del__(s->data);
			temp = s;
			s = s->next;
			free(temp);
		}
	else
		while(s != (Stack) NULL) {
			temp = s;
			s = s->next;
			free(temp);
		}

}

#undef stack_push
void *stack_push(Stack *s, void *data)
{
#ifdef INTERNAL_ERROR_HANDLING
	__datastruct_elem__ *new = (__datastruct_elem__*) xmalloc(sizeof(__datastruct_elem__));
#else
	__datastruct_elem__ *new = (__datastruct_elem__*) malloc(sizeof(__datastruct_elem__));
	if(likely(new != NULL)) {
#endif	/* #ifdef INTERNAL_ERROR_HANDLING */
		new->data = data;
		new->next = *s;
		*s = new;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	return data;
}

#undef stack_pop
void *stack_pop(Stack *s)
{
	void *ret = (void*) NULL;
	Stack new;

	if(likely(*s != (Stack) NULL)) {
		ret = (*s)->data;
		new = (*s)->next;
		free(*s);
		*s = new;
	}
	return ret;
}

void *stack_peek(Stack s)
{
	return unlikely(s == (Stack) NULL) ? (void*) NULL : s->data;
}

/* ----- Queue ----- */
Queue new_queue(void)
{
#ifdef INTERNAL_ERROR_HANDLING
	Queue q = (Queue) xmalloc(sizeof(__datastruct__));
#else
	Queue q = (Queue) malloc(sizeof(__datastruct__));
	if(likely(q != (Queue) NULL)) {
#endif	/* #ifdef INTERNAL_ERROR_HANDLING */
		q->in = (__datastruct_elem__*) NULL;
		q->out = (__datastruct_elem__*) NULL;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	return q;
}

void delete_queue(Queue q, void (*__del__)(void*))
{
	__datastruct_elem__ *temp, *iterator;

	iterator = q->in;
	if(__del__ != (void(*)(void*)) NULL)
		while(iterator != (__datastruct_elem__*) NULL) {
			__del__(iterator->data);
			temp = iterator;
			iterator = iterator->next;
			free(temp);
		}
	else
		while(iterator != (__datastruct_elem__*) NULL) {
			temp = iterator;
			iterator = iterator->next;
			free(temp);
		}
	free(q);
}

void queue_push(Queue q, void *data)
{
#ifdef INTERNAL_ERROR_HANDLING
	__datastruct_elem__ *new = (__datastruct_elem__*) xmalloc(sizeof(__datastruct_elem__));
#else
	__datastruct_elem__ *new = (__datastruct_elem__*) malloc(sizeof(__datastruct_elem__));

	if(likely(new != (__datastruct_elem__*) NULL)) {
#endif	/* #ifdef INTERNAL_ERROR_HANDLING */
		new->data = data;
		new->next = (__datastruct_elem__*) NULL;
		if(q->in != (__datastruct_elem__*) NULL)
			q->in->next = new;
		q->in = new;
		if(q->out == (__datastruct_elem__*) NULL)
			q->out = q->in;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
}

void *queue_pop(Queue q)
{
	void *ret = (void*) NULL;
	__datastruct_elem__ *temp;

	if(q->out != (__datastruct_elem__*) NULL)
		ret = q->out->data;
	temp = q->out;
	q->out = temp->next;
	if(q->out == (__datastruct_elem__*) NULL)
		q->in = (__datastruct_elem__*) NULL;
	free(temp);

	return ret;
}

void *queue_peek(Queue q)
{
	return likely(q->out != (__datastruct_elem__*) NULL) ? q->out->data : (void*) NULL;
}

/* ----- Bitset ----- */
Bitset new_bitset(size_t size)
{
	size_t mem = (size >> 3) + (size % 8 != 0);
	Bitset b;
#ifdef INTERNAL_ERROR_HANDLING
	b = (Bitset) xmalloc(sizeof(__bitset_struct__) + mem);
#else
	b = (Bitset) malloc(sizeof(__bitset_struct__) + mem);
	if(likely(b != (Bitset) NULL))
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		memset(b->data, 0, mem);
	b->size = size;
	return b;
}

Bitset clone_bitset(Bitset set)
{
	Bitset set_clone;
	size_t mem = (set->size >> 3) + (set->size % 8 != 0);

#ifdef INTERNAL_ERROR_HANDLING
	set_clone = (Bitset) xmalloc(sizeof(__bitset_struct__) + mem);
#else
	set_clone = (Bitset) malloc(sizeof(__bitset_struct__) + mem);
	if(likely(set_clone != (Bitset) NULL)) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		memcpy(set_clone->data, set->data, mem);
		set_clone->size = set->size;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */

	return set_clone;
}

int getbit(const Bitset set, int pos)
{
	return (set->data[pos >> 3] >> (pos % 8)) & 1;
}

void setbit(Bitset set, int pos)
{
	set->data[pos >> 3] |= (1 << (pos % 8));
}

void unsetbit(Bitset set, int pos)
{
	set->data[pos >> 3] &= ~(1 << (pos % 8));
}

int togglebit(Bitset set, int pos)
{
	set->data[pos >> 3] ^= 1 << pos % 8;

	return (set->data[pos >> 3] >> (pos % 8)) & 1;
}
#endif /* #ifdef ENABLE_Bitset */


/* -------------------- Filesystem functions -------------------- */
#ifdef ENABLE_FILESYSTEM
int is_dir(const char *path)
{
	struct stat buf;

	if(lstat(path, &buf) != 0) {
		log_message(LOG_ERROR, "Error statting file: %s", strerror(errno));
		return -1;
	} else
		return S_ISDIR(buf.st_mode);
}

#ifdef	_WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif /* #ifdef _WIN32 */

#undef make_path
char *make_path(const char *path, ...)
{
	char *new_path = (char*) NULL, *ptr = (char*) NULL;
	va_list ap;
	size_t len = strlen(path) + 1;

	va_start(ap, path);
	while((ptr = va_arg(ap, char*)) != (char*) NULL)
		len += strlen(ptr) + 1;
	va_end(ap);
#ifdef INTERNAL_ERROR_HANDLING
	ptr = new_path = (char*) xmalloc(len * sizeof(char));
#else
	ptr = new_path = (char*) malloc(len * sizeof(char));
	if(new_path != (char*) NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		va_start(ap, path);
		ptr = stpcpy(new_path, path);
		while((path = va_arg(ap, char*)) != (char*) NULL) {
			*ptr++ = FILE_SEPARATOR;
			ptr = stpcpy(ptr, path);
		}
		va_end(ap);
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
	return new_path;
}
#undef FILE_SEPARATOR
#ifdef C99
# define make_path(...)	make_path(__VA_ARGS__, (char*) NULL)
#endif /* #ifdef C99 */

void *dirwalk(const char *path, BOOL_TYPE recurse, void *(*func)(char*, void*), void *arg)
{
	DIR *dir;
	struct dirent *entry;
	char *new_path;

	dir = opendir(path);
	if(dir != (DIR*) NULL) {
        while((entry = readdir(dir)) != (struct dirent*) NULL)
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
#ifdef C99
                new_path = make_path(path, entry->d_name);
#else
                new_path = make_path(path, entry->d_name, NULL);
#endif /* #ifdef C99 */
#ifndef INTERNAL_ERROR_HANDLING
                if(unlikely(new_path == (char*) NULL)) {
                    closedir(dir);
                    return (void*) NULL;
                }
#endif /* #ifndef INTERNAL_ERROR_HANDLING */
#ifdef _DIRENT_HAVE_D_TYPE
                if(entry->d_type == DT_DIR)
#else
                    if(is_dir(new_path))
#endif /* #ifdef _WIN32 */
                    {
                        if(recurse)
                            dirwalk(new_path, recurse, func, arg);
                    } else
                        arg = func(new_path, arg);
                free(new_path);
            }
		closedir(dir);
	} else
		arg = (void*) NULL;
	return arg;
}
#endif /* #ifdef ENABLE_FILESYSTEM */


/* -------------------- Networking functions -------------------- */
#ifdef ENABLE_NETWORKING

int connect_to(const char *server_name, unsigned port)
{
	struct addrinfo *adresses = (struct addrinfo*) NULL, *iterator = (struct addrinfo*) NULL;
	char ip_addr[INET6_ADDRSTRLEN] = { 0 }, port_buff[11] = { 0 };
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	snprintf(port_buff, 11, "%u", port);
	if(sockfd == -1) {
		log_message(LOG_ERROR, "Error creating socket: %s", strerror(errno));
		sockfd = CONNECT_ERROR;
	} else if(getaddrinfo(server_name, port_buff, (const struct addrinfo*) NULL, &adresses) != 0) {
		/* DNS resolution. getaddrinfo creates a linked list (pointed to by adresses),
		 * 1 element == 1 IP Address. Function IPv6 compatible. */
		log_message(LOG_ERROR, "Unable to resolve '%s' to a valid IP address", server_name);
		sockfd = CONNECT_ERROR;
	} else {
		log_message(LOG_INFO, "DNS lookup succesful");
		iterator = adresses;
		/* Iterate over linked list until we connect successfully or we run out of addresses. */
		while(connect(sockfd, iterator->ai_addr, (socklen_t) sizeof *iterator->ai_addr) != 0) {
			iterator = iterator->ai_next;
			if(unlikely(iterator == (struct addrinfo*) NULL)) {
				sockfd = CONNECT_ERROR;
				break;
			}
		}
		if(likely(iterator != (struct addrinfo*) NULL))
			log_message(LOG_INFO, "Connection established with %s (%s:%d)", server_name,
					inet_ntop(iterator->ai_family,
						&((struct sockaddr_in*) iterator->ai_addr)->sin_addr,
						ip_addr, sizeof ip_addr),
					ntohs(((struct sockaddr_in*) iterator->ai_addr)->sin_port));
		freeaddrinfo(adresses);
	}
	return sockfd;
}

int create_server(unsigned port)
{
	int sockfd;
	unsigned queue = 1;
	struct sockaddr_in config;

	memset(&config, 0, sizeof config);
	config.sin_family = PF_INET;
	config.sin_addr.s_addr = htonl(INADDR_ANY);
	config.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		log_message(LOG_ERROR, "Error creating socket: %s", strerror(errno));
		sockfd = CONNECT_ERROR;
	} else {
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &queue, sizeof queue) != 0)
			log_message(LOG_ERROR, "Error setting socket option SO_REUSEADDR: %s", strerror(errno));
		/* print error message but keep on going. Failing here will not prevent program from
		 * working correctly */
		queue = SOMAXCONN;
		if(bind(sockfd, (struct sockaddr*) &config, sizeof config) != 0 || listen(sockfd, queue) != 0) {
			shutdown(sockfd, SHUT_RDWR);
			log_message(LOG_ERROR, "Error binding server %d: %s", port, strerror(errno));
			sockfd = CONNECT_ERROR;
		} else
			log_message(LOG_INFO, "Server initialized on port %u", port);
	}
	return sockfd;
}

int get_single_client(int server_socket)
{
	struct sockaddr_in client_config;
	int client_fd;
	char buffer[INET6_ADDRSTRLEN];
	socklen_t size = sizeof client_config;

	client_fd = accept(server_socket, (struct sockaddr*) &client_config, &size);
	if(client_fd != -1)
		log_message(LOG_INFO, "Connection established to %s:%u",
				inet_ntop(AF_INET, &client_config.sin_addr, buffer, INET6_ADDRSTRLEN),
				ntohs(client_config.sin_port));
	else {
		client_fd = CONNECT_ERROR;
		log_message(LOG_ERROR, "Error establishing connection: %s", strerror(errno));
	}

	return client_fd;
}

#endif /* ifdef ENABLE_NETWORKING */


/* -------------------- Terminal manipulation functions and color printing -------------------- */
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)

typedef struct {
	Color c;
	Color bgc;
	Style s;
} ColorEnv;

static ColorEnv __color_session__;

void set_style(Color c, Color bgc, Style s)
{
	printf("\x1B[%d;%dm", c + 30, s);
	__color_session__.c = c;
	__color_session__.s = s;
	printf("\x1B[%dm", bgc + 40);
	__color_session__.bgc = bgc;
}

void stylish_fprintf(FILE *stream, Color c, Color bgc, Style s, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stream, "\x1B[%d;%dm", c + 30, s);
	fprintf(stream, "\x1B[%dm", bgc + 40);
	vprintf(fmt, ap);
	va_end(ap);
	fprintf(stream, "\x1B[%dm", __color_session__.bgc);
	fprintf(stream, "\x1B[%d;%dm", __color_session__.c + 30, __color_session__.s);
	fflush(stream);
}

int turn_echoing_off(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	term.c_lflag &= ~ECHO;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	return 0;
}

int turn_echoing_on(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	term.c_lflag |= ECHO;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	return 0;
}

int instant_getchar(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	term.c_lflag &= ~ICANON;
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	return 0;
}

int normal_getchar(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	term.c_lflag |= ICANON;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#ifdef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error manipulating terminal: %s", strerror(errno));
		exit(EXIT_FAILURE);
#else
		return -1;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
	return 0;
}

#endif /* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */


/* -------------------- Threading -------------------- */
#ifdef ENABLE_THREADING

pthread_t launch_thread(void *(*start_routine)(void*), void *arg, const pthread_attr_t *attr)
{
	pthread_t th;
	if(pthread_create(&th, attr, start_routine, arg) != 0) {
		log_message(LOG_ERROR, "Error creating thread: %s", strerror(errno));
		th = 0;
	}
	return th;
}

#ifdef ENABLE_ERROR_HANDLING

pthread_t xlaunch_thread(void *(*start_routine)(void*), void *arg, const pthread_attr_t *attr)
{
	pthread_t th = launch_thread(start_routine, arg, attr);
	if(th == 0) {
		log_message(LOG_FATAL, "Thread not created: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return th;
}

void *xpthread_join(pthread_t thread)
{
	void *retval;

	if(pthread_join(thread, &retval) != 0) {
		log_message(LOG_FATAL, "Error joining threads: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return retval;
}
#endif /* #ifdef ENABLE_ERROR_HANDLING */

#endif /* #ifdef ENABLE_THREADING */

/* -------------------- Memory pool -------------------- */
#ifdef ENABLE_MEMPOOL

void new_mempool(struct mempool *mp, size_t size, size_t nmemb)
{
	unsigned i;
	void *val;

#ifdef INTERNAL_ERROR_HANDLING
	mp->mem = xmalloc((sizeof(unsigned) + size) * nmemb);
	mp->ptrs = xmalloc(sizeof(unsigned*) * nmemb);
#else
	mp->mem = malloc((sizeof(unsigned) + size) * nmemb);
	mp->ptrs = malloc(sizeof(unsigned*) * nmemb);
	if(unlikely(mp->mem == (void*) NULL || mp->ptrs == (void**) NULL)) {
		free(mp->mem);
		free(mp->ptrs);
		mp->mem = (void*) NULL;
		mp->ptrs = (void**) NULL;
		mp->size = 0;
		return;
	}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	mp->size = size;
	mp->nmemb = nmemb;
	mp->index = 0;

	val = mp->mem;
	for(i = 0; i < nmemb; i++) {
		mp->ptrs[i] = val;
		val = (void*) ((byte*) val + sizeof(unsigned) + size);
	}
}

void *mempool_alloc(struct mempool *mp)
{
	void *ptr = (void*) NULL;
	/*
	 * Problem when enlarging memory pool: mp->mem will point to a different chunk of mem,
	 * but the pointers pointing to the memory in the mempool will not be updated. Not much
	 * we can do about that => let's disable pool enlarging
	 */
	/*
	 * unsigned i;
	 * void *val;
	 *
	 * if(mp->index == mp->nmemb) {
	 *	mp->mem = xrealloc(mp->mem, (sizeof(unsigned) + mp->size) * (mp->nmemb <<= 1));
	 *	mp->ptrs = xrealloc(mp->ptrs, sizeof(unsigned*) * mp->nmemb);
	 *
	 *	val = mp->mem;
	 *	for(i = 0; i < mp->nmemb; i++) {
	 *		mp->ptrs[i] = val;
	 *		val += sizeof(unsigned) + mp->size;
	 *	}
	 * }
	 */
	if(mp->index < mp->nmemb) {
		*(unsigned*) mp->ptrs[mp->index] = mp->index;
		ptr = (void*) ((intptr_t) mp->ptrs[mp->index++] + sizeof(unsigned));
	}
	return ptr;
}

void mempool_free(struct mempool *mp, void *ptr)
{
	void *swap;
	unsigned idx = *(unsigned*) ((intptr_t) ptr - sizeof(unsigned));

	swap = mp->ptrs[idx];
	mp->ptrs[idx] = mp->ptrs[--mp->index];
	mp->ptrs[mp->index] = swap;
}

void delete_mempool(struct mempool *mp)
{
	free(mp->mem);
	free(mp->ptrs);
}
#endif /* #ifdef ENABLE_MEMPOOL */



/* -------------------- Config file -------------------- */

#if 0 /* TODO */
#ifdef ENABLE_CONFIG_FILE
#define CFG_FILE_START_SIZE 8
Config_File create_config_file(const char *path)
{
	Config_File cfg_file;
	cfg_file.size = 0;
#ifdef INTERNAL_ERROR_HANDLING
	cfg_file.file = xfopen(path, "w");
	cfg_file. = (Cfg_Var*) xmalloc(CFG_FILE_START_SIZE * sizeof(Cfg_Var));
#else
	cfg_file.file = fopen(path, "w");
	if(cfg_file.file == (FILE*) NULL)
		cfg_file. = (Cfg_Var*) malloc(CFG_FILE_START_SIZE * sizeof(Cfg_Var));
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	return cfg_file;
}
#endif /* #ifdef ENABLE_CONFIG_FILE */
#endif


/* -------------------- High-level mmap() -------------------- */
#if defined(ENABLE_MMAP) && defined(_POSIX_MAPPED_FILES)

Mmap *mopen(const char *path, const char *mode)
{
	int prot = PROT_NONE, i, o_flags = 0;
	int exec_flag = 0, flags = MAP_PRIVATE;
	off_t offset;
#ifdef INTERNAL_ERROR_HANDLING
	Mmap *f = (Mmap*) xmalloc(sizeof(Mmap));
#else
	Mmap *f = (Mmap*) malloc(sizeof(Mmap));
	if(unlikely(f == (Mmap*) NULL))
		return (Mmap*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */

	for(i = 0; mode[i] != '\0'; i++)
		switch(mode[i]) {
			case 'r':
				prot |= PROT_READ;
#if defined(__linux__) && defined(_GNU_SOURCE)
				if(prot & PROT_WRITE)
					o_flags = O_RDWR | O_CREAT | O_CLOEXEC;
				else
					o_flags = O_RDONLY | O_CLOEXEC;
				break;
			case 'w':
				prot |= PROT_WRITE;
				if(prot & PROT_READ)
					o_flags = O_RDWR | O_CREAT | O_CLOEXEC;
				else
					o_flags = O_WRONLY | O_CREAT | O_CLOEXEC;
				break;
#else
				if(prot & PROT_WRITE)
					o_flags = O_RDWR | O_CREAT;
				else
					o_flags = O_RDONLY;
				break;
			case 'w':
				prot |= PROT_WRITE;
				if(prot & PROT_READ)
					o_flags = O_RDWR | O_CREAT;
				else
					o_flags = O_WRONLY | O_CREAT;
				break;
#endif /* #if defined(__linux__) && defined(_GNU_SOURCE) */
			case 'x':
				exec_flag |= PROT_EXEC;
				break;
			case 'p':
				flags = MAP_PRIVATE;
				break;
			case 's':
				flags = MAP_SHARED;
				break;
			default:
#ifdef INTERNAL_ERROR_HANDLING
				log_message(LOG_FATAL, "Error: invalid mode");
				exit(EXIT_FAILURE);
#else
				log_message(LOG_ERROR, "Error: invalid mode");
				free(f);
				return (Mmap*) NULL;
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		}
#ifdef INTERNAL_ERROR_HANDLING
	i = xopen(path, o_flags);
#if (! defined(__linux__)) || (! defined(_GNU_SOURCE))
	if(unlikely(fcntl(i, F_SETFD, FD_CLOEXEC) != 0)) {
		log_message(LOG_FATAL, "Failed setting FD_CLOEXEC flag on file descriptor: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
#endif /* #if (! defined(__linux__)) || (! defined(_GNU_SOURCE)) */
	if(unlikely((offset = lseek(i, 0, SEEK_END)) == -1)) {
		perror("Error obtaining file size");
		exit(EXIT_FAILURE);
	}
	if(unlikely(lseek(i, 0, SEEK_SET) == -1)) {
		perror("Error obtaining file size");
		exit(EXIT_FAILURE);
	}
	if(unlikely((f->ptr = (byte*) mmap(NULL, offset,
						prot, flags, i, (off_t) 0)) == MAP_FAILED)) {
		perror("Error loading file into memory");
		exit(EXIT_FAILURE);
	}
#else
	i = open(path, o_flags);
	if(unlikely(i == -1)) {
		free(f);
		return (Mmap*) NULL;
	}
#if (! defined(__linux__)) || (! defined(_GNU_SOURCE))
	if(unlikely(fcntl(i, F_SETFD, FD_CLOEXEC) != 0)) {
		close(i);
		free(f);
		return (Mmap*) NULL;
	}
#endif /* #if (! defined(__linux__)) || (! defined(_GNU_SOURCE)) */
	if(unlikely((offset = lseek(i, 0, SEEK_END)) == -1)) {
		close(i);
		free(f);
		return (Mmap*) NULL;
	}
	if(unlikely(lseek(i, 0, SEEK_SET) == -1)) {
		close(i);
		free(f);
		return (Mmap*) NULL;
	}
	if(unlikely((f->ptr = (byte*) mmap(NULL, offset,
						prot | exec_flag, flags, i, (off_t) 0)) == MAP_FAILED)) {
		close(i);
		free(f);
		return (Mmap*) NULL;
	}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	f->endptr = f->offset = f->ptr;
	f->endptr += offset;
	close(i);

	return f;
}

size_t mread(void *ptr, size_t size, size_t nmemb, Mmap *f)
{
	size_t rsize = size * nmemb;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else {
		if(f->offset + size * nmemb >= f->endptr)
			rsize = f->endptr - f->offset;
		/* use of memmove instead of memcpy is intentional.
		 * We can now 'shift' data in a file */
		memmove(ptr, f->offset, rsize);
		f->offset += rsize;
	}

	return rsize;
}

/* TODO: when insufficient memory is available, allocate more memory */
size_t mwrite(void *ptr, size_t size, size_t nmemb, Mmap *f)
{
	size_t wsize = size * nmemb;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else {
		if(f->offset + size * nmemb >= f->endptr)
			wsize = f->endptr - f->offset;
		memmove(f->offset, ptr, wsize);
		f->offset += wsize;
	}

	return wsize;
}

int mgetc(Mmap *f)
{
	int ret = EOF;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else
		ret = f->offset < f->endptr ? (int) *f->offset++ : EOF;
	return ret;
}

/* TODO: implement */
int mputc(int c, Mmap *f)
{
	(void) c;
	(void) f;
	failwith("function not yet implemented");
	return EOF;
}

char *mgets(char *s, int size, Mmap *f)
{
	int i = 0;
	char *ret = (char*) NULL;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else if(f->offset < f->endptr) {
		while(i < size && f->offset < f->endptr) {
			s[i++] = *f->offset++;
			if(s[i-1] == EOF || s[i-1] == '\n')
				break;
		}
		s[i] = '\0';
		ret = s;
	}
	return ret;
}

int mungetc(int c, Mmap *f)
{
	int ret = EOF;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else if(f->offset >= f->ptr) {
		*f->offset = c;
		if(likely(f->offset != f->ptr))
			f->offset--;
		ret = c;
	} else
		errno = 1;
	return ret;
}

/* TODO: determine by how much to increment f->offset
 * Related TODO: "append" &nchars to end of va_list passed to vsscanf */
#if 0
int mscanf(Mmap *f, const char *fmt, ...)
{
	va_list ap;
	int ret, nchars;
	char *new_fmt = (char*) NULL;
	size_t len = strlen(fmt);

#ifdef INTERNAL_ERROR_HANDLING
	new_fmt = (char*) xmalloc(len + 3);
#else
	new_fmt = (char*) malloc(len + 3);
	if(new_fmt != NULL) {
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
		memcpy(new_fmt, fmt, len);
		memcpy(&new_fmt[len], "%n\000", 3);
		va_start(ap, fmt);
		ret = vsscanf(f->offset, fmt, ap, &nchars);
		va_end(ap);
		free(new_fmt);
		f->offset += nchars;
#ifndef INTERNAL_ERROR_HANDLING
	}
#endif /* #ifndef INTERNAL_ERROR_HANDLING */

	return ret;
}
#endif /* #if 0 */

/* TODO: prevent buffer overflow in case of insufficient memory */
int mprintf(Mmap *f, const char *fmt, ...)
{
	va_list ap;
	int ret = -1;

	failwith("this function can cause buffer overflows in its current implementation");
	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else {
		va_start(ap, fmt);
		ret = vsprintf((char*) f->offset, fmt, ap);
		f->offset += ret;
		va_end(ap);
	}

	return ret;
}

/* TODO: remove limitation on size */
int mnprintf(Mmap *f, size_t size, const char *fmt, ...)
{
	va_list ap;
	int ret = -1;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else {
		size = int_max(size, f->ptr - f->offset);
		va_start(ap, fmt);
		ret = vsnprintf((char*) f->offset, size, fmt, ap);
		f->offset += ret;
		va_end(ap);
	}

	return ret;
}

off_t mseek(Mmap *f, off_t offset, int whence)
{
	off_t ret = (off_t) -1;

	if(unlikely(f == (Mmap*) NULL || f->ptr == (byte*) NULL ||
				f->offset == (byte*) NULL || f->endptr == (byte*) NULL))
		errno = EBADF;
	else
		switch(whence) {
			case SEEK_SET:
				ret = (off_t) (f->offset = f->ptr + offset);
				break;
			case SEEK_CUR:
				ret = (off_t) (f->offset += offset);
				break;
			case SEEK_END:
				ret = (off_t) (f->offset = f->endptr + offset);
				break;
#if defined(_GNU_SOURCE) && (( defined(__linux__) && LINUX_VERSION_CODE >=\
			KERNEL_VERSION(3, 1, 0) ) || defined(__DragonFly__) ||\
		defined(__FreeBSD__) || (defined(__sun) && defined(__SVR4)))
				/* man lseek(3) for compatibility. Current compatibility (2016/01/11) includes
				 * GNU-Linux for kernels 3.1.0+, DragonFlyBSD, FreeBSD and Solaris */
			case SEEK_DATA:
				if(f->offset >= f->endptr || f->offset + offset >= f->endptr)
					errno = ENXIO;
				else
					ret = (off_t) (f->offset += offset);
				break;
			case SEEK_HOLE:
				if(f->offset >= f->endptr || f->endptr + offset > f->endptr)
					errno = ENXIO;
				else
					ret = (off_t) (f->offset = f->endptr + offset);
				break;
#endif /* #if defined(_GNU_SOURCE) && ((defined(__linux__) && LINUX_VERSION_CODE >=\
	  KERNEL_VERSION(3, 1, 0)) || defined(__DragonFly__) ||\
	  defined(__FreeBSD__) || (defined(__sun) && defined(__SVR4))) */
			default:
				errno = EINVAL;
		}
	return ret;
}

int mclose(Mmap *f)
{
	int ret = 0;

	if(unlikely(f == NULL || f->ptr == NULL
				|| f->offset == NULL || f->endptr == NULL)) {
		errno = EBADF;
		ret = EOF;
	}
	munmap(f->ptr, (size_t) (f->endptr - f->ptr));
	free(f);

	return ret;
}

#endif /* #if defined(ENABLE_MMAP) && defined(_POSIX_MAPPED_FILES) */

/* -------------------- Misc functions -------------------- */
#ifdef ENABLE_MISC

Weekday get_day_of_week(int day, int month, int year)
{
	int k, j;
	Weekday d;

	if(month == 1) {
		month = 13;
		year--;
	}
	if(month == 2) {
		month = 14;
		year--;
	}
	k = year % 100;
	j = year / 100;
	d = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

	return d <= 1 ? d + 5 : d - 2;
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

unsigned hexatoi(const char *hex)
{
	unsigned res = 0;

	if(*hex == '0' && *(hex + 1) == 'x')
		hex += 2;

	for(; *hex != '\0'; hex++)
		if('0' <= *hex && *hex <= '9')
			res = (res << 4) + *hex - '0';
		else if('A' <= *hex && *hex <= 'Z')
			res = (res << 4) + *hex - 'A' + 10;
		else if('a' <= *hex && *hex <= 'z')
			res = (res << 4) + *hex - 'a' + 10;
		else
			break;
	return res;
}

unsigned int gcd(unsigned int u, unsigned int v)
{
	int shift;

	if(u == 0)
		return v;
	if(v == 0)
		return u;
	/* Let shift := lg K, where K is the greatest power of 2
	   dividing both u and v. */
	for(shift = 0; ((u | v) & 1) == 0; shift++) {
		u >>= 1;
		v >>= 1;
	}

	/* while u is even */
	while((u & 1) == 0)
		u >>= 1;
	do {
		while ((v & 1) == 0)
			v >>= 1;
		/* Now u and v are both odd. Swap if necessary so u <= v,
		   then set v = v - u (which is even). */
		if (u > v)
			swap(u, v, unsigned int);
		v -= u;
	} while(v != 0);

	/* restore common factors of 2 */
	return u << shift;
}

int32_t int_max(int32_t a, int32_t b)
{
	register int32_t c = a - b;
	register int32_t k = (c >> 31) & 0x1;
	return a - k * c;
}

int32_t int_min(int32_t a, int32_t b)
{
	register int32_t c = b - a;
	register int32_t k = (c >> 31) & 0x1;
	return a + k * c;
}

void *initialize_vector(void *dest, const void *src, size_t size, size_t nmemb)
{
	size_t i;

	memcpy(dest, src, size);
	for(i = 1; i << 1 <= nmemb; i <<= 1)
		memcpy((byte*)dest + i * size, dest, i * size);
	memcpy((byte*)dest + i * size, dest, (nmemb - i) * size);
	return dest;
}

size_t human_readable(const char *str)
{
	size_t size = 0;
	size_t len = strlen(str);
	size_t mult = 1024;
	char *endptr;

	size = (size_t) strtoll(str, &endptr, 0);
	if(size == 0 || (*endptr != '\0' && endptr != &str[len - 1])) {
		fprintf(stderr, "Invalid size '%s'\n", str);
		exit(EXIT_FAILURE);
	}
	switch(str[len - 1]) {
		case 'T':
			mult <<= 10;
			__attribute__ ((fallthrough));
		case 'G':
			mult <<= 10;
			__attribute__ ((fallthrough));
		case 'M':
			mult <<= 10;
			__attribute__ ((fallthrough));
		case 'K':
		case 'k':
			break;
		case 'B':
			mult = 1;
			break;
		default:
			if(str[len - 1] < '0' || str[len - 1] > '9') {
				fprintf(stderr, "Unknown suffix %c\n", str[len - 1]);
				exit(EXIT_FAILURE);
			}
	}
	size *= mult;

	return size;
}

/* TODO: add possibility to modify sa_flags and sa_mask. See sigaction(2) for more info */
void register_signal_handler(int signum, void (*sighandler)(int))
{
	struct sigaction new_sigaction;

	memset(&new_sigaction, 0, sizeof(struct sigaction));
	new_sigaction.sa_handler = sighandler;
	if(sigaction(signum, &new_sigaction, (struct sigaction*) NULL) != 0) {
#ifndef INTERNAL_ERROR_HANDLING
		log_message(LOG_FATAL, "Error registering signal handler: %s", strerror(errno));
		exit(EXIT_FAILURE);
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
	}
}

/* From
http://www.emoticode.net/c/an-example-log-function-using-different-log-levels-and-variadic-macros.html */
static log_level_t __g_loglevel = LOG_DEBUG;
static FILE *__g_loghandle = (FILE*) NULL;

void init_log(FILE *stream, log_level_t loglevel)
{
	__g_loglevel = loglevel;
	__g_loghandle = stream;
}

void va_log_message(log_level_t level, const char *fmt, va_list ap)
{
	char buffer[255] = { 0 }, timestamp[255] = { 0 }, *slevel;
	time_t rawtime;
	struct tm *timeinfo;
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)
	Color color = WHITE, bgcolor = BLACK;
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */
	if(unlikely(__g_loghandle == (FILE*) NULL)) {
		__g_loghandle = stderr;
		fprintf(stderr, "\x1B[%d;0mWARNING: init_log() was not called\n", YELLOW + 30);
		reset_style(stderr);
	}

	if(level >= __g_loglevel) {
		vsnprintf(buffer, 255, fmt, ap);
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timestamp, 255, "%d/%m/%Y %X", timeinfo);

		switch(level) {
			case LOG_DEBUG:
				slevel = "DEBUG";
				break;
			case LOG_INFO:
				slevel = "INFO";
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)
				bgcolor = GREEN;
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */
				break;
			case LOG_WARNING:
				slevel = "WARNING";
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)
				bgcolor = BLUE;
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */
				break;
			case LOG_ERROR:
				slevel = "ERROR";
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)
				bgcolor = YELLOW;
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */
				break;
			case LOG_FATAL:
			default:
				slevel = "FATAL";
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)
				bgcolor = RED;
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */
				break;
		}
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)
		if(__g_loghandle == stdout || __g_loghandle == stderr) {
			fprintf(__g_loghandle, "\x1B[%d;0m", color + 30);
			fprintf(__g_loghandle, "\x1B[%dm[%s] [%s] %s", bgcolor + 40, timestamp, slevel, buffer);
			reset_style(__g_loghandle);
			putc('\n', __g_loghandle);
		} else
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */
			fprintf(__g_loghandle, "[%s] [%s] %s\n", timestamp, slevel, buffer);
	}
}

void log_message(log_level_t level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	va_log_message(level, fmt, ap);
	va_end(ap);
}

void failwith(const char *errmsg, ...)
{
	va_list ap;

	va_start(ap, errmsg);
	va_log_message(LOG_FATAL, errmsg, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

#endif /* #ifdef ENABLE_MISC */

