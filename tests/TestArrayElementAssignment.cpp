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
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/PrimitiveTypes.hpp"

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
  BasicBlock* mBasicBlock;
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
    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mModel);
    IConcreteObjectType::declareVTable(mContext, mModel);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
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

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MModel*, %systems.vos.wisey.compiler.tests.MModel** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MModel* %0 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %1)"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** null"
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
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MModel*, %systems.vos.wisey.compiler.tests.MModel** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MModel* %0 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %1, i64 -1)"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MModel* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %2, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** null"
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
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  store i32 5, i32* null"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
