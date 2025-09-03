#include "header.h"

using namespace std;

void echo(const command& cmd) {
    if (cmd.args.empty()) {
        cout << endl;  // no arguments, just print newline
        return;
    }

    for (size_t i = 0; i < cmd.args.size(); ++i) {
        cout << cmd.args[i];
        if (i != cmd.args.size() - 1) cout << " ";  // space between args
    }
    cout << endl;
}