//
//  TestComposer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Composer}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockReferenceVariable.hpp"
#include "TestPrefix.hpp"
#include "Composer.hpp"
#include "EmptyStatement.hpp"
#include "IMethod.hpp"
#include "Method.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ProgramFile.hpp"
#include "ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ComposerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Model* mModel;
  Function* mMainFunction;
  ImportProfile* mImportProfile;
  string mPackage = "systems.vos.wisey.compiler.tests";

public:
  
  ComposerTest() : mLLVMContext(mContext.getLLVMContext()){
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             modelStructType,
                             mContext.getImportProfile(),
                             0);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mContext.getImportProfile()->setSourceFileName(mContext, "test.yz");

    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mModel->getTypeName());
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       mModel->getObjectNameGlobalVariableName());

    mContext.getScopes().pushScope();

    mContext.setObjectType(mModel);
    vector<const wisey::Argument*> arguments;
    vector<const Model*> exceptions;
    Method* method = new Method(mModel,
                                "foo",
                                PUBLIC_ACCESS,
                                PrimitiveTypes::VOID,
                                arguments,
                                exceptions,
                                NULL,
                                NULL,
                                5);
    mContext.setCurrentMethod(method);
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ComposerTest() {
    delete mStringStream;
  }
};

TEST_F(ComposerTest, pushCallStackTest) {
  Composer::pushCallStack(mContext, 5);

  *mStringStream << *mMainFunction;
  string expected =
  "define internal i32 @main() {"
  "\nentry:"
  "\n  %0 = load i32, ptr @constant.wisey.threads.CCallStack.CALL_STACK_SIZE, align 4"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr %CCallStack, ptr %1, i32 0, i32 3"
  "\n  %3 = load i32, ptr %2, align 4"
  "\n  %4 = icmp sge i32 %3, %0"
  "\n  br i1 %4, label %if.overflow, label %if.continue"
  "\n"
  "\nif.overflow:                                      ; preds = %entry"
  "\n  call void @wisey.threads.CCallStack.method.throwStackOverflowException(ptr null, ptr null)"
  "\n  unreachable"
  "\n"
  "\nif.continue:                                      ; preds = %entry"
  "\n  %5 = getelementptr %CCallStack, ptr %1, i32 0, i32 1"
  "\n  %6 = load ptr, ptr %5, align 8"
  "\n  %7 = getelementptr { i64, i64, i64, [0 x ptr] }, ptr %6, i32 0, i32 3"
  "\n  %8 = getelementptr [0 x ptr], ptr %7, i32 0, i32 %3"
  "\n  store ptr @\"systems.vos.wisey.compiler.tests.MMyModel.foo(test.yz\", ptr %8, align 8"
  "\n  %9 = getelementptr %CCallStack, ptr %1, i32 0, i32 2"
  "\n  %10 = load ptr, ptr %9, align 8"
  "\n  %11 = getelementptr { i64, i64, i64, [0 x i32] }, ptr %10, i32 0, i32 3"
  "\n  %12 = getelementptr [0 x i32], ptr %11, i32 0, i32 %3"
  "\n  store i32 5, ptr %12, align 4"
  "\n  %13 = add i32 %3, 1"
  "\n  store i32 %13, ptr %2, align 4"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, popCallStackTest) {
  Composer::popCallStack(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:"
  "\n  %0 = bitcast ptr null to ptr"
  "\n  %1 = getelementptr %CCallStack, ptr %0, i32 0, i32 3"
  "\n  %2 = load i32, ptr %1, align 4"
  "\n  %3 = sub i32 %2, 1"
  "\n  store i32 %3, ptr %1, align 4"
  "\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, setLineNumberTestTest) {
  Composer::setLineNumber(mContext, 5);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:"
  "\n  %0 = bitcast ptr null to ptr"
  "\n  %1 = getelementptr %CCallStack, ptr %0, i32 0, i32 3"
  "\n  %2 = load i32, ptr %1, align 4"
  "\n  %3 = sub i32 %2, 1"
  "\n  %4 = getelementptr %CCallStack, ptr %0, i32 0, i32 2"
  "\n  %5 = load ptr, ptr %4, align 8"
  "\n  %6 = getelementptr { i64, i64, i64, [0 x i32] }, ptr %5, i32 0, i32 3"
  "\n  %7 = getelementptr [0 x i32], ptr %6, i32 0, i32 %3"
  "\n  store i32 5, ptr %7, align 4"
  "\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, incrementReferenceCountUnsafelyTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Composer::incrementReferenceCountUnsafely(mContext, nullPointerValue);
  
  *mStringStream << *mMainFunction;
  string expected =
  "define internal i32 @main() {"
  "\nentry:"
  "\n  %0 = icmp eq ptr null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr i64, ptr %1, i64 -1"
  "\n  %count = load i64, ptr %2, align 4"
  "\n  %3 = add i64 %count, 1"
  "\n  store i64 %3, ptr %2, align 4"
  "\n  br label %if.end"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ComposerTest, decrementReferenceCountUnsafelyTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Composer::decrementReferenceCountUnsafely(mContext, nullPointerValue);
  
  *mStringStream << *mMainFunction;
  string expected =
  "define internal i32 @main() {"
  "\nentry:"
  "\n  %0 = icmp eq ptr null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr i64, ptr %1, i64 -1"
  "\n  %count = load i64, ptr %2, align 4"
  "\n  %3 = add i64 %count, -1"
  "\n  store i64 %3, ptr %2, align 4"
  "\n  br label %if.end"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ComposerTest, incrementReferenceCountSafelyTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Composer::incrementReferenceCountSafely(mContext, nullPointerValue);
  
  *mStringStream << *mMainFunction;
  string expected =
  "define internal i32 @main() {"
  "\nentry:"
  "\n  %0 = icmp eq ptr null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr i64, ptr %1, i64 -1"
  "\n  %3 = atomicrmw add ptr %2, i64 1 monotonic, align 8"
  "\n  br label %if.end"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ComposerTest, decrementReferenceCountSafelyTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Composer::decrementReferenceCountSafely(mContext, nullPointerValue);
  
  *mStringStream << *mMainFunction;
  string expected =
  "define internal i32 @main() {"
  "\nentry:"
  "\n  %0 = icmp eq ptr null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast ptr null to ptr"
  "\n  %2 = getelementptr i64, ptr %1, i64 -1"
  "\n  %3 = atomicrmw add ptr %2, i64 -1 monotonic, align 8"
  "\n  br label %if.end"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
