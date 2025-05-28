#ifndef __FORMAT_HPP
#define __FORMAT_HPP

#include <list>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
using namespace std;

/**
 * Keywords management class
 * Stores language keywords and maps them to type IDs
 */
struct Keywords {
  unordered_map<string, int> exist;
  static constexpr int tokenNum = 36;
  string list[tokenNum] = {
    "def", "main", "return", "void", "int", "float", "vec", "for", "while", "if", "else",   // Keywords   (typeID  1 ~ 11)
    "draw", "line", "circle", "triangle", "rectangle",                                      // Keywords   (typeID 12 ~ 16)
    "+", "-", "*", "/", "^", "<", ">", "=", "<=", ">=", "==", "++", "--", "->",             // Operators  (typeID 17 ~ 30)
    ",", ";", "(", ")", "{", "}"                                                            // Symbols    (typeID 31 ~ 36)
  };

  // "[0-9]+"                 Integer     (typeID 37)
  // "[0-9]+.[0-9]+"          Float       (typeID 38)
  // "[a-zA-Z_][0-9a-zA-Z_]*" Identifier  (typeID 39)
  // "$[0-9a-fA-F]{6}"        Color       (typeID 40)

  /**
   * Initializes the keyword map with all language keywords
   * Maps each keyword to its corresponding type ID
   */
  Keywords() {
    for (int i = 0; i < tokenNum; i++) {
      exist[list[i]] = i + 1;
    }
  }

  /**
   * Gets the type ID for a given string
   * @param str String to check for type ID
   * @return Type ID if found, 0 if not a recognized type
   */
  int 
  id(
    string str
  ) {
    if (exist[str]) return exist[str];
    else if (str == "integer"   ) return 37;
    else if (str == "float"     ) return 38;
    else if (str == "identifier") return 39;
    else if (str == "color"     ) return 40;
    else return 0;
  }
};

/**
 * Single token information from lexical analysis
 * Stores token content, type and position information
 */
struct 
LexiItem {
  string content;  // The actual token text
  string typeDis;  // Type description
  int lexiID;      // Token type ID
  int line;        // Line number in source
  int column;      // Column number in source
};
typedef vector<LexiItem> LexiInfo;

/**
 * Formatted item for syntax analysis and output
 * Enhanced version of LexiItem with formatting capabilities
 */
struct
FormItem {
  string content;
  string typeDis;
  int line, column;
  int length, exceed;

  /**
   * Default constructor
   * Creates an empty FormItem
   */
  FormItem() {}

  /**
   * Construct FormItem from LexiItem
   * @param lexiitem Source LexiItem to copy data from
   */
  FormItem(LexiItem& lexiitem) {
    content = lexiitem.content;
    typeDis = lexiitem.typeDis;
    line = lexiitem.line;
    column = lexiitem.column;
    length = lexiitem.content.length();
    exceed = 0;
  }

  /**
   * Creates new FormItem with different content
   * @param str New content string
   * @return FormItem with updated content
   */
  FormItem 
  withCon(
    string str
  ) {
    FormItem newItem = *this;
    newItem.content = str;
    return newItem;
  }

  /**
   * Creates new FormItem with different type description
   * @param str New type description
   * @return FormItem with updated type description
   */
  FormItem 
  withDis(
    string str
  ) {
    FormItem newItem = *this;
    newItem.typeDis = str;
    return newItem;
  }

  /**
   * Adds string to front of content
   * @param str String to prepend (defaults to space)
   * @return FormItem with modified content
   */
  FormItem 
  front_push(
    string str = " "
  ) {
    FormItem newItem = *this;
    newItem.content = str + newItem.content;
    return newItem;
  }

  /**
   * Adds string to end of content
   * @param str String to append (defaults to space)
   * @return FormItem with modified content
   */
  FormItem 
  back_push(
    string str = " "
  ) {
    FormItem newItem = *this;
    newItem.content += str;
    return newItem;
  }

  /**
   * Adds spaces around content
   * @return FormItem with spaces added to front and back
   */
  FormItem 
  space() {
    FormItem newItem = *this;
    return newItem.back_push().front_push();
  }
};

/**
 * Custom hash function for pair<string,int>
 * Used in variable scope management
 */
struct 
PairHash {
  /**
   * Hash function for string,int pairs
   * @param p Pair to hash
   * @return Hash value for the pair
   */
  template <typename T1, typename T2>
  std::size_t operator()(const std::pair<T1, T2>& p) const {
    auto hash1 = std::hash<T1>{}(p.first);  
    auto hash2 = std::hash<T2>{}(p.second); 
    return hash1 ^ (hash2 << 1);            
  }
};

