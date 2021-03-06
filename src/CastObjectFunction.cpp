//
//  CastObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//


#include <llvm/IR/Constants.h>

#include "CastObjectFunction.hpp"
#include "GetOriginalObjectNameFunction.hpp"
#include "Environment.hpp"
#include "FakeExpression.hpp"
#include "GetOriginalObjectFunction.hpp"
#include "HeapBuilder.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "InstanceOfFunction.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "ModelTypeSpecifier.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* CastObjectFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

Value* CastObjectFunction::call(IRGenerationContext& context,
                                Value* fromValue,
                                const IObjectType* toObjectType,
                                int line) {
  
  llvm::Constant* toTypeName = IObjectType::getObjectNamePointer(toObjectType, context);
  Type* int8PointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* fromValueBitcast = fromValue->getType() != int8PointerType
  ? IRWriter::newBitCastInst(context, fromValue, int8PointerType)
  : fromValue;

  vector<Value*> arguments;
  arguments.push_back(fromValueBitcast);
  arguments.push_back(toTypeName);
 
  Function* function = get(context);
  Value* value = IRWriter::createInvokeInst(context, function, arguments, "", line);
  
  return IRWriter::newBitCastInst(context, value, toObjectType->getLLVMType(context));
}

string CastObjectFunction::getName() {
  return "__castObject";
}

Function* CastObjectFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* CastObjectFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));

  return context.getLLVMFunctionType(LLVMPrimitiveTypes::I8->getPointerType(context, 0),
                                     argumentTypes);
}

void CastObjectFunction::compose(IRGenerationContext& context, llvm::Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  context.getScopes().pushScope();
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Value* fromObjectValue = &*functionArguments;
  fromObjectValue->setName("fromObjectValue");
  functionArguments++;
  Value* toTypeName = &*functionArguments;
  toTypeName->setName("toTypeName");
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  BasicBlock* lessThanZero = BasicBlock::Create(llvmContext, "less.than.zero", function);
  BasicBlock* notLessThanZero = BasicBlock::Create(llvmContext, "not.less.than.zero", function);
  BasicBlock* moreThanZero = BasicBlock::Create(llvmContext, "more.than.zero", function);
  BasicBlock* zeroExactly = BasicBlock::Create(llvmContext, "zero.exactly", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* condition =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, fromObjectValue, null, "isNull");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, null);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* instanceof = InstanceOfFunction::call(context, fromObjectValue, toTypeName);
  Value* originalObject = GetOriginalObjectFunction::call(context, fromObjectValue);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ICmpInst* compareLessThanZero =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, lessThanZero, notLessThanZero, compareLessThanZero);
  
  context.setBasicBlock(lessThanZero);
  Value* fromTypeName = GetOriginalObjectNameFunction::call(context, fromObjectValue);

  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(packageExpression,
                                                                  Names::getCastExceptionName(),
                                                                  0);
  BuilderArgumentList builderArgumnetList;
  FakeExpression* fromTypeValue = new FakeExpression(fromTypeName, PrimitiveTypes::STRING);
  BuilderArgument* fromTypeArgument = new BuilderArgument("withFromType",
                                                                      fromTypeValue);
  FakeExpression* toTypeValue = new FakeExpression(toTypeName, PrimitiveTypes::STRING);
  BuilderArgument* toTypeArgument = new BuilderArgument("withToType", toTypeValue);
  builderArgumnetList.push_back(fromTypeArgument);
  builderArgumnetList.push_back(toTypeArgument);
  
  HeapBuilder* heapBuilder = new HeapBuilder(modelTypeSpecifier, builderArgumnetList, 0);
  ThrowStatement throwStatement(heapBuilder, 0);
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  context.setBasicBlock(notLessThanZero);
  ICmpInst* compareGreaterThanZero =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_SGT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, moreThanZero, zeroExactly, compareGreaterThanZero);
  
  context.setBasicBlock(moreThanZero);
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        Environment::getAddressSizeInBytes());
  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* offset = IRWriter::createBinaryOperator(context, Instruction::Sub, instanceof, one, "");
  BitCastInst* bitcast =
  IRWriter::newBitCastInst(context, originalObject, int8Type->getPointerTo());
  Value* thunkBy = IRWriter::createBinaryOperator(context, Instruction::Mul, offset, bytes, "");
  Value* index[1];
  index[0] = thunkBy;
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  IRWriter::createReturnInst(context, thunk);
  
  context.setBasicBlock(zeroExactly);
  IRWriter::createReturnInst(context, originalObject);
  
  context.getScopes().popScope(context, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
