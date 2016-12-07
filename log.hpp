//
//  Log.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Log_h
#define Log_h

#include <iostream>

namespace yazyk {

typedef enum LogLevelEnum { ERRORLEVEL, INFOLEVEL, DEBUGLEVEL } LogLevel;

/**
 * Simple Logger with 3 levels of logging. 
 *
 * DEBUGLEVEL is the most detailed. ERRORLEVEL is the least detailed.
 */
class Log {
  static LogLevel sLogLevel;
  
public:
  
  static void setLogLevel(LogLevel logLevel);
  
  static void e(std::string message);
  
  static void i(std::string message);

  static void d(std::string message);
  
  static bool isDebugLevel();
};
}

#endif /* Log_h */
