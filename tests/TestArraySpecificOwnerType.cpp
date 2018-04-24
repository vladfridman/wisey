//
//  TestArraySpecificOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArraySpecificOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArraySpecificOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArraySpecificType* mArraySpecificType;
  ArraySpecificOwnerType* mArraySpecificOwnerType;
  NiceMock<MockExpression> mFiveExpression;
  NiceMock<MockExpression> mTenExpression;
  llvm::BasicBlock *mBasicBlock;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;

  ArraySpecificOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    llvm::Constant* five = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 5);
    ON_CALL(mFiveExpression, generateIR(_, _)).WillByDefault(Return(five));
    ON_CALL(mFiveExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(mFiveExpression, printToStream(_, _)).WillByDefault(Invoke(printFive));
    list<const IExpression*> dimensions;
    dimensions.push_back(&mFiveExpression);
    mArraySpecificType = new ArraySpecificType(PrimitiveTypes::LONG_TYPE, dimensions);
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 1u);
    mArraySpecificOwnerType = new ArraySpecificOwnerType(mArraySpecificType);

    llvm::FunctionType* functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(mLLVMContext),
                                                               false);
    llvm::Function* function = llvm::Function::Create(functionType,
                                                      llvm::GlobalValue::InternalLinkage,
                                                      "test",
                                                      mContext.getModule());

    mBasicBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
  }
  
  static void printFive(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(ArraySpecificOwnerTypeTest, getArraySpecificTypeTest) {
  EXPECT_EQ(mArraySpecificType, mArraySpecificOwnerType->getArraySpecificType());
}

TEST_F(ArraySpecificOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[]*", mArraySpecificOwnerType->getTypeName().c_str());
}

TEST_F(ArraySpecificOwnerTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArraySpecificOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArraySpecificOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mArraySpecificOwnerType->canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArraySpecificOwnerType->canCastTo(mContext, mArraySpecificOwnerType));
  EXPECT_TRUE(mArraySpecificOwnerType->canCastTo(mContext, mArrayType));
}

TEST_F(ArraySpecificOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArraySpecificOwnerType->canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArraySpecificOwnerType->canAutoCastTo(mContext, mArraySpecificOwnerType));
  EXPECT_TRUE(mArraySpecificOwnerType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ArraySpecificOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mArraySpecificOwnerType->isPrimitive());
  EXPECT_TRUE(mArraySpecificOwnerType->isOwner());
  EXPECT_FALSE(mArraySpecificOwnerType->isReference());
  EXPECT_TRUE(mArraySpecificOwnerType->isArray());
  EXPECT_FALSE(mArraySpecificOwnerType->isFunction());
  EXPECT_FALSE(mArraySpecificOwnerType->isPackage());
  EXPECT_FALSE(mArraySpecificOwnerType->isNative());
  EXPECT_FALSE(mArraySpecificOwnerType->isPointer());
  EXPECT_FALSE(mArraySpecificOwnerType->isImmutable());
}

TEST_F(ArraySpecificOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mArraySpecificOwnerType->isController());
  EXPECT_FALSE(mArraySpecificOwnerType->isInterface());
  EXPECT_FALSE(mArraySpecificOwnerType->isModel());
  EXPECT_FALSE(mArraySpecificOwnerType->isNode());
}

TEST_F(ArraySpecificOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mArraySpecificOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("long[5]*", stringStream.str().c_str());
}

TEST_F(ArraySpecificOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mContext.getArrayType(PrimitiveTypes::LONG_TYPE, 1u),
            mArraySpecificOwnerType->getArrayType(mContext));
}

TEST_F(ArraySpecificOwnerTypeTest, injectTest) {
  InjectionArgumentList arguments;
  mArraySpecificOwnerType->inject(mContext, arguments, 0);

  *mStringStream << *mBasicBlock;

  string expected =
  "\nentry:"
  "\n  %arraySize = alloca i64"
  "\n  store i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i64* %arraySize"
  "\n  %conv = bitcast i64 5 to i64"
  "\n  %size = load i64, i64* %arraySize"
  "\n  %0 = mul i64 %conv, %size"
  "\n  %1 = add i64 %0, 24"
  "\n  store i64 %1, i64* %arraySize"
  "\n  %2 = load i64, i64* %arraySize"
  "\n  %malloccall = tail call i8* @malloc(i64 %2)"
  "\n  %3 = bitcast i8* %malloccall to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 %2, i32 4, i1 false)"
  "\n  %4 = bitcast i8* %malloccall to [0 x i64]*"
  "\n  %5 = getelementptr [0 x i64], [0 x i64]* %4, i64 0, i64 1"
  "\n  store i64 %conv, i64* %5"
  "\n  %6 = getelementptr [0 x i64], [0 x i64]* %4, i64 0, i64 2"
  "\n  store i64 %size, i64* %6"
  "\n  %7 = bitcast i8* %malloccall to { i64, i64, i64, [0 x i64] }*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
