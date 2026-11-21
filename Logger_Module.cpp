#include "ProjectHeaders.h"
#include <iostream>

Logger::Logger(string fileName) {
    file.open(fileName, ios::app);     // app = add at the end, old data is kept
    if (!file)
        throw string("Cannot open the log file");
    writeLine("========== NEW SESSION ==========");
}

Logger::~Logger() {
    writeLine("========== SESSION ENDED ==========");
    file.close();
}

void Logger::write(int time, string type, string message) {
    writeLine("[" + to_string(time) + " sec] [" + type + "] " + message);
}

void Logger::writeLine(string text) {
    file << text << endl;
    cout << text << endl;
    lines.push_back(text);
}

vector<string> Logger::getLines() {
    return lines;
}
