//
//  CastObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//


#include <llvm/IR/Constants.h>

#include "wisey/CastObjectFunction.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThrowStatement.hpp"

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
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition, 0);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, null, 0);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* instanceof = InstanceOfFunction::call(context, fromObjectValue, toTypeName);
  Value* originalObject = GetOriginalObjectFunction::call(context, fromObjectValue);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ICmpInst* compareLessThanZero =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, lessThanZero, notLessThanZero, compareLessThanZero, 0);
  
  context.setBasicBlock(lessThanZero);
  Value* fromTypeName = GetOriginalObjectNameFunction::call(context, fromObjectValue);

  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(packageExpression,
                                                                  Names::getCastExceptionName(),
                                                                  0);
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* fromTypeValue = new FakeExpression(fromTypeName, PrimitiveTypes::STRING);
  ObjectBuilderArgument* fromTypeArgument = new ObjectBuilderArgument("withFromType",
                                                                      fromTypeValue);
  FakeExpression* toTypeValue = new FakeExpression(toTypeName, PrimitiveTypes::STRING);
  ObjectBuilderArgument* toTypeArgument = new ObjectBuilderArgument("withToType", toTypeValue);
  objectBuilderArgumnetList.push_back(fromTypeArgument);
  objectBuilderArgumnetList.push_back(toTypeArgument);
  
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement throwStatement(objectBuilder, 0);
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);
  
  context.setBasicBlock(notLessThanZero);
  ICmpInst* compareGreaterThanZero =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_SGT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, moreThanZero, zeroExactly, compareGreaterThanZero, 0);
  
  context.setBasicBlock(moreThanZero);
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        Environment::getAddressSizeInBytes());
  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* offset = IRWriter::createBinaryOperator(context, Instruction::Sub, instanceof, one, "", 0);
  BitCastInst* bitcast =
  IRWriter::newBitCastInst(context, originalObject, int8Type->getPointerTo());
  Value* thunkBy = IRWriter::createBinaryOperator(context, Instruction::Mul, offset, bytes, "", 0);
  Value* index[1];
  index[0] = thunkBy;
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  IRWriter::createReturnInst(context, thunk, 0);
  
  context.setBasicBlock(zeroExactly);
  IRWriter::createReturnInst(context, originalObject, 0);
  
  context.getScopes().popScope(context, 0);

  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
