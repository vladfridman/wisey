//
//  LLVMFunctionArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionArgument_h
#define LLVMFunctionArgument_h

#include "wisey/ILLVMType.hpp"

namespace wisey {
  
  /**
   * Contains information about a method argument including its type and name
   */
  class LLVMFunctionArgument {
    const ILLVMType* mType;
    std::string mName;
    
  public:
    
    LLVMFunctionArgument(const ILLVMType* type, std::string name) : mType(type), mName(name) { }
    
    ~LLVMFunctionArgument() { }
    
    const ILLVMType* getType() { return mType; }
    
    std::string getName() { return mName; }
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionArgument_h */
