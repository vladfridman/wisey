//
//  TestReturnVoidStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReturnVoidStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ReturnVoidStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;
using ::testing::Test;

struct ReturnVoidStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  ReturnVoidStatementTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, "MShape");
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", fieldArguments));
    mModel = Model::newModel(modelFullName, structType);
    mModel->setFields(fields, 1u);

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);
    IConcreteObjectType::composeDestructorBody(mContext, mModel);
  }
  
  ~ReturnVoidStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ReturnVoidStatementTest, returnVoidTest) {
  BasicBlock *basicBlock = BasicBlock::Create(mLLVMContext, "entry");
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  
  ReturnVoidStatement returnVoidStatement;
  returnVoidStatement.generateIR(mContext);
  
  ASSERT_EQ(1ul, basicBlock->size());
  *mStringStream << basicBlock->front();
  ASSERT_STREQ(mStringStream->str().c_str(), "  ret void");
}

TEST_F(ReturnVoidStatementTest, ownerVariablesAreClearedTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID_TYPE);
  
  Type* structType = mModel->getLLVMType(mLLVMContext)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* fooPointer = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, fooMalloc, fooPointer);
  IVariable* foo = new LocalOwnerVariable("foo", mModel->getOwner(), fooPointer);
  mContext.getScopes().setVariable(foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* barPointer = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, barMalloc, barPointer);
  IVariable* bar = new LocalOwnerVariable("bar", mModel->getOwner(), barPointer);
  mContext.getScopes().setVariable(bar);
  
  ReturnVoidStatement returnStatement;
  
  returnStatement.generateIR(mContext);
  
  *mStringStream << *basicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%MShape* getelementptr ("
  "%MShape, %MShape* null, i32 1) to i64))"
  "\n  %0 = bitcast i8* %malloccall to %MShape*"
  "\n  %pointer = alloca %MShape*"
  "\n  store %MShape* %0, %MShape** %pointer"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%MShape* getelementptr "
  "(%MShape, %MShape* null, i32 1) to i64))"
  "\n  %1 = bitcast i8* %malloccall1 to %MShape*"
  "\n  %pointer2 = alloca %MShape*"
  "\n  store %MShape* %1, %MShape** %pointer2"
  "\n  %2 = load %MShape*, %MShape** %pointer2"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape(%MShape* %2)"
  "\n  %3 = load %MShape*, %MShape** %pointer"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape(%MShape* %3)"
  "\n  ret void\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReturnVoidStatementTest, referenceVariablesAreNotClearedTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID_TYPE);
  
  Type* structType = Type::getInt8Ty(mLLVMContext);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  IVariable* foo = new LocalReferenceVariable("foo", mModel, fooMalloc);
  mContext.getScopes().setVariable(foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  IVariable* bar = new LocalReferenceVariable("bar", mModel, barMalloc);
  mContext.getScopes().setVariable(bar);
  
  ReturnVoidStatement returnStatement;
  
  returnStatement.generateIR(mContext);
  
  ASSERT_EQ(3ul, basicBlock->size());
  *mStringStream << *basicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
  "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint "
  "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  ret void\n";
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}
