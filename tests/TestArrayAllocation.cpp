//
//  TestArrayAllocation.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/21/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayAllocation}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
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

struct ArrayAllocationTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  ArraySpecificTypeSpecifier* mArraySpecificTypeSpecifier;
  wisey::ArrayType* mArrayType;
  ArrayAllocation* mArrayAllocation;
  NiceMock<MockExpression> mFiveExpression;

  ArrayAllocationTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    llvm::Constant* five = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 5);
    ON_CALL(mFiveExpression, generateIR(_, _)).WillByDefault(Return(five));
    ON_CALL(mFiveExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(mFiveExpression, printToStream(_, _)).WillByDefault(Invoke(printFive));
    list<const IExpression*> dimensions;
    dimensions.push_back(&mFiveExpression);
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    mArraySpecificTypeSpecifier = new ArraySpecificTypeSpecifier(intSpecifier, dimensions, 0);
    
    mArrayAllocation = new ArrayAllocation(mArraySpecificTypeSpecifier, 0);
  }
  
  ~ArrayAllocationTest() {
    delete mStringStream;
  }
  
  static void printFive(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(ArrayAllocationTest, generateIRTest) {
  mArrayAllocation->generateIR(mContext, mArrayType->getOwner());
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %arraySize = alloca i64"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64), i64* %arraySize"
  "\n  %conv = bitcast i64 5 to i64"
  "\n  %size = load i64, i64* %arraySize"
  "\n  %0 = mul i64 %conv, %size"
  "\n  %1 = add i64 %0, 24"
  "\n  store i64 %1, i64* %arraySize"
  "\n  %2 = load i64, i64* %arraySize"
  "\n  %malloccall = tail call i8* @malloc(i64 %2)"
  "\n  %3 = bitcast i8* %malloccall to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 %2, i1 false)"
  "\n  %4 = bitcast i8* %malloccall to [0 x i64]*"
  "\n  %5 = getelementptr [0 x i64], [0 x i64]* %4, i64 0, i64 1"
  "\n  store i64 %conv, i64* %5"
  "\n  %6 = getelementptr [0 x i64], [0 x i64]* %4, i64 0, i64 2"
  "\n  store i64 %size, i64* %6"
  "\n  %7 = bitcast i8* %malloccall to { i64, i64, i64, [0 x i32] }*"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayAllocationTest, isConstantTest) {
  EXPECT_FALSE(mArrayAllocation->isConstant());
}

TEST_F(ArrayAllocationTest, isAssignableTest) {
  EXPECT_FALSE(mArrayAllocation->isAssignable());
}

TEST_F(ArrayAllocationTest, getTypeTest) {
  const IType* expected = mContext.getArrayType(PrimitiveTypes::INT, 1u)->getOwner();
  EXPECT_EQ(expected, mArrayAllocation->getType(mContext));
}

TEST_F(ArrayAllocationTest, printToStreamTest) {
  stringstream stringStream;
  mArrayAllocation->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("new int[5]", stringStream.str().c_str());
}

TEST_F(TestFileRunner, localPrimitiveArrayAllocationRunTest) {
  runFile("tests/samples/test_local_primitive_array_allocation.yz", 0);
}

TEST_F(TestFileRunner, localOwnerArrayOfIntsCreatedDynamicallyRunTest) {
  runFile("tests/samples/test_local_owner_array_of_ints_created_dynamically.yz", 1);
}

TEST_F(TestFileRunner, assignSubArrayRunTest) {
  runFile("tests/samples/test_assign_sub_array.yz", 5);
}

TEST_F(TestFileRunner, localOwnerArrayOfIntsFloatDimensionDeathRunTest) {
  expectFailCompile("tests/samples/test_local_owner_array_of_ints_float_dimension.yz",
                    1,
                    "tests/samples/test_local_owner_array_of_ints_float_dimension.yz\\(10\\): "
                    "Error: Dimension in array allocation should be castable to long, but it is of float type");
}
