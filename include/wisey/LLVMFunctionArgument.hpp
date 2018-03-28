//
//  LLVMFunctionArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionArgument_h
#define LLVMFunctionArgument_h

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Contains information about a method argument including its type and name
   */
  class LLVMFunctionArgument {
    const IType* mType;
    std::string mName;
    
  public:
    
    LLVMFunctionArgument(const IType* type, std::string name) : mType(type), mName(name) {
      assert(type->isNative());
    }
    
    ~LLVMFunctionArgument() { }
    
    const IType* getType() const { return mType; }
    
    std::string getName() const { return mName; }
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionArgument_h */
