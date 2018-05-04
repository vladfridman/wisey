//
//  Catch.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Catch.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Catch::Catch(IModelTypeSpecifier* modelTypeSpecifier,
             string identifier,
             CompoundStatement* compoundStatement,
             int line) :
mModelTypeSpecifier(modelTypeSpecifier),
mIdentifier(identifier),
mCompoundStatement(compoundStatement),
mLine(line) { }

Catch::~Catch() {
  delete mModelTypeSpecifier;
  delete mCompoundStatement;
}

const ModelOwner* Catch::getType(IRGenerationContext& context) const {
  Model* model = mModelTypeSpecifier->getType(context);
  return (const ModelOwner*) model->getOwner();
}

bool Catch::generateIR(IRGenerationContext& context,
                       Value* wrappedException,
                       BasicBlock* catchBlock,
                       BasicBlock* exceptionContinueBlock) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function* beginCatchFunction = IntrinsicFunctions::getBeginCatchFunction(context);
  Function* endCatchFunction = IntrinsicFunctions::getEndCatchFunction(context);

  context.getScopes().pushScope();
  context.setBasicBlock(catchBlock);
  
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  CallInst* exceptionPointer = IRWriter::createCallInst(context, beginCatchFunction, arguments, "");
  
  const Model* exceptionType = getType(context)->getReference();
  llvm::PointerType* exceptionLLVMType = exceptionType->getLLVMType(context);
  Type* exceptionStructLLVMType = exceptionLLVMType->getPointerElementType();
  llvm::Constant* modelSize = ConstantExpr::getSizeOf(exceptionStructLLVMType);
  llvm::Constant* refCounterSize = ConstantExpr::getSizeOf(Type::getInt64Ty(llvmContext));
  llvm::Constant* mallocSize = ConstantExpr::getAdd(modelSize, refCounterSize);
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context, exceptionType, "");

  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  BitCastInst* mallocBitcast = IRWriter::newBitCastInst(context, malloc, int8PointerType);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), -Environment::getAddressSizeInBytes());
  Value* modelShellStart = IRWriter::createGetElementPtrInst(context, mallocBitcast, index);

  vector<Value*> memCopyArguments;
  unsigned int memoryAlignment = Environment::getDefaultMemoryAllignment();
  memCopyArguments.push_back(modelShellStart);
  memCopyArguments.push_back(exceptionPointer);
  memCopyArguments.push_back(mallocSize);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), memoryAlignment));
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memCopyFunction = IntrinsicFunctions::getMemCopyFunction(context);
  IRWriter::createCallInst(context, memCopyFunction, memCopyArguments, "");

  vector<Value*> endCatchArguments;
  IRWriter::createCallInst(context, endCatchFunction, endCatchArguments, "");
  
  Value* pointer = IRWriter::newAllocaInst(context, malloc->getType(), "exceptionPointer");
  IRWriter::newStoreInst(context, malloc, pointer);
  
  IVariable* exceptionVariable = new ParameterOwnerVariable(mIdentifier,
                                                            exceptionType->getOwner(),
                                                            pointer,
                                                            mModelTypeSpecifier->getLine());
  context.getScopes().getScope()->setVariable(mIdentifier, exceptionVariable);
  mCompoundStatement->generateIR(context);
  context.getScopes().popScope(context, mLine);
  
  bool hasTerminator = context.getBasicBlock()->getTerminator() != NULL;

  if (!hasTerminator) {
    IRWriter::createBranch(context, exceptionContinueBlock, mLine);
  }
  
  return hasTerminator;
}
