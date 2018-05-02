//
//  CheckForModelFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CheckForModelFunction.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/IsModelFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Function* CheckForModelFunction::get(IRGenerationContext& context) {
  Function* function = context.getModule()->getFunction(getName());
  
  if (function) {
    return function;
  }
  
  function = define(context);
  context.addComposingCallback0Objects(compose, function);
  
  return function;
}

void CheckForModelFunction::call(IRGenerationContext& context, Value* value) {
  llvm::PointerType* int8PointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8PointerType);
  
  Function* function = get(context);
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  IRWriter::createInvokeInst(context, function, arguments, "", 0);
}

string CheckForModelFunction::getName() {
  return "__checkForModel";
}

Function* CheckForModelFunction::define(IRGenerationContext& context) {
  return Function::Create(getLLVMFunctionType(context)->getLLVMType(context),
                          GlobalValue::ExternalLinkage,
                          getName(),
                          context.getModule());
}

LLVMFunctionType* CheckForModelFunction::getLLVMFunctionType(IRGenerationContext& context) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType());
  
  return context.getLLVMFunctionType(LLVMPrimitiveTypes::VOID, argumentTypes);
}

void CheckForModelFunction::compose(IRGenerationContext& context, Function* function) {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* object = &*llvmArguments;
  object->setName("object");
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* returnBlock = BasicBlock::Create(llvmContext, "return.block", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  BasicBlock* ifNotModelBlock = BasicBlock::Create(llvmContext, "if.not.model", function);
  
  context.setBasicBlock(entryBlock);
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(llvmContext)->getPointerTo());
  Value* condition =
  IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, object, null, "isNull");
  IRWriter::createConditionalBranch(context, returnBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(returnBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  Value* isModel = IsModelFunction::call(context, object);
  IRWriter::createConditionalBranch(context, returnBlock, ifNotModelBlock, isModel);
  
  context.setBasicBlock(ifNotModelBlock);
  Value* objectName = GetOriginalObjectNameFunction::call(context, object);
  PackageType* packageType = context.getPackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(packageExpression,
                                                                  Names::getCastExceptionName(),
                                                                  0);
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* fromTypeValue = new FakeExpression(objectName, PrimitiveTypes::STRING);
  ObjectBuilderArgument* fromTypeArgument = new ObjectBuilderArgument("withFromType",
                                                                      fromTypeValue);
  StringLiteral* modelStringLiteral = new StringLiteral("model", 0);
  ObjectBuilderArgument* toTypeArgument = new ObjectBuilderArgument("withToType",
                                                                    modelStringLiteral);
  objectBuilderArgumnetList.push_back(fromTypeArgument);
  objectBuilderArgumnetList.push_back(toTypeArgument);
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement->generateIR(context);
  context.getScopes().popScope(context, 0);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context));
}
