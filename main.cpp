#include "format.hpp"

Keywords keywords;
LexiInfo lexiinfo;
VariInfo variinfo;
FuncInfo funcinfo;

/**
 * Displays help information and usage instructions
 * Prints detailed command-line options and examples
 */
void 
showHelp() {
  printf("pfc (Pixel Format Compiler) - A tool to compile image description code into actual images.                       \n");
  printf("                                                                                                                 \n");
  printf("\033[33mUsage:\033[0m                                                                                            \n");
  printf("  pfc [options] file...                                                                                          \n");
  printf("                                                                                                                 \n");
  printf("\033[33mOptions:\033[0m                                                                                          \n");
  printf("  -h                            Show this help message and exit.                                                 \n");
  printf("  -o <filename>                 Set the output image file name to <filename>.                                    \n");
  printf("  -d                            Generate intermediate code.                                                      \n");
  printf("  -c                            Generate proxy code (C++).                                                       \n");
  printf("  -l                            Generate lexical analysis results.                                               \n");
  printf("  -s <width> <height>           Set the image height and width to <width> and <height>.                          \n");
  printf("                                                                                                                 \n");
  printf("\033[33mExamples:\033[0m                                                                                         \n");
  printf("  pfc -h                        Display help information.                                                        \n");
  printf("  pfc -o output input.pf        Compile \"input.pf\" and set the output file name to \"output\".                 \n");
  printf("  pfc -d input.pf               Generate intermediate code from \"input.pf\".                                    \n");
  printf("  pfc -c input.pf               Generate proxy code in C++ from \"input.pf\".                                    \n");
  printf("  pfc -l input.pf               Show the results of lexical analysis for \"input.pf\".                           \n");
  printf("  pfc -s 800 600 input.pf       Set the image dimensions to 800x600 (width x height) and compile \"input.pf\".   \n");
  printf("                                                                                                                 \n");
  printf("\033[33mDescription:\033[0m                                                                                      \n");
  printf("  pfc is a powerful compiler that reads image description code and generates images.                             \n");
  printf("  It supports multiple options for intermediate code generation, proxy code generation,                          \n");
  printf("  and lexical analysis. Use the appropriate options to customize the output and behavior.                        \n");
  printf("                                                                                                                 \n");
  printf("For more information, visit the documentation or contact support.                                                \n");
  exit(0);
}

/**
 * Generates a random filename for temporary proxy files
 * @return String containing randomly generated filename
 */
string
random_filename() {
  srand(time(NULL) + 114514);
  ostringstream index; index << setw(5) << setfill('0') << to_string(rand() % 100000);
  return "proxy_" + index.str();
}

/**
 * Executes generated proxy code and processes drawing commands
 * @param content Proxy code content to execute
 * @param ouName Output filename
 * @param drawCMD Drawing command to execute
 * @param drawcode Whether to save drawing commands to file
 * @param tempDir Directory for temporary files
 */
void 
execute_proxy(
  const string& content,
  const string& ouName,
  string drawCMD,
  bool drawcode,
  string tempDir = "/tmp/"
) {
  int failTime = 0;
  string proxyName;
  fstream proxy;
  do {
    proxyName = tempDir + random_filename();
    proxy = fstream(proxyName + ".cpp", ios::out | ios::trunc);
  } while (!proxy.is_open() && ++failTime < 5);
  if (failTime >= 5) error_info("[Compiler Error]", "Cannot create temporary proxy file.");
  else {
    proxy << content;
    system(("g++ " + proxyName + ".cpp -o " + proxyName).c_str());
    if (drawcode) {
      system((proxyName + " > " + ouName + ".draw").c_str());
      system((drawCMD + " < " + ouName + ".draw").c_str());
    } else system((proxyName + " | " + drawCMD).c_str());
    system(("rm -rf " + proxyName + ".cpp " + proxyName).c_str());
  }
}

/**
 * Constructs drawing command string
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param ouName Output filename
 * @return Formatted drawing command string
 */
string
drawCMD(
  int width,
  int height,
  string ouName
) {
  return "pfc-draw " + to_string(width) + " " + to_string(height) + " " + ouName;
}

string inName;
string ouName = "a.out";

int width = 200, height = 200;

bool drawcode;   // Generate drawing commands file
bool cprxcode;   // Generate proxy code
bool lexicode;   // Generate lexical analysis output

int 
main(
  int argc, 
  char* argv[]
) {

  int index = 1;
  while (index < argc) {
    if (argv[index][0] == '-') {
      for (int i = 1; i < strlen(argv[index]); i++) {
        bool outTag = false;
        switch (argv[index][i]) {
          case 'h': // -h
            showHelp();
            break;
          case 'd': // -d
            drawcode = true;
            break;
          case 'c': // -c
            cprxcode = true;
            break;
          case 'l': // -l
            lexicode = true;
            break;
          case 'o': // -o <filename>
            if (index + 1 < argc) {
              ouName = string(argv[++index]);
              outTag = true;
            } else error_info("[Compiler Error]", "No filename after -o option.");
            break;
          case 's': // -s <width> <height>
            if (index + 2 < argc) {
              width = abs(stoi(argv[++index]));
              height = abs(stoi(argv[++index]));
              outTag = true;
            } else error_info("[Compiler Error]", "Not complete width and height after -s option.");
            break;
          default:
            error_info("[Compiler Error]", "Unknown option -" + string(1, argv[index][i]) + ".");
            break;
        }
        if (outTag) break;
      }
    } else {
      inName = argv[index];
    }
    index++;
  }

  if (inName.empty()) error_info("[Compiler Error]", "Input filename empty.");
    
  error_name(inName);
  lexicalize(inName, ouName, lexicode);
  string& content = recognize(ouName, cprxcode);
  execute_proxy(content, ouName, drawCMD(width, height, ouName), drawcode);
}