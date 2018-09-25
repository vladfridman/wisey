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
     * Returns name of the CTextStreamWriter controller
     */
    static std::string getTextStreamControllerFullName();
    
    /**
     * Get the name of the method returning a pointer to stream from CTextStreamWriter
     */
    static std::string getStreamMethodName();
    
    /**
     * Retuns IException inteface name
     */
    static std::string getExceptionInterfaceName();

    /**
     * Retuns IException inteface full name
     */
    static std::string getExceptionInterfaceFullName();

    /**
     * Returns name of the model representing the null pointer exception
     */
    static std::string getNPEModelName();
    
    /**
     * Returns name of the model representing the reference count exception
     */
    static std::string getReferenceCountExceptionName();
    
    /**
     * Returns name of the model representing the memory pool non empty exception
     */
    static std::string getMemoryPoolNonEmptyExceptionName();

    /**
     * Returns name of the model representing the cast exception
     */
    static std::string getCastExceptionName();
    
    /**
     * Returns name of the model representing interface not bound exception
     */
    static std::string getInterfaceNotBoundExceptionName();
    
    /**
     * Returns name of the model representing the array index out of bounds exception
     */
    static std::string getArrayIndexOutOfBoundsModelName();
    
    /**
     * Returns name of the lang package
     */
    static std::string getLangPackageName();
    
    /**
     * Returns name of the threads package
     */
    static std::string getThreadsPackageName();
    
    /**
     * Returns name of the io package
     */
    static std::string getIOPackageName();
    
    /**
     * Returns name of the data package
     */
    static std::string getDataPackageName();

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
     * Returns getContextManager method name of the IThread
     */
    static std::string getGetContextManagerMethodName();
    
    /**
     * Get the name of the CCallStack controller
     */
    static std::string getCallStackControllerName();
    
    /**
     * Get the full name of the CCallStack controller
     */
    static std::string getCallStackControllerFullName();
    
    /**
     * Get the name of the CContextManager controller
     */
    static std::string getContextManagerName();
    
    /**
     * Get the full name of the CContextManager controller
     */
    static std::string getContextManagerFullName();
    
    /**
     * Get the name of the CMemoryPool controller
     */
    static std::string getCMemoryPoolName();
    
    /**
     * Get the full name of the CApr controller
     */
    static std::string getCMemoryPoolFullName();

    /**
     * Returns name of the function that allocates memory on a memory pool
     */
    static std::string getMemoryPoolAllocateFunctionName();

    /**
     * Returns name of the function that clears a memory pool
     */
    static std::string getMemoryPoolClearFunctionName();

    /**
     * Returns name of the getInstance method of CContextManager
     */
    static std::string getGetInstanceMethodName();
    
    /**
     * Returns name of the setInstance method of CContextManager
     */
    static std::string getSetInstanceMethodName();
    
    /**
     * Get the method that throws stack overflow exception
     */
    static std::string getThrowStackOverflowMethodName();
    
    /**
     * Returns the name of the constant containing call stack max size
     */
    static std::string getCallStackSizeConstantName();

  };
  
} /* namespace wisey */

#endif /* Names_h */

