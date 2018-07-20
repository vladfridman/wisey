//
//  PoolBuilder.cpp
//  wiseyrun
//
//  Created by Vladimir Fridman on 6/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/ControllerTypeSpecifierFull.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/PoolBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StaticMethodCall.hpp"
#include "wisey/ThrowNullPointerExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

PoolBuilder::PoolBuilder(IBuildableObjectTypeSpecifier* typeSpecifier,
                         BuilderArgumentList builderArgumentList,
                         IExpression* poolExpression,
                         int line) :
mTypeSpecifier(typeSpecifier),
mBuilderArgumentList(builderArgumentList),
mPoolExpression(poolExpression),
mLine(line) { }

PoolBuilder::~PoolBuilder() {
  delete mTypeSpecifier;
  for (BuilderArgument* argument : mBuilderArgumentList) {
    delete argument;
  }
  mBuilderArgumentList.clear();
}

int PoolBuilder::getLine() const {
  return mLine;
}

Value* PoolBuilder::generateIR(IRGenerationContext& context, const IType* assignToType) const {
  const IBuildableObjectType* buildableType = mTypeSpecifier->getType(context);
  if (!buildableType->isPooled()) {
    context.reportError(mLine, "Object " + buildableType->getTypeName() +
                        " can not be allocated on a memory pool beause it is not marked with "
                        "onPool qualifier, it should be allocated on heap");
    throw 1;
  }
  Value* malloc = allocate(context, buildableType);
  
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

const IType* PoolBuilder::getType(IRGenerationContext& context) const {
  const IBuildableObjectType* objectType = mTypeSpecifier->getType(context);
  return objectType->getOwner();
}

bool PoolBuilder::isConstant() const {
  return false;
}

bool PoolBuilder::isAssignable() const {
  return false;
}

void PoolBuilder::printToStream(IRGenerationContext& context, std::iostream& stream) const {
  stream << "allocator(";
  mTypeSpecifier->printToStream(context, stream);
  stream << ")";
  for (BuilderArgument* argument : mBuilderArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ".onPool(";
  mPoolExpression->printToStream(context, stream);
  stream << ")";
}

Value* PoolBuilder::allocate(IRGenerationContext& context,
                             const IBuildableObjectType* buildable) const {
  checkArguments(context, buildable, mBuilderArgumentList, mLine);

  const Controller* cMemoryPool = context.getController(Names::getCMemoryPoolFullName(), 0);
  const IType* poolType = mPoolExpression->getType(context);
  if (poolType != cMemoryPool && poolType != cMemoryPool->getOwner()) {
    context.reportError(mLine,
                        "pool expression in allocate is not of type " + cMemoryPool->getTypeName());
    throw 1;
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* pool = mPoolExpression->generateIR(context, PrimitiveTypes::VOID);
  
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.pool.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.pool.notnull", function);
  
  Value* null = ConstantPointerNull::get((PointerType*) pool->getType());
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, pool, null, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  ThrowNullPointerExceptionFunction::call(context);
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(ifNotNullBlock);
  vector<Value*> creationArguments;
  buildable->generateCreationArguments(context,
                                       mBuilderArgumentList,
                                       creationArguments,
                                       mLine);

  StructType* refStruct = IConcreteObjectType::getOrCreateRefCounterStruct(context, buildable);
  llvm::Constant* blockSize = ConstantExpr::getSizeOf(refStruct);
  
  llvm::Constant* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  llvm::Constant* two = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
  Value* index[2];
  index[0] = zero;
  index[1] = two;
  Type* cMemoryPoolType = IConcreteObjectType::getCMemoryPoolStruct(context)->getPointerTo();
  Value* poolCast = IRWriter::newBitCastInst(context, pool, cMemoryPoolType);
  Value* aprPoolStore = IRWriter::createGetElementPtrInst(context, poolCast, index);
  Value* aprPool = IRWriter::newLoadInst(context, aprPoolStore, "");
  
  index[0] = zero;
  index[1] = one;
  Value* objectCountStore = IRWriter::createGetElementPtrInst(context, poolCast, index);
  Value* objectCount = IRWriter::newLoadInst(context, objectCountStore, "");
  llvm::Constant* oneLong = ConstantInt::get(Type::getInt64Ty(llvmContext), 1);
  Value* sum = IRWriter::createBinaryOperator(context, Instruction::Add, objectCount, oneLong, "");
  IRWriter::newStoreInst(context, sum, objectCountStore);
  
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCMemoryPoolName(), 0);
  ExpressionList pallocCallArguments;
  LLVMStructType* aprPoolStruct = context.getLLVMStructType("AprPool", 0);
  pallocCallArguments.push_back(new FakeExpression(aprPool,
                                                   aprPoolStruct->getPointerType(context, 0)));
  pallocCallArguments.push_back(new FakeExpression(blockSize, PrimitiveTypes::LONG));
  StaticMethodCall* pallocCall = StaticMethodCall::createCantThrow(controllerTypeSpecifier,
                                                                   Names::getPallocateMethodName(),
                                                                   pallocCallArguments,
                                                                   0);
  Value* memory = pallocCall->generateIR(context, PrimitiveTypes::VOID);
  index[0] = zero;
  index[1] = zero;
  Value* shellObject = IRWriter::newBitCastInst(context, memory, refStruct->getPointerTo());
  Instruction* refCounter = IRWriter::createGetElementPtrInst(context, shellObject, index);
  IRWriter::newStoreInst(context, ConstantInt::get(Type::getInt64Ty(llvmContext), 0), refCounter);
  index[0] = zero;
  index[1] = one;
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, shellObject, index);
  
  unsigned long numberOfInterfaces = buildable->getFlattenedInterfaceHierarchy().size();
  unsigned long poolStoreIndex = numberOfInterfaces > 1 ? numberOfInterfaces + 1 : 1;
  index[0] = zero;
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), poolStoreIndex);
  Instruction* poolStore = IRWriter::createGetElementPtrInst(context, objectStart, index);
  IRWriter::newStoreInst(context, pool, poolStore);
  
  assert(creationArguments.size() == buildable->getFields().size());
  
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
  
  delete pallocCall;
  
  return objectStart;
}
