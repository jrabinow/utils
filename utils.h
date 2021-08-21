/* utils.h
 * Copyright (C) Julien Rabinow <jnr305@nyu.edu>
 *
 * utils.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * utils.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H
#define UTILS_H

/*
 * READ THROUGH THIS FILE TO FIND OUT PRECISELY WHAT IS ENABLED BY THE
 * FOLLOWING #defineS
 */




/* define bool variable type. I recommend taking Torvalds advice and not using
 * this feature:
 * https://lkml.org/lkml/2013/8/31/138 */
/* #define ENABLE_BOOL_TYPE */

/* Enables memory management such as heap memory identification. Planned: leak
 * detection and related. Very primitive at this time, I recommend not using this until
 * properly implemented */
/* #define MANAGE_MEM */

/* define error "squashing" functions. Program exits if error happens. Use sparingly
 * if your application must meet certain robustness requirements */
#define ENABLE_ERROR_HANDLING

/* Various string manipulation functions */
#define ENABLE_STRING_MANIPULATION

/* Easily read data from streams/file descriptors */
#define ENABLE_READ_DATA

/* Data structures: double linked list, stack, queue, Bitset */
#define ENABLE_DATASTRUCTS

/* Directory navigation functions */
#define ENABLE_FILESYSTEM

/* Networking related functions */
#define ENABLE_NETWORKING

/* Terminal manipulation functions, as well as color text printing
 * Will only work with compatible terminals and terminal emulators */
#define ENABLE_TERMIOS_MANIPULATION

/* Threading functions. Still very basic */
#define ENABLE_THREADING

/* Memory pool, to cache heap memory and avoid having to call malloc. Idea is that
 * we often ask for same memory sizes (nodes in a linked list or tree, data structures
 * each requiring the same amount of memory...). This memory pool reduces overhead caused
 * by memory allocation as well as enormously improving memory locality */
#define ENABLE_MEMPOOL

/* High-level mmap. Still experimental */
#define ENABLE_MMAP

/* miscellaneous functions */
#define ENABLE_MISC

/* calling init_alloc() as enabled by MANAGE_MEM causes of a 1-byte allocation.
 * Although the overhead associated with freeing that single byte isn't worth the effort,
 * the allocation will appear in valgrind as a memory leak unless you enable
 * USING_VALGRIND */
#ifdef MANAGE_MEM
# define USING_VALGRIND
#endif /* ifdef MANAGE_MEM */

/* whether to quit when encountering an error, or report it back to the user */
#define INTERNAL_ERROR_HANDLING

/* Library configuration done. You shouldn't need to modify anything below this
 * line.
 * -------------------------------------------------------------------------- */

#if defined(__STDC__)
# define C89
# if defined(__STDC_VERSION__)
#  define C90
#  if (__STDC_VERSION__ >= 199901L) && (! defined C99)
#   define C99
#  endif /* #  if (__STDC_VERSION__ >= 199901L) && (! defined C99) */
# endif /* # if defined(__STDC_VERSION__) */
#endif /* #if defined(__STDC__) */

#ifdef __GNUC__
# define likely(x)	__builtin_expect(!!(x), 1)
# define unlikely(x)	__builtin_expect(!!(x), 0)
#else
# define likely(x)	(x)
# define unlikely(x)	(x)
#endif /* #ifdef __GNUC__ */

/* Operating system detection macros:
 * http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system */

/* Generic macro for detecting Unix systems */
#if (defined(__unix) || defined(__MACH__)) && (! defined(__unix__))
# define __unix__
#endif /* #if (defined(__unix) || defined(__MACH__)) && (! defined(__unix__)) */


/* Generic macro for detecting BSD-type operating systems */
#if (defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__)\
		|| defined(__OpenBSD__) || defined(__bsdi__)) && (! defined(BSD))
# define BSD
#endif /* #if (defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) ||\
	  defined(__OpenBSD__) || defined(__bsdi__)) && (! defined(BSD)) */

#ifdef __unix__
# define _GNU_SOURCE
#endif /* #ifdef __unix__ */

/* fix KERNEL_VERSION() macro preprocessing errors */
#ifndef __linux__
# define KERNEL_VERSION(x, y, z)	(1 << 30)
#endif /* #ifndef __linux__ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#include <stdint.h>
#ifdef _STDINT_H
 typedef uint8_t byte;
#else
 typedef unsigned char byte;
