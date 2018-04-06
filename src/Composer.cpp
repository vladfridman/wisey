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
  LLVMContext& llvmContext = context.getLLVMContext();
  const IObjectType* objectType = context.getObjectType();
  if (objectType == NULL || !objectType->getTypeName().find(Names::getLangPackageName())) {
    // avoid inifinite recursion in wisey.lang.TMainThread
    return;
  }

  Value* sourceFileNamePointer = objectType->getImportProfile()->getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return;
  }

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);

  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context);

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
  arguments.push_back(currentObjectVariable->generateIdentifierIR(context));
  arguments.push_back(currentMethodVariable->generateIdentifierIR(context));
  arguments.push_back(sourceFileNamePointer);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), line));
  string pushStackFunctionName =
  IMethodCall::translateObjectMethodToLLVMFunctionName(callStackController,
                                                       Names::getThreadPushStack());
  Function* pushStackFunction = context.getModule()->getFunction(pushStackFunctionName.c_str());
  IRWriter::createCallInst(context, pushStackFunction, arguments, "");
}

void Composer::popCallStack(IRGenerationContext& context) {
  const IObjectType* objectType = context.getObjectType();
  if (objectType == NULL || !objectType->getTypeName().find(Names::getLangPackageName())) {
    // avoid inifinite recursion in wisey.lang.TMainThread
    return;
  }

  Value* sourceFileNamePointer = objectType->getImportProfile()->getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return;
  }

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);

  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackObject = callStackVariable->generateIdentifierIR(context);

  vector<Value*> arguments;

  Controller* callStackController = context.getController(Names::getCallStackControllerFullName(),
                                                          0);
  arguments.clear();
  arguments.push_back(callStackObject);
  arguments.push_back(threadObject);
  arguments.push_back(callStackObject);
  string popStackFunctionName =
  IMethodCall::translateObjectMethodToLLVMFunctionName(callStackController,
                                                       Names::getThreadPopStack());
  Function* popStackFunction = context.getModule()->getFunction(popStackFunctionName.c_str());
  IRWriter::createCallInst(context, popStackFunction, arguments, "");
}
