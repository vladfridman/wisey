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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "Block.hpp"
#include "IRWriter.hpp"
#include "LocalOwnerVariable.hpp"
#include "LocalReferenceVariable.hpp"
#include "Names.hpp"
#include "ThrowStatement.hpp"

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
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ThrowStatementTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);

    mStringStream = new raw_string_ostream(mStringBuffer);

    string circleFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, circleFullName);
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                   circleFullName,
                                   circleStructType,
                                   mContext.getImportProfile(),
                                   0);
    Interface* exceptionInterface = mContext.getInterface(Names::getExceptionInterfaceFullName(), 0);
    vector<Interface*> interfaces;
    interfaces.push_back(exceptionInterface);
    mCircleModel->setInterfaces(interfaces);
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  circleFullName + ".typename");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       circleFullName + ".typename");
    mCircleModel->declareRTTI(mContext);

    IConcreteObjectType::declareTypeNameGlobal(mContext, mCircleModel);
    IConcreteObjectType::defineVTable(mContext, mCircleModel);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    EXPECT_CALL(mMockType, die());
  }
  
  ~ThrowStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ThrowStatementTest, wrongExpressionTypeDeathTest) {
  Mock::AllowLeak(&mMockType);
  Mock::AllowLeak(mMockExpression);
  
  ON_CALL(mMockType, isController()).WillByDefault(Return(true));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(&mMockType));
  ThrowStatement throwStatement(mMockExpression, 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(throwStatement.generateIR(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Thrown object can only be a model owner\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ThrowStatementTest, modelExpressionTypeTest) {
  llvm::Constant* exceptionObject =
    ConstantPointerNull::get(mCircleModel->getLLVMType(mContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel->getOwner()));
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression, 0);
  
  throwStatement.generateIR(mContext);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);

  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MCircle* null to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 -8"
  "\n  %3 = call i8* @__cxa_allocate_exception(i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* %2, i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i1 false)"
  "\n  tail call void @free(i8* %2)"
  "\n  invoke void @__cxa_throw(i8* %3, i8* %0, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %4 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %5 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %4, { i8*, i32 }* %5"
  "\n  %6 = getelementptr { i8*, i32 }, { i8*, i32 }* %5, i32 0, i32 0"
  "\n  %7 = load i8*, i8** %6"
  "\n  %8 = call i8* @__cxa_get_exception_ptr(i8* %7)"
  "\n  %9 = getelementptr i8, i8* %8, i64 8"
  "\n  resume { i8*, i32 } %4"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, ownerVariablesAreClearedTest) {
  Type* structType = mCircleModel->getLLVMType(mContext)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 1);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* fooPointer = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, fooMalloc, fooPointer);
  IVariable* foo = new LocalOwnerVariable("foo", mCircleModel->getOwner(), fooPointer, 0);
  mContext.getScopes().setVariable(mContext, foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* barPointer = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, barMalloc, barPointer);
  IVariable* bar = new LocalOwnerVariable("bar", mCircleModel->getOwner(), barPointer, 0);
  mContext.getScopes().setVariable(mContext, bar);
  
  llvm::Constant* exceptionObject = ConstantPointerNull::get(mCircleModel->getLLVMType(mContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel->getOwner()));
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression, 0);
  
  throwStatement.generateIR(mContext);
  BranchInst::Create(mEntryBlock, mDeclareBlock);

  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %pointer = alloca %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  %pointer2 = alloca %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call i8* @malloc(i64 0)"
  "\n  %0 = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  store %systems.vos.wisey.compiler.tests.MCircle* %0, %systems.vos.wisey.compiler.tests.MCircle** %pointer"
  "\n  %malloccall1 = tail call i8* @malloc(i64 0)"
  "\n  %1 = bitcast i8* %malloccall1 to %systems.vos.wisey.compiler.tests.MCircle*"
  "\n  store %systems.vos.wisey.compiler.tests.MCircle* %1, %systems.vos.wisey.compiler.tests.MCircle** %pointer2"
  "\n  %2 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MCircle* null to i8*"
  "\n  %4 = getelementptr i8, i8* %3, i64 -8"
  "\n  %5 = call i8* @__cxa_allocate_exception(i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %4, i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i1 false)"
  "\n  tail call void @free(i8* %4)"
  "\n  invoke void @__cxa_throw(i8* %5, i8* %2, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %6 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %7 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %6, { i8*, i32 }* %7"
  "\n  %8 = getelementptr { i8*, i32 }, { i8*, i32 }* %7, i32 0, i32 0"
  "\n  %9 = load i8*, i8** %8"
  "\n  %10 = call i8* @__cxa_get_exception_ptr(i8* %9)"
  "\n  %11 = getelementptr i8, i8* %10, i64 8"
  "\n  %12 = load %systems.vos.wisey.compiler.tests.MCircle*, %systems.vos.wisey.compiler.tests.MCircle** %pointer2"
  "\n  %13 = bitcast %systems.vos.wisey.compiler.tests.MCircle* %12 to i8*"
  "\n  call void @systems.vos.wisey.compiler.tests.MCircle.destructor(i8* %13, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* %11)"
  "\n  %14 = load %systems.vos.wisey.compiler.tests.MCircle*, %systems.vos.wisey.compiler.tests.MCircle** %pointer"
  "\n  %15 = bitcast %systems.vos.wisey.compiler.tests.MCircle* %14 to i8*"
  "\n  call void @systems.vos.wisey.compiler.tests.MCircle.destructor(i8* %15, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* %11)"
  "\n  resume { i8*, i32 } %6"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, referenceVariablesGetTheirRefCountDecrementedTest) {
  StructType* structType = StructType::create(mLLVMContext, "MModel");
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 1);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* fooStore = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "");
  IRWriter::newStoreInst(mContext, fooMalloc, fooStore);
  IVariable* foo = new LocalReferenceVariable("foo", mCircleModel, fooStore, 0);
  mContext.getScopes().setVariable(mContext, foo);

  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* barStore = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "");
  IRWriter::newStoreInst(mContext, barMalloc, barStore);
  IVariable* bar = new LocalReferenceVariable("bar", mCircleModel, barStore, 0);
  mContext.getScopes().setVariable(mContext, bar);

  llvm::Constant* exceptionObject = ConstantPointerNull::get(mCircleModel->getLLVMType(mContext));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(mCircleModel->getOwner()));
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(exceptionObject));
  ThrowStatement throwStatement(mMockExpression, 0);
  
  throwStatement.generateIR(mContext);
  BranchInst::Create(mEntryBlock, mDeclareBlock);

  EXPECT_EQ(mContext.getScopes().getScope()->getExceptions().size(), 1u);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %0 = alloca %MModel*"
  "\n  %1 = alloca %MModel*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64))"
  "\n  %2 = bitcast i8* %malloccall to %MModel*"
  "\n  store %MModel* %2, %MModel** %0"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64))"
  "\n  %3 = bitcast i8* %malloccall1 to %MModel*"
  "\n  store %MModel* %3, %MModel** %1"
  "\n  %4 = bitcast { i8*, i8* }* @systems.vos.wisey.compiler.tests.MCircle.rtti to i8*"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MCircle* null to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 -8"
  "\n  %7 = call i8* @__cxa_allocate_exception(i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* %6, i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i1 false)"
  "\n  tail call void @free(i8* %6)"
  "\n  invoke void @__cxa_throw(i8* %7, i8* %4, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %8 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %9 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %8, { i8*, i32 }* %9"
  "\n  %10 = getelementptr { i8*, i32 }, { i8*, i32 }* %9, i32 0, i32 0"
  "\n  %11 = load i8*, i8** %10"
  "\n  %12 = call i8* @__cxa_get_exception_ptr(i8* %11)"
  "\n  %13 = getelementptr i8, i8* %12, i64 8"
  "\n  %14 = load %MModel*, %MModel** %1"
  "\n  %15 = icmp eq %MModel* %14, null"
  "\n  br i1 %15, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %cleanup.cont"
  "\n  %16 = load %MModel*, %MModel** %0"
  "\n  %17 = icmp eq %MModel* %16, null"
  "\n  br i1 %17, label %if.end2, label %if.notnull3"
  "\n"
  "\nif.notnull:                                       ; preds = %cleanup.cont"
  "\n  %18 = bitcast %MModel* %14 to i64*"
  "\n  %19 = getelementptr i64, i64* %18, i64 -1"
  "\n  %20 = atomicrmw add i64* %19, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n"
  "\nif.end2:                                          ; preds = %if.notnull3, %if.end"
  "\n  resume { i8*, i32 } %8"
  "\n"
  "\nif.notnull3:                                      ; preds = %if.end"
  "\n  %21 = bitcast %MModel* %16 to i64*"
  "\n  %22 = getelementptr i64, i64* %21, i64 -1"
  "\n  %23 = atomicrmw add i64* %22, i64 -1 monotonic"
  "\n  br label %if.end2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, throwNonExceptionRunDeathTest) {
  expectFailCompile("tests/samples/test_throw_non_exception.yz",
                    1,
                    "tests/samples/test_throw_non_exception.yz\\(11\\): Error: Exceptions must implement wisey.lang.IException");
}
