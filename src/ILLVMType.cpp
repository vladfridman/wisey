//
//  ILLVMType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "ILLVMType.hpp"
#include "IRWriter.hpp"
#include "LocalLLVMVariable.hpp"
#include "ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalLLVMVariable* ILLVMType::createLocalLLVMVariable(IRGenerationContext& context,
                                                      const ILLVMType* type,
                                                      std::string name,
                                                      int line) {
  Type* llvmType = type->getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalLLVMVariable* variable = new LocalLLVMVariable(name, type, alloc, line);
  context.getScopes().setVariable(context, variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
  
  return variable;
}
