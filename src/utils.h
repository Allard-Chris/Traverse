#ifndef _HEADER_UTILS_H
#define _HEADER_UTILS_H

/* imports libs */
#include <errno.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* type */
typedef unsigned char u8;

/* constants */
#define MAX_DIGITS 3
#define SAVES_FOLDER "/.cache/traverse/"

extern const char* ITOA[10];

/* used with the libxml */
void GetXmlVariableByName(xmlNode* rootNode, const char* variable, u8* result);
void GetXmlPlayerNode(xmlNode* rootNode, xmlNode** result, u8 playerId);
void GetXmlPawnPositions(xmlNode* playerNode, u8 pawnId, u8* line, u8* column);
xmlNodePtr SetXmlVariable(const char* propName, const char* propValue, char* varName, u8 varValue);

/* From David Egan */
/* Use it to be sure that the user give us an integer via stdin */
/* https://dev-notes.eu/2019/05/Integer-Input-in-C/ */
void ClearInputBuffer();
void GetIntegerFromStdin(u8* inputInteger);

#endif /* _UTILS_TRAVERSE_H */