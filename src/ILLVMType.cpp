//
//  ILLVMType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ILLVMType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalLLVMVariable.hpp"
#include "wisey/ParameterLLVMVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LocalLLVMVariable* ILLVMType::createLocalVariable(IRGenerationContext& context,
                                                  const ILLVMType* type,
                                                  std::string name) {
  Type* llvmType = type->getLLVMType(context);
  AllocaInst* alloc = IRWriter::newAllocaInst(context, llvmType, "");
  
  LocalLLVMVariable* variable = new LocalLLVMVariable(name, type, alloc);
  context.getScopes().setVariable(variable);
  
  Value* value = ConstantInt::get(llvmType, 0);
  IRWriter::newStoreInst(context, value, alloc);
  
  return variable;
}

ParameterLLVMVariable* ILLVMType::createParameterVariable(IRGenerationContext &context,
                                                          const ILLVMType* type,
                                                          Value* value,
                                                          string name) {
  ParameterLLVMVariable* variable = new ParameterLLVMVariable(name, type, value);
  context.getScopes().setVariable(variable);

  return variable;
}
