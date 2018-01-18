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
  ArrayTypeSpecifier* mArrayTypeSpecifier;
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
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 5);
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mArrayTypeSpecifier = new ArrayTypeSpecifier(intSpecifier, 5);
    
    mArrayAllocation = new ArrayAllocation(mArrayTypeSpecifier);
  }
  
  ~ArrayAllocationTest() {
    delete mStringStream;
  }
  
  static void printArrayExpression(IRGenerationContext& context, iostream& stream) {
    stream << "array";
  }
  
  static void printArrayIndex(IRGenerationContext& context, iostream& stream) {
    stream << "index";
  }
};

TEST_F(ArrayAllocationTest, generateIRTest) {
  mArrayAllocation->generateIR(mContext, IR_GENERATION_RELEASE);
  
  *mStringStream << *mBasicBlock;
  
  EXPECT_STREQ("\nentry:"
               "\n  %malloccall = tail call i8* @malloc(i64 mul nuw (i64 ptrtoint "
               "(i32* getelementptr (i32, i32* null, i32 1) to i64), i64 5))"
               "\n  %newarray = bitcast i8* %malloccall to [5 x i32]*"
               "\n  %0 = bitcast [5 x i32]* %newarray to i8*"
               "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 mul nuw "
               "(i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64), i64 5), "
               "i32 4, i1 false)\n",
               mStringStream->str().c_str());
}

TEST_F(ArrayAllocationTest, isConstantTest) {
  EXPECT_FALSE(mArrayAllocation->isConstant());
}

TEST_F(ArrayAllocationTest, getTypeTest) {
  EXPECT_EQ(mArrayType->getOwner(), mArrayAllocation->getType(mContext));
}

TEST_F(ArrayAllocationTest, printToStreamTest) {
  stringstream stringStream;
  mArrayAllocation->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("new int[5]", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, localPrimitiveArrayAllocationRunTest) {
  runFile("tests/samples/test_local_primitive_array_allocation.yz", "0");
}
