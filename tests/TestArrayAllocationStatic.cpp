//
//  TestArrayAllocationStatic.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayAllocationStatic}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConstantExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayAllocationStatic.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ArrayAllocationStaticTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  BasicBlock* mBasicBlock;
  ArrayAllocationStatic* mArrayAllocationStatic;
  NiceMock<MockConstantExpression>* mMockExpression1;
  NiceMock<MockConstantExpression>* mMockExpression2;
  ConstantExpressionList mExpressionList;

  ArrayAllocationStaticTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression1(new NiceMock<MockConstantExpression>()),
  mMockExpression2(new NiceMock<MockConstantExpression>()) {
    TestPrefix::generateIR(mContext);
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    Type* int32Type = Type::getInt32Ty(mLLVMContext);
    llvm::Constant* fifty = ConstantInt::get(int32Type, 50);
    llvm::Constant* sixty = ConstantInt::get(int32Type, 60);
    ON_CALL(*mMockExpression1, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mMockExpression2, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mMockExpression1, generateIR(_, _)).WillByDefault(Return(fifty));
    ON_CALL(*mMockExpression2, generateIR(_, _)).WillByDefault(Return(sixty));
    ON_CALL(*mMockExpression1, isConstant()).WillByDefault(Return(true));
    ON_CALL(*mMockExpression2, isConstant()).WillByDefault(Return(true));
    ON_CALL(*mMockExpression1, printToStream(_, _)).WillByDefault(Invoke(printExpression1));
    ON_CALL(*mMockExpression2, printToStream(_, _)).WillByDefault(Invoke(printExpression2));

    mExpressionList.push_back(mMockExpression1);
    mExpressionList.push_back(mMockExpression2);
    
    mArrayAllocationStatic = new ArrayAllocationStatic(mExpressionList, 0);
  }
  
  ~ArrayAllocationStaticTest() {
    delete mMockExpression1;
    delete mMockExpression2;
    delete mStringStream;
  }
  
  static void printExpression1(IRGenerationContext& context, iostream& stream) {
    stream << "50";
  }
  
  static void printExpression2(IRGenerationContext& context, iostream& stream) {
    stream << "60";
  }
};

TEST_F(ArrayAllocationStaticTest, generateIRTest) {
  llvm::Constant* constantArray = mArrayAllocationStatic->generateIR(mContext,
                                                                     PrimitiveTypes::VOID);
  vector<llvm::Constant*> arrayElements;
  Type* int32 = Type::getInt32Ty(mLLVMContext);
  Type* int64 = Type::getInt64Ty(mLLVMContext);
  arrayElements.push_back(ConstantInt::get(int32, 50));
  arrayElements.push_back(ConstantInt::get(int32, 60));
  llvm::ArrayType* arrayType = llvm::ArrayType::get(int32, 2);
  llvm::Constant* llvmArray = ConstantArray::get(arrayType, arrayElements);
  vector<llvm::Constant*> structElements;
  structElements.push_back(ConstantInt::get(int64, 0));
  structElements.push_back(ConstantInt::get(int64, 2));
  structElements.push_back(ConstantExpr::getSizeOf(int32));
  structElements.push_back(llvmArray);
  
  list<unsigned long> dimensions;
  dimensions.push_back(2u);
  const ArrayExactType* arrayExactType = mContext.getArrayExactType(PrimitiveTypes::INT,
                                                                    dimensions);
  llvm::StructType* structType = arrayExactType->getLLVMType(mContext);
  
  llvm::Constant* constantStruct = ConstantStruct::get(structType, structElements);
  EXPECT_EQ(constantStruct, constantArray);
}

TEST_F(ArrayAllocationStaticTest, isConstantTest) {
  EXPECT_TRUE(mArrayAllocationStatic->isConstant());
}

TEST_F(ArrayAllocationStaticTest, isAssignableTest) {
  EXPECT_FALSE(mArrayAllocationStatic->isAssignable());
}

TEST_F(ArrayAllocationStaticTest, getTypeTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(2u);
  const ArrayExactType* arrayExactType = mContext.getArrayExactType(PrimitiveTypes::INT,
                                                                    dimensions);
  EXPECT_EQ(arrayExactType, mArrayAllocationStatic->getType(mContext));
}

TEST_F(ArrayAllocationStaticTest, printToStreamTest) {
  stringstream stringStream;
  mArrayAllocationStatic->printToStream(mContext, stringStream);

  EXPECT_STREQ("{ 50, 60 }", stringStream.str().c_str());
}

TEST_F(TestFileRunner, localArrayOwnerOfIntsStaticAllocationRunTest) {
  runFile("tests/samples/test_local_array_owner_of_ints_static_allocation.yz", 5);
}

TEST_F(TestFileRunner, localArrayOwnerOfIntsStaticAllocationTrailingCommaRunTest) {
  runFile("tests/samples/test_local_array_owner_of_ints_static_allocation_trailing_comma.yz", 5);
}

