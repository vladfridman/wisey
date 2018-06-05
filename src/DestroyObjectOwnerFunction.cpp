//
//  DestroyObjectOwnerFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunctionType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Names.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyObjectOwnerFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyObjectOwnerFunction::call(IRGenerationContext& context,
                                      Value* objectReference,
                                      Value* exception,
                                      int line) {
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(objectReference);

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  arguments.push_back(threadVariable->generateIdentifierIR(context, line));
  IVariable* callstackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  arguments.push_back(callstackVariable->generateIdentifierIR(context, line));

  if (exception) {
    arguments.push_back(exception);
    IRWriter::createCallInst(context, function, arguments, "");
  } else {
    llvm::PointerType* int8Pointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
    Value* nullPointer = ConstantPointerNull::get(int8Pointer);
    arguments.push_back(nullPointer);
    IRWriter::createInvokeInst(context, function, arguments, "", line);
  }
}

string DestroyObjectOwnerFunction::getName() {
  return "__destroyObjectOwnerFunction";
}

Function* DestroyObjectOwnerFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* DestroyObjectOwnerFunction::getLLVMFunctionType(IRGenerationContext& context) {
  const Interface* thread = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);

  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(thread);
  argumentTypes.push_back(callstack);
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));

  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void DestroyObjectOwnerFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  context.getScopes().pushScope();
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Value* thisGeneric = &*functionArguments;
  thisGeneric->setName("thisGeneric");
  functionArguments++;
  Value* threadArgument = &*functionArguments;
  threadArgument->setName(ThreadExpression::THREAD);
  functionArguments++;
  Value* callstackArgument = &*functionArguments;
  callstackArgument->setName(ThreadExpression::CALL_STACK);
  functionArguments++;
  Value* exception = &*functionArguments;
  exception->setName("exception");

  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  
  Value* nullValue = ConstantPointerNull::get(int8Type->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisGeneric, nullValue, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  
  Value* originalObjectVTable = GetOriginalObjectFunction::call(context, thisGeneric);
  
  Type* pointerToVTablePointer = function->getFunctionType()
  ->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* objectDestructor = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  vector<Value*> arguments;
  arguments.push_back(originalObjectVTable);
  arguments.push_back(threadArgument);
  arguments.push_back(callstackArgument);
  arguments.push_back(exception);

  IRWriter::createInvokeInst(context, objectDestructor, arguments, "", 0);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
