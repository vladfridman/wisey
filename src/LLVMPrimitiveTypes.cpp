//
//  LLVMPrimitiveTypes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace wisey;

LLVMi1Type* LLVMPrimitiveTypes::I1 = new LLVMi1Type();
LLVMi8Type* LLVMPrimitiveTypes::I8 = new LLVMi8Type();
LLVMi16Type* LLVMPrimitiveTypes::I16 = new LLVMi16Type();
LLVMi32Type* LLVMPrimitiveTypes::I32 = new LLVMi32Type();
LLVMi64Type* LLVMPrimitiveTypes::I64 = new LLVMi64Type();
