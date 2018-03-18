//
//  LLVMPrimitiveTypes.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMPrimitiveTypes_h
#define LLVMPrimitiveTypes_h

#include "wisey/LLVMi1Type.hpp"
#include "wisey/LLVMi8Type.hpp"
#include "wisey/LLVMi16Type.hpp"
#include "wisey/LLVMi32Type.hpp"
#include "wisey/LLVMi64Type.hpp"

namespace wisey {
  
  /**
   * Has constants that stand for llvm primitive types
   */
  class LLVMPrimitiveTypes {
    
  public:
    
    static LLVMi1Type* I1;
    static LLVMi8Type* I8;
    static LLVMi16Type* I16;
    static LLVMi32Type* I32;
    static LLVMi64Type* I64;
    
  };
  
} /* namespace wisey */

#endif /* LLVMPrimitiveTypes_h */
