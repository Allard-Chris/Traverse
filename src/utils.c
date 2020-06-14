#include "utils.h"

const char* ITOA[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

/* get back the value of the variable by his name */
/* Warning, we didn't do any kind of control about value inside XML */
/* Some "Custom" values can softlock */
void GetXmlVariableByName(xmlNode* rootNode, const char* variableName, u8* result) {
  xmlNode* tmpNode = NULL;

  for (tmpNode = rootNode; tmpNode != NULL; tmpNode = tmpNode->next) {
    if ((tmpNode->type == XML_ELEMENT_NODE) && (strcmp((const char*)tmpNode->name, variableName) == 0)) {
      *result = (u8)atoi((const char*)(xmlNodeGetContent(tmpNode)));
    }
    GetXmlVariableByName(tmpNode->children, variableName, result);
  }
}

/* return address of player node inside DOM structure */
void GetXmlPlayerNode(xmlNode* rootNode, xmlNode** result, u8 playerId) {
  xmlNode*       tmpNode = NULL;
  unsigned char* tmpIdString = NULL;

  for (tmpNode = rootNode; tmpNode != NULL; tmpNode = tmpNode->next) {
    if ((tmpNode->type == XML_ELEMENT_NODE) && (strcmp((const char*)tmpNode->name, "player") == 0)) {
      tmpIdString = xmlGetProp(tmpNode, BAD_CAST "id");
      if ((tmpIdString != NULL) && (strcmp((const char*)tmpIdString, ITOA[playerId]) == 0)) {
        *result = tmpNode;
      }
    }
    GetXmlPlayerNode(tmpNode->children, result, playerId);
  }
}

/* for XML player node, return position of his pawn */
void GetXmlPawnPositions(xmlNode* playerNode, u8 pawnId, u8* line, u8* column) {
  xmlNode*       tmpNode = NULL;
  unsigned char* tmpIdString = NULL;

  for (tmpNode = playerNode; tmpNode != NULL; tmpNode = tmpNode->children) {
    if ((tmpNode->type == XML_ELEMENT_NODE) && (strcmp((const char*)tmpNode->name, "pawn") == 0)) {
      tmpIdString = xmlGetProp(tmpNode, BAD_CAST "id");
      if ((tmpIdString != NULL) && (strcmp((const char*)tmpIdString, ITOA[pawnId]) == 0)) {
        *line   = (u8)atoi((const char*)(xmlNodeGetContent(tmpNode->children)));
        *column = (u8)atoi((const char*)(xmlNodeGetContent(tmpNode->children->next)));
      }
    }
    GetXmlPawnPositions(tmpNode->next, pawnId, line, column);
  }
}

/* return new Xml node pointer to add into our save file */
xmlNodePtr SetXmlVariable(const char* propName, const char* propValue, char* varName, u8 varValue) {
  xmlNodePtr variableNode;

  if ((variableNode = xmlNewNode(NULL, BAD_CAST varName)) == NULL) {
    return NULL;
  }

  if (propName != NULL) {
    if (xmlSetProp(variableNode, BAD_CAST propName, BAD_CAST propValue) == NULL) {
      xmlFreeNode(variableNode);
      return NULL;
    }
  }
  xmlNodeSetContent(variableNode, BAD_CAST ITOA[varValue]);
  return variableNode;
}

void ClearInputBuffer() {
  char c = 0;
  // Loop over input buffer and consume chars until buffer is empty
  while ((c = getchar()) != '\n' && c != EOF);
}

void GetIntegerFromStdin(u8* inputInteger) {
  char* inputBuffer = malloc(sizeof(char) * MAX_DIGITS);
  memset(inputBuffer, 0, MAX_DIGITS);
  char* input = NULL;
  while (input == NULL) {
    // Note that fgets returns inputBuffer on success.
    // This becomes important when freeing - free either `input` or
    // `inputBuffer` to avoid an attempted double-free error.
    input = fgets(inputBuffer, MAX_DIGITS, stdin);

    // If fgets() receives less than MAX_DIGITS, the last char in the array is
    // '\n'. Therefore if the last char is not '\n', too many characters were
    // entered.
    if (inputBuffer[strlen(inputBuffer) - 1] != '\n') {
      fprintf(stderr, "[ERROR]: Too many characters: max input is %d chars.\n", MAX_DIGITS);
      ClearInputBuffer();
      input = NULL;
      continue;
    }

    // Check that the input can be intepreted as an integer
    // Convert to integer using `strtol()`
    errno = 0;
    char* endptr = NULL;
    *inputInteger = strtol(input, &endptr, 10);

    // If an integer was not found, endptr remains set to input
    if (input == endptr) {
      // Remove trailing newline by adding NUL at the index of the
      // terminating '\n' character. See man strcspn - this function
      // gets the length of a prefix substring.
      input[strcspn(input, "\n")] = 0;
      printf("Invalid input: no integer found in %s.\n", input);
      input = NULL;
    }
    if (errno != 0) {
      fprintf(stderr, "[ERROR]: That doesn't look like an integer.\n");
      input = NULL;
    }
  }
  free(inputBuffer);
}