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

  IVariable* currentObjectVariable = context.getScopes()
    .getVariable(Names::getCurrentObjectVariableName());
  IVariable* currentMethodVariable = context.getScopes()
    .getVariable(Names::getCurrentMethodVariableName());

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
