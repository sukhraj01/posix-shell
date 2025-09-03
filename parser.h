#include <vector>
#include <sstream>
#include <string>

using namespace std;

void cmdParser(const string& singleCmd, command& cmd);
cmdlist parser(const string& cmdLine);
void print_info(const cmdlist &info);