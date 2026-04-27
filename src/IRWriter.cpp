//
//  IRWriter.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/Support/Signals.h>

#include "Cleanup.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ReturnInst* IRWriter::createReturnInst(IRGenerationContext& context, Value* returnValue) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if (currentBlock->getTerminator()) {
    return NULL;
  }
  
  currentBlock = context.getBasicBlock();
  
  if (currentBlock->getTerminator()) {
    return NULL;
  }

  return ReturnInst::Create(context.getLLVMContext(), returnValue, currentBlock);
}

BranchInst* IRWriter::createBranch(IRGenerationContext& context, BasicBlock* toBlock) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return BranchInst::Create(toBlock, currentBlock);
}

BranchInst* IRWriter::createConditionalBranch(IRGenerationContext& context,
                                              BasicBlock* ifTrueBlock,
                                              BasicBlock* ifFalseBlock,
                                              Value* condition) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return BranchInst::Create(ifTrueBlock, ifFalseBlock, condition, currentBlock);
}

BinaryOperator* IRWriter::createBinaryOperator(IRGenerationContext& context,
                                               Instruction::BinaryOps instruction,
                                               Value* leftValue,
                                               Value* rightValue,
                                               string llvmVariableName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return BinaryOperator::Create(instruction, leftValue, rightValue, llvmVariableName, currentBlock);
}

CallInst* IRWriter::createCallInst(IRGenerationContext& context,
                                   Function* function,
                                   vector<Value*> arguments,
                                   string resultName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return CallInst::Create(function, arguments, resultName, currentBlock);
}

CallInst* IRWriter::createCallInst(IRGenerationContext& context,
                                   FunctionType* functionType,
                                   Value* function,
                                   vector<Value*> arguments,
                                   string resultName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return CallInst::Create(functionType, function, arguments, resultName, currentBlock);
}

InvokeInst* IRWriter::createInvokeInst(IRGenerationContext& context,
                                       Function* function,
                                       vector<Value*> arguments,
                                       string resultName,
                                       int line) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  BasicBlock* landingPadBlock = context.getScopes().getLandingPadBlock(context, line);

  BasicBlock* invokeContinueBlock = BasicBlock::Create(context.getLLVMContext(),
                                                       "invoke.continue",
                                                       currentBlock->getParent());
  InvokeInst* invokeInst = InvokeInst::Create(function,
                                              invokeContinueBlock,
                                              landingPadBlock,
                                              arguments,
                                              resultName,
                                              currentBlock);

  context.setBasicBlock(invokeContinueBlock);

  return invokeInst;
}

InvokeInst* IRWriter::createInvokeInst(IRGenerationContext& context,
                                       FunctionType* functionType,
                                       Value* function,
                                       vector<Value*> arguments,
                                       string resultName,
                                       int line) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  BasicBlock* landingPadBlock = context.getScopes().getLandingPadBlock(context, line);

  BasicBlock* invokeContinueBlock = BasicBlock::Create(context.getLLVMContext(),
                                                       "invoke.continue",
                                                       currentBlock->getParent());
  InvokeInst* invokeInst = InvokeInst::Create(functionType,
                                              function,
                                              invokeContinueBlock,
                                              landingPadBlock,
                                              arguments,
                                              resultName,
                                              currentBlock);

  context.setBasicBlock(invokeContinueBlock);

  return invokeInst;
}