#endif /* #ifdef _STDINT_H */

#ifdef ENABLE_BOOL_TYPE
# define BOOL_FALSE false
# define BOOL_TRUE true
# define BOOL_TYPE bool
# ifdef C99
#  include <stdbool.h>
# else
   typedef enum { false = 0, true = 1 } bool;
# endif /* ifdef C99 */
#else
# define BOOL_FALSE 0
# define BOOL_TRUE 1
# define BOOL_TYPE int
#endif /* #ifdef ENABLE_BOOL_TYPE */

#if defined(MANAGE_MEM) || defined(ENABLE_ERROR_HANDLING)
# define MAX_RETRIES_ALLOC	3
#endif /* #if defined(MANAGE_MEM) || defined(ENABLE_ERROR_HANDLING) */

/* -------------------- MEMORY MANAGEMENT -------------------- */
#ifdef MANAGE_MEM

#include <unistd.h>

/* To use the following functions, you must call init_alloc from your program before allocating any memory
 * WARNING: DO NOT MIX calls to malloc() with calls to xmalloc(). The functions will only work correctly
 * for memory allocated through xmalloc.
 * Finally, remember to delete memory structs allocated by other functions (getaddrinfo for example) */

/* TODO: get rid of all these limitations by exporting a hook to malloc */
void init_alloc(void) __attribute__ ((constructor));

/* returns true if ptr points to dynamically allocated memory, false otherwise. You can now do things like
 * if(is_allocated(myptr))
 *	free(myptr);
 */
BOOL_TYPE is_allocated(const void *ptr) __attribute__ ((pure));

/* if you're too lazy to do like in the previous example, this function does it for you */
void xfree(void *ptr);
#endif /* #ifdef MANAGE_MEM */



/* -------------------- ERROR HANDLING -------------------- */
#ifdef ENABLE_ERROR_HANDLING

#ifdef __unix__
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
#endif /* #ifdef __unix__ */

#ifdef __linux__
# include <linux/version.h>
#endif /* #ifdef __linux__ */

/* exit program with failed status if malloc and consorts fail
 * -> no more error checking necessary
 * Calls exit() at failure
 * free() when done */
void *xmalloc(size_t size) __attribute__ ((malloc));
void *xcalloc(size_t nmemb, size_t size) __attribute__ ((malloc));
char *xstrdup(const char *str);
void *xrealloc(void *ptr, size_t size);

#define MAX_RETRIES_OPEN	3
/* attempt to open the file with the corresponding mode. Calls exit() at failure */
FILE *xfopen(const char *file, const char *mode) __attribute__ ((nonnull));
/* attempt to create a stream opened in mode mode from file descriptor fd.
 * Calls exit() at failure */
FILE *xfdopen(int fd, const char *mode) __attribute__ ((nonnull));

#ifdef __unix__

/* a horrid kludge trying to make sure that this will fail on old kernels */
#ifndef O_TMPFILE
# define O_TMPFILE (__O_TMPFILE | O_DIRECTORY)
#endif /* #ifndef O_TMPFILE */

int xopen(const char *path, int flags, ...) __attribute__ ((nonnull));
/* attempts to create a pipe. Calls exit() at failure */
#define xpipe(pipefd)\
	if(pipe(pipefd) != 0) {\
		perror("Error creating pipe ");\
		exit(EXIT_FAILURE);\
	}

/* attempts to duplicate file descriptor oldfd to newfd. Calls exit() at failure */
#define xdup2(oldfd, newfd)\
	if(dup2(oldfd, newfd) == -1) {\
		perror("Error redirecting output ");\
		exit(EXIT_FAILURE);\
	}
#endif /* #ifdef __unix__ */

#endif /* #ifdef ENABLE_ERROR_HANDLING */



/* -------------------- STRING MANIPULATION -------------------- */
#ifdef ENABLE_STRING_MANIPULATION

#include <ctype.h>
/* returns true if isdigit() returns true for all chars in str, false otherwise */
BOOL_TYPE is_valid_int(const char *str) __attribute__ ((nonnull))
					__attribute__ ((pure));

/* Same idea as is_valid_int, except checks for float (allows for a single '.' in str) */
BOOL_TYPE is_valid_float(const char *str) __attribute__ ((nonnull))
					  __attribute__ ((pure));

/* Same idea as is_valid_int, except checks for hexadecimals [0-9a-fA-F]* */
BOOL_TYPE is_valid_hex(const char *str) __attribute__ ((nonnull))
					__attribute__ ((pure));

