//
//  ThreadInfrastructure.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadInfrastructure_h
#define ThreadInfrastructure_h

#include <llvm/IR/Instructions.h>

#include "wisey/NativeType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Used to define functions and structures related to threads
   */
  class ThreadInfrastructure {
    
  public:
    
    /**
     * Native function that creates a thread object and starts its execution
     */
    static llvm::Function* getThreadCreateFunction(IRGenerationContext& context);

    /**
     * Gets or creates a function that calls Thread's run method
     */
    static llvm::Function* getRunBridgeFunction(IRGenerationContext& context, const Thread* thread);

    /**
     * Returns NativeType object with the llvm type that represents the native thread object
     */
    static NativeType* createNativeThreadType(IRGenerationContext& context);
    
    /**
     * Returns NativeType object with the llvm type that represents the native thread attributes
     */
    static NativeType* createNativeThreadAttributesType(IRGenerationContext& context);
    
  private:
    
    static llvm::Function* defineThreadCreateFunction(IRGenerationContext& context);
    
    static llvm::Function* defineRunBridgeFunction(IRGenerationContext& context,
                                                   const Thread* thread);
    
    static void composeRunBridgeFunction(IRGenerationContext& context,
                                         llvm::Function* function,
                                         const IObjectType* thread);

    static llvm::StructType* getNativeThreadStruct(IRGenerationContext& context);
    
    static llvm::StructType* getNativeThreadHandlerStruct(IRGenerationContext& context);
    
    static llvm::StructType* getNativeThreadAttributesStruct(IRGenerationContext& context);

    static std::string getThreadCreateFunctionName();

    static std::string getNativeThreadStructName();
    
    static std::string getNativeThreadHandlerName();
    
    static std::string getNativeThreadAttributesName();
    
    static std::string getRunBridgeFunctionName(const Thread* thread);

  };
}

#endif /* ThreadInfrastructure_h */
