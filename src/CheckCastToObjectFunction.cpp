//
//  CheckCastToObjectFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Block.hpp"
#include "wisey/CheckCastToObjectFunction.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/IsObjectFunction.hpp"
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
  Value* letter = ConstantInt::get(Type::getInt8Ty(context.getLLVMContext()), 77);
  return call(context, object, letter, getTypeName(context, "model"));
}

Value* CheckCastToObjectFunction::getTypeName(IRGenerationContext& context, string name) {
  LLVMContext& llvmContext = context.getLLVMContext();

  string variableName = "__" + name;
  GlobalVariable* variable = context.getModule()->getGlobalVariable(variableName.c_str());
  if (!variable) {
    llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, name);
    variable = new GlobalVariable(*context.getModule(),
                                  stringConstant->getType(),
                                  true,
                                  GlobalValue::InternalLinkage,
                                  stringConstant,
                                  variableName.c_str());
  }
  llvm::Constant* zero = llvm::Constant::getNullValue(IntegerType::getInt32Ty(llvmContext));
  llvm::Constant* indices[] = {zero, zero};
  
  return ConstantExpr::getGetElementPtr(NULL, variable, indices, true);
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
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  FakeExpression* fromTypeValue = new FakeExpression(objectName, PrimitiveTypes::STRING);
  ObjectBuilderArgument* fromTypeArgument = new ObjectBuilderArgument("withFromType",
                                                                      fromTypeValue);
  FakeExpression* toTypeStringLiteral = new FakeExpression(toType, PrimitiveTypes::STRING);
  ObjectBuilderArgument* toTypeArgument = new ObjectBuilderArgument("withToType",
                                                                    toTypeStringLiteral);
  objectBuilderArgumnetList.push_back(fromTypeArgument);
  objectBuilderArgumnetList.push_back(toTypeArgument);
  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement* throwStatement = new ThrowStatement(objectBuilder, 0);
  
  context.getScopes().pushScope();
  throwStatement->generateIR(context);
  context.getScopes().popScope(context, 0);
  
  context.registerLLVMInternalFunctionNamedType(getName(), getLLVMFunctionType(context), 0);
}
