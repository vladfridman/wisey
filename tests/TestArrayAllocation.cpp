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
#include "TestFileSampleRunner.hpp"
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
  BasicBlock* mBasicBlock;
  ArraySpecificTypeSpecifier* mArraySpecificTypeSpecifier;
  wisey::ArrayType* mArrayType;
  ArrayAllocation* mArrayAllocation;
  
  ArrayAllocationTest() :
  mLLVMContext(mContext.getLLVMContext()) {
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
    
    list<unsigned long> dimensions;
    dimensions.push_back(5u);
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mArraySpecificTypeSpecifier = new ArraySpecificTypeSpecifier(intSpecifier, dimensions);
    
    mArrayAllocation = new ArrayAllocation(mArraySpecificTypeSpecifier);
  }
  
  ~ArrayAllocationTest() {
    delete mStringStream;
  }
};

TEST_F(ArrayAllocationTest, generateIRTest) {
  mArrayAllocation->generateIR(mContext, mArrayType->getOwner());
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %arraySize = alloca i64"
  "\n  store i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64), i64* %arraySize"
  "\n  %size = load i64, i64* %arraySize"
  "\n  %0 = mul i64 5, %size"
  "\n  %1 = add i64 %0, 24"
  "\n  store i64 %1, i64* %arraySize"
  "\n  %2 = load i64, i64* %arraySize"
  "\n  %malloccall = tail call i8* @malloc(i64 %2)"
  "\n  %3 = bitcast i8* %malloccall to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 %2, i32 4, i1 false)"
  "\n  %4 = bitcast i8* %malloccall to [0 x i64]*"
  "\n  %5 = getelementptr [0 x i64], [0 x i64]* %4, i64 0, i64 1"
  "\n  store i64 5, i64* %5"
  "\n  %6 = getelementptr [0 x i64], [0 x i64]* %4, i64 0, i64 2"
  "\n  store i64 %size, i64* %6"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayAllocationTest, isConstantTest) {
  EXPECT_FALSE(mArrayAllocation->isConstant());
}

TEST_F(ArrayAllocationTest, getTypeTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(5u);
  const IType* expected = mContext.getArraySpecificType(PrimitiveTypes::INT_TYPE,
                                                        dimensions)->getOwner();
  EXPECT_EQ(expected, mArrayAllocation->getType(mContext));
}

TEST_F(ArrayAllocationTest, printToStreamTest) {
  stringstream stringStream;
  mArrayAllocation->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("new int[5]", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, localPrimitiveArrayAllocationRunTest) {
  runFile("tests/samples/test_local_primitive_array_allocation.yz", "0");
}