/* returns true if str starts with prefix */
BOOL_TYPE startswith(const char *str, const char *prefix) __attribute__ ((nonnull))
							  __attribute__ ((pure));

/* returns true if str ends with suffix */
BOOL_TYPE endswith(const char *str, const char *suffix) __attribute__ ((nonnull))
							__attribute__ ((pure));

/* sets str to lower case */
void str_tolower(char *str)  __attribute__ ((nonnull));

/* sets str to upper case */
void str_toupper(char *str)  __attribute__ ((nonnull));

#if (! defined(__linux__)) && (! defined(BSD)) && (! defined(__MACH__))
char *stpcpy(char *dest, const char *src) __attribute__ ((nonnull))
					  __attribute__((pure));
#endif /* #if (! defined(__linux__)) && (! defined(BSD)) && (! defined(__MACH__)) */

#ifndef __linux__
char *strchrnul(const char *s, int c) __attribute__ ((nonnull))
				      __attribute__ ((pure));
#endif /* #ifndef __linux__ */

#ifdef C99
#include <stdarg.h>
/* __VA_ARGS__ is not available for macros before the C99 standard
 * TODO: figure out a way to port these functions to C89
 * http://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
 */

/* Appends together all string args passed as parameters.
 * free() when done */
char *const_append(const char *str, ...) __attribute__ ((nonnull (1)));

#define const_append(...)		const_append(__VA_ARGS__, (char*) NULL)

/* Following workaround for C89 won't work:
 * foo = const_append("bar", "baz");
 * expands to
 * foo = #pragma("WARNING ...) const_append
 */
/* #define const_append			#pragma("WARNING: make sure const_append's last argument is (char*) NULL")\
					const_append*/

/* Appends together all strings passed as parameters. str (first arg) MUST be dynamically
 * allocated and will be modified by calling this function. It might also be moved to
 * a different memory location.
 * free() when done */
char *append(char *str, ...) __attribute__ ((nonnull (1)));

#define append(...)	append(__VA_ARGS__, (char*) NULL)
/* See const_append for why the following won't work */
/* #define append			#pragma("WARNING: make sure append's last argument is (char*) NULL")\
				append*/
#endif /* ifdef C99 */

/* extracts portion of str contained between 1st occurence of start and first occurence (following start) of end.
 * If str does not contain start or end char, or there is no end character after start,
 * returns NULL.
 * Example:
 * extract("foo(bar)", '(', ')') will return a pointer to the string "bar"
 * free after usage */
char *extract(const char *str, char start, char end) __attribute__ ((nonnull));

/* copy str while ignoring leading and trailing whitespace
 * free after usage */
char *trim(const char *str) __attribute__ ((nonnull));

/* return a copy of str with c inserted at position pos
 * free after usage */
char *insert(const char *str, char c, size_t pos) __attribute__ ((nonnull));

/* return a copy of str with ins inserted at position pos
 * free after usage */
char *insert_str(const char *str, const char *ins, size_t pos) __attribute__ ((nonnull));

/* return a copy of str with character at position pos deleted
 * free after usage */
char *erase(const char *str, size_t pos) __attribute__ ((nonnull));

/* return a copy of str with len characters at position pos deleted
 * free after usage */
char *erase_str(const char *str, size_t pos, size_t len) __attribute__ ((nonnull));

/* return a copy of haystack with first occurence of needle replaced by replacement
 * free after usage */
char *replace_str(const char *haystack, const char *needle, const char *replacement) __attribute__ ((nonnull));

/* locates last occurence in str of any of the bytes in accept */
const char *rev_strcspn(const char *str, const char *accept) __attribute__ ((nonnull))
							     __attribute__ ((pure));

/* return pointer to first occurence of char in s not equal to c. If str is made up entirely
 * of c characters, returns NULL */
char *neg_strchr(const char *s, int c) __attribute__ ((nonnull))
				       __attribute__ ((pure));

/* Count occurences of character c in string s */
unsigned count_characters(const char *s, char c) __attribute__ ((nonnull))
						 __attribute__ ((pure));

/* Splits str along separator chars into non-empty tokens. If str is composed only
 * of separator chars, return_array will point to NULL.
 * Otherwise, return_array will point to dynamically allocated array with one string token
 * per array element.
 * return value is size of array.
 * free() *return_array, (*return_array)[0], (*return_array)[1] ... when done.
 * In case of error, returns 0, *return_array is NULL and errno is set appropriately */