Instruction* IRWriter::createMalloc(IRGenerationContext& context,
                                    Type* structType,
                                    Value* allocSize,
                                    Value* arraySize,
                                    string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  LLVMContext& llvmContext = context.getLLVMContext();
  Module* module = currentBlock->getModule();
  Type* int64Ty = Type::getInt64Ty(llvmContext);
  Type* int8PtrTy = PointerType::get(llvmContext, 0);

  FunctionCallee mallocFn = module->getOrInsertFunction("malloc", int8PtrTy, int64Ty);

  Value* totalSize;
  auto* arraySizeConst = dyn_cast<ConstantInt>(arraySize);
  if (arraySizeConst && arraySizeConst->isOne()) {
    totalSize = allocSize;
  } else {
    totalSize = BinaryOperator::CreateMul(allocSize, arraySize, "", currentBlock);
  }

  CallInst* mallocCall = CallInst::Create(mallocFn, {totalSize}, "malloccall", currentBlock);
  mallocCall->setTailCall();

  if (!variableName.empty()) {
    mallocCall->setName(variableName);
  }
  return mallocCall;
}

Instruction* IRWriter::createFree(IRGenerationContext& context, Value* value) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  LLVMContext& llvmContext = context.getLLVMContext();
  Module* module = currentBlock->getModule();
  Type* voidTy = Type::getVoidTy(llvmContext);
  Type* int8PtrTy = PointerType::get(llvmContext, 0);

  FunctionCallee freeFn = module->getOrInsertFunction("free", voidTy, int8PtrTy);
  CallInst* freeCall = CallInst::Create(freeFn, {value}, "", currentBlock);
  freeCall->setTailCall();
  return freeCall;
}

/**
 * Recover the pointee type from a value's producer. Used by the legacy
 * (no-type) overloads of newLoadInst/createGetElementPtrInst as a stop-gap
 * during the opaque-pointer migration. Returns nullptr if the type cannot
 * be recovered (e.g. bitcast pointers — opaque pointers erase pointee type).
 */
static Type* recoverPointeeType(Value* pointer) {
  if (auto* alloca = llvm::dyn_cast<AllocaInst>(pointer)) {
    return alloca->getAllocatedType();
  }
  if (auto* gep = llvm::dyn_cast<GetElementPtrInst>(pointer)) {
    return gep->getResultElementType();
  }
  if (auto* gv = llvm::dyn_cast<GlobalVariable>(pointer)) {
    return gv->getValueType();
  }
  // Load/Bitcast/Call: under opaque pointers the pointee type is not
  // recoverable from the IR. Sites loading or GEPing through these must
  // use the explicit-type overload.
  return nullptr;
}

GetElementPtrInst* IRWriter::createGetElementPtrInst(IRGenerationContext& context,
                                                     Value* value,
                                                     ArrayRef<Value *> index) {
  Type* elementType = recoverPointeeType(value);
  if (!elementType) {
    llvm::errs() << "GEP recover FAIL: ";
    value->print(llvm::errs());
    llvm::errs() << "\n";
    llvm::sys::PrintStackTrace(llvm::errs());
    abort();
  }
  return createGetElementPtrInst(context, elementType, value, index);
}

GetElementPtrInst* IRWriter::createGetElementPtrInst(IRGenerationContext& context,
                                                     Type* elementType,
                                                     Value* value,
                                                     ArrayRef<Value *> index) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return GetElementPtrInst::Create(elementType,
                                   value,
                                   index,
                                   "",
                                   currentBlock);
}

BitCastInst* IRWriter::newBitCastInst(IRGenerationContext& context, Value* value, Type* type) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new BitCastInst(value, type, "", currentBlock);
}

StoreInst* IRWriter::newStoreInst(IRGenerationContext& context, Value* value, Value* pointer) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new StoreInst(value, pointer, currentBlock);
}

AllocaInst* IRWriter::newAllocaInst(IRGenerationContext& context, Type* type, string variableName) {
  BasicBlock* currentBlock = context.getDeclarationsBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new AllocaInst(type, 0, variableName, currentBlock);
}

LoadInst* IRWriter::newLoadInst(IRGenerationContext& context,
                                Value* pointer,
                                string variableName) {
  Type* loadType = recoverPointeeType(pointer);
  if (!loadType) {
    llvm::errs() << "LOAD recover FAIL: ";
    pointer->print(llvm::errs());
    llvm::errs() << "\n";
    llvm::sys::PrintStackTrace(llvm::errs());
    abort();
  }
  return newLoadInst(context, loadType, pointer, variableName);
}

