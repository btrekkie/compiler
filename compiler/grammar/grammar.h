#ifndef GRAMMAR_H_INCLUDED
#define GRAMMAR_H_INCLUDED

#include <stdio.h>

int yyparse();
/**
 * Set the file whose contents we will parse in the next call to yyparse().
 * @param file the file.
 * @return an opaque object indicating the resources this function call
 *     allocated.  After calling yyparse() (or if we choose not to call
 *     yyparse()), we should deallocate the object using "freeParseData".
 */
void* setFileToParse(FILE* file);
/**
 * Deallocates the object returned from a previous call to "setFileToParse".
 */
void freeParseData(void* data);

#endif
