//
//  TestImmutableArrayType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ImmutableArrayType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "ImmutableArrayOwnerType.hpp"
#include "ImmutableArrayType.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ImmutableArrayTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArrayType* mMultiDimentionalArrayType;
  ImmutableArrayType* mImmutableArrayType;
  ImmutableArrayType* mMultiDimentionalImmutableArrayType;
  llvm::BasicBlock* mEntryBlock;
  llvm::BasicBlock* mDeclareBlock;
  llvm::Function* mFunction;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
  ImmutableArrayTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<unsigned long> dimensions;
    mArrayType = new ArrayType(PrimitiveTypes::LONG, 1u);
    mImmutableArrayType = new ImmutableArrayType(mArrayType);
    mMultiDimentionalArrayType = new ArrayType(PrimitiveTypes::LONG, 2u);
    mMultiDimentionalImmutableArrayType = new ImmutableArrayType(mMultiDimentionalArrayType);
    
    llvm::FunctionType* functionType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = llvm::Function::Create(functionType,
                                       llvm::GlobalValue::InternalLinkage,
                                       "main",
                                       mContext.getModule());
    mDeclareBlock = llvm::BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    InjectionArgumentList injectionArgumentList;
    IField* field = new ReceivedField(mImmutableArrayType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ImmutableArrayTypeTest, getOwnerTest) {
  const ImmutableArrayOwnerType* immutableArrayOwnerType = mImmutableArrayType->getOwner();
  EXPECT_TRUE(immutableArrayOwnerType->isArray());
  EXPECT_TRUE(immutableArrayOwnerType->isOwner());
  EXPECT_EQ(mArrayType, immutableArrayOwnerType->getArrayType(mContext, 0));
}

TEST_F(ImmutableArrayTypeTest, getElementTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG, mMultiDimentionalImmutableArrayType->getElementType());
}

TEST_F(ImmutableArrayTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalImmutableArrayType->getNumberOfDimensions());
}

TEST_F(ImmutableArrayTypeTest, getNameTest) {
  EXPECT_STREQ("immutable long[]", mImmutableArrayType->getTypeName().c_str());
}

TEST_F(ImmutableArrayTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mImmutableArrayType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ImmutableArrayTypeTest, canCastToTest) {
  EXPECT_FALSE(mImmutableArrayType->canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mImmutableArrayType->canCastTo(mContext, mImmutableArrayType));
  EXPECT_FALSE(mImmutableArrayType->canCastTo(mContext, mArrayType));
  EXPECT_FALSE(mImmutableArrayType->canCastTo(mContext, mImmutableArrayType->getOwner()));
}

TEST_F(ImmutableArrayTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mImmutableArrayType->canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mImmutableArrayType->canAutoCastTo(mContext, mImmutableArrayType));
  EXPECT_FALSE(mImmutableArrayType->canAutoCastTo(mContext, mArrayType));
  EXPECT_FALSE(mImmutableArrayType->canAutoCastTo(mContext, mImmutableArrayType->getOwner()));
}

TEST_F(ImmutableArrayTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mImmutableArrayType->isPrimitive());
  EXPECT_FALSE(mImmutableArrayType->isOwner());
  EXPECT_TRUE(mImmutableArrayType->isReference());
  EXPECT_TRUE(mImmutableArrayType->isArray());
  EXPECT_FALSE(mImmutableArrayType->isFunction());
  EXPECT_FALSE(mImmutableArrayType->isPackage());
  EXPECT_FALSE(mImmutableArrayType->isNative());
  EXPECT_FALSE(mImmutableArrayType->isPointer());
  EXPECT_TRUE(mImmutableArrayType->isImmutable());
}

TEST_F(ImmutableArrayTypeTest, isObjectTest) {
  EXPECT_FALSE(mImmutableArrayType->isController());
  EXPECT_FALSE(mImmutableArrayType->isInterface());
  EXPECT_FALSE(mImmutableArrayType->isModel());
  EXPECT_FALSE(mImmutableArrayType->isNode());
}

TEST_F(ImmutableArrayTypeTest, createLocalVariableTest) {
  mImmutableArrayType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %0 = alloca { i64, i64, i64, [0 x i64] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store { i64, i64, i64, [0 x i64] }* null, { i64, i64, i64, [0 x i64] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ImmutableArrayTypeTest, createFieldVariableTest) {
  mImmutableArrayType->createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(nullptr, variable);
  EXPECT_EQ(mImmutableArrayType, variable->getType());
}

TEST_F(ImmutableArrayTypeTest, createParameterVariableTest) {
  llvm::Value* value = llvm::ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
  mImmutableArrayType->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mFunction;

  string expected =
  "\ndefine internal i32 @main() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = icmp eq { i64, i64, i64, [0 x i64] }* null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i64] }* null to i64*"
  "\n  %2 = atomicrmw add i64* %1, i64 1 monotonic"
  "\n  br label %if.end"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ImmutableArrayTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mImmutableArrayType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type immutable long[] is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, immutableSubarrayRunTest) {
  runFile("tests/samples/test_immutable_subarray.yz", 7);
}