LoadInst* IRWriter::newLoadInst(IRGenerationContext& context,
                                Type* loadType,
                                Value* pointer,
                                string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();

  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return new LoadInst(loadType, pointer, variableName, currentBlock);
}

CastInst* IRWriter::createSExtOrBitCast(IRGenerationContext& context,
                                        Value* fromValue,
                                        Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return CastInst::CreateSExtOrBitCast(fromValue, toLLVMType, "conv", currentBlock);
}

CastInst* IRWriter::createZExtOrBitCast(IRGenerationContext& context,
                                        Value* fromValue,
                                        Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return CastInst::CreateZExtOrBitCast(fromValue, toLLVMType, "conv", currentBlock);
}

TruncInst* IRWriter::newTruncInst(IRGenerationContext& context,
                                  Value* fromValue,
                                  Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new TruncInst(fromValue, toLLVMType, "conv", currentBlock);
}

FPTruncInst* IRWriter::newFPTruncInst(IRGenerationContext& context,
                                      Value* fromValue,
                                      Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new FPTruncInst(fromValue, toLLVMType, "conv", currentBlock);
}

FPExtInst* IRWriter::newFPExtInst(IRGenerationContext& context,
                                  Value* fromValue,
                                  Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new FPExtInst(fromValue, toLLVMType, "conv", currentBlock);
}

SIToFPInst* IRWriter::newSIToFPInst(IRGenerationContext& context,
                                   Value* fromValue,
                                   Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new SIToFPInst(fromValue, toLLVMType, "conv", currentBlock);
}

FPToSIInst* IRWriter::newFPToSIInst(IRGenerationContext& context,
                                    Value* fromValue,
                                    Type* toLLVMType) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new FPToSIInst(fromValue, toLLVMType, "conv", currentBlock);
}

PtrToIntInst* IRWriter::newPtrToIntInst(IRGenerationContext& context,
                                        Value* fromValue,
                                        Type* toLLVMType,
                                        string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new PtrToIntInst(fromValue, toLLVMType, variableName, currentBlock);
}

PHINode* IRWriter::createPhiNode(IRGenerationContext& context,
                                 Type* type,
                                 string variableName,
                                 Value* condTrueValue,
                                 BasicBlock* condTrueBasicBlock,
                                 Value* condFalseValue,
                                 BasicBlock* condFalseBasicBlock) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }

  PHINode* phiNode = PHINode::Create(type, 2, variableName, currentBlock);
  phiNode->addIncoming(condTrueValue, condTrueBasicBlock);
  phiNode->addIncoming(condFalseValue, condFalseBasicBlock);
  
  return phiNode;
}

FCmpInst* IRWriter::newFCmpInst(IRGenerationContext& context,
                                FCmpInst::Predicate predicate,
                                Value* leftValue,
                                Value* rightValue,
                                string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new FCmpInst(currentBlock, predicate, leftValue, rightValue, variableName);
}

ICmpInst* IRWriter::newICmpInst(IRGenerationContext& context,
                                ICmpInst::Predicate predicate,
                                Value* leftValue,
                                Value* rightValue,
                                string variableName) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }

  return new ICmpInst(currentBlock, predicate, leftValue, rightValue, variableName);
}

ResumeInst* IRWriter::createResumeInst(IRGenerationContext& context,
                                       LandingPadInst* landingPadInst) {
  BasicBlock* currentBlock = context.getBasicBlock();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return ResumeInst::Create(landingPadInst, currentBlock);
}

UnreachableInst* IRWriter::newUnreachableInst(IRGenerationContext& context) {

  BasicBlock* currentBlock = context.getBasicBlock();
  string test = currentBlock->getName().str();
  
  if(currentBlock->getTerminator()) {
    return NULL;
  }
  
  return new UnreachableInst(context.getLLVMContext(), currentBlock);
}
