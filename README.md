# utils
Various utility functions for C programming

C is rather low-level, and while this can be seen as one of its strengths, it can also result in lots of time spent on figuring out details rather than concentrating on the task at hand.
The goal of this "library" is to ensure that as little time as possible is spent reinventing the wheel.
The end goal is to provide functions built-in to other languages such as Java or Python, but which are not part of the GNU libc.

This "library" provides functions for:
  - easy error handling
  - string manipulation
  - high-level interaction with FILE*s and file descriptors (read lines, empty buffer, etc)
  - bitset management
  - directory navigation and interaction with files
  - high level mmap functions
  - termios struct manipulation (echoing text onscreen, text coloration, getchar() properties, etc)
  - threading
  - memory pool management
  - logging
  - basic networking
  - and more!

The different functions are grouped into sections, which can each be "activated" or "deactivated" individually. This is done with a series of #define'd constants that one can comment out to deactivate the relevant section. This results in code that can be messy at times, but reducing the executable file size is greatly facilited this way.

Portability to systems other than GNU/Linux or using an alternative to glibc will be attempted. However, portability is NOT a priority. Currently, no warning will be issued when compiling the source file with both -Wpedantic and -ansi flags (gcc 5.3.1, clang 3.5.2). If you run into problems anywhere, please let me know. Pull requests are also welcome.

Please see utils.h for information concerning the available functions.
