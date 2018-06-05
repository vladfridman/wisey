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
  
  StructType* refStruct = IConcreteObjectType::getOrCreateRefCounterStruct(context, buildable);
  llvm::Constant* blockSize = ConstantExpr::getSizeOf(refStruct);
  llvm::Constant* key = getObjectNamePointer(buildable, context);
  
  FakeExpression* poolMapExpression = new FakeExpression(poolMap, cPoolMap);
  IdentifierChain* allocate = new IdentifierChain(poolMapExpression,
                                                  Names::getAllocateMethodName(),
                                                  0);
  ExpressionList allocateCallArguments;
  allocateCallArguments.push_back(new FakeExpression(key, PrimitiveTypes::STRING));
  allocateCallArguments.push_back(new FakeExpression(blockSize, PrimitiveTypes::LONG));
  MethodCall allocateCall(allocate, allocateCallArguments, 0);
  Value* memory = allocateCall.generateIR(context, PrimitiveTypes::VOID);
  Value* shellObject = IRWriter::newBitCastInst(context, memory, refStruct->getPointerTo());
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, shellObject, index);
  initializeReceivedFieldsForObject(context, buildFunction, buildable, objectStart);
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
