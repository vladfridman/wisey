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
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = bitcast ptr @systems.vos.wisey.compiler.tests.MCircle.rtti to ptr"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr i8, ptr %1, i64 -8"
  "\n  %3 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %2, i64 add (i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %2)"
  "\n  invoke void @__cxa_throw(ptr %3, ptr %0, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %4 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %5 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %4, ptr %5, align 8"
  "\n  %6 = getelementptr { ptr, i32 }, ptr %5, i32 0, i32 0"
  "\n  %7 = load ptr, ptr %6, align 8"
  "\n  %8 = call ptr @__cxa_get_exception_ptr(ptr %7)"
  "\n  %9 = getelementptr i8, ptr %8, i64 8"
  "\n  resume { ptr, i32 } %4"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}"
  "\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowStatementTest, ownerVariablesAreClearedTest) {
  Type* structType = mCircleModel->getLLVMStructType(mContext);
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
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %pointer = alloca ptr, align 8"
  "\n  %pointer2 = alloca ptr, align 8"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall, ptr %pointer, align 8"
  "\n  %malloccall1 = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall1, ptr %pointer2, align 8"
  "\n  %0 = bitcast ptr @systems.vos.wisey.compiler.tests.MCircle.rtti to ptr"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr i8, ptr %1, i64 -8"
  "\n  %3 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %3, ptr %2, i64 add (i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %2)"
  "\n  invoke void @__cxa_throw(ptr %3, ptr %0, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %4 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %5 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %4, ptr %5, align 8"
  "\n  %6 = getelementptr { ptr, i32 }, ptr %5, i32 0, i32 0"
  "\n  %7 = load ptr, ptr %6, align 8"
  "\n  %8 = call ptr @__cxa_get_exception_ptr(ptr %7)"
  "\n  %9 = getelementptr i8, ptr %8, i64 8"
  "\n  %10 = load ptr, ptr %pointer2, align 8"
  "\n  %11 = bitcast ptr %10 to ptr"
  "\n  call void @systems.vos.wisey.compiler.tests.MCircle.destructor(ptr %11, ptr null, ptr null, ptr %9)"
  "\n  %12 = load ptr, ptr %pointer, align 8"
  "\n  %13 = bitcast ptr %12 to ptr"
  "\n  call void @systems.vos.wisey.compiler.tests.MCircle.destructor(ptr %13, ptr null, ptr null, ptr %9)"
  "\n  resume { ptr, i32 } %4"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}"
  "\n";

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
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %0 = alloca ptr, align 8"
  "\n  %1 = alloca ptr, align 8"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%MModel, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall, ptr %0, align 8"
  "\n  %malloccall1 = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%MModel, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall1, ptr %1, align 8"
  "\n  %2 = bitcast ptr @systems.vos.wisey.compiler.tests.MCircle.rtti to ptr"
  "\n  %3 = bitcast ptr null to ptr"
  "\n  %4 = getelementptr i8, ptr %3, i64 -8"
  "\n  %5 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %5, ptr %4, i64 add (i64 ptrtoint (ptr getelementptr (%systems.vos.wisey.compiler.tests.MCircle, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %4)"
  "\n  invoke void @__cxa_throw(ptr %5, ptr %2, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %6 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %7 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %6, ptr %7, align 8"
  "\n  %8 = getelementptr { ptr, i32 }, ptr %7, i32 0, i32 0"
  "\n  %9 = load ptr, ptr %8, align 8"
  "\n  %10 = call ptr @__cxa_get_exception_ptr(ptr %9)"
  "\n  %11 = getelementptr i8, ptr %10, i64 8"
  "\n  %12 = load ptr, ptr %1, align 8"
  "\n  %13 = icmp eq ptr %12, null"
  "\n  br i1 %13, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %cleanup.cont"
  "\n  %14 = load ptr, ptr %0, align 8"
  "\n  %15 = icmp eq ptr %14, null"
  "\n  br i1 %15, label %if.end2, label %if.notnull3"
  "\n"
  "\nif.notnull:                                       ; preds = %cleanup.cont"
  "\n  %16 = bitcast ptr %12 to ptr"
  "\n  %17 = getelementptr i64, ptr %16, i64 -1"
  "\n  %18 = atomicrmw add ptr %17, i64 -1 monotonic, align 8"
  "\n  br label %if.end"
  "\n"
  "\nif.end2:                                          ; preds = %if.notnull3, %if.end"
  "\n  resume { ptr, i32 } %6"
  "\n"
  "\nif.notnull3:                                      ; preds = %if.end"
  "\n  %19 = bitcast ptr %14 to ptr"
  "\n  %20 = getelementptr i64, ptr %19, i64 -1"
  "\n  %21 = atomicrmw add ptr %20, i64 -1 monotonic, align 8"
  "\n  br label %if.end2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}"
  "\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, throwNonExceptionRunDeathTest) {
  expectFailCompile("tests/samples/test_throw_non_exception.yz",
                    1,
                    "tests/samples/test_throw_non_exception.yz\\(11\\): Error: Exceptions must implement wisey.lang.IException");
}