size_t split_str(const char *str, char separator, char ***return_array) __attribute__ ((nonnull));

/* Same thing as split_str, except that elements in return_array are pointers to locations
 * in the original string. Use only if you have no more use for original data as it is
 * modified (each separator char is replaced by a '\0')
 * This function is more efficient as there is no data copying involved, and the only
 * allocated memory is *return_array
 * In case of error, returns 0, *return_array is NULL and errno is set appropriately */
size_t split_str_lite(char *str, char separator, char ***return_array) __attribute__ ((nonnull));

/* joins all strings in str_array. All strings are joined end to end with a separator
 * in between each string
 * returns the resulting dynamically allocated string. free() when done */
char *str_join(int str_array_size, char **str_array, char *separator) __attribute__ ((nonnull));

/* Cancel construction of string buffer without assembling final string */
/* void delete_stringbuffer(StringBuffer sb); */

/* returns true if str1 and str2 are 2 the same strings. Helps make code more readable */
#define str_equals(str1, str2)	(strcmp(str1, str2) == 0)

#endif /* #ifdef ENABLE_STRING_MANIPULATION */



/* -------------------- Reading data -------------------- */
#ifdef ENABLE_READ_DATA

/* reads a complete line (no length limit) from file.
 * free() buffer when done. As an alternative, look up getline(3) */
char *read_line(FILE *stream) __attribute__ ((nonnull));

/* for some reason, the solaris version I tested on didn't declare
 * flockfile(3), funlockfile(3) or getc_unlocked(3) function prototypes
 * even when #including <stdio.h> */
#if defined(__sun) && defined(__SVR4)
void flockfile(FILE *filehandle) __attribute__ ((nonnull));
void funlockfile(FILE *filehandle) __attribute__ ((nonnull));
int getc_unlocked(FILE *stream) __attribute__ ((nonnull));
#endif /* #if defined(__sun) && defined(__SVR4) */

/* Use fgets without a preallocated buffer. free() when done */
#define afgets(stream) getline(NULL, 0, (stream))

#ifdef __unix__
/* read as much as possible from file descriptor.
 * free() buffer when done */
#include <ctype.h>
#include <unistd.h>
char *read_fd_str(int fd);
char *read_file_str(const char *path);
byte *read_file_descriptor(int fd, ssize_t *n);
byte *read_file(const char *path, ssize_t *n);
#endif /* #ifdef __unix__ */

/* Empties buffer till nothing left to read or hits end of line. Useful with scanf/fscanf */
#define empty_buffer(stream)	{\
	int __c__;\
	while((__c__ = getc(stream)) != EOF && __c__ != '\n');\
}

#endif /* #ifdef ENABLE_READ_DATA */



/* -------------------- Memory pool -------------------- */
#ifdef ENABLE_MEMPOOL

struct mempool {
	void *mem, **ptrs;
	size_t size, nmemb, index;
};

/* create memory pool of nmemb elements, each of size size. If internal error
 * handling is disabled and this function fails, mp->size = 0 */
void new_mempool(struct mempool *mp, size_t size, size_t nmemb) __attribute__ ((nonnull));

/* obtain one element from the mempool
 * Returns pointer to valid element space on success and NULL on failure. See
 * implementation comment for details */
void *mempool_alloc(struct mempool *mp) __attribute__ ((nonnull));

/* free the memory pointed to by ptr back into the memory pool */
void mempool_free(struct mempool *mp, void *ptr) __attribute__ ((nonnull));

/* once done using the memory pool, delete it */
void delete_mempool(struct mempool *mp) __attribute__ ((nonnull));

#endif /* #ifdef ENABLE_MEMPOOL */



/* -------------------- DATA STRUCTURES -------------------- */
#ifdef ENABLE_DATASTRUCTS

/* ISO C forbids zero-size array ‘data’ */
#define __ARRAY_SIZEOF_DATA_ELEM	1
#ifdef C89
# define __SIZEOF_ARRAY_STRUCT	(offsetof(struct __array_data__, data))
#else
/* Must be updated manually when redefining struct __array_data__ */
# define __SIZEOF_ARRAY_STRUCT	(sizeof(size_t) << 1)
#endif

/* Must manually update __SIZEOF_ARRAY_STRUCT a few lines above when
 * redefining struct __array_data__ */
