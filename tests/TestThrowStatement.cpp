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
#include "TestPrefix.hpp"
#include "wisey/Block.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
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
  Function* mFunction;
  
  ThrowStatementTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mStringStream = new raw_string_ostream(mStringBuffer);

    string circleFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, circleFullName);
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, circleFullName, circleStructType);
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  circleFullName + ".name");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       circleFullName + ".name");
    mCircleModel->createRTTI(mContext);

    IConcreteObjectType::generateNameGlobal(mContext, mCircleModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mCircleModel);
    IConcreteObjectType::generateVTable(mContext, mCircleModel);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
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
  ThrowStatement throwStatement(mMockExpression, 0);
  
  EXPECT_EXIT(throwStatement.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Thrown object can only be a model");
}

TEST_F(ThrowStatementTest, modelExpressionTypeTest) {
  llvm::Constant* exceptionObject =
    ConstantPointerNull::get(mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression, 0);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);

  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MCircle* null to i8*"
  "\n  %1 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MCircle, "
    "%systems.vos.wisey.compiler.tests.MCircle* null, i32 1"
  "\n  %3 = ptrtoint %systems.vos.wisey.compiler.tests.MCircle* %2 to i64"
  "\n  %4 = call i8* @__cxa_allocate_exception(i64 %3)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %4, i8* %0, i64 %3, i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %4, i8* %1, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %5 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %5"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, ownerVariablesAreClearedTest) {
  Type* structType = mCircleModel->getLLVMType(mLLVMContext)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* fooPointer = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, fooMalloc, fooPointer);
  IVariable* foo = new LocalOwnerVariable("foo", mCircleModel->getOwner(), fooPointer);
  mContext.getScopes().setVariable(foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* barPointer = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, barMalloc, barPointer);
  IVariable* bar = new LocalOwnerVariable("bar", mCircleModel->getOwner(), barPointer);
  mContext.getScopes().setVariable(bar);
  
  llvm::Constant* exceptionObject =
    ConstantPointerNull::get(mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression, 0);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 0)"
  "\n  %0 = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  %pointer = alloca %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  store %systems.vos.wisey.compiler.tests.MCircle* %0, "
    "%systems.vos.wisey.compiler.tests.MCircle** %pointer"
  "\n  %malloccall1 = tail call i8* @malloc(i64 0)"
  "\n  %1 = bitcast i8* %malloccall1 to %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  %pointer2 = alloca %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  store %systems.vos.wisey.compiler.tests.MCircle* %1, "
    "%systems.vos.wisey.compiler.tests.MCircle** %pointer2"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MCircle* null to i8*"
  "\n  %3 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %4 = getelementptr %systems.vos.wisey.compiler.tests.MCircle, "
    "%systems.vos.wisey.compiler.tests.MCircle* null, i32 1"
  "\n  %5 = ptrtoint %systems.vos.wisey.compiler.tests.MCircle* %4 to i64"
  "\n  %6 = call i8* @__cxa_allocate_exception(i64 %5)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %6, i8* %2, i64 %5, i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %6, i8* %3, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %7 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %8 = load %systems.vos.wisey.compiler.tests.MCircle*, "
  "%systems.vos.wisey.compiler.tests.MCircle** %pointer2"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MCircle("
  "%systems.vos.wisey.compiler.tests.MCircle* %8)"
  "\n  %9 = load %systems.vos.wisey.compiler.tests.MCircle*, "
  "%systems.vos.wisey.compiler.tests.MCircle** %pointer"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MCircle("
  "%systems.vos.wisey.compiler.tests.MCircle* %9)"
  "\n  resume { i8*, i32 } %7"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, referenceVariablesGetTheirRefCountDecrementedTest) {
  StructType* structType = StructType::create(mLLVMContext, "MModel");
  vector<Type*> types;
  types.push_back(Type::getInt64Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* fooStore = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "");
  IRWriter::newStoreInst(mContext, fooMalloc, fooStore);
  IVariable* foo = new LocalReferenceVariable("foo", mCircleModel, fooStore);
  mContext.getScopes().setVariable(foo);

  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* barStore = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "");
  IRWriter::newStoreInst(mContext, barMalloc, barStore);
  IVariable* bar = new LocalReferenceVariable("bar", mCircleModel, barStore);
  mContext.getScopes().setVariable(bar);

  llvm::Constant* exceptionObject =
    ConstantPointerNull::get(mCircleModel->getLLVMType(mLLVMContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel));
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression, 0);
  
  Value* result = throwStatement.generateIR(mContext);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%MModel* "
  "getelementptr (%MModel, %MModel* null, i32 1) to i64))"
  "\n  %0 = bitcast i8* %malloccall to %MModel*"
  "\n  %1 = alloca %MModel*"
  "\n  store %MModel* %0, %MModel** %1"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%MModel* "
  "getelementptr (%MModel, %MModel* null, i32 1) to i64))"
  "\n  %2 = bitcast i8* %malloccall1 to %MModel*"
  "\n  %3 = alloca %MModel*"
  "\n  store %MModel* %2, %MModel** %3"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MCircle* null to i8*"
  "\n  %5 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %6 = getelementptr %systems.vos.wisey.compiler.tests.MCircle, "
  "%systems.vos.wisey.compiler.tests.MCircle* null, i32 1"
  "\n  %7 = ptrtoint %systems.vos.wisey.compiler.tests.MCircle* %6 to i64"
  "\n  %8 = call i8* @__cxa_allocate_exception(i64 %7)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %8, i8* %4, i64 %7, i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %8, i8* %5, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %9 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %10 = load %MModel*, %MModel** %3"
  "\n  %11 = bitcast %MModel* %10 to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i64* %11, i64 -1)"
  "\n  %12 = load %MModel*, %MModel** %1"
  "\n  %13 = bitcast %MModel* %12 to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i64* %13, i64 -1)"
  "\n  resume { i8*, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
