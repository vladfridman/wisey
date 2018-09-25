//
//  CheckCastToObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "Block.hpp"
#include "CheckCastToObjectFunction.hpp"
#include "CompoundStatement.hpp"
#include "Composer.hpp"
#include "FakeExpression.hpp"
#include "GetOriginalObjectNameFunction.hpp"
#include "HeapBuilder.hpp"
#include "IRWriter.hpp"
#include "IConcreteObjectType.hpp"
#include "IfStatement.hpp"
#include "IsObjectFunction.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "ModelTypeSpecifier.hpp"
#include "Names.hpp"
#include "ObjectKindGlobal.hpp"
#include "PrimitiveTypes.hpp"
#include "PrintOutStatement.hpp"
#include "StringLiteral.hpp"
#include "ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* CheckCastToObjectFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void CheckCastToObjectFunction::callCheckCastToModel(IRGenerationContext& context, Value* object) {
  Value* letter = ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()),
                                   IConcreteObjectType::MODEL_FIRST_LETTER_ASCII_CODE);
  return call(context, object, letter, ObjectKindGlobal::getModel(context));
}

void CheckCastToObjectFunction::callCheckCastToNode(IRGenerationContext& context, Value* object) {
  Value* letter = ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()),
                                   IConcreteObjectType::NODE_FIRST_LETTER_ASCII_CODE);
  return call(context, object, letter, ObjectKindGlobal::getNode(context));
}

void CheckCastToObjectFunction::call(IRGenerationContext& context,
                                     Value* object,
                                     Value* letter,
                                     Value* toTypeName) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  llvm::PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, object, int8PointerType);

  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  arguments.push_back(letter);
  arguments.push_back(toTypeName);

  IRWriter::createInvokeInst(context, function, arguments, "", 0);
}

string CheckCastToObjectFunction::getName() {
  return "__checkCastToObject";
}

Function* CheckCastToObjectFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* CheckCastToObjectFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));
  argumentTypes.push_back(LLVMPrimitiveTypes::I8);
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType(context, 0));

  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void CheckCastToObjectFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* object = &*llvmArguments;
  object->setName("object");
  llvmArguments++;
  llvm::Argument* letter = &*llvmArguments;
  letter->setName("letter");
  llvmArguments++;
  llvm::Argument* toType = &*llvmArguments;
  toType->setName("toType");

  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* returnBlock = BasicBlock::Create(llvmContext, "return.block", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  BasicBlock* ifNotObjectBlock = BasicBlock::Create(llvmContext, "if.not.object", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* condition =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, object, null, "isNull");
  IRWriter::createConditionalBranch(context, returnBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(returnBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* isModel = IsObjectFunction::call(context, object, letter);
  IRWriter::createConditionalBranch(context, returnBlock, ifNotObjectBlock, isModel);
  
  context.setBasicBlock(ifNotObjectBlock);
  Value* objectName = GetOriginalObjectNameFunction::call(context, object);
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(packageExpression,
                                                                  Names::getCastExceptionName(),
                                                                  0);
  BuilderArgumentList builderArgumnetList;
  FakeExpression* fromTypeValue = new FakeExpression(objectName, PrimitiveTypes::STRING);
  BuilderArgument* fromTypeArgument = new BuilderArgument("withFromType", fromTypeValue);
  FakeExpression* toTypeStringLiteral = new FakeExpression(toType, PrimitiveTypes::STRING);
  BuilderArgument* toTypeArgument = new BuilderArgument("withToType", toTypeStringLiteral);
  builderArgumnetList.push_back(fromTypeArgument);
  builderArgumnetList.push_back(toTypeArgument);
  HeapBuilder* heapBuilder = new HeapBuilder(modelTypeSpecifier, builderArgumnetList, 0);
  ThrowStatement* throwStatement = new ThrowStatement(heapBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement->generateIR(context);
  context.getScopes().popScope(context, 0);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
