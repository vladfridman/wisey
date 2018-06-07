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
#include "wisey/GlobalStringConstant.hpp"
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

  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* ifOverflowBlock = BasicBlock::Create(llvmContext, "if.overflow", function);
  BasicBlock* ifContinueBlock = BasicBlock::Create(llvmContext, "if.continue", function);

  const IObjectType* objectType = context.getObjectType();
  const Controller* callStackController =
  context.getController(Names::getCallStackControllerFullName(), 0);
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackValue = callStackVariable->generateIdentifierIR(context, line);
  
  string objectName = objectType->getTypeName();
  string methodName = context.getCurrentMethod()->getName();
  string fileName = objectType->getImportProfile()->getSourceFileName();
  string value = objectName + "." + methodName + "(" + fileName;
  llvm::Constant* infoConstant = GlobalStringConstant::get(context, value);
  
  Constant* constant = callStackController->findConstant(context,
                                                         Names::getCallStackSizeConstantName(),
                                                         0);
  string constantGlobalName = constant->getConstantGlobalName(callStackController);
  llvm::Constant* constantStore = context.getModule()->getNamedGlobal(constantGlobalName);
  Value* maxIndex = IRWriter::newLoadInst(context, constantStore, "");

  Type* i32 = Type::getInt32Ty(llvmContext);
  llvm::Constant* zero = ConstantInt::get(i32, 0);
  llvm::Constant* one = ConstantInt::get(i32, 1);
  llvm::Constant* two = ConstantInt::get(i32, 2);
  llvm::Constant* three = ConstantInt::get(i32, 3);
  PointerType* callStackStuct = getCCallStackStruct(context)->getPointerTo();
  Value* callStack = IRWriter::newBitCastInst(context, callStackValue, callStackStuct);
  Value* indexes[2];
  indexes[0] = zero;
  indexes[1] = three;
  Value* callstackIndexStore = IRWriter::createGetElementPtrInst(context, callStack, indexes);
  Value* callstackIndex = IRWriter::newLoadInst(context, callstackIndexStore, "");
  Value* condition = IRWriter::newICmpInst(context,
                                           ICmpInst::ICMP_SGE,
                                           callstackIndex,
                                           maxIndex,
                                           "");
  IRWriter::createConditionalBranch(context, ifOverflowBlock, ifContinueBlock, condition);
  
  context.setBasicBlock(ifOverflowBlock);
  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadValue = threadVariable->generateIdentifierIR(context, 0);
  vector<Value*> arguments;
  arguments.clear();
  arguments.push_back(threadValue);
  arguments.push_back(callStackValue);
  string throwMethodName =
  IMethodCall::translateObjectMethodToLLVMFunctionName(callStackController,
                                                       Names::getThrowStackOverflowMethodName());
  Function* throwFunction = context.getModule()->getFunction(throwMethodName.c_str());
  IRWriter::createCallInst(context, throwFunction, arguments, "");
  IRWriter::newUnreachableInst(context);

  context.setBasicBlock(ifContinueBlock);
  indexes[1] = one;
  Value* infoArrayStructPointer = IRWriter::createGetElementPtrInst(context, callStack, indexes);
  Value* infoArrayStruct = IRWriter::newLoadInst(context, infoArrayStructPointer, "");
  indexes[1] = three;
  Value* infoArrayPointer = IRWriter::createGetElementPtrInst(context, infoArrayStruct, indexes);
  indexes[1] = callstackIndex;
  Value* infoStore = IRWriter::createGetElementPtrInst(context, infoArrayPointer, indexes);
  IRWriter::newStoreInst(context, infoConstant, infoStore);
  
  indexes[1] = two;
  Value* linesArrayStructPointer = IRWriter::createGetElementPtrInst(context, callStack, indexes);
  Value* linesArrayStruct = IRWriter::newLoadInst(context, linesArrayStructPointer, "");
  indexes[1] = three;
  Value* lineArrayPointer = IRWriter::createGetElementPtrInst(context, linesArrayStruct, indexes);
  indexes[1] = callstackIndex;
  Value* lineStore = IRWriter::createGetElementPtrInst(context, lineArrayPointer, indexes);
  llvm::Constant* lineNumber = ConstantInt::get(i32, line);
  IRWriter::newStoreInst(context, lineNumber, lineStore);
  
  Value* newCallstackIndex = IRWriter::createBinaryOperator(context,
                                                            Instruction::Add,
                                                            callstackIndex,
                                                            one,
                                                            "");
  IRWriter::newStoreInst(context, newCallstackIndex, callstackIndexStore);
}

