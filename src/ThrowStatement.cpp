//
//  ThrowStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/Composer.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ThrowStatement::ThrowStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

ThrowStatement::~ThrowStatement() {
  delete mExpression;
}

Value* ThrowStatement::generateIR(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  TypeKind typeKind = expressionType->getTypeKind();
  if (typeKind != MODEL_TYPE && typeKind != MODEL_OWNER_TYPE) {
    Log::e("Thrown object can only be a model");
    exit(1);
  }
  Model* model = typeKind == MODEL_OWNER_TYPE
  ? (Model*) ((IObjectOwnerType*) expressionType)->getObject()
  : (Model*) expressionType;

  LLVMContext& llvmContext = context.getLLVMContext();
  context.getScopes().getScope()->addException(model);

  if (mLine) {
    Composer::pushCallStack(context, mLine);
  }
  
  GlobalVariable* rtti = context.getModule()->getNamedGlobal(model->getRTTIVariableName());

  PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* exceptionObject = mExpression->generateIR(context, IR_GENERATION_NORMAL);
  
  BitCastInst* expressionValueBitcast =
    IRWriter::newBitCastInst(context, exceptionObject, int8PointerType);
  BitCastInst* rttiBitcast = IRWriter::newBitCastInst(context, rtti, int8PointerType);

  Value* modelSize = model->getSize(context);
  vector<Value*> allocateExceptionArguments;
  allocateExceptionArguments.push_back(modelSize);
  Function* allocateExceptionFunction = IntrinsicFunctions::getAllocateExceptionFunction(context);
  Value* exceptionAlloca = IRWriter::createCallInst(context,
                                                    allocateExceptionFunction,
                                                    allocateExceptionArguments,
                                                    "");

  vector<Value*> memCopyArguments;
  unsigned int memoryAlignment = Environment::getDefaultMemoryAllignment();
  memCopyArguments.push_back(exceptionAlloca);
  memCopyArguments.push_back(expressionValueBitcast);
  memCopyArguments.push_back(modelSize);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), memoryAlignment));
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memCopyFunction = IntrinsicFunctions::getMemCopyFunction(context);
  IRWriter::createCallInst(context, memCopyFunction, memCopyArguments, "");

  vector<Value*> throwArguments;
  throwArguments.push_back(exceptionAlloca);
  throwArguments.push_back(rttiBitcast);
  throwArguments.push_back(ConstantPointerNull::get(int8PointerType));

  Function* throwFunction = IntrinsicFunctions::getThrowFunction(context);
  
  Value* result = IRWriter::createInvokeInst(context, throwFunction, throwArguments, "");

  IRWriter::newUnreachableInst(context);

  if (mLine) {
    Composer::popCallStack(context);
  }
  
  return result;
}
