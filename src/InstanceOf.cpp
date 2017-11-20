//
//  InstanceOf.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/InstanceOf.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

CallInst* InstanceOf::call(IRGenerationContext& context,
                           const Interface* interface,
                           Value* interfaceObject,
                           const IObjectType* callableObjectType) {
  Function* function = getOrCreateFunction(context, interface);
  
  llvm::Constant* namePointer = IObjectType::getObjectNamePointer(callableObjectType, context);
  
  vector<Value*> arguments;
  arguments.push_back(interfaceObject);
  arguments.push_back(namePointer);
  
  return IRWriter::createCallInst(context, function, arguments, "instanceof");
}

Function* InstanceOf::getOrCreateFunction(IRGenerationContext& context,
                                          const Interface* interface) {
  Function* function = context.getModule()->getFunction(InstanceOf::getFunctionName(interface));
  
  if (function != NULL) {
    return function;
  }
  
  function = createFunction(context, interface);
  context.addComposingCallback(compose, function, interface);

  return function;
}

string InstanceOf::getFunctionName(const Interface* interface) {
  return interface->getName() + ".instanceof";
}

Function* InstanceOf::compose(IRGenerationContext& context,
                              Function* function,
                              const IObjectType* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const Interface* interface = (const Interface*) object;

  BasicBlock* lastBasicBlock = context.getBasicBlock();
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  BasicBlock* whileCond = BasicBlock::Create(llvmContext, "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(llvmContext, "while.body", function);
  BasicBlock* returnNotFound = BasicBlock::Create(llvmContext, "return.notfound", function);
  BasicBlock* returnFound = BasicBlock::Create(llvmContext, "return.found", function);
  
  context.setBasicBlock(entryBlock);
  Value* iterator = IRWriter::newAllocaInst(context, Type::getInt32Ty(llvmContext), "iterator");
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  IRWriter::newStoreInst(context, zero, iterator);
  
  Value* arrayOfStrings = composeEntryBlock(context, entryBlock, whileCond, function);
  
  Value* stringPointer = composeWhileConditionBlock(context,
                                                    whileCond,
                                                    whileBody,
                                                    returnNotFound,
                                                    iterator,
                                                    arrayOfStrings);
  composeWhileBodyBlock(context,
                        whileBody,
                        whileCond,
                        returnFound,
                        iterator,
                        stringPointer,
                        function);
  composeReturnFound(context, returnFound, iterator);
  composeReturnNotFound(context, returnNotFound);
  context.setBasicBlock(lastBasicBlock);
  
  return function;
}

Function* InstanceOf::createFunction(IRGenerationContext& context, const Interface* interface) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(interface->getLLVMType(llvmContext));
  argumentTypes.push_back(int8Type->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = PrimitiveTypes::INT_TYPE->getLLVMType(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::ExternalLinkage,
                                        getFunctionName(interface),
                                        context.getModule());
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  functionArguments++;
  Argument* compareToArgument = &*functionArguments;
  compareToArgument->setName("compareto");
  
  return function;
}

BitCastInst* InstanceOf::composeEntryBlock(IRGenerationContext& context,
                                           BasicBlock* entryBlock,
                                           BasicBlock* whileCond,
                                           Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  context.setBasicBlock(entryBlock);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  Value* originalObjectVTable = Interface::getOriginalObjectVTable(context, thisArgument);
  
  Type* pointerToArrayOfStrings = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToArrayOfStrings);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 1);
  GetElementPtrInst* typeArrayPointerI8 = IRWriter::createGetElementPtrInst(context, vTable, index);
  LoadInst* typeArrayI8 = IRWriter::newLoadInst(context, typeArrayPointerI8, "typeArrayI8");
  BitCastInst* arrayOfStrings =
  IRWriter::newBitCastInst(context, typeArrayI8, int8Type->getPointerTo()->getPointerTo());
  
  IRWriter::createBranch(context, whileCond);
  
  return arrayOfStrings;
}

LoadInst* InstanceOf::composeWhileConditionBlock(IRGenerationContext& context,
                                                 BasicBlock* whileCond,
                                                 BasicBlock* whileBody,
                                                 BasicBlock* returnFalse,
                                                 Value* iterator,
                                                 Value* arrayOfStrings) {
  Type* int8Type = Type::getInt8Ty(context.getLLVMContext());
  
  context.setBasicBlock(whileCond);
  
  LoadInst* iteratorLoaded = IRWriter::newLoadInst(context, iterator, "");
  Value* index[1];
  index[0] = iteratorLoaded;
  Value* stringPointerPointer = IRWriter::createGetElementPtrInst(context, arrayOfStrings, index);
  
  LoadInst* stringPointer = IRWriter::newLoadInst(context, stringPointerPointer, "stringPointer");
  
  Value* nullPointerValue = ConstantPointerNull::get(int8Type->getPointerTo());
  Value* checkStringIsNull =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, stringPointer, nullPointerValue, "cmpnull");
  
  IRWriter::createConditionalBranch(context, returnFalse, whileBody, checkStringIsNull);
  
  return stringPointer;
}

void InstanceOf::composeWhileBodyBlock(IRGenerationContext& context,
                                       BasicBlock* whileBody,
                                       BasicBlock* whileCond,
                                       BasicBlock* returnTrue,
                                       Value* iterator,
                                       Value* stringPointer,
                                       Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  context.setBasicBlock(whileBody);
  
  LoadInst* iteratorLoaded = IRWriter::newLoadInst(context, iterator, "");
  ConstantInt* one =  ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* increment = IRWriter::createBinaryOperator(context,
                                                    Instruction::Add,
                                                    iteratorLoaded,
                                                    one,
                                                    "inc");
  IRWriter::newStoreInst(context, increment, iterator);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  functionArguments++;
  Argument* compareToArgument = &*functionArguments;
  
  Value* doesTypeMatch =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, stringPointer, compareToArgument, "cmp");
  IRWriter::createConditionalBranch(context, returnTrue, whileCond, doesTypeMatch);
}

void InstanceOf::composeReturnFound(IRGenerationContext& context,
                                    BasicBlock* returnFound,
                                    Value* iterator) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  context.setBasicBlock(returnFound);
  LoadInst* iteratorLoaded = IRWriter::newLoadInst(context, iterator, "");
  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  
  Value* decrement =
  IRWriter::createBinaryOperator(context, Instruction::Sub, iteratorLoaded, one, "dec");
  IRWriter::createReturnInst(context, decrement);
}

void InstanceOf::composeReturnNotFound(IRGenerationContext& context, BasicBlock* returnNotFound) {
  LLVMContext& llvmContext = context.getLLVMContext();
  ConstantInt* negativeOne = ConstantInt::get(Type::getInt32Ty(llvmContext), -1);
  
  context.setBasicBlock(returnNotFound);
  IRWriter::createReturnInst(context, negativeOne);
}