void Composer::popCallStack(IRGenerationContext& context) {
  if (shouldSkip(context)) {
    return;
  }

  LLVMContext& llvmContext = context.getLLVMContext();
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackValue = callStackVariable->generateIdentifierIR(context, 0);
  
  Type* i32 = Type::getInt32Ty(llvmContext);
  llvm::Constant* zero = ConstantInt::get(i32, 0);
  llvm::Constant* one = ConstantInt::get(i32, 1);
  llvm::Constant* three = ConstantInt::get(i32, 3);
  PointerType* callStackStuct = getCCallStackStruct(context)->getPointerTo();
  Value* callStack = IRWriter::newBitCastInst(context, callStackValue, callStackStuct);
  Value* indexes[2];
  indexes[0] = zero;
  indexes[1] = three;
  Value* callstackIndexStore = IRWriter::createGetElementPtrInst(context, callStack, indexes);
  Value* callstackIndex = IRWriter::newLoadInst(context, callstackIndexStore, "");
  Value* newCallstackIndex = IRWriter::createBinaryOperator(context,
                                                            Instruction::Sub,
                                                            callstackIndex,
                                                            one,
                                                            "");
  IRWriter::newStoreInst(context, newCallstackIndex, callstackIndexStore);
}

void Composer::setLineNumber(IRGenerationContext& context, int line) {
  if (shouldSkip(context)) {
    return;
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  IVariable* callStackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callStackValue = callStackVariable->generateIdentifierIR(context, 0);
  
  Type* i32 = Type::getInt32Ty(llvmContext);
  llvm::Constant* zero = ConstantInt::get(i32, 0);
  llvm::Constant* one = ConstantInt::get(i32, 1);
  llvm::Constant* two = ConstantInt::get(i32, 2);
  llvm::Constant* three = ConstantInt::get(i32, 3);
  PointerType* callStackStuct = getCCallStackStruct(context)->getPointerTo();
  Value* callStack = IRWriter::newBitCastInst(context, callStackValue, callStackStuct);
  Value* indexes[2];
  indexes[0] = zero;
  indexes[1] = three;
  Value* callstackIndexStore = IRWriter::createGetElementPtrInst(context, callStack, indexes);
  Value* callstackIndex = IRWriter::newLoadInst(context, callstackIndexStore, "");
  Value* newCallstackIndex = IRWriter::createBinaryOperator(context,
                                                            Instruction::Sub,
                                                            callstackIndex,
                                                            one,
                                                            "");
  indexes[1] = two;
  Value* arrayStructPointer = IRWriter::createGetElementPtrInst(context, callStack, indexes);
  Value* arrayStruct = IRWriter::newLoadInst(context, arrayStructPointer, "");
  indexes[1] = three;
  Value* arrayPointer = IRWriter::createGetElementPtrInst(context, arrayStruct, indexes);
  indexes[1] = newCallstackIndex;
  Value* lineStore = IRWriter::createGetElementPtrInst(context, arrayPointer, indexes);
  llvm::Constant* lineNumber = ConstantInt::get(i32, line);
  IRWriter::newStoreInst(context, lineNumber, lineStore);
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
  
  if (context.isRunningComposingCallbacks()) {
    return true;
  }
  
  return false;
}

StructType* Composer::getCCallStackStruct(IRGenerationContext& context) {
  string structTypeName = "CCallStack";
  StructType* structType = context.getModule()->getTypeByName(structTypeName);
  if (structType) {
    return structType;
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  structType = StructType::create(llvmContext, structTypeName);
  vector<Type*> bodyTypes;
  bodyTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  bodyTypes.push_back(context.getArrayType(PrimitiveTypes::STRING, 1)->getLLVMType(context));
  bodyTypes.push_back(context.getArrayType(PrimitiveTypes::INT, 1)->getLLVMType(context));
  bodyTypes.push_back(PrimitiveTypes::INT->getLLVMType(context));
  structType->setBody(bodyTypes);
  
  return structType;
}
