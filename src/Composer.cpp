//
//  Composer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Composer.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void Composer::checkNullAndThrowNPE(IRGenerationContext& context,
                                    Value* value,
                                    Value* threadObject,
                                    const IObjectType* objectType,
                                    int line) {
  pushCallStack(context, threadObject, objectType, line);
  
  PointerType* int8PointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8PointerType);
  
  Function* function = context.getModule()->getFunction(Names::getNPECheckFunctionName());
  vector<Value*> arguments;
  arguments.push_back(bitcast);

  IRWriter::createInvokeInst(context, function, arguments, "");

  popCallStack(context, threadObject, objectType);
}

void Composer::pushCallStack(IRGenerationContext& context,
                             Value* threadObject,
                             const IObjectType* objectType,
                             int line) {
  if (!Names::getThreadStackNodeName().compare(objectType->getName())) {
    // avoid inifinite recursion in wisey.lang.CThread.pushStack()
    return;
  }

  Value* sourceFileNamePointer = context.getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return;
  }

  IVariable* currentObjectVariable = context.getScopes().getVariable("currentObject");
  IVariable* currentMethodVariable = context.getScopes().getVariable("currentMethod");

  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  arguments.push_back(currentObjectVariable->getValue());
  arguments.push_back(currentMethodVariable->getValue());
  arguments.push_back(sourceFileNamePointer);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), line));
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  string pushStackFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(threadController,
                                                         Names::getThreadPushStack());
  Function* pushStackFunction = context.getModule()->getFunction(pushStackFunctionName.c_str());
  IRWriter::createCallInst(context, pushStackFunction, arguments, "");
}

Value* Composer::getObjectNamePointer(IRGenerationContext& context,
                                      const IObjectType* objectType,
                                      Value* objectValue) {
  if (IType::isConcreteObjectType(objectType)) {
    return IObjectType::getObjectNamePointer(objectType, context);
  }
  
  const Interface* interface = (const Interface*) objectType;
  Value* originalObjectVTable = interface->getOriginalObjectVTable(context, objectValue);
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer = IRWriter::newBitCastInst(context, originalObjectVTable, pointerType);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  GetElementPtrInst* typeArrayPointerI8 = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* typeArrayI8 = IRWriter::newLoadInst(context, typeArrayPointerI8, "typeArrayI8");
  BitCastInst* arrayOfStrings =
  IRWriter::newBitCastInst(context, typeArrayI8, int8Type->getPointerTo()->getPointerTo());
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 0);
  Value* stringPointerPointer = IRWriter::createGetElementPtrInst(context, arrayOfStrings, index);
  
  LoadInst* stringPointer = IRWriter::newLoadInst(context, stringPointerPointer, "stringPointer");
  
  return stringPointer;
}

void Composer::popCallStack(IRGenerationContext& context,
                            Value* threadObject,
                            const IObjectType* objectType) {
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  if (!Names::getThreadStackNodeName().compare(objectType->getName())) {
    // avoid inifinite recursion in wisey.lang.CThread.popStack()
    return;
  }

  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  string popStackFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(threadController,
                                                         Names::getThreadPopStack());
  Function* popStackFunction = context.getModule()->getFunction(popStackFunctionName.c_str());
  IRWriter::createCallInst(context, popStackFunction, arguments, "");
}
