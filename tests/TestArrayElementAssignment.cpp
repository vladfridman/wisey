//
//  TestArrayElementAssignment.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayElementAssignment}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "ArrayElementAssignment.hpp"
#include "ArrayOwnerType.hpp"
#include "ArrayType.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "IntConstant.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArrayElementAssignmentTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  Function* mFunction;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ArrayElementAssignmentTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<Type*> modelTypes;
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    modelStructType->setBody(modelTypes);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             modelStructType,
                             mContext.getImportProfile(),
                             0);
    mContext.addModel(mModel, 0);
    IConcreteObjectType::declareTypeNameGlobal(mContext, mModel);
    IConcreteObjectType::declareVTable(mContext, mModel);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();

    IConcreteObjectType::declareTypeNameGlobal(mContext, mModel);
    IConcreteObjectType::defineVTable(mContext, mModel);
  }
  
  ~ArrayElementAssignmentTest() {
  }
};

TEST_F(ArrayElementAssignmentTest, generateOwnerArrayAssignmentTest) {
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  Value* value = ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mContext));
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  Value* elementStore = ConstantPointerNull::get(mModel->getLLVMType(mContext)->getPointerTo());
  EXPECT_CALL(mockExpression, generateIR(_, mModel->getOwner()));

  ArrayElementAssignment::generateElementAssignment(mContext,
                                                    mModel->getOwner(),
                                                    &mockExpression,
                                                    elementStore,
                                                    0);
  BranchInst::Create(mEntryBlock, mDeclareBlock);

  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @test() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MModel*, %systems.vos.wisey.compiler.tests.MModel** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MModel* %0 to i8*"
  "\n  invoke void @systems.vos.wisey.compiler.tests.MModel.destructor(i8* %1, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %2 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %3 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %2, { i8*, i32 }* %3"
  "\n  %4 = getelementptr { i8*, i32 }, { i8*, i32 }* %3, i32 0, i32 0"
  "\n  %5 = load i8*, i8** %4"
  "\n  %6 = call i8* @__cxa_get_exception_ptr(i8* %5)"
  "\n  %7 = getelementptr i8, i8* %6, i64 8"
  "\n  resume { i8*, i32 } %2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** null"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayElementAssignmentTest, generateReferenceArrayAssignmentTest) {
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mModel));
  Value* value = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  Value* elementStore = ConstantPointerNull::get(mModel->getLLVMType(mContext)->getPointerTo());
  EXPECT_CALL(mockExpression, generateIR(_, mModel));
  
  ArrayElementAssignment::generateElementAssignment(mContext,
                                                    mModel,
                                                    &mockExpression,
                                                    elementStore,
                                                    0);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MModel*, %systems.vos.wisey.compiler.tests.MModel** null"
  "\n  %1 = icmp eq %systems.vos.wisey.compiler.tests.MModel* %0, null"
  "\n  br i1 %1, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %2 = icmp eq %systems.vos.wisey.compiler.tests.MModel* null, null"
  "\n  br i1 %2, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MModel* %0 to i64*"
  "\n  %4 = getelementptr i64, i64* %3, i64 -1"
  "\n  %5 = atomicrmw add i64* %4, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** null"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %6 = bitcast %systems.vos.wisey.compiler.tests.MModel* null to i64*"
  "\n  %7 = getelementptr i64, i64* %6, i64 -1"
  "\n  %8 = atomicrmw add i64* %7, i64 1 monotonic"
  "\n  br label %if.end1"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayElementAssignmentTest, generatePrimitiveArrayAssignmentTest) {
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  Value* elementStore = ConstantPointerNull::get(Type::getInt32Ty(mLLVMContext)->getPointerTo());
  EXPECT_CALL(mockExpression, generateIR(_, PrimitiveTypes::INT));
  
  ArrayElementAssignment::generateElementAssignment(mContext,
                                                    PrimitiveTypes::INT,
                                                    &mockExpression,
                                                    elementStore,
                                                    0);
  
  *mStringStream << *mEntryBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  store i32 5, i32* null"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
