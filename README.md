# utils
Various utility functions for C programming

C is rather low-level, and while this can be seen as one of its strengths, it can also result in lots of time spent on figuring out details rather than concentrating on the task at hand.
The goal of this "library" is to ensure that as little time as possible is spent reinventing the wheel.
The end goal is to provide functions built-in to other languages such as Java or Python, but which are not part of libc

This "library" provides functions for:
  - easy error handling
  - string manipulation
  - high-level interaction with FILE*s and file descriptors (read lines, empty buffer, etc)
  - bitset management
  - directory navigation and interaction with files
  - termios struct manipulation (echoing text onscreen, text coloration, getchar() properties, etc)
  - threading
  - memory pool management
  - configuration file management (coming soon)
  - logging
  - networking
  - and more!

The different functions are grouped into sections, which can each be "activated" or "deactivated" individually. This is done with a series of #define'd constants that one can comment out to deactivate the relevant section. This results in code that can be messy at times, but reducing the executable file size is greatly facilited when using static linking.

Please see utils.h for information concerning the available functions.

NB: This project has only been added to git since V5. I kept the old files as "souvenirs", but they are most likely of no interest to anyone but myself
