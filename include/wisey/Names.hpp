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
     * Returns MMainThreadWorker class name
     */
    static std::string getMainThreadWorkerShortName();
    
    /**
     * Returns name of the CFile controller
     */
    static std::string getFileControllerFullName();
    
    /**
     * Get the name of the method returning a pointer to the __sFile struct from CFile controller
     */
    static std::string getFileStructMethodName();
    
    /**
     * Returns name of the model representing the null pointer exception
     */
    static std::string getNPEModelName();
    
    /**
     * Returns name of the model representing the reference count exception
     */
    static std::string getReferenceCountExceptionName();
    
    /**
     * Returns name of the model representing the cast exception
     */
    static std::string getCastExceptionName();
    
    /**
     * Returns name of the model representing the array index out of bounds exception
     */
    static std::string getArrayIndexOutOfBoundsModelName();
    
    /**
     * Returns name of the IProgram interface that main function calls
     */
    static std::string getIProgramName();
    
    /**
     * Returns name of the lang package
     */
    static std::string getLangPackageName();
    
    /**
     * Returns name of the threads package
     */
    static std::string getThreadsPackageName();

    /**
     * Get name of the stderr pointer
     */
    static std::string getStdErrName();
    
    /**
     * Retuns IThread inteface name
     */
    static std::string getThreadInterfaceName();
    
    /**
     * Retuns IThread inteface full name
     */
    static std::string getThreadInterfaceFullName();
    
    /**
     * Get the name of the CCallStack controller
     */
    static std::string getCallStackControllerName();
    
    /**
     * Get the full name of the CCallStack controller
     */
    static std::string getCallStackControllerFullName();
    
    /**
     * Get the method for pushing the call stack
     */
    static std::string getPushStackMethodName();
    
    /**
     * Get the method for popping the call stack
     */
    static std::string getPopStackMethoName();
    
    /**
     * Get the method for setting the current line number
     */
    static std::string getSetLineNumberMethodName();

    /**
     * Variable name containing current object name
     */
    static std::string getCurrentObjectVariableName();
    
    /**
     * Variable name containing current method name
     */
    static std::string getCurrentMethodVariableName();

  };
  
} /* namespace wisey */

#endif /* Names_h */

