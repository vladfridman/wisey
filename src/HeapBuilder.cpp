//
//  HeapBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/HeapBuilder.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

HeapBuilder::HeapBuilder(IBuildableObjectTypeSpecifier* typeSpecifier,
                             BuilderArgumentList builderArgumentList,
                             int line) :
mTypeSpecifier(typeSpecifier),
mBuilderArgumentList(builderArgumentList),
mLine(line) { }

HeapBuilder::~HeapBuilder() {
  delete mTypeSpecifier;
  for (BuilderArgument* argument : mBuilderArgumentList) {
    delete argument;
  }
  mBuilderArgumentList.clear();
}

int HeapBuilder::getLine() const {
  return mLine;
}

Value* HeapBuilder::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IBuildableObjectType* buildableType = mTypeSpecifier->getType(context);
  Value* malloc = build(context, buildableType);
  
  if (assignToType->isOwner()) {
    return malloc;
  }
  
  Value* alloc = IRWriter::newAllocaInst(context, malloc->getType(), "");
  IRWriter::newStoreInst(context, malloc, alloc);

  IVariable* heapVariable = new LocalOwnerVariable(IVariable::getTemporaryVariableName(this),
                                                   buildableType->getOwner(),
                                                   alloc,
                                                   mLine);
  context.getScopes().setVariable(context, heapVariable);
  
  return malloc;
}

const IType* HeapBuilder::getType(IRGenerationContext& context) const {
  const IBuildableObjectType* objectType = mTypeSpecifier->getType(context);
  return objectType->getOwner();
}

bool HeapBuilder::isConstant() const {
  return false;
}

bool HeapBuilder::isAssignable() const {
  return false;
}

void HeapBuilder::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "builder(";
  mTypeSpecifier->printToStream(context, stream);
  stream << ")";
  for (BuilderArgument* argument : mBuilderArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ".build()";
}

Value* HeapBuilder::build(IRGenerationContext& context,
                          const IBuildableObjectType* buildable) const {
  checkArguments(context, buildable, mBuilderArgumentList, mLine);
  
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context,
                                                                   buildable,
                                                                   "buildervar");
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, malloc, index);
  
  vector<Value*> creationArguments;
  buildable->generateCreationArguments(context,
                                       mBuilderArgumentList,
                                       creationArguments,
                                       mLine);

  auto iterator = creationArguments.begin();
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (const IField* field : buildable->getFields()) {
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), buildable->getFieldIndex(field));
    GetElementPtrInst* fieldPointer =
    IRWriter::createGetElementPtrInst(context, objectStart, index);
    IRWriter::newStoreInst(context, *iterator, fieldPointer);
    const IType* fieldType = field->getType();
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, *iterator);
    }
    iterator++;
  }
  
  IConcreteObjectType::initializeVTable(context, buildable, objectStart);

  return objectStart;
}
