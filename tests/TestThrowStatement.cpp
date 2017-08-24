//
//  TestThrowStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Block.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ThrowStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mMockExpression;
  NiceMock<MockType> mMockType;
  Model* mCircleModel;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ThrowStatementTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mStringStream = new raw_string_ostream(mStringBuffer);

    string circleFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, circleFullName);
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = new Model(circleFullName, circleStructType);
    Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, circleFullName + ".name");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       circleFullName + ".name");
    mCircleModel->createRTTI(mContext);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
  }
  
  ~ThrowStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ThrowStatementTest, wrongExpressionTypeDeathTest) {
  Mock::AllowLeak(&mMockType);
  Mock::AllowLeak(mMockExpression);
  
  ON_CALL(mMockType, getTypeKind()).WillByDefault(Return(CONTROLLER_TYPE));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(&mMockType));
  ThrowStatement throwStatement(mMockExpression);
  
  EXPECT_EXIT(throwStatement.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Thrown object can only be a model");
}

TEST_F(ThrowStatementTest, modelExpressionTypeTest) {
  Constant* exceptionObject =
    ConstantPointerNull::get(mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(*mMockExpression, generateIR(_)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);

  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MCircle*, "
  "%systems.vos.wisey.compiler.tests.MCircle** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MCircle* %0 to i8*"
  "\n  %2 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.MCircle, "
    "%systems.vos.wisey.compiler.tests.MCircle* null, i32 1"
  "\n  %4 = ptrtoint %systems.vos.wisey.compiler.tests.MCircle* %3 to i64"
  "\n  %5 = call i8* @__cxa_allocate_exception(i64 %4)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %1, i64 %4, i32 4, i1 false)"
  "\n  call void @__cxa_throw(i8* %5, i8* %2, i8* null)"
  "\n  unreachable\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, heapVariablesAreClearedTest) {
  Type* structType = Type::getInt8Ty(mLLVMContext);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* fooPointer = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, fooMalloc, fooPointer);
  IVariable* foo = new HeapOwnerVariable("foo", mCircleModel->getOwner(), fooPointer);
  mContext.getScopes().setVariable(foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* barPointer = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, barMalloc, barPointer);
  IVariable* bar = new HeapOwnerVariable("bar", mCircleModel->getOwner(), barPointer);
  mContext.getScopes().setVariable(bar);
  
  Constant* exceptionObject =
    ConstantPointerNull::get(mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(*mMockExpression, generateIR(_)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);
  
  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
    "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %pointer = alloca i8*"
  "\n  store i8* %malloccall, i8** %pointer"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint "
    "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %pointer2 = alloca i8*"
  "\n  store i8* %malloccall1, i8** %pointer2"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MCircle*, "
    "%systems.vos.wisey.compiler.tests.MCircle** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MCircle* %0 to i8*"
  "\n  %2 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.MCircle, "
    "%systems.vos.wisey.compiler.tests.MCircle* null, i32 1"
  "\n  %4 = ptrtoint %systems.vos.wisey.compiler.tests.MCircle* %3 to i64"
  "\n  %5 = call i8* @__cxa_allocate_exception(i64 %4)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %1, i64 %4, i32 4, i1 false)"
  "\n  %modelOwnerToFree = load i8*, i8** %pointer2"
  "\n  tail call void @free(i8* %modelOwnerToFree)"
  "\n  %modelOwnerToFree3 = load i8*, i8** %pointer"
  "\n  tail call void @free(i8* %modelOwnerToFree3)"
  "\n  call void @__cxa_throw(i8* %5, i8* %2, i8* null)"
  "\n  unreachable\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, heapVariablesAreNotClearedTest) {
  Type* structType = Type::getInt8Ty(mLLVMContext);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  IVariable* foo = new HeapReferenceVariable("foo", mCircleModel, fooMalloc);
  mContext.getScopes().setVariable(foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  IVariable* bar = new HeapReferenceVariable("bar", mCircleModel, barMalloc);
  mContext.getScopes().setVariable(bar);
  
  Constant* exceptionObject =
  ConstantPointerNull::get(mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(*mMockExpression, generateIR(_)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);
  
  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
  "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint "
  "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MCircle*, "
  "%systems.vos.wisey.compiler.tests.MCircle** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MCircle* %0 to i8*"
  "\n  %2 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.MCircle, "
  "%systems.vos.wisey.compiler.tests.MCircle* null, i32 1"
  "\n  %4 = ptrtoint %systems.vos.wisey.compiler.tests.MCircle* %3 to i64"
  "\n  %5 = call i8* @__cxa_allocate_exception(i64 %4)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %1, i64 %4, i32 4, i1 false)"
  "\n  call void @__cxa_throw(i8* %5, i8* %2, i8* null)"
  "\n  unreachable\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
