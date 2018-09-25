//
//  ThrowStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "Composer.hpp"
#include "Environment.hpp"
#include "IConcreteObjectType.hpp"
#include "IntrinsicFunctions.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"
#include "ModelOwner.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ThrowStatement::ThrowStatement(IExpression* expression, int line) :
mExpression(expression), mLine(line) { }

ThrowStatement::~ThrowStatement() {
  delete mExpression;
}

void ThrowStatement::generateIR(IRGenerationContext& context) const {
  const IType* expressionType = mExpression->getType(context);
  if (!expressionType->isModel() || !expressionType->isOwner()) {
    context.reportError(mLine, "Thrown object can only be a model owner");
    throw 1;
  }
  const Model* model = ((const ModelOwner*) expressionType)->getReference();

  string exceptionInterfaceName = Names::getExceptionInterfaceFullName();
  const Interface* exceptionInterface = context.getInterface(exceptionInterfaceName, mLine);
  if (IConcreteObjectType::getInterfaceIndex(model, exceptionInterface) < 0) {
    context.reportError(mLine, "Exceptions must implement " + exceptionInterfaceName);
    throw 1;
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  context.getScopes().getScope()->addException(model, mLine);

  GlobalVariable* rtti = context.getModule()->getNamedGlobal(model->getRTTIVariableName());

  llvm::PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* exceptionObject = mExpression->generateIR(context, expressionType);
  BitCastInst* rttiBitcast = IRWriter::newBitCastInst(context, rtti, int8PointerType);

  BitCastInst* expressionValueBitcast =
    IRWriter::newBitCastInst(context, exceptionObject, int8PointerType);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), -Environment::getAddressSizeInBytes());
  Value* excpetionShellStart =
  IRWriter::createGetElementPtrInst(context, expressionValueBitcast, index);

  llvm::Constant* modelSize = ConstantExpr::getSizeOf(model->getLLVMType(context)->
                                                      getPointerElementType());
  llvm::Constant* refCounterSize = ConstantExpr::getSizeOf(Type::getInt64Ty(llvmContext));
  llvm::Constant* mallocSize = ConstantExpr::getAdd(modelSize, refCounterSize);
  
  vector<Value*> allocateExceptionArguments;
  allocateExceptionArguments.push_back(mallocSize);
  Function* allocateExceptionFunction = IntrinsicFunctions::getAllocateExceptionFunction(context);
  Value* exceptionAlloca = IRWriter::createCallInst(context,
                                                    allocateExceptionFunction,
                                                    allocateExceptionArguments,
                                                    "");

  vector<Value*> memCopyArguments;
  memCopyArguments.push_back(exceptionAlloca);
  memCopyArguments.push_back(excpetionShellStart);
  memCopyArguments.push_back(mallocSize);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memCopyFunction = IntrinsicFunctions::getMemCopyFunction(context);
  IRWriter::createCallInst(context, memCopyFunction, memCopyArguments, "");
  IRWriter::createFree(context, excpetionShellStart);
  
  Composer::setLineNumber(context, mLine);

  vector<Value*> throwArguments;
  throwArguments.push_back(exceptionAlloca);
  throwArguments.push_back(rttiBitcast);
  throwArguments.push_back(ConstantPointerNull::get(int8PointerType));

  Function* throwFunction = IntrinsicFunctions::getThrowFunction(context);
  
  IRWriter::createInvokeInst(context, throwFunction, throwArguments, "", mLine);

  IRWriter::newUnreachableInst(context);
}
