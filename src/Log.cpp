//
//  Log.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include "yazyk/log.hpp"

using namespace std;
using namespace yazyk;

LogLevel Log::sLogLevel;

void Log::setLogLevel(LogLevel logLevel) {
  Log::sLogLevel = logLevel;
}

void Log::e(std::string message) {
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
