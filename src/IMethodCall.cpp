//
//  IMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/IMethodCall.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string IMethodCall::translateObjectMethodToLLVMFunctionName(const IObjectType* object,
                                                            string methodName) {
  if (object == NULL) {
    return methodName;
  }
  return object->getName() + "." + methodName;
}

string IMethodCall::getMethodNameConstantName(string methodName) {
  return "methodname." + methodName;
}

void IMethodCall::pushCallStack(IRGenerationContext& context,
                                const IObjectType* object,
                                string methodName,
                                Value* expressionValue,
                                Value* threadObject) {
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  if (!Names::getThreadStackNodeName().compare(object->getName())) {
    // avoid inifinite recursion in wisey.lang.CThread.pushStack()
    return;
  }
  
  vector<Value*> arguments;
  
  GlobalVariable* emptyStringGlobal =
  context.getModule()->getNamedGlobal(Names::getEmptyStringName());
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = emptyStringGlobal->getType()->getPointerElementType();
  Value* emptyStringPointer = ConstantExpr::getGetElementPtr(elementType, emptyStringGlobal, Idx);
  arguments.clear();
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  arguments.push_back(emptyStringPointer);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0));
  string pushStackFunctionName =
    translateObjectMethodToLLVMFunctionName(threadController, Names::getThreadPushStack());
  Function* pushStackFunction = context.getModule()->getFunction(pushStackFunctionName.c_str());
  IRWriter::createCallInst(context, pushStackFunction, arguments, "");

  arguments.clear();
  Value* objectName = getObjectNamePointer(context, object, expressionValue);
  Constant* functionName = getMethodNameConstantPointer(context, methodName);
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  arguments.push_back(objectName);
  arguments.push_back(functionName);
  string setObjectAndMethodFunctionName =
  translateObjectMethodToLLVMFunctionName(threadController, Names::getThreadSetObjectAndMethod());
  Function* setObjectAndMethodFunction = context.getModule()->
  getFunction(setObjectAndMethodFunctionName.c_str());
  IRWriter::createCallInst(context, setObjectAndMethodFunction, arguments, "");
}

void IMethodCall::popCallStack(IRGenerationContext& context,
                               const IObjectType* object,
                               Value* threadObject) {
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  if (!Names::getThreadStackNodeName().compare(object->getName())) {
    // avoid inifinite recursion in wisey.lang.CThread.popStack()
    return;
  }

  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  string popStackFunctionName =
    translateObjectMethodToLLVMFunctionName(threadController, Names::getThreadPopStack());
  Function* popStackFunction = context.getModule()->getFunction(popStackFunctionName.c_str());
  IRWriter::createCallInst(context, popStackFunction, arguments, "");
}

Value* IMethodCall::getObjectNamePointer(IRGenerationContext& context,
                                         const IObjectType* object,
                                         Value* expressionValue) {
  if (IType::isConcreteObjectType(object)) {
    return IObjectType::getObjectNamePointer(object, context);
  }
  
  const Interface* interface = (const Interface*) object;
  Value* originalObject = interface->getOriginalObject(context, expressionValue);
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

Constant* IMethodCall::getMethodNameConstantPointer(IRGenerationContext& context,
                                                    string methodName) {
  string constantName = getMethodNameConstantName(methodName);
  GlobalVariable* constant = context.getModule()->getNamedGlobal(constantName);
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = constant->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, constant, Idx);
}
