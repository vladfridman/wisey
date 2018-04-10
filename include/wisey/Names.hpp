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
     * Main thread short name
     */
    static std::string getMainThreadShortName();
    
    /**
     * Retuns IThread inteface name
     */
    static std::string getThreadInterfaceName();
    
    /**
     * Retuns IThread inteface full name
     */
    static std::string getThreadInterfaceFullName();
    
    /**
     * Main thread full name
     */
    static std::string getMainThreadFullName();
    
    /**
     * Model representing program result
     */
    static std::string getProgramResultShortName();
    
    /**
     * Get the method for setting call stack's current object and method
     */
    static std::string getThreadSetObjectAndMethod();
    
    /**
     * Get the name of the CCallStack controller
     */
    static std::string getCallStackControllerName();
    
    /**
     * Get the full name of the CCallStack controller
     */
    static std::string getCallStackControllerFullName();
    
    /**
     * Get method name for getting the call stack object from the thread
     */
    static std::string getCallStackMethodName();
    
    /**
     * Get the method for pushing the call stack
     */
    static std::string getThreadPushStack();
    
    /**
     * Get the method for popping the call stack
     */
    static std::string getThreadPopStack();
    
    /**
     * Variable name containing current object name
     */
    static std::string getCurrentObjectVariableName();
    
    /**
     * Variable name containing current method name
     */
    static std::string getCurrentMethodVariableName();
    
    /**
     * llvm function that checks that the current object and the current thread are the same thing
     */
    static std::string getCheckConcealedMethodCallFunctionName();
    
    /**
     * llvm function that checks that the current object and the current thread are different
     */
    static std::string getCheckRevealedMethodCallFunctionName();

  };
  
} /* namespace wisey */

#endif /* Names_h */

