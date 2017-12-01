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
  const IObjectType* objectType = context.getObjectType();
  if (objectType == NULL || !objectType->getName().find(Names::getLangPackageName())) {
    // avoid inifinite recursion in wisey.lang.CThread
    return;
  }

  Value* sourceFileNamePointer = objectType->getImportProfile()->getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return;
  }

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);
  
  IVariable* currentObjectVariable = context.getScopes()
    .getVariable(Names::getCurrentObjectVariableName());
  IVariable* currentMethodVariable = context.getScopes()
    .getVariable(Names::getCurrentMethodVariableName());
  
  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  arguments.push_back(currentObjectVariable->generateIdentifierIR(context));
  arguments.push_back(currentMethodVariable->generateIdentifierIR(context));
  arguments.push_back(sourceFileNamePointer);
  arguments.push_back(ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), line));
  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  string pushStackFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(threadController,
                                                         Names::getThreadPushStack());
  Function* pushStackFunction = context.getModule()->getFunction(pushStackFunctionName.c_str());
  IRWriter::createCallInst(context, pushStackFunction, arguments, "");
}

void Composer::popCallStack(IRGenerationContext& context) {
  const IObjectType* objectType = context.getObjectType();
  if (objectType == NULL || !objectType->getName().find(Names::getLangPackageName())) {
    // avoid inifinite recursion in wisey.lang.CThread
    return;
  }

  Value* sourceFileNamePointer = objectType->getImportProfile()->getSourceFileNamePointer();
  if (sourceFileNamePointer == NULL) {
    return;
  }

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context);

  Controller* threadController = context.getController(Names::getThreadControllerFullName());
  vector<Value*> arguments;
  arguments.push_back(threadObject);
  arguments.push_back(threadObject);
  string popStackFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(threadController,
                                                         Names::getThreadPopStack());
  Function* popStackFunction = context.getModule()->getFunction(popStackFunctionName.c_str());
  IRWriter::createCallInst(context, popStackFunction, arguments, "");
}