struct __array_data__ {
	size_t size, nmemb;
	byte data[__ARRAY_SIZEOF_DATA_ELEM];
};

typedef struct __datastruct_elem__ {
	void *data;
	struct __datastruct_elem__ *next;
} __datastruct_elem__;

typedef struct __datastruct__ {
	__datastruct_elem__ *in, *out;
} __datastruct__;

/* ----- Double linked list ----- */
typedef __datastruct__* DLinkedList;

DLinkedList new_dlinkedlist(void);
void delete_dlinkedlist(DLinkedList dl, void (*__del__)(void*)) __attribute__ ((nonnull (1)));

void *dll_add(DLinkedList dl, void *data) __attribute__ ((nonnull (1)));
void *dll_remove(DLinkedList dl) __attribute__ ((nonnull));

void dll_rewind(DLinkedList dl) __attribute__ ((nonnull));
void *dll_iterate(DLinkedList dl) __attribute__ ((nonnull));
void *dll_rev_iterate(DLinkedList dl) __attribute__ ((nonnull));
void *dll_head(DLinkedList dl) __attribute__ ((nonnull));
DLinkedList dll_tail(DLinkedList dl) __attribute__ ((nonnull));
DLinkedList dll_copy_interator(DLinkedList dl) __attribute__ ((nonnull));
/*
DLinkedList dll_clone(DLinkedList dl, void *(*__clonefunc__)(void*));
DLinkedList dll_join(DLinkedList dl1, DLinkedList dl2);
DLinkedList dll_filter(DLinkedList dl, BOOL_TYPE (*__filterfunc__)(void*))
void dll_sort(DLinkedList dl, int (*__cmp__)(void *d1, void *d2));
*/
void *dll_map(DLinkedList dl, void *(*__mapfunc__)(void *data, void *arg), void *arg) __attribute__ ((nonnull (1)));


/* ----- Stack ----- */
typedef __datastruct_elem__* Stack;

#define new_stack()	(Stack) NULL
void delete_stack(Stack s, void (*__del__)(void*)) __attribute__ ((nonnull (1)));

void *stack_push(Stack *s, void *data);
void *stack_pop(Stack *s);
void *stack_peek(Stack s);

/* for a consistent interface */
#define stack_push(s, data)	stack_push(&(s), (data))
#define stack_pop(s)		stack_pop(&(s))

/* ----- Queue ----- */
typedef __datastruct__* Queue;

Queue new_queue(void);
void delete_queue(Queue q, void (*__del__)(void*)); __attribute__ ((nonnull (1)))

void queue_push(Queue q, void *data) __attribute__ ((nonnull (1)));
void *queue_pop(Queue q) __attribute__ ((nonnull));
void *queue_peek(Queue q) __attribute__ ((nonnull));

/* ----- Bitset ----- */
typedef struct {
	size_t size;
	byte data[1];
} __bitset_struct__;

typedef __bitset_struct__ *Bitset;

Bitset new_bitset(size_t size);

#define free_bitset	free

Bitset clone_bitset(const Bitset set);

/* obtain bit at position pos from set array */
int getbit(const Bitset set, int pos) __attribute__ ((pure));

/* set bit at position pos to 1 */
void setbit(Bitset set, int pos);

/* set bit at position pos to 0 */
void unsetbit(Bitset set, int pos);

/* flip bit at position pos */
int togglebit(Bitset set, int pos) __attribute__ ((pure));

#endif /* #ifdef ENABLE_DATASTRUCTS */



/* -------------------- Filesystem functions -------------------- */
#ifdef ENABLE_FILESYSTEM

#ifdef __unix__
#include <sys/stat.h>
#endif /* #ifdef __unix__ */

/* get modern readdir_r function prototype on solaris */
#if defined(__sun) && defined(__SVR4)
# define _POSIX_C_SOURCE 199506L
#endif /* #if defined(__sun) && defined(__SVR4) */

#include <dirent.h>
#include <stdarg.h>
#include <limits.h>

int is_dir(const char *path) __attribute__ ((nonnull));

#ifdef C99
/* creates a new dynamically allocated string of the form "arg1/arg2/.../argN"
 * or "arg1\arg2\...\argN" if on windows
 * free() when done */
char *make_path(const char *path, ...) __attribute__ ((nonnull (1)));

#define make_path(...)	make_path(__VA_ARGS__, (char*) NULL)

