//
//  ILLVMType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ILLVMType_h
#define ILLVMType_h

#include "wisey/IType.hpp"

namespace wisey {
  
  class LLVMPointerType;
  class LocalLLVMVariable;
  
  /**
   * Represents an LLVM native type such as i8, i32, or llvm struct or llvm array
   */
  class ILLVMType : public IType {
  
  public:
    
    /**
     * Returns pointer type to this type. Only works for llvm types.
     */
    virtual const LLVMPointerType* getPointerType() const = 0;
    
    /**
     * Creates local llvm variable
     */
    static LocalLLVMVariable* createLocalVariable(IRGenerationContext& context,
                                                  const ILLVMType* type,
                                                  std::string name);

  };
  
} /* namespace wisey */

#endif /* ILLVMType_h */
