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

#include "MockExpression.hpp"
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
  NiceMock<MockExpression>* mMockExpression1;
  NiceMock<MockExpression>* mMockExpression2;
  ExpressionList mExpressionList;

  ArrayAllocationStaticTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression1(new NiceMock<MockExpression>()),
  mMockExpression2(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
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
  const wisey::ArrayType* arrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
  mArrayAllocationStatic->generateIR(mContext, arrayType->getOwner());
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint ({ i64, i64, i64, [2 x i32] }* getelementptr ({ i64, i64, i64, [2 x i32] }, { i64, i64, i64, [2 x i32] }* null, i32 1) to i64))"
  "\n  %newarray = bitcast i8* %malloccall to { i64, i64, i64, [2 x i32] }*"
  "\n  %0 = bitcast { i64, i64, i64, [2 x i32] }* %newarray to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint ({ i64, i64, i64, [2 x i32] }* getelementptr ({ i64, i64, i64, [2 x i32] }, { i64, i64, i64, [2 x i32] }* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr { i64, i64, i64, [2 x i32] }, { i64, i64, i64, [2 x i32] }* %newarray, i64 0, i32 1"
  "\n  store i64 2, i64* %1"
  "\n  %2 = getelementptr { i64, i64, i64, [2 x i32] }, { i64, i64, i64, [2 x i32] }* %newarray, i64 0, i32 3"
  "\n  %3 = getelementptr [2 x i32], [2 x i32]* %2, i64 0, i32 0"
  "\n  %4 = getelementptr { i64, i64, i64, [2 x i32] }, { i64, i64, i64, [2 x i32] }* %newarray, i64 0, i32 2"
  "\n  store i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64), i64* %4"
  "\n  %5 = getelementptr { i64, i64, i64, [2 x i32] }, { i64, i64, i64, [2 x i32] }* %newarray, i64 0, i32 3"
  "\n  %6 = getelementptr [2 x i32], [2 x i32]* %5, i64 0, i64 0"
  "\n  store i32 50, i32* %6"
  "\n  %7 = getelementptr [2 x i32], [2 x i32]* %5, i64 0, i64 1"
  "\n  store i32 60, i32* %7"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayAllocationStaticTest, isConstantTest) {
  EXPECT_FALSE(mArrayAllocationStatic->isConstant());
}

TEST_F(ArrayAllocationStaticTest, isAssignableTest) {
  EXPECT_FALSE(mArrayAllocationStatic->isAssignable());
}

TEST_F(ArrayAllocationStaticTest, getTypeTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(2u);
  const ArrayExactType* arrayExactType = mContext.getArrayExactType(PrimitiveTypes::INT,
                                                                    dimensions);
  EXPECT_EQ(arrayExactType->getOwner(), mArrayAllocationStatic->getType(mContext));
}

TEST_F(ArrayAllocationStaticTest, printToStreamTest) {
  stringstream stringStream;
  mArrayAllocationStatic->printToStream(mContext, stringStream);

  EXPECT_STREQ("{ 50, 60 }", stringStream.str().c_str());
}

TEST_F(TestFileRunner, localArrayOwnerOfIntsStaticAllocationRunTest) {
  runFile("tests/samples/test_local_array_owner_of_ints_static_allocation.yz", "5");
}

TEST_F(TestFileRunner, localArrayOwnerOfIntsStaticAllocationTrailingCommaRunTest) {
  runFile("tests/samples/test_local_array_owner_of_ints_static_allocation_trailing_comma.yz", "5");
}

