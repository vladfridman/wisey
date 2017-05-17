//
//  Log.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/7/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Log_h
#define Log_h

#include <iostream>

namespace wisey {

typedef enum LogLevelEnum { ERRORLEVEL, INFOLEVEL, DEBUGLEVEL } LogLevel;

/**
 * Simple Logger with 3 levels of logging. 
 *
 * DEBUGLEVEL is the most detailed.
 * INFOLEVEL is in between DEBUGLEVEL and ERRORLEVEL
 * ERRORLEVEL is the least detailed.
 */
class Log {
  static LogLevel sLogLevel;
  
public:
  
  /**
   * Set log level for the entire run of the program or until it is reset to a different level
   */
  static void setLogLevel(LogLevel logLevel);
  
  /**
   * Log an error message
   */
  static void e(std::string message);
  
  /**
   * Log an information message
   */
  static void i(std::string message);

  /**
   * Log a debug message
   */
  static void d(std::string message);
};
  
} /* namespace wisey */

#endif /* Log_h */
