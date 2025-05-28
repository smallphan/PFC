#include "format.hpp"

/**
 * Checks if a token ID represents a type
 * @param id Token type ID to check
 * @return true if ID represents a type, false otherwise
 */
bool 
isType(
  int id
) {
  return (keywords.id("void") <= id) && (id <= keywords.id("vec"));
}

/**
 * Checks if a token ID represents a number type
 * @param id Token type ID to check
 * @return true if ID represents integer or float, false otherwise
 */
bool
isNumber(
  int id
) {
  return id == keywords.id("integer") || id == keywords.id("float");
}

/**
 * Checks if a token ID represents a drawing command
 * @param id Token type ID to check
 * @return true if ID represents a drawing command, false otherwise
 */
bool
isDrawtype(
  int id
) {
  return (keywords.id("line") <= id) && (id <= keywords.id("rectangle"));
}

/**
 * Checks if a token ID represents an arithmetic operator
 * @param id Token type ID to check
 * @return true if ID represents an arithmetic operator, false otherwise
 */
bool
isAritOperator(
  int id
) {
  return (keywords.id("+") <= id) && (id <= keywords.id("^"));
}

/**
 * Checks if a token ID represents a comparison operator
 * @param id Token type ID to check
 * @return true if ID represents a comparison operator, false otherwise
 */
bool 
isCompOperator(
  int id
) {
  return (keywords.id("<") <= id) && (id <= keywords.id("=="));
}

/**
 * Checks if a token ID represents increment/decrement operator
 * @param id Token type ID to check
 * @return true if ID represents ++ or --, false otherwise
 */
bool 
isInDeOperator(
  int id
) {
  return keywords.id("++") == id || id == keywords.id("--");
}

/**
 * Concatenates two FormItems
 * @param a Left FormItem operand
 * @param b Right FormItem operand
 * @return New FormItem containing concatenated content
 */
FormItem operator + (const FormItem& a, const FormItem& b) {
  FormItem c;
  if (a.content == "") c = b;
  else {
    c.content = a.content + b.content;
    c.typeDis = "subexpresion";
    c.line = a.line;
    c.column = a.column;
    c.length = (a.line == b.line) ? (b.column - a.column + b.length) : a.length;
    c.exceed = (a.line == b.line) ? 0 : 1;
  }
  return c;
}

/**
 * Appends one FormItem to another
 * @param a FormItem to append to
 * @param b FormItem to append
 * @return Reference to modified FormItem
 */
FormItem& operator += (FormItem& a, const FormItem& b) {
  a = a + b;
  return a;
}

/**
 * Restores source line content from tokens
 * @param line Line number to restore
 * @return String containing reconstructed line content
 */
string
restore_line(
  int line
) {
  LexiItem *item = &*lower_bound(lexiinfo.begin(), lexiinfo.end(), line, 
    [](const LexiItem& item, int value) { 
      return item.line < value; 
    }
  );
  string lineContent; int index = 0;
  while (item->line == line) {
    for (; index < item->column; index++) lineContent += " ";
    lineContent += item->content, index += (item++)->content.length();
  }

  return lineContent;
}

string errorName;

/**
 * Reports lexical error with token information
 * @param errorType Type of error
 * @param message Error message
 * @param lexiitem Token where error occurred
 */
void 
error_item(
  string errorType,
  string message,
  LexiItem& lexiitem
) {
  string lineContent = restore_line(lexiitem.line);
  error_line(errorType, message, lineContent, lexiitem.line, lexiitem.column, lexiitem.content.length());
}

/**
 * Reports syntax error with form information
 * @param errorType Type of error
 * @param message Error message
 * @param formitem Form where error occurred
 */
void 
error_form(
  string errorType,
  string message,
  FormItem& formitem
) {
  string lineContent = restore_line(formitem.line);
  error_line(errorType, message, lineContent, formitem.line, formitem.column, formitem.length);
}

/**
 * Reports error with line content and formatting
 * @param errorType Type of error
 * @param message Error message
 * @param lineContent Content of error line
 * @param line Line number
 * @param column Column number
 * @param length Length of error region
 */
void 
error_line(
  string errorType,
  string message,
  string& lineContent,
  int line,
  int column,
  int length
) {
  message = errorName + ":" + to_string(line) + ":" + to_string(column + 1) + ": " + "\033[35m" + errorType + "\033[0m " + message;
  ostringstream oss;
  oss << right << setw(6) << line << " | ";
  string errorLine[2];
  errorLine[0] = oss.str();
  for (int i = 0; i < lineContent.length(); i++) {
    if (i == column) errorLine[0] += "\033[31m";
    if (i == column + length) errorLine[0] += "\033[0m";
    errorLine[0] += lineContent[i];
  }
  errorLine[0] += "\033[0m";
  errorLine[1] = string(6, ' ') + " | " + string(column, ' ') + "\033[31m" + "^" + string(length - 1, '~') + "\033[0m";
  cout << message + "\n" + errorLine[0] + "\n" + errorLine[1] + "\n";
  exit(0);
}

/**
 * Reports general compiler error and exits program
 * @param errorType Type of error (e.g. "[Compiler Error]")
 * @param message Error message to display
 */
void
error_info(
  string errorType,
  string message
) {
  message = "pfc: \033[35m" + errorType + "\033[0m " + message;
  cout << message << endl;
  exit(0);
}

/**
 * Sets the input filename for error reporting
 * @param name Name of the input file being processed
 */
void
error_name(
  string name
) {
  errorName = name;
}