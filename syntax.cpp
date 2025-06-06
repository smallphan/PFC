#include "format.hpp"

// Recognize Functions
FormItem reco_parameters(int&, string);
FormItem reco_call(int&, string);
FormItem reco_power(list<FormItem>&, list<FormItem>::iterator&);
FormItem reco_formula_inner(int&);
string reco_formula(int&);
string reco_vec(int&, bool);
string reco_draw(int&);
string reco_define(int&, int);
string reco_compare(int&);
string reco_multiformula(int&);
string reco_for(int&);
string reco_if(int&);
string reco_while(int&);
string reco_return(int&);
string reco_block(int&, bool* = NULL);
string reco_paralist(int&, int&);
string reco_function(int&);

int blockLayer;
bool reqReturnVal;
string nowFuncName;

/**
 * Checks if current token is a syntax boundary
 * @param index Current token index
 * @return true if token is not a boundary symbol
 */
bool 
check_boarder(
  int index
) {
  return 
    lexiinfo[index].lexiID != keywords.id(",") &&
    lexiinfo[index].lexiID != keywords.id(";") &&
    lexiinfo[index].lexiID != keywords.id(")") &&
    lexiinfo[index].lexiID != keywords.id("color") &&
    !isCompOperator(lexiinfo[index].lexiID);
}

/**
 * Repeats a string a specified number of times
 * @param str String to repeat
 * @param times Number of times to repeat
 * @return Concatenated string
 */
string
repeatString(
  string str,
  int times
) {
  string retString;
  for (int i = 0; i < times; i++) retString += str;
  return retString;
}

/**
 * Processes function parameters in a call
 * @param index Current token index
 * @param funcName Name of function being called
 * @return FormItem containing processed parameters
 */
