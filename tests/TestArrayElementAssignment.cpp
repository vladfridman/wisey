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
  "define internal i32 @test() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = load ptr, ptr null, align 8"
  "\n  %1 = bitcast ptr %0 to ptr"
  "\n  invoke void @systems.vos.wisey.compiler.tests.MModel.destructor(ptr %1, ptr null, ptr null, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %2 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %3 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %2, ptr %3, align 8"
  "\n  %4 = getelementptr { ptr, i32 }, ptr %3, i32 0, i32 0"
  "\n  %5 = load ptr, ptr %4, align 8"
  "\n  %6 = call ptr @__cxa_get_exception_ptr(ptr %5)"
  "\n  %7 = getelementptr i8, ptr %6, i64 8"
  "\n  resume { ptr, i32 } %2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  store ptr null, ptr null, align 8"
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
  "define internal i32 @test() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load ptr, ptr null, align 8"
  "\n  %1 = icmp eq ptr %0, null"
  "\n  br i1 %1, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %2 = icmp eq ptr null, null"
  "\n  br i1 %2, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %3 = bitcast ptr %0 to ptr"
  "\n  %4 = getelementptr i64, ptr %3, i64 -1"
  "\n  %5 = atomicrmw add ptr %4, i64 -1 monotonic, align 8"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store ptr null, ptr null, align 8"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %6 = bitcast ptr null to ptr"
  "\n  %7 = getelementptr i64, ptr %6, i64 -1"
  "\n  %8 = atomicrmw add ptr %7, i64 1 monotonic, align 8"
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
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  store i32 5, ptr null, align 4"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
