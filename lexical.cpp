#include "format.hpp"

/**
 * Checks if a string is a language keyword
 * @param str String to check
 * @return true if str is a keyword, false otherwise
 */
bool 
isKeyword(
  string str
) {
  return !!keywords.exist[str];
}

/**
 * Gets the type description for a token
 * @param str Token to get description for
 * @return String describing token type (Keyword, Operator, etc.)
 */
string
keywordDis(
  string str
) {
  int id = keywords.id(str);
  if (id <= 16) return "Keyword";
  if (id <= 30) return "Operator";
  if (id <= 36) return "Symbol";
  if (id == 37) return "Integer";
  if (id == 38) return "Float";
  if (id == 39) return "Identifier";
  if (id == 40) return "Color";
  return "";
}

/**
 * Checks if a character can be part of a number
 * @param c Character to check
 * @return true if c is digit or decimal point
 */
bool 
isNumberPart(
  char c
) {
  return isdigit(c) || c == '.';
}

/**
 * Checks if a character can start an identifier
 * @param c Character to check
 * @return true if c is letter or underscore
 */
bool 
isIdentifierHead(
  char c
) {
  return isalpha(c) || c == '_';
}

/**
 * Checks if a character can be part of an identifier
 * @param c Character to check
 * @return true if c is alphanumeric or underscore
 */
bool 
isIdentifierPart(
  char c
) {
  return isalnum(c) || c == '_';
}

/**
 * Tokenizes a single line of source code
 * @param line Reference to line being tokenized
 * @param lineCnt Current line number in source
 */
void 
tokenize(
  string& line,
  int lineCnt
) {
  int i = 0;
  while (i < line.length()) {
    char c = line[i];

    // Spaces
    if (isspace(c)) {
      i++;
      continue;
    }

    // Numbers (float and integer)
    if (isNumberPart(c)) {
      string num;
      bool hasDecimal = false;
      while (i < line.length() && isNumberPart(line[i])) {
        if (line[i] == '.') {
          if (hasDecimal) break;
          hasDecimal = true;
        }
        num += line[i++];
      }
      if (isIdentifierHead(line[i])) {
        int pos = i; while (isIdentifierPart(line[pos++]));
        error_line("[Lexical Error]", "Unqualified identifier.", line, lineCnt, i - num.length(), pos - i - 1 + num.length());
      }
      if (hasDecimal) lexiinfo.push_back((LexiItem) { num, "Float", keywords.id("float"), lineCnt, int(i - num.length()) });
      else lexiinfo.push_back((LexiItem) { num, "Integer", keywords.id("integer"), lineCnt, int(i - num.length()) });
      // cout << "Number: " << num << endl;
      continue;
    }

    // Words (keyword and identifier)
    if (isIdentifierHead(c)) {
      string id;
      while (i < line.length() && isIdentifierPart(line[i])) {
        id += line[i++];
      }
      if (isKeyword(id)) lexiinfo.push_back((LexiItem) { id, keywordDis(id), keywords.id(id), lineCnt, int(i - id.length()) });
      else lexiinfo.push_back((LexiItem) { id, "Identifier", keywords.id("identifier"), lineCnt, int(i - id.length()) });
      // cout << (isKeyword(id) ? "Keyword: " : "Identifier: ") << id << endl;
      continue;
    }

    // Colors
    if (c == '#') {
      string color = "$"; i++;
      for (; i <= line.length() && isxdigit(line[i]);) {
        color += line[i++];
      }
      if (color.length() == 7) {
        lexiinfo.push_back((LexiItem) { color, "Color", keywords.id("color"), lineCnt, i - 7 });
        // cout << "Color: " << color << endl;
      } else {
        error_line("[Lexical Error]", "Incorrect color format.", line, lineCnt, i - color.length(), color.length());
      }
      continue;
    }

    // Comment
    if (
      c == '/' && 
      i + 1 < line.length() && 
      line[i + 1] == '/'
    ) break;

    // Operators
    string op, dop; op += line[i++];
    if (i < line.length() && isKeyword(dop = op + line[i])) {
      lexiinfo.push_back((LexiItem) { dop, keywordDis(dop), keywords.id(dop), lineCnt, int(++i - dop.length()) });
      // cout << "Operator: " << dop << endl;
    } else if (isKeyword(op)) {
      lexiinfo.push_back((LexiItem) { op, keywordDis(op), keywords.id(op), lineCnt, int(i - op.length()) });
      // cout << (isKeyword(op) ? "Operator: " : "Symbol: ") << op << endl;
    } else {
      error_line("[Lexical Error]", "Undefined symbol.", line, lineCnt, i - 1, 1);
    }
  }
}

/**
 * Outputs tokenization results to file
 * Writes all tokens with their information to lexi.txt
 */
void
output(
  string ouName
) {
  fstream lexiOut(ouName, ios::out | ios::trunc);
  lexiOut << " "
  << left << setw(25) << "Lexical Content"
  << left << setw(25) << "Type Discription"
  << left << setw(13) << "Lexical ID"
  << left << setw(13) << "Line"
  << left << setw(13) << "Column"
  << endl;
  lexiOut << string(2 + 25 + 25 + 13 + 13 + 13, '-') << endl;
  for (LexiItem& item: lexiinfo) {
    lexiOut << " "
    << left << setw(25) << item.content
    << left << setw(25) << item.typeDis
    << left << setw(13) << item.lexiID
    << left << setw(13) << item.line
    << left << setw(13) << item.column
    << endl;
  }
}

/**
 * Main lexical analysis function
 * Reads source file and performs tokenization
 */
void 
lexicalize(
  string inName,
  string ouName,
  bool lexicode
) {
  fstream code(inName, ios::in);
  if (!code.is_open()) {
    error_info("[Compiler Error]", inName + ": No such file or directory.");
  } else {
    string line; int lineCnt = 0;
    while(getline(code, line) && ++lineCnt) {
      tokenize(line, lineCnt);
    } 
    code.close();
    if (lexicode) output(ouName + ".lexi");
  }
}
