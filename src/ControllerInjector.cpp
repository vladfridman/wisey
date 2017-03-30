//
//  ControllerInjector.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>
#include <llvm/IR/Constants.h>

#include "yazyk/Environment.hpp"
#include "yazyk/ControllerInjector.hpp"
#include "yazyk/LocalHeapVariable.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ControllerInjector::generateIR(IRGenerationContext& context) const {
  
  Controller* controller = context.getController(mControllerTypeSpecifier.getName());

  Instruction* malloc = createMalloc(context);
  initializeVTable(context, controller, malloc);
  
  LocalHeapVariable* heapVariable = new LocalHeapVariable(getVariableName(), controller, malloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

Instruction* ControllerInjector::createMalloc(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* pointerType = Type::getInt32Ty(llvmContext);
  IType* yazykType = mControllerTypeSpecifier.getType(context);
  Type* structType = yazykType->getLLVMType(llvmContext)->getPointerElementType();
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, pointerType);
  Instruction* malloc = CallInst::CreateMalloc(context.getBasicBlock(),
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               "injectvar");
  context.getBasicBlock()->getInstList().push_back(malloc);
  
  return malloc;
}

void ControllerInjector::initializeVTable(IRGenerationContext& context,
                                          Controller* controller,
                                          Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();
  GlobalVariable* vTableGlobal =
    context.getModule()->getGlobalVariable(controller->getVTableName());
  
  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vTableType = functionType->getPointerTo()->getPointerTo();
  
  vector<Interface*> interfaces = controller->getFlattenedInterfaceHierarchy();
  for (unsigned int interfaceIndex = 0; interfaceIndex < interfaces.size(); interfaceIndex++) {
    Value* vTableStart;
    if (interfaceIndex == 0) {
      vTableStart = malloc;
    } else {
      Value* vTableStartCalculation = new BitCastInst(malloc, genericPointerType, "", basicBlock);
      Value *Idx[1];
      unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
      Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
      vTableStart = GetElementPtrInst::Create(genericPointerType->getPointerElementType(),
                                              vTableStartCalculation,
                                              Idx,
                                              "",
                                              basicBlock);
    }
    
    Value* vTablePointer = new BitCastInst(vTableStart, vTableType->getPointerTo(), "", basicBlock);
    Value *Idx[3];
    Idx[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), interfaceIndex);
    Idx[2] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Value* initializerStart = GetElementPtrInst::Create(vTableGlobal->getType()->
                                                        getPointerElementType(),
                                                        vTableGlobal,
                                                        Idx,
                                                        "",
                                                        basicBlock);
    BitCastInst* bitcast = new BitCastInst(initializerStart, vTableType, "", basicBlock);
    new StoreInst(bitcast, vTablePointer, basicBlock);
  }
}

IType* ControllerInjector::getType(IRGenerationContext& context) const {
  return mControllerTypeSpecifier.getType(context);
}

void ControllerInjector::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(getVariableName());
}

string ControllerInjector::getVariableName() const {
  ostringstream stream;
  stream << "__tmp" << (long) this;
  
  return stream.str();
}


