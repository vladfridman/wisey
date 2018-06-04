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
#include "wisey/ParameterSystemReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* IBuildableObjectType::
declareBuildFunctionForObject(IRGenerationContext& context,
                              const IBuildableObjectType* object) {
  const Interface* thread = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  const Controller* callstack = context.getController(Names::getCallStackControllerFullName(), 0);
  vector<Type*> argumentTypes;
  if (object->isPooled()) {
    argumentTypes.push_back(thread->getLLVMType(context));
    argumentTypes.push_back(callstack->getLLVMType(context));
  }
  for (const IField* field : object->getFields()) {
    argumentTypes.push_back(field->getType()->getLLVMType(context));
  }
  
  FunctionType* functionType = FunctionType::get(object->getLLVMType(context),
                                                 argumentTypes,
                                                 false);
  GlobalValue::LinkageTypes linkageType = object->isPublic()
  ? GlobalValue::ExternalLinkage
  : GlobalValue::InternalLinkage;
  
  return Function::Create(functionType,
                          linkageType,
                          getBuildFunctionNameForObject(object),
                          context.getModule());
  
}

Function* IBuildableObjectType::
defineBuildFunctionForObject(IRGenerationContext& context,
                             const IBuildableObjectType* object) {
  Function* buildFunction = declareBuildFunctionForObject(context, object);
  if (object->isPooled()) {
    context.addComposingCallback1Objects(composeBuildPooledFunctionBody, buildFunction, object);
  } else {
    context.addComposingCallback1Objects(composeBuildFunctionBody, buildFunction, object);
  }
  
  return buildFunction;
}

string IBuildableObjectType::
getBuildFunctionNameForObject(const IBuildableObjectType* object) {
  return object->getTypeName() + ".build";
}

void IBuildableObjectType::composeBuildPooledFunctionBody(IRGenerationContext& context,
                                                          Function* buildFunction,
                                                          const void* objectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  llvm::PointerType* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  const IBuildableObjectType* buildable = (const IBuildableObjectType*) objectType;
  Type* buildableLLVMType = buildable->getLLVMType(context);
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", buildFunction);
  BasicBlock* newPoolBlock = BasicBlock::Create(llvmContext, "new.pool", buildFunction);
  BasicBlock* existingPoolBlock = BasicBlock::Create(llvmContext, "existing.pool", buildFunction);
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
  IVariable* threadVariable = new ParameterSystemReferenceVariable(ThreadExpression::THREAD,
                                                                   thread,
                                                                   threadArgument,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  IVariable* callstackVariable = new ParameterSystemReferenceVariable(ThreadExpression::CALL_STACK,
                                                                      callstack,
                                                                      callstackArgument,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

  const Controller* cPoolMap = context.getController(Names::getCPoolMapFullName(), 0);
  const InjectionArgumentList injectionArguments;
  Value* poolMap = cPoolMap->inject(context, injectionArguments, 0);
  
  unsigned long numberOfInterfaces = buildable->getFlattenedInterfaceHierarchy().size();
  unsigned long headerSizeInBytes = numberOfInterfaces ? numberOfInterfaces + 1 : 2;
  llvm::Constant* headerSize = ConstantInt::get(Type::getInt64Ty(llvmContext),
                                                headerSizeInBytes *
                                                Environment::getAddressSizeInBytes());
  llvm::Constant* structSize = ConstantExpr::getSizeOf(buildable->getLLVMType(context)->
                                                       getPointerElementType());
  llvm::Constant* eight = ConstantInt::get(Type::getInt64Ty(llvmContext),
                                           Environment::getAddressSizeInBytes());
  llvm::Constant* blockSizeMinusEight = ConstantExpr::getSub(structSize, headerSize);
  llvm::Constant* blockSize = ConstantExpr::getAdd(blockSizeMinusEight, eight);
  llvm::Constant* key = getObjectNamePointer(buildable, context);
  
  FakeExpression* poolMapExpression = new FakeExpression(poolMap, cPoolMap);
  IdentifierChain* getPool = new IdentifierChain(poolMapExpression,
                                                 Names::getGetPoolMethodName(),
                                                 0);
  ExpressionList getPoolCallArguments;
  getPoolCallArguments.push_back(new FakeExpression(key, PrimitiveTypes::STRING));
  MethodCall getPoolCall(getPool, getPoolCallArguments, 0);
  Value* existingPool = getPoolCall.generateIR(context, PrimitiveTypes::VOID);
  Value* nullValue = ConstantPointerNull::get(int8Pointer);
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, existingPool, nullValue, "");
  IRWriter::createConditionalBranch(context, newPoolBlock, existingPoolBlock, condition);

  context.setBasicBlock(newPoolBlock);
  poolMapExpression = new FakeExpression(poolMap, cPoolMap);
  IdentifierChain* createPool = new IdentifierChain(poolMapExpression,
                                                    Names::getCreatePoolMethodName(),
                                                    0);
  ExpressionList createPoolCallArguments;
  createPoolCallArguments.push_back(new FakeExpression(key, PrimitiveTypes::STRING));
  createPoolCallArguments.push_back(new FakeExpression(headerSize, PrimitiveTypes::LONG));
  createPoolCallArguments.push_back(new FakeExpression(blockSize, PrimitiveTypes::LONG));
  MethodCall createPoolCall(createPool, createPoolCallArguments, 0);
  Value* newPool = createPoolCall.generateIR(context, 0);
  Instruction* build = IConcreteObjectType::createMallocForObject(context, buildable, "build");
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, build, index);
  Value* objectStartI8Pointer = IRWriter::newBitCastInst(context, objectStart, int8Pointer);
  initializeReceivedFieldsForObject(context, buildFunction, buildable, objectStart);
  initializeVTable(context, buildable, objectStart);
  Function* memCopy = IntrinsicFunctions::getMemCopyFunction(context);
  vector<Value*> memCopyArguments;
  memCopyArguments.push_back(newPool);
  memCopyArguments.push_back(objectStartI8Pointer);
  memCopyArguments.push_back(structSize);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext),
                                              Environment::getDefaultMemoryAllignment()));
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  IRWriter::createCallInst(context, memCopy, memCopyArguments, "");
  IRWriter::createFree(context, build);
  Value* result = IRWriter::newBitCastInst(context, newPool, buildableLLVMType);
  IRWriter::createReturnInst(context, result);
  
  context.setBasicBlock(existingPoolBlock);
  result = IRWriter::newBitCastInst(context, existingPool, buildableLLVMType);
  IRWriter::createReturnInst(context, result);
  
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
  
  initializeReceivedFieldsForObject(context, buildFunction, buildable, objectStart);
  initializeVTable(context, buildable, objectStart);
  IRWriter::createReturnInst(context, objectStart);
  
  context.getScopes().popScope(context, 0);
}

void IBuildableObjectType::
initializeReceivedFieldsForObject(IRGenerationContext& context,
                                  llvm::Function* buildFunction,
                                  const IBuildableObjectType* object,
                                  Instruction* malloc) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function::arg_iterator functionArguments = buildFunction->arg_begin();
  if (object->isPooled()) {
    functionArguments++;
    functionArguments++;
  }
  
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
