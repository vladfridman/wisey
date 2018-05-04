//
//  Composer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void Composer::pushCallStack(IRGenerationContext& context, int line) {
  if (shouldSkip(context)) {
    return;
  }

  const IObjectType* objectType = context.getObjectType();
  Value* sourceFileNamePointer = objectType->getImportProfile()->getSourceFileNamePointer();
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context, line);

  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context, line);

  IVariable* currentObjectVariable = context.getScopes()
    .getVariable(Names::getCurrentObjectVariableName());
  IVariable* currentMethodVariable = context.getScopes()
    .getVariable(Names::getCurrentMethodVariableName());
  
  vector<Value*> arguments;

  Controller* callStackController = context.getController(Names::getCallStackControllerFullName(),
                                                          line);
  arguments.clear();
  arguments.push_back(callStackObject);
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);
  arguments.push_back(currentObjectVariable->generateIdentifierIR(context, line));
  arguments.push_back(currentMethodVariable->generateIdentifierIR(context, line));
  arguments.push_back(sourceFileNamePointer);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), line));
  string pushStackFunctionName =
  IMethodCall::translateObjectMethodToLLVMFunctionName(callStackController,
                                                       Names::getPushStackMethodName());
  Function* pushStackFunction = context.getModule()->getFunction(pushStackFunctionName.c_str());
  IRWriter::createCallInst(context, pushStackFunction, arguments, "", line);
}

void Composer::popCallStack(IRGenerationContext& context) {
  if (shouldSkip(context)) {
    return;
  }

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context, 0);

  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context, 0);

  vector<Value*> arguments;

  Controller* callStackController = context.getController(Names::getCallStackControllerFullName(),
                                                          0);
  arguments.clear();
  arguments.push_back(callStackObject);
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);
  string popStackFunctionName =
  IMethodCall::translateObjectMethodToLLVMFunctionName(callStackController,
                                                       Names::getPopStackMethoName());
  Function* popStackFunction = context.getModule()->getFunction(popStackFunctionName.c_str());
  IRWriter::createCallInst(context, popStackFunction, arguments, "", 0);
}

void Composer::setLineNumber(IRGenerationContext& context, int line) {
  if (shouldSkip(context)) {
    return;
  }
  
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  
  Value* threadObject = threadVariable->generateIdentifierIR(context, line);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context, line);
  
  vector<Value*> arguments;
  
  Controller* callStackController = context.getController(Names::getCallStackControllerFullName(),
                                                          line);
  arguments.clear();
  arguments.push_back(callStackObject);
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), line));
  string functionName =
  IMethodCall::translateObjectMethodToLLVMFunctionName(callStackController,
                                                       Names::getSetLineNumberMethodName());
  Function* function = context.getModule()->getFunction(functionName.c_str());
  IRWriter::createCallInst(context, function, arguments, "", line);
}

bool Composer::shouldSkip(IRGenerationContext& context) {
  const IObjectType* objectType = context.getObjectType();
  if (objectType == NULL || !objectType->getTypeName().find(Names::getThreadsPackageName())
      || !objectType->getTypeName().find(Names::getLangPackageName())
      || !objectType->getTypeName().find(Names::getIOPackageName())) {
    // avoid inifinite recursion in wisey.lang.TMainThread
    return true;
  }
  
  Value* sourceFileNamePointer = objectType->getImportProfile()->getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return true;
  }
  
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  
  if (!threadVariable || !callStackVariable) {
    return true;
  }
  
  return false;
}
