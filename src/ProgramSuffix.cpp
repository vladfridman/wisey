//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForArrayFunction.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/AdjustReferenceCounterForImmutableArrayFunction.hpp"
#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/CastObjectFunction.hpp"
#include "wisey/CheckArrayNotReferencedFunction.hpp"
#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/DestroyNativeObjectFunction.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/IsModelFunction.hpp"
#include "wisey/ModelTypeSpecifierFull.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/StaticMethodCall.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void ProgramSuffix::generateIR(IRGenerationContext& context) const {
  defineEssentialFunctions(context);
  maybeGenerateMain(context);
}

void ProgramSuffix::defineEssentialFunctions(IRGenerationContext& context) const {
  AdjustReferenceCounterForArrayFunction::get(context);
  AdjustReferenceCounterForConcreteObjectSafelyFunction::get(context);
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(context);
  AdjustReferenceCounterForImmutableArrayFunction::get(context);
  AdjustReferenceCountFunction::get(context);
  CastObjectFunction::get(context);
  CheckArrayNotReferencedFunction::get(context);
  CheckArrayIndexFunction::get(context);
  DestroyNativeObjectFunction::get(context);
  DestroyOwnerArrayFunction::get(context);
  DestroyObjectOwnerFunction::get(context);
  DestroyPrimitiveArrayFunction::get(context);
  DestroyReferenceArrayFunction::get(context);
  GetOriginalObjectFunction::get(context);
  InstanceOfFunction::get(context);
  IsModelFunction::get(context);
  ThrowReferenceCountExceptionFunction::get(context);
}

void ProgramSuffix::maybeGenerateMain(IRGenerationContext& context) const {
  PackageType* langPackageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* langPackageExpression = new FakeExpression(NULL, langPackageType);
  InterfaceTypeSpecifier* programInterfaceSpecifier =
  new InterfaceTypeSpecifier(langPackageExpression, Names::getIProgramName(), 0);
  Interface* programInterface = (Interface*) programInterfaceSpecifier->getType(context);
  if (!context.hasBoundController(programInterface)) {
    return;
  }
  
  generateMain(context, programInterface);
}

void ProgramSuffix::generateMain(IRGenerationContext& context, Interface* programInterface) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  FunctionType* mainFunctionType =
  FunctionType::get(Type::getInt32Ty(llvmContext), false);
  Function* mainFunction = Function::Create(mainFunctionType,
                                            GlobalValue::ExternalLinkage,
                                            "main",
                                            context.getModule());
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", mainFunction);
  context.setBasicBlock(entryBlock);
  context.getScopes().pushScope();
  context.getScopes().setReturnType(PrimitiveTypes::INT_TYPE);

  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  Value* threadNullValue = ConstantPointerNull::get(threadInterface->getLLVMType(context));
  IReferenceVariable* threadVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::THREAD, threadInterface, threadNullValue);
  context.getScopes().setVariable(threadVariable);

  Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);
  Value* callstackValue = ConstantPointerNull::get(callstack->getLLVMType(context));
  IReferenceVariable* callstackVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::CALL_STACK, callstack, callstackValue);
  context.getScopes().setVariable(callstackVariable);

  InjectionArgumentList injectionArguments;
  Value* injectedProgram = programInterface->inject(context, injectionArguments, 0);
  
  PackageType* packageType = new PackageType("wisey.lang");
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* mainThreadWorker =
  new ModelTypeSpecifierFull(packageExpression, "MMainThreadWorker", 0);

  ExpressionList callArguments;
  FakeExpression* argumentExpression = new FakeExpression(injectedProgram,
                                                          programInterface->getOwner());
  callArguments.push_back(argumentExpression);
  StaticMethodCall* startMethodCall =
  new StaticMethodCall(mainThreadWorker, "startMainThread", callArguments, 0);
  ReturnStatement returnResultStatement(startMethodCall, 0);
  returnResultStatement.generateIR(context);

  context.getScopes().popScope(context, 0);
  context.setMainFunction(mainFunction);
}