/**
 * Variable scope and type management
 * Tracks variables across different scope layers
 */
struct
VariInfo {
  unordered_map<pair<string, int>, string, PairHash> map;
  //          name <-=====^  ^==--> numLayer
  vector<pair<string, int>> vec;

  /**
   * Adds a new variable to current scope
   * @param name Variable name
   * @param type Variable type
   * @param layer Scope layer number
   */
  void 
  add(
    string name,
    string type,
    int layer
  ) {
    map[make_pair(name, layer)] = type;
    vec.push_back(make_pair(name, layer));
  }

  /**
   * Checks if variable exists in given scope
   * @param name Variable name to check
   * @param layer Scope layer to check
   * @return true if variable exists in scope
   */
  bool
  exist(
    string name,
    int layer
  ) {
    for (int i = layer; i >= 0; i--) {
      if (map[make_pair(name, i)] != "") return true;
    }
    return false;
  }

  /**
   * Removes all variables from a scope layer
   * @param layer Scope layer to clear
   */
  void 
  del(
    int layer
  ) {
    for (int i = vec.size() - 1; i >= 0; i--) {
      if (vec[i].second == layer) {
        map[make_pair(vec[i].first, layer)] = "";
        vec.pop_back();
      }
    }
  }

  /**
   * Displays all variables in current scope layer
   * @param layer Current scope layer to display
   */
  void 
  show(
    int layer
  ) {
    int pos = vec.size() - 1;
    for (; pos > 0 && vec[pos - 1].second == layer; pos--);
    cout << endl << "++++++++++++++++++++++" << endl;
    for (; pos < vec.size(); pos++) {
      cout 
      << left << setw(15) << vec[pos].first 
      << left << setw(10) << " layer: " + to_string(vec[pos].second) 
      << left << setw(15) << " type: " + map[make_pair(vec[pos].first, layer)] << endl;
    }
    cout << "++++++++++++++++++++++" << endl << endl;
  }
}; 

/**
 * Function declaration management
 * Tracks function signatures and parameter counts
 */
struct
FuncInfo {
  unordered_map<string, pair<int, bool>> map;
  vector<string> vec;

  /**
   * Adds a new function definition
   * @param name Function name
   * @param numParam Number of parameters
   */
  void 
  add(
    string name, 
    int numParam
  ) {
    map[name] = make_pair(numParam, true);
    vec.push_back(name);
  }

  /**
   * Checks if function is defined
   * @param name Function name to check
   * @return true if function exists
   */
  bool
  exist(
    string name
  ) {
    return map[name].second;
  }

  /**
   * Gets number of parameters for a function
   * @param name Function name
   * @return Number of parameters
   */
  int
  num(
    string name
  ) {
    return map[name].first;
  }

  /**
   * Displays all defined functions
   * Prints function names and parameter counts
   */
  void 
  show() {
    for (string funcName: vec) {
      cout << funcName << " " << this->num(funcName) << endl;
    }
  }
}; 

/**
 * Drawing command information
 * Stores parameters for various drawing operations
 */
struct 
DrawItem {
  string itemName;      // Type of the shape ("line", "circ", "tria", "rect")
  double params[7];     // Coordinate parameters as floating point
  string colorParams;   // Color in hex format (e.g. "ff0000" for red)
};
typedef vector<DrawItem> DrawInfo;

/**
 * Function parameter information
 * Used during function declaration parsing
 */
struct ParaItem {
  string type, paraName;
};

bool isType(int);
bool isNumber(int);
bool isDrawtype(int);
bool isAritOperator(int);
bool isCompOperator(int);
bool isInDeOperator(int);

FormItem operator + (const FormItem&, const FormItem&);
FormItem& operator += (FormItem&, const FormItem&);

void error_item(string, string, LexiItem&);
void error_form(string, string, FormItem&);
void error_line(string, string, string&, int, int, int);
void error_info(string, string);
void error_name(string);

void lexicalize(string, string, bool);
string& recognize(string, bool);

// Global variables
extern Keywords keywords;   // Global keyword manager
extern LexiInfo lexiinfo;   // Global token storage
extern VariInfo variinfo;   // Global variable manager
extern FuncInfo funcinfo;   // Global function manager
extern DrawInfo drawinfo;   // Global drawing command storage

#endif