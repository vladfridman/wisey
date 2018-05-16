//
//  IBuildableConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* IBuildableConcreteObjectType::
declareBuildFunctionForObject(IRGenerationContext& context,
                              const IBuildableConcreteObjectType* object) {
  vector<Type*> argumentTypes;
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

Function* IBuildableConcreteObjectType::
defineBuildFunctionForObject(IRGenerationContext& context,
                             const IBuildableConcreteObjectType* object) {
  Function* buildFunction = declareBuildFunctionForObject(context, object);
  context.addComposingCallback1Objects(composeBuildFunctionBody, buildFunction, object);
  
  return buildFunction;
}

string IBuildableConcreteObjectType::
getBuildFunctionNameForObject(const IBuildableConcreteObjectType* object) {
  return object->getTypeName() + ".build";
}

void IBuildableConcreteObjectType::composeBuildFunctionBody(IRGenerationContext& context,
                                                            Function* buildFunction,
                                                            const void* objectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const IBuildableConcreteObjectType* buildable = (const IBuildableConcreteObjectType*) objectType;
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", buildFunction);
  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  context.setObjectType(buildable);
  
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context,
                                                                   buildable,
                                                                   "buildervar");
  initializeReceivedFieldsForObject(context, buildFunction, buildable, malloc);
  initializeVTable(context, buildable, malloc);
  IRWriter::createReturnInst(context, malloc);
  
  context.getScopes().popScope(context, 0);
}

void IBuildableConcreteObjectType::
initializeReceivedFieldsForObject(IRGenerationContext& context,
                                  llvm::Function* buildFunction,
                                  const IBuildableConcreteObjectType* object,
                                  Instruction* malloc) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function::arg_iterator functionArguments = buildFunction->arg_begin();
  
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
