//
//  CastObjectFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef CastObjectFunction_h
#define CastObjectFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * Casts a given object to a given type or throws a cast exception
   *
   * Pointer to object is passed in the form of i8* pointer. The object can be either an interface,
   * or a concrete object. The type to cast to is passed in the form of i8* pointer to a global
   * string constant containing the type name.
   *
   * The function returns an i8* pointer that is then bitcast to the needed type
   */
  class CastObjectFunction {
    
  public:
    
    /**
     * Returns the castObject function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls castObject function
     */
    static llvm::Value* call(IRGenerationContext& context,
                             llvm::Value* fromValue,
                             const IObjectType* toObjectType,
                             int line);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* CastObjectFunction_h */
