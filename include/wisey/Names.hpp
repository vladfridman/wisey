//
//  Names.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Names_h
#define Names_h

#include <string>

namespace wisey {

/**
 * Contains common names of classes and methods used through out the wisey compiler
 */
class Names {
  
public:
  
  /**
   * Returns the name of the function that checks whether the given parameter is null
   * and throws an null pointer exception
   */
  static std::string getNPECheckFunctionName();
  
  /**
   * Returns name of the model representing the null pointer exception
   */
  static std::string getNPEModelName();

  /**
   * Returns full name of the model representing the null pointer exception
   */
  static std::string getNPEModelFullName();
  
  /**
   * Returns name of the IProgram interface that main function calls
   */
  static std::string getIProgramName();
  
  /**
   * Returns name of the lang package
   */
  static std::string getLangPackageName();
  
  /**
   * Global function name that frees memory pointed to by owner pointers
   */
  static std::string getFreeIfNotNullFunctionName();
  
  /**
   * Get name of the stderr pointer
   */
  static std::string getStdErrName();
};

} /* namespace wisey */

#endif /* Names_h */
