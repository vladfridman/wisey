//
//  IBuildableObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IBuildableObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterReferenceVariableStatic.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void IBuildableObjectType::composeAllocateFunctionBody(IRGenerationContext& context,
                                                       Function* buildFunction,
                                                       const void* objectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const IBuildableObjectType* buildable = (const IBuildableObjectType*) objectType;
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", buildFunction);
  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  context.setObjectType(buildable);

  const Interface* thread = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);
  Function::arg_iterator llvmFunctionArguments = buildFunction->arg_begin();
  llvm::Argument* threadArgument = &*llvmFunctionArguments;
  threadArgument->setName(ThreadExpression::THREAD);
  llvmFunctionArguments++;
  llvm::Argument* callstackArgument = &*llvmFunctionArguments;
  callstackArgument->setName(ThreadExpression::CALL_STACK);
  llvmFunctionArguments++;
  llvm::Argument* poolArgument = &*llvmFunctionArguments;
  poolArgument->setName("pool");

  IVariable* threadVariable = new ParameterReferenceVariableStatic(ThreadExpression::THREAD,
                                                                   thread,
                                                                   threadArgument,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  IVariable* callstackVariable = new ParameterReferenceVariableStatic(ThreadExpression::CALL_STACK,
                                                                      callstack,
                                                                      callstackArgument,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

  const Controller* cMemoryPool = context.getController(Names::getCMemoryPoolFullName(), 0);
  StructType* refStruct = IConcreteObjectType::getOrCreateRefCounterStruct(context, buildable);
  llvm::Constant* blockSize = ConstantExpr::getSizeOf(refStruct);
  
  FakeExpression* poolMapExpression = new FakeExpression(poolArgument, cMemoryPool);
  IdentifierChain* allocate = new IdentifierChain(poolMapExpression,
                                                  Names::getAllocateMethodName(),
                                                  0);
  ExpressionList allocateCallArguments;
  allocateCallArguments.push_back(new FakeExpression(blockSize, PrimitiveTypes::LONG));
  MethodCall allocateCall(allocate, allocateCallArguments, 0);
  Value* memory = allocateCall.generateIR(context, PrimitiveTypes::VOID);
  llvm::Constant* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* shellObject = IRWriter::newBitCastInst(context, memory, refStruct->getPointerTo());
  Value* index[2];
  index[0] = zero;
  index[1] = zero;
  Instruction* refCounter = IRWriter::createGetElementPtrInst(context, shellObject, index);
  IRWriter::newStoreInst(context, ConstantInt::get(Type::getInt64Ty(llvmContext), 0), refCounter);
  index[0] = zero;
  index[1] = one;
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, shellObject, index);
  
  unsigned long numberOfInterfaces = buildable->getFlattenedInterfaceHierarchy().size();
  unsigned long poolStoreIndex = numberOfInterfaces > 0 ? numberOfInterfaces + 1 : 1;
  index[0] = zero;
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), poolStoreIndex);
  Instruction* poolStore = IRWriter::createGetElementPtrInst(context, objectStart, index);
  IRWriter::newStoreInst(context, poolArgument, poolStore);

  Function::arg_iterator functionArguments = buildFunction->arg_begin();
  functionArguments++;
  functionArguments++;
  functionArguments++;
  initializeReceivedFieldsForObject(context, functionArguments, buildable, objectStart);
  initializeVTable(context, buildable, objectStart);
  IRWriter::createReturnInst(context, objectStart);
  
  context.getScopes().popScope(context, 0);
}

void IBuildableObjectType::composeBuildFunctionBody(IRGenerationContext& context,
                                                            Function* buildFunction,
                                                            const void* objectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const IBuildableObjectType* buildable = (const IBuildableObjectType*) objectType;
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", buildFunction);
  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  context.setObjectType(buildable);
  
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context,
                                                                   buildable,
                                                                   "buildervar");
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, malloc, index);
  
  Function::arg_iterator functionArguments = buildFunction->arg_begin();
  initializeReceivedFieldsForObject(context, functionArguments, buildable, objectStart);
  initializeVTable(context, buildable, objectStart);
  IRWriter::createReturnInst(context, objectStart);
  
  context.getScopes().popScope(context, 0);
}

void IBuildableObjectType::
initializeReceivedFieldsForObject(IRGenerationContext& context,
                                  Function::arg_iterator functionArguments,
                                  const IBuildableObjectType* object,
                                  Instruction* malloc) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (const IField* field : object->getFields()) {
    Value* functionArgument = &*functionArguments;
    functionArgument->setName(field->getName());
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), object->getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, functionArgument, fieldPointer);
    const IType* fieldType = field->getType();
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, functionArgument);
    }
    functionArguments++;
  }
}
