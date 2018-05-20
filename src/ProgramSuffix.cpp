//
//  ProgramSuffix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificType.hpp"
#include "wisey/CastObjectFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/ModelTypeSpecifierFull.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramSuffix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/StaticMethodCall.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void ProgramSuffix::generateIR(IRGenerationContext& context) const {
  maybeGenerateMain(context);
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
  
  generateMain(context);
}

void ProgramSuffix::generateMain(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  vector<Type*> mainArguments;
  mainArguments.push_back(Type::getInt32Ty(llvmContext));
  mainArguments.push_back(Type::getInt8Ty(llvmContext)->getPointerTo()->getPointerTo());
  FunctionType* mainFunctionType =
  FunctionType::get(Type::getInt32Ty(llvmContext), mainArguments, false);
  Function* mainFunction = Function::Create(mainFunctionType,
                                            GlobalValue::ExternalLinkage,
                                            "main",
                                            context.getModule());
  Function::arg_iterator llvmArguments = mainFunction->arg_begin();
  Value* argc = &*llvmArguments;
  argc->setName("argc");
  llvmArguments++;
  Value* argv = &*llvmArguments;
  argv->setName("argv");

  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", mainFunction);
  context.setBasicBlock(entryBlock);
  context.getScopes().pushScope();
  context.getScopes().setReturnType(PrimitiveTypes::INT);

  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  Value* threadNullValue = ConstantPointerNull::get(threadInterface->getLLVMType(context));
  IReferenceVariable* threadVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::THREAD,
                                       threadInterface,
                                       threadNullValue,
                                       0);
  context.getScopes().setVariable(context, threadVariable);

  Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);
  Value* callstackValue = ConstantPointerNull::get(callstack->getLLVMType(context));
  IReferenceVariable* callstackVariable =
  new ParameterSystemReferenceVariable(ThreadExpression::CALL_STACK,
                                       callstack,
                                       callstackValue,
                                       0);
  context.getScopes().setVariable(context, callstackVariable);

  context.bindInterfaces(context);
  
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* mainThreadWorker =
  new ModelTypeSpecifierFull(packageExpression, Names::getMainThreadWorkerShortName(), 0);

  list<const IExpression*> dimensions;
  dimensions.push_back(new FakeExpression(argc, PrimitiveTypes::INT));
  ArraySpecificType arraySpecificType(PrimitiveTypes::STRING, dimensions);
  const ArrayType* genericArrayType = arraySpecificType.getArrayType(context, 0);
  
  Value* argumentsArrayI8Pointer = ArrayAllocation::allocateArray(context, &arraySpecificType, 0);
  Value* argumentsArray = IRWriter::newBitCastInst(context,
                                                   argumentsArrayI8Pointer,
                                                   genericArrayType->getLLVMType(context));

  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* arraySizeStore = IRWriter::createGetElementPtrInst(context, argumentsArray, index);
  Value* arraySize = IRWriter::newLoadInst(context, arraySizeStore, "");
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, argumentsArray, index);
  Value* elementSize = IRWriter::newLoadInst(context, elementSizeStore, "");

  Value* size = IRWriter::createBinaryOperator(context,
                                               Instruction::Mul,
                                               arraySize,
                                               elementSize,
                                               "");
  
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* arrayStore = IRWriter::createGetElementPtrInst(context, argumentsArray, index);
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* arrayStoreBitcast = IRWriter::newBitCastInst(context, arrayStore, int8PointerType);
  Value* argvBitcast = IRWriter::newBitCastInst(context, argv, int8PointerType);

  vector<Value*> memCopyArguments;
  unsigned int memoryAlignment = Environment::getDefaultMemoryAllignment();
  memCopyArguments.push_back(arrayStoreBitcast);
  memCopyArguments.push_back(argvBitcast);
  memCopyArguments.push_back(size);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), memoryAlignment));
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memCopyFunction = IntrinsicFunctions::getMemCopyFunction(context);
  IRWriter::createCallInst(context, memCopyFunction, memCopyArguments, "");

  ExpressionList callArguments;
  callArguments.push_back(new FakeExpression(argumentsArray,
                                             genericArrayType->getOwner()));
  StaticMethodCall* startMethodCall =
  new StaticMethodCall(mainThreadWorker, "startMainThread", callArguments, 0);
  
  ReturnStatement returnResultStatement(startMethodCall, 0);
  returnResultStatement.generateIR(context);

  context.getScopes().popScope(context, 0);
}

