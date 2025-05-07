#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>

// Opens file while making sure it has the specified extension.
// If the file doesn't exist, it will be created.
// Exits with EXIT_FAILURE if the file can't be opened, doesn't exist, or has the wrong extension.
// If the file is opened successfully, it returns a pointer to the file stream.
FILE *open_file(const char *filename, const char *mode, const char *extension);

#endif