/* TODO: figure out a way to port this function to C89
 * http://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
 */
/*#define make_path		#pragma("WARNING: make sure make_path's last argument is (char*) NULL")\
				make_path */
#endif /* ifdef C99 */

/* iterates over and calls func() on every file in filesystem under path. arg is
 * an argument passed as 2nd argument to func.
 * Returns arg after iterating through every file under path
 * In case of an error, dirwalk will return NULL and errno will be set appropriately.
 * Please note that if func() returns NULL at every call, you will have to check
 * errno to determine if an error occured */
void *dirwalk(const char *path, BOOL_TYPE recurse, void *(*func)(char *path, void *arg), void *arg) __attribute__ ((nonnull (1,3)));

#endif /* #ifdef ENABLE_FILESYSTEM */



/* -------------------- Networking functions -------------------- */
#ifdef ENABLE_NETWORKING

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define CONNECT_ERROR		-1

/* connect to server server_name running on port port. server_name can be a
 * hostname or an IP address. Returns a file descriptor through which
 * communication with the server is possible, or CONNECT_ERROR in case
 * of error */
int connect_to(const char *server_name, unsigned port) __attribute__ ((nonnull(1)));

/* create a server listening on port port. Returns a file descriptor to a socket
 * ready to accept() clients, or CONNECT_ERROR in case of error */
int create_server(unsigned port);

/* accept() a client and return a file descriptor through which program can
 * communicate with client, or CONNECT_ERROR in case of error */
int get_single_client(int server_socket);

#endif /* #ifdef ENABLE_NETWORKING */



/* -------------------- Terminal manipulation functions and color printing -------------------- */
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__)

#include <unistd.h>
#include <termios.h>

/* clear all content from terminal */
#define clear_screen()			printf("\x1B[2J\x1B[0;0H");

typedef enum { BLACK, RED, GREEN, YELLOW, BLUE, PINK, CYAN, WHITE } Color;

typedef enum { NORMAL, BOLD, DARK, ITALIC, UNDERLINED } Style;

/* set font color to c, background color to bgc and font style to s
 * WARNING: THIS FUNCTION IS NOT THREAD-SAFE !!! */
void set_style(Color c, Color bgc, Style s);

/* print a string fmt with font color c, background color bgc and style s
 * restores previous state of terminal after printing fmt
 * WARNING: THIS FUNCTION IS NOT THREAD-SAFE !!! */
void stylish_fprintf(FILE *stream, Color c, Color bgc, Style s, const char *fmt, ...) __attribute__ ((nonnull (1, 5)));

/* reset everything to default */
#define reset_style(stream)		fprintf(stream, "\x1B[0m")

/* Do not display keyboard input on terminal
 * If internal error management is disabled, returns 0 on success, -1 on
 * error and sets errno to appropriate value */
int turn_echoing_off(void);

/* Display keyboard input on terminal
 * If internal error management is disabled, returns 0 on success, -1 on
 * error and sets errno to appropriate value */
int turn_echoing_on(void);

/* program reads input without user having to press enter
 * If internal error management is disabled, returns 0 on success, -1 on
 * error and sets errno to appropriate value */
int instant_getchar(void);

/* Restore default behaviour
 * If internal error management is disabled, returns 0 on success, -1 on
 * error and sets errno to appropriate value */
int normal_getchar(void);

#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix__) */

/* -------------------- Threading -------------------- */
#ifdef ENABLE_THREADING

#include <pthread.h>

#define DETACH_THREAD		1
#define NO_DETACH_THREAD	0

/* launches start_routine in a new thread with arg as argument. detach is a flag
 * to tell if we should call pthread_detach on new thread or not (see
 * pthread_detach(3) for more details). Returns thread id variable */
pthread_t launch_thread(void *(*start_routine)(void*), void *arg, const pthread_attr_t *attr) __attribute__ ((nonnull (1)));

#ifdef ENABLE_ERROR_HANDLING
pthread_t xlaunch_thread(void *(*start_routine)(void*), void *arg, const pthread_attr_t *attr) __attribute__ ((nonnull (1)));

void *xpthread_join(pthread_t thread);
#endif /* #ifdef ENABLE_ERROR_HANDLING */

#endif /* #ifdef ENABLE_THREADING */



/* -------------------- High-level mmap() -------------------- */
#if defined(ENABLE_MMAP) && defined(__unix__)

