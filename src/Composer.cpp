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

using namespace std;
using namespace llvm;
using namespace wisey;

void Composer::checkNullAndThrowNPE(IRGenerationContext& context, Value* value) {
  PointerType* int8PointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8PointerType);
  
  Function* function = context.getModule()->getFunction(Names::getNPECheckFunctionName());
  vector<Value*> arguments;
  arguments.push_back(bitcast);

  IRWriter::createInvokeInst(context, function, arguments, "");
}

void Composer::pushCallStack(IRGenerationContext& context, Value* threadObject, int line) {
  Value* sourceFileNamePointer = context.getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return;
  }
  
  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  arguments.push_back(sourceFileNamePointer);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), line));
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  string pushStackFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(threadController,
                                                         Names::getThreadPushStack());
  Function* pushStackFunction = context.getModule()->getFunction(pushStackFunctionName.c_str());
  IRWriter::createCallInst(context, pushStackFunction, arguments, "");
}

void Composer::setNextOnCallStack(IRGenerationContext& context,
                                  Value* threadObject,
                                  const IObjectType* objectType,
                                  Value* objectValue,
                                  string methodName) {
  vector<Value*> arguments;
  Value* objectName = getObjectNamePointer(context, objectType, objectValue);
  Constant* functionName = getMethodNameConstantPointer(context, methodName);
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  arguments.push_back(objectName);
  arguments.push_back(functionName);
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  string setObjectAndMethodFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(threadController,
                                                         Names::getThreadSetObjectAndMethod());
  Function* setObjectAndMethodFunction = context.getModule()->
    getFunction(setObjectAndMethodFunctionName.c_str());
  IRWriter::createCallInst(context, setObjectAndMethodFunction, arguments, "");
}

Value* Composer::getObjectNamePointer(IRGenerationContext& context,
                                      const IObjectType* objectType,
                                      Value* objectValue) {
  if (IType::isConcreteObjectType(objectType)) {
    return IObjectType::getObjectNamePointer(objectType, context);
  }
  
  const Interface* interface = (const Interface*) objectType;
  Value* originalObject = interface->getOriginalObject(context, objectValue);
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer = IRWriter::newBitCastInst(context, originalObject, pointerType);
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

Constant* Composer::getMethodNameConstantPointer(IRGenerationContext& context, string methodName) {
  string constantName = IMethodCall::getMethodNameConstantName(methodName);
  GlobalVariable* constant = context.getModule()->getNamedGlobal(constantName);
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = constant->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, constant, Idx);
}