FormItem 
reco_parameters(
  int& index,
  string funcName
) {
  FormItem item;
  int numParam = funcinfo.num(funcName);

  if (lexiinfo[index].lexiID == keywords.id("(")) {
    item = FormItem(lexiinfo[index++]);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  while (lexiinfo[index].lexiID != keywords.id(")")) {
    if (lexiinfo[index].lexiID == keywords.id(",")) {
      item += FormItem(lexiinfo[index++]).back_push(" ");
    } else item += reco_formula_inner(index), numParam--;
  }

  if (numParam != 0) error_item(
    "[Semantic Error]", 
    "Wrong number of parameters of function " + funcName + ". Function " + funcName + " has " + to_string(funcinfo.num(funcName)) + " parameters.", 
    lexiinfo[index]
  );

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    item += FormItem(lexiinfo[index++]);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  return item;
} 

/**
 * Processes a function call
 * @param index Current token index
 * @param funcName Name of function to call
 * @return FormItem containing processed call
 */
FormItem 
reco_call(
  int& index,
  string funcName
) {
  FormItem item;

  if (lexiinfo[index].lexiID == keywords.id("identifier")) {
    item = FormItem(lexiinfo[index++]);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be IDENTIFIER.", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("(")) {
    item += reco_parameters(index, funcName);
  }

  return item;
}

/**
 * Processes power operator expressions
 * @param phrase List of tokens in expression
 * @param it Iterator to current position
 * @return FormItem containing processed power expression
 */
FormItem
reco_power(
  list<FormItem>& phrase,
  list<FormItem>::iterator& it
) {
  FormItem item = *(it++);
  item = item.front_push("Power(") + (it++)->withCon(", ");
  if (
    it != phrase.end() && 
    next(it) != phrase.end() && 
    next(it)->content == "^"
  ) item += reco_power(phrase, it).back_push(")");
  else item += (it++)->back_push(")");
  return item;
}

/**
 * Processes inner parts of a formula
 * @param index Current token index
 * @return FormItem containing processed formula
 */
FormItem
reco_formula_inner(
  int& index
) {
  FormItem item;
  list<FormItem> phrase;

  while (check_boarder(index)) {

    if (lexiinfo[index].lexiID == keywords.id("(")) {
      FormItem item = FormItem(lexiinfo[index++]);
      item += reco_formula_inner(index);
      if (lexiinfo[index].lexiID == keywords.id(")")) {
        item += lexiinfo[index++];
      } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);
      phrase.push_back(item);
    }
    
    if (lexiinfo[index].lexiID == keywords.id("identifier")) {
      if (lexiinfo[index + 1].lexiID == keywords.id("(")) {
        if (funcinfo.exist(lexiinfo[index].content)) {
          phrase.push_back(reco_call(index, lexiinfo[index].content).withDis("function"));
        } else error_item("[Semantic Error]", "Undefined function.", lexiinfo[index]);
      } else {
        if (variinfo.exist(lexiinfo[index].content, blockLayer)) {
          FormItem item = FormItem(lexiinfo[index++]).withDis("identifier");
          if (!phrase.empty() && phrase.back().typeDis == "indecrement") {
            item = phrase.back() + item;
            phrase.pop_back();
          }
          phrase.push_back(item);
        } else error_item("[Semantic Error]", "Undefined variable.", lexiinfo[index]);
      }
    }

    if (isNumber(lexiinfo[index].lexiID)) {
      phrase.push_back(FormItem(lexiinfo[index++]).withDis("number"));
    }

    if (isAritOperator(lexiinfo[index].lexiID)) {
      phrase.push_back(FormItem(lexiinfo[index++]).withDis("arithmetic"));
    }

    if (isInDeOperator(lexiinfo[index].lexiID)) {
      FormItem item = FormItem(lexiinfo[index++]).withDis("indecrement");
      if (!phrase.empty() && phrase.back().typeDis == "identifier") {
        item = phrase.back() + item;
        phrase.pop_back();
      }
      phrase.push_back(item);
    }

  }

  auto it = phrase.begin(), it_ = next(it);
  if (
    phrase.size() >= 2 && 
    ((it->content == "+" || it->content == "-") && 
    ((it_)->typeDis != "arithmetic") && (it_->typeDis != "indecrement"))
  ) {
    FormItem item = (it->back_push() + (*it_)).withDis("subexpression");
    phrase.pop_front(), phrase.pop_front();
    phrase.push_front(item);
  }

  it = phrase.begin();
  while (++it != phrase.end()) {
    if (it->typeDis == "arithmetic") {
      if (prev(it)->typeDis == "arithmetic" || next(it)->typeDis == "arithmetic") {
        error_form("[Syntax Error]", "Redundant arithmetic symbol.", *it);
      }
    } else if (it->typeDis == "indecrement") {
      error_form("[Syntax Error]", "Redundant increment/decrement symbol.", *it);
    } else {
      if (!(prev(it)->typeDis == "arithmetic" && !(next(it) != phrase.end() && (next(it)->typeDis != "arithmetic")))) {
        error_form("[Syntax Error]", "Redundant subexpression.", *it);
      }
    }
  }

  it = phrase.begin();
  while (it != phrase.end()) {
    if (next(it) != phrase.end() && next(it)->content == "^") item += reco_power(phrase, it);
    else if (it->typeDis == "arithmetic") item += (it++)->space();
    else item += *(it++);
  }

  // cout << endl << "+++++++++++++++++++++++++++++" << endl;
  // for (FormItem formitem: phrase) {
  //   cout << formitem.content << " " << formitem.typeDis << " " << formitem.line << " " << formitem.column << " " << formitem.length << " " << formitem.exceed << endl;
  // }
  // cout << "+++++++++++++++++++++++++++++" << endl << endl;

  return item;
}

/**
 * Processes a complete formula
 * @param index Current token index
 * @return String containing processed formula
 */
string
reco_formula(
  int& index
) {
  FormItem item = reco_formula_inner(index);
  if (item.content == "") error_item("[Syntax Error]", "Formula missing.", lexiinfo[index]);
  return item.content;
}

/**
 * Processes a vector definition
 * @param index Current token index
 * @param isDraw Whether vector is used in draw command
 * @return String containing processed vector
 */
string 
reco_vec(
  int& index,
  bool isDraw
) {
  string content;

  if (lexiinfo[index].lexiID == keywords.id("vec")) { 
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"vec\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("(")) { 
    content += (isDraw ? "(double) " : "") + reco_formula(++index) + ", ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id(",")) {
    content += (isDraw ? "(double) " : "") + reco_formula(++index);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \",\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  return content;
}

/**
 * Processes a draw command
 * @param index Current token index
 * @return String containing processed draw command
 */
string
reco_draw(
  int& index
) {
  string content;
  int vecNumber = 0;
  bool hasParam = false;

  if (lexiinfo[index].lexiID == keywords.id("draw")) { 
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"draw\".", lexiinfo[index]);

  if (isDrawtype(lexiinfo[index].lexiID)) {  

    if (lexiinfo[index].lexiID == keywords.id("line")) {
      content = "printf(\"line %.2lf %.2lf %.2lf %.2lf %.2lf %s\\n\", ";
      vecNumber = 2;
      hasParam = true;
    }

    if (lexiinfo[index].lexiID == keywords.id("circle")) {
      content = "printf(\"circ %.2lf %.2lf %.2lf %s\\n\", ";
      vecNumber = 1;
      hasParam = true;
    }

    if (lexiinfo[index].lexiID == keywords.id("triangle")) {
      content = "printf(\"tria %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %s\\n\", ";
      vecNumber = 3;
      hasParam = false;
    }

    if (lexiinfo[index].lexiID == keywords.id("rectangle")) {
      content = "printf(\"rect %.2lf %.2lf %.2lf %.2lf %s\\n\", ";
      vecNumber = 2;
      hasParam = false;
    }

    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords of DRAW-TYPE.", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("(")) { 
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  for (int i = 0; i < vecNumber; i++) {
    if (lexiinfo[index].lexiID == keywords.id("vec")) { 
      content += reco_vec(index, true) + ", ";
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"vec\".", lexiinfo[index]);
  
    if (lexiinfo[index].lexiID == keywords.id(",")) {
      index++;
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \",\".", lexiinfo[index]);
  }

  if (hasParam) {
    content += "(double) " + reco_formula(index) + ", ";

    if (lexiinfo[index].lexiID == keywords.id(",")) {
      index++;
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \",\".", lexiinfo[index]);
  }


  if (lexiinfo[index].lexiID == keywords.id("color")) {
    content += "\"" + lexiinfo[index++].content + "\"";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"color\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id(";")) {
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \";\".", lexiinfo[index]);

  content += ");";

  return content;
}

/**
 * Processes a variable definition
 * @param index Current token index
 * @param layer Current block layer
 * @return String containing processed definition
 */
string
reco_define(
  int& index,
  int layer
) {
  string content, name, type;

  if (isType(lexiinfo[index].lexiID)) {
    type = lexiinfo[index++].content;
    content += type + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords of TYPE.", lexiinfo[index]);

  while (lexiinfo[index].lexiID != keywords.id(";")) {
    if (lexiinfo[index].lexiID == keywords.id("identifier")) {
      name = lexiinfo[index++].content;
      if (!variinfo.exist(name, layer)) {
        variinfo.add(name, type, layer);
        content += name;
      } else error_item("[Semantic Error]", "Redefined variable.", lexiinfo[index - 1]);
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be IDENTIFIER.", lexiinfo[index]);
    
    if (lexiinfo[index].lexiID == keywords.id("=")) {
      content += " " + lexiinfo[index++].content + " ";
      content += reco_formula(index);
    }

    if (lexiinfo[index].lexiID == keywords.id(",")) {
      content += lexiinfo[index++].content + " ";
    }
  }

  if (lexiinfo[index].lexiID == keywords.id(";")) {
    content += lexiinfo[index++].content;
  }

  return content;
}

/**
 * Processes a comparison expression
 * @param index Current token index
 * @return String containing processed comparison
 */
string
reco_compare(
  int& index
) {
  string content;
  content += reco_formula(index);

  if (isCompOperator(lexiinfo[index].lexiID)) {
    content += " " + lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords of COMPARE OPERATORS.", lexiinfo[index]);

  content += reco_formula(index);

  return content;
}

/**
 * Processes multiple comma-separated formulas
 * @param index Current token index
 * @return String containing processed formulas
 */
string
reco_multiformula(
  int& index
) {
  string content;

  while (lexiinfo[index].lexiID != keywords.id(";")) {
    content += reco_formula(index);
    if (lexiinfo[index].lexiID == keywords.id(",")) {
      content += lexiinfo[index++].content + " ";
    } else if (lexiinfo[index].lexiID == keywords.id("=")) {
      content += " " + lexiinfo[index++].content + " ";
    }
  }

  if (lexiinfo[index].lexiID == keywords.id(";")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \";\".", lexiinfo[index]);

  return content;
}

/**
 * Processes a for loop
 * @param index Current token index
 * @return String containing processed for loop
 */
string 
reco_for(
  int& index
) {
  string content;

  if (lexiinfo[index].lexiID == keywords.id("for")) {
    content += lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"for\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("(")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  if (isType(lexiinfo[index].lexiID)) {
    content += reco_define(index, ++blockLayer) + " ";
  } else {
    content += reco_multiformula(index);
    if (lexiinfo[index].lexiID == keywords.id(";")) {
      content += lexiinfo[index++].content + " ";
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \";\".", lexiinfo[index]);
  }

  content += reco_compare(index);
  
  if (lexiinfo[index].lexiID == keywords.id(";")) {
    content += lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \";\".", lexiinfo[index]);

  while (lexiinfo[index].lexiID != keywords.id(")")) {
    content += reco_formula(index);
    if (lexiinfo[index].lexiID == keywords.id(",")) {
      content += lexiinfo[index++].content + " ";
    } else if (lexiinfo[index].lexiID != keywords.id(")")) {
      error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \",\".", lexiinfo[index]);
    }
  }

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    content += lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  blockLayer--;

  if (lexiinfo[index].lexiID == keywords.id("{")) {
    content += reco_block(index);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"{\".", lexiinfo[index]);
  
  return content;
} 

/**
 * Processes an if statement
 * @param index Current token index
 * @return String containing processed if statement
 */
string
reco_if(
  int& index
) {
  string content;

  if (lexiinfo[index].lexiID == keywords.id("if")) {
    content += lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"if\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("(")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  content += reco_compare(index);

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    content += lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("{")) {
    content += reco_block(index) + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"{\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("else")) {
    content += lexiinfo[index++].content + " ";
    if (lexiinfo[index].lexiID == keywords.id("if")) {
      content += reco_if(index);
    } else if (lexiinfo[index].lexiID == keywords.id("{")) {
      content += reco_block(index);
    }
  }

  return content;
}

/**
 * Processes a while loop
 * @param index Current token index
 * @return String containing processed while loop
 */
string
reco_while(
  int& index
) {
  string content;

  if (lexiinfo[index].lexiID == keywords.id("while")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"while\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("(")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  content += reco_compare(index);

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    content += lexiinfo[index++].content + " ";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("{")) {
    content += reco_block(index);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"{\".", lexiinfo[index]);

  return content;
}

/**
 * Processes a return statement
 * @param index Current token index
 * @return String containing processed return statement
 */
string
reco_return(
  int& index
) {
  string content;

  if (lexiinfo[index].lexiID == keywords.id("return")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"return\".", lexiinfo[index]);

  if (check_boarder(index)) {
    content += " " + reco_formula(index);
  } else if (reqReturnVal) {
    error_item("[Semantic Error]", "Function need return value to return.", lexiinfo[index]);
  }

  if (lexiinfo[index].lexiID == keywords.id(";")) {
    content += lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \";\".", lexiinfo[index]);

  return content;
}

/**
 * Processes a code block
 * @param index Current token index
 * @param hasReturn Pointer to bool tracking if return found
 * @return String containing processed block
 */
string
reco_block(
  int& index,
  bool* hasReturn
) {
  ++blockLayer;
  string content;

  if (lexiinfo[index].lexiID == keywords.id("{")) {
    content += lexiinfo[index++].content + "\n";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"{\".", lexiinfo[index]);

  while (lexiinfo[index].lexiID != keywords.id("}")) {
    if (lexiinfo[index].lexiID == keywords.id("draw")) {
      if (isDrawtype(lexiinfo[index + 1].lexiID)) {
        content += repeatString("  ", blockLayer) + reco_draw(index) + "\n";
      } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords of DRAW-TYPE.", lexiinfo[index + 1]);
    } else if (lexiinfo[index].lexiID == keywords.id("for")) {
      content += repeatString("  ", blockLayer) + reco_for(index) + "\n";
    } else if (lexiinfo[index].lexiID == keywords.id("while")) {
      content += repeatString("  ", blockLayer) + reco_while(index) + "\n";
    } else if (lexiinfo[index].lexiID == keywords.id("if")) {
      content += repeatString("  ", blockLayer) + reco_if(index) + "\n";
    } else if (lexiinfo[index].lexiID == keywords.id("return")) {
      content += repeatString("  ", blockLayer) + reco_return(index) + "\n";
      if (hasReturn) *hasReturn = true;
    } else if (isType(lexiinfo[index].lexiID)) {
      content += repeatString("  ", blockLayer) + reco_define(index, blockLayer) + "\n";
    } else {
      content += repeatString("  ", blockLayer) + reco_multiformula(index) + "\n";
    }
  }

  // variinfo.show(blockLayer);
  variinfo.del(blockLayer--);

  if (lexiinfo[index].lexiID == keywords.id("}")) {
    content += repeatString("  ", blockLayer) + lexiinfo[index++].content;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"}\".", lexiinfo[index]);

  return content;
}

/**
 * Processes function parameter list
 * @param index Current token index
 * @param numParam Reference to parameter count
 * @return String containing processed parameter list
 */
string 
reco_paralist(
  int& index,
  int& numParam
) {
  string content;

  if (lexiinfo[index].lexiID == keywords.id("(")) { 
    index++;            
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  while (lexiinfo[index].lexiID != keywords.id(")")) {
    if (isType(lexiinfo[index].lexiID)) {
      if (lexiinfo[index + 1].lexiID == keywords.id("identifier")) {
        numParam++;
        string type = lexiinfo[index++].content, name = lexiinfo[index++].content;
        content += type + " " + name;
        if (!variinfo.exist(name, blockLayer + 1)) {
          variinfo.add(name, type, blockLayer + 1);
        } else error_item("[Semantic Error]", "Redefined variable.", lexiinfo[index - 1]);
      } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be IDENTIFIER.", lexiinfo[index]);
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords of TYPE.", lexiinfo[index]);
    if (lexiinfo[index].lexiID == keywords.id(",")) {
      content += ", ";
      index++;
    } else if (lexiinfo[index].lexiID != keywords.id(")")) {
      error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);
    }
  }

  if (lexiinfo[index].lexiID == keywords.id(")")) {
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \")\".", lexiinfo[index]);

  return content;
}

/**
 * Processes a function definition
 * @param index Current token index
 * @return String containing processed function
 */
string
reco_function(
  int& index
) {

  if (lexiinfo[index].lexiID == keywords.id("def")) {
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords \"def\".", lexiinfo[index]);

  string content, paraContent, returnType;          // def calculate(float num) -> float {
  string functionName = lexiinfo[index++].content;  //     ^~~~~~~~~  
  int functionPos = index - 1;
                                                    
  if (lexiinfo[index].lexiID == keywords.id("(")) { 
    int numParam = 0;
    paraContent = reco_paralist(index, numParam);
    if (!funcinfo.exist(functionName)) {
      funcinfo.add(functionName, numParam);
    } else error_item("[Semantic Error]", "Redefined function.", lexiinfo[functionPos]);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"(\".", lexiinfo[index]);

  if (lexiinfo[index].lexiID == keywords.id("->")) {
    index++;
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"->\".", lexiinfo[index]);

  if (isType(lexiinfo[index].lexiID)) {
    returnType = lexiinfo[index++].content;
    if (functionName == "main") returnType = "int";
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keywords of TYPE.", lexiinfo[index]);

  nowFuncName = functionName;
  reqReturnVal = (returnType != "void");
  content = returnType + " " + functionName + "(" + paraContent + ") ";

  bool hasReturn = false;
  if (lexiinfo[index].lexiID == keywords.id("{")) {
    content += reco_block(index, &hasReturn);
  } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be \"{\".", lexiinfo[index]);

  if (functionName != "main" && returnType != "void" && !hasReturn) {
    error_item("[Semantic Error]", "Function " + functionName + " does not have RETURN SENTENCE.", lexiinfo[index - 1]);
  }

  return content;
}

/**
 * Generates proxy C++ source file from processed content
 * @param content String containing processed source code
 * @param ouName Output filename without extension
 */
void 
generate_proxy(
  const string& content,
  string ouName
) {
  fstream outProxy(ouName + ".cpp", ios::out | ios::trunc);
  if (!outProxy.is_open()) error_info("[Compiler Error]", "Cannot create proxy file.");
  else {
    outProxy << content;
  }
}

string content = 
"#include <cmath>                                \n" 
"#include <iostream>                             \n" 
"                                                \n" 
"float Power(double n, double k) {               \n" 
"  bool neg = false;                             \n" 
"  if (k < 0) neg = true, k = -k;                \n" 
"  long long ink = k;                            \n" 
"  double ans = std::pow(n, k - (double) ink);   \n" 
"  while (ink) {                                 \n" 
"    if (ink & 1) ans *= n;                      \n" 
"    n *= n;                                     \n" 
"    ink >>= 1;                                  \n" 
"  }                                             \n" 
"  return neg ? (1.0 / ans) : ans;               \n" 
"}                                             \n\n";

/**
 * Main recognition function
 * Processes entire source file and generates output
 */
string& 
recognize(
  string ouName,
  bool cprxcode
) {
  int index = 0;
  while (lexiinfo[index].lexiID) {
    if (lexiinfo[index].lexiID == keywords.id("def")) {
      content += reco_function(index) + "\n\n";
    } else error_item("[Syntax Error]", "Incomplete syntax structure. Here should be keyword \"def\".", lexiinfo[index]);
  }
  content += "// Proxy code ends.\n";
  if (cprxcode) generate_proxy(content, ouName);
  return content;
}