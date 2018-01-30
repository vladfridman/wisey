//
//  DestroyOwnerArrayFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* DestroyOwnerArrayFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void DestroyOwnerArrayFunction::call(IRGenerationContext& context,
                                     Value* array,
                                     unsigned long numberOfDimensions,
                                     Value* destructor) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(array);
  arguments.push_back(ConstantInt::get(Type::getInt64Ty(llvmContext), numberOfDimensions));
  arguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 1));
  arguments.push_back(destructor);
  
  IRWriter::createCallInst(context, function, arguments, "");
}

string DestroyOwnerArrayFunction::getName() {
  return "__destroyOwnerArrayFunction";
}

Function* DestroyOwnerArrayFunction::define(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* genericPointer = Type::getInt64Ty(llvmContext)->getPointerTo();

  FunctionType* destructorFunctionType = IConcreteObjectType::getDestructorFunctionType(context);

  vector<Type*> argumentTypes;
  argumentTypes.push_back(genericPointer);
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  argumentTypes.push_back(Type::getInt1Ty(llvmContext));
  argumentTypes.push_back(destructorFunctionType->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  return Function::Create(ftype, GlobalValue::InternalLinkage, getName(), context.getModule());
}

void DestroyOwnerArrayFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int64type = Type::getInt64Ty(llvmContext);
  llvm::Constant* zero = ConstantInt::get(int64type, 0);
  llvm::Constant* one = ConstantInt::get(int64type, 1);
  llvm::Constant* two = ConstantInt::get(int64type, 2);
  PointerType* genericPointer = int64type->getPointerTo();
  PointerType* bytePointer = Type::getInt8Ty(llvmContext)->getPointerTo();

  context.getScopes().pushScope();

  Function::arg_iterator llvmArguments = function->arg_begin();
  Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("arrayPointer");
  Value* arrayPointer = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("noOfDimensions");
  Value* numberOfDimensions = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("shouldFree");
  Value* shouldFree = llvmArgument;
  llvmArguments++;
  llvmArgument = &*llvmArguments;
  llvmArgument->setName("destructor");
  Value* destructor = llvmArgument;

  BasicBlock* entry = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* returnVoid = BasicBlock::Create(llvmContext, "return.void", function);
  BasicBlock* ifNotNull = BasicBlock::Create(llvmContext, "if.not.null", function);
  BasicBlock* refCountZeroBlock = BasicBlock::Create(llvmContext, "ref.count.zero", function);
  BasicBlock* refCountNotZeroBlock = BasicBlock::Create(llvmContext, "ref.count.notzero", function);
  BasicBlock* forCond = BasicBlock::Create(llvmContext, "for.cond", function);
  BasicBlock* forBody = BasicBlock::Create(llvmContext, "for.body", function);
  BasicBlock* multiDimensional = BasicBlock::Create(llvmContext, "multi.dimensional", function);
  BasicBlock* oneDimensional = BasicBlock::Create(llvmContext, "one.dimensional", function);
  BasicBlock* maybeFreeArray = BasicBlock::Create(llvmContext, "maybe.free.array", function);
  BasicBlock* freeArray = BasicBlock::Create(llvmContext, "free.array", function);

  context.setBasicBlock(entry);
  
  Value* null = ConstantPointerNull::get(genericPointer);
  Value* isNull = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, arrayPointer, null, "isNull");
  IRWriter::createConditionalBranch(context, returnVoid, ifNotNull, isNull);
  
  context.setBasicBlock(returnVoid);

  IRWriter::createReturnInst(context, NULL);

  context.setBasicBlock(ifNotNull);
  
  Value* index[1];
  index[0] = one;
  Value* sizeStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* size = IRWriter::newLoadInst(context, sizeStore, "size");
  index[0] = two;
  Value* elementSizeStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* elementSize = IRWriter::newLoadInst(context, elementSizeStore, "elementSize");

  Value* numberOfDimensionsMinusOne = IRWriter::createBinaryOperator(context,
                                                                     llvm::Instruction::Sub,
                                                                     numberOfDimensions,
                                                                     one,
                                                                     "dimensionsMinusOne");

  if (context.isDestructorDebugOn()) {
    ExpressionList printOutArguments;
    printOutArguments.push_back(new StringLiteral("destructor object<"));
    printOutArguments.push_back(new FakeExpression(elementSize, PrimitiveTypes::INT_TYPE));
    printOutArguments.push_back(new StringLiteral(" bytes>*["));
    printOutArguments.push_back(new FakeExpression(size, PrimitiveTypes::INT_TYPE));
    printOutArguments.push_back(new StringLiteral("]\n"));
    PrintOutStatement printOutStatement(printOutArguments);
    printOutStatement.generateIR(context);
  }

  Value* referenceCount = IRWriter::newLoadInst(context, arrayPointer, "refCount");
  Value* isZero = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, referenceCount, zero, "isZero");
  IRWriter::createConditionalBranch(context, refCountZeroBlock, refCountNotZeroBlock, isZero);
  
  context.setBasicBlock(refCountNotZeroBlock);
  
  ThrowReferenceCountExceptionFunction::call(context, referenceCount);
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(refCountZeroBlock);

  index[0] = ConstantInt::get(int64type, ArrayType::ARRAY_ELEMENTS_START_INDEX);
  Value* arrayStore = IRWriter::createGetElementPtrInst(context, arrayPointer, index);
  Value* array = IRWriter::newBitCastInst(context, arrayStore, bytePointer);

  Value* indexStore = IRWriter::newAllocaInst(context, int64type, "indexStore");
  IRWriter::newStoreInst(context, zero, indexStore);
  Value* offsetStore = IRWriter::newAllocaInst(context, int64type, "offsetStore");
  IRWriter::newStoreInst(context, zero, offsetStore);

  IRWriter::createBranch(context, forCond);

  context.setBasicBlock(forCond);
  
  Value* offsetValue = IRWriter::newLoadInst(context, offsetStore, "offset");
  Value* indexValue = IRWriter::newLoadInst(context, indexStore, "index");
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, indexValue, size, "cmp");
  IRWriter::createConditionalBranch(context, forBody, maybeFreeArray, compare);
  
  context.setBasicBlock(forBody);
  
  Value* idx[1];
  idx[0] = offsetValue;
  Value* elementStore = IRWriter::createGetElementPtrInst(context, array, idx);
  Value* newIndex = IRWriter::createBinaryOperator(context,
                                                   Instruction::Add,
                                                   indexValue,
                                                   one,
                                                   "newIndex");
  IRWriter::newStoreInst(context, newIndex, indexStore);
  Value* newOffset = IRWriter::createBinaryOperator(context,
                                                    Instruction::Add,
                                                    offsetValue,
                                                    elementSize,
                                                    "offsetIncrement");
  IRWriter::newStoreInst(context, newOffset, offsetStore);
  Value* isMultiDimensional = IRWriter::newICmpInst(context,
                                                    ICmpInst::ICMP_SGT,
                                                    numberOfDimensions,
                                                    one,
                                                    "cmp");
  IRWriter::createConditionalBranch(context, multiDimensional, oneDimensional, isMultiDimensional);

  context.setBasicBlock(multiDimensional);

  vector<Value*> recursiveCallArguments;
  recursiveCallArguments.push_back(IRWriter::newBitCastInst(context, elementStore, genericPointer));
  recursiveCallArguments.push_back(numberOfDimensionsMinusOne);
  recursiveCallArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  recursiveCallArguments.push_back(destructor);
  IRWriter::createCallInst(context, function, recursiveCallArguments, "");
  IRWriter::createBranch(context, forCond);

  context.setBasicBlock(oneDimensional);
  
  Value* objectStore = IRWriter::newBitCastInst(context, elementStore, bytePointer->getPointerTo());
  Value* elementPointer = IRWriter::newLoadInst(context, objectStore, "");
  vector<Value*> destructorArguments;
  destructorArguments.push_back(elementPointer);
  IRWriter::createCallInst(context, (Function*) destructor, destructorArguments, "");
  IRWriter::createBranch(context, forCond);

  context.setBasicBlock(maybeFreeArray);
  
  IRWriter::createConditionalBranch(context, freeArray, returnVoid, shouldFree);

  context.setBasicBlock(freeArray);
  
  IRWriter::createFree(context, arrayPointer);
  IRWriter::createReturnInst(context, NULL);

  context.getScopes().popScope(context, 0);
}

