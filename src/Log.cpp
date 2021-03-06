//
//  Log.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "ImportProfile.hpp"
#include "Log.hpp"

using namespace std;
using namespace wisey;

LogLevel Log::sLogLevel;

void Log::setLogLevel(LogLevel logLevel) {
  Log::sLogLevel = logLevel;
}

void Log::e(std::string sourceFile, int line, std::string message) {
  if (Log::sLogLevel >= ERRORLEVEL) {
    cerr << sourceFile << "(" << line << "): Error: " << message << endl;
  }
}

void Log::errorNoSourceFile(std::string message) {
  if (Log::sLogLevel >= ERRORLEVEL) {
    cerr << "Error: " << message << endl;
  }
}

void Log::i(std::string message) {
  if (Log::sLogLevel >= INFOLEVEL) {
    cout << "Info: " << message << endl;
  }
}

void Log::d(std::string message) {
  if (Log::sLogLevel >= DEBUGLEVEL) {
    cout << "Debug: " << message << endl;
  }
}