#include <unistd.h>
#include <sys/mman.h>

typedef struct {
	byte *ptr, *offset, *endptr;
} Mmap;

/* Map file to memory space. Mode can contain any of the characters 'r', 'w' or 'x'
 * in any order, or none of "rwx" for PROT_NONE (see mmap(2) for details)
 * Mode can also contain 's' or 'p' for shared or private memory mapping. Only
 * the last 's' or 'p' in the string will have an effect. Address space is mapped
 * private by default */
Mmap *mopen(const char *path, const char *mode) __attribute__ ((nonnull));

size_t mread(void *ptr, size_t size, size_t nmemb, Mmap *f) __attribute__ ((nonnull (1)));
size_t mwrite(void *ptr, size_t size, size_t nmemb, Mmap *f) __attribute__ ((nonnull (1)));

int mgetc(Mmap *f);

char *mgets(char *s, int size, Mmap *f) __attribute__ ((nonnull (1)));
/* In its current implementation, this function can cause buffer overflows. Use mnprintf instead for the time being */
/*int mprintf(Mmap *f, const char *fmt, ...); */
int mnprintf(Mmap *f, size_t size, const char *fmt, ...) __attribute__ ((nonnull (3)));

/* Unmap and close resources */
int mclose(Mmap *f);

#endif /* #if defined(ENABLE_MMAP) && defined(__unix__) */



/* -------------------- Misc functions -------------------- */
#ifdef ENABLE_MISC

#include <stdarg.h>
#include <time.h>

typedef enum {
	Monday = 0,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
} Weekday;

/* return day of week. 0 for Monday, 1 for Tuesday ... 6 for Sunday. Uses
 * Zeller's congruence for the algorithm */
Weekday get_day_of_week(int day, int month, int year) __attribute__ ((pure));

/* returns pointer to buffer (must be preallocated and sufficiently large) containing base 10
 * string representation of val. */
char *itoa(int val, char *buffer) __attribute__ ((nonnull));

/* convert from hexadecimal format string to integer */
unsigned hexatoi(const char *hex) __attribute__ ((nonnull))
				  __attribute__ ((pure));

/* return greatest common divisor of u and v */
unsigned int gcd(unsigned int u, unsigned int v) __attribute__ ((pure));

int32_t int_max(int32_t a, int32_t b) __attribute__ ((pure));
int32_t int_min(int32_t a, int32_t b) __attribute__ ((pure));
/*#ifdef C99
#else
# define int_max(a, b)	((a) - ((((a) - (b)) >> 31) & 0x1) * ((a) - (b)))
# define int_min(a, b)	((a) + ((((b) - (a)) >> 31) & 0x1) * ((b) - (a)))
#endif *//* #ifdef C99 */

/* Efficiently fill dest with contents of src. src is a single element of size size. dest is a
 * memory buffer of size size * nmemb
 * Example:
 * long unsigned int dest[100], value = 42;
 * initialize_vector(dest, &value, sizeof value, 100); fills all 100 elements of dest with the
 * value 42 */
void *initialize_vector(void *dest, const void *src, size_t size, size_t nmemb) __attribute__ ((nonnull));

#ifdef __unix__
#include <signal.h>
/* register sighandler as the signal handler function to be called when program receives
 * signal signum. As usual, signals SIGKILL and SIGSTOP cannot be ignored or intercepted */
void register_signal_handler(int signum, void (*sighandler)(int));
#endif /* #ifdef __unix__ */

typedef enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL
} log_level_t;

/* Initialize log system on file stream. Logs by default on stderr. To completely
 * disable logging, call init_log with stream parameter set to NULL */
void init_log(FILE *stream, log_level_t loglevel) __attribute__ ((nonnull));
/* logs a message following printf conventions */
void log_message(log_level_t level, const char *format, ... ) __attribute__ ((nonnull (2)));

/* prints errmsg to stderr and calls exit(). Functions previously registered with atexit()
 * will be called */
void failwith(const char *errmsg, ...) __attribute__ ((nonnull (1)));

/* swap variables a and b. Both a and b must be of type TYPE */
#define swap(a, b, TYPE) {\
	TYPE __tmp__ = a;\
	a = b;\
	b = __tmp__;\
}

#else
# define log_message(loglvl, fmt, ...)	fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#endif /* #ifdef ENABLE_MISC */

#endif	/* #ifndef UTILS_H */
