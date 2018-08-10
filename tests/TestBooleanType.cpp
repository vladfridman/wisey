//
//  TestBooleanType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BooleanType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/BooleanType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct BooleanTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  BooleanType mBoleanType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  BooleanTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    IField* field = new ReceivedField(&mBoleanType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~BooleanTypeTest() {
    delete mStringStream;
  }
};

TEST_F(BooleanTypeTest, booleanTypeTest) {
  EXPECT_EQ(Type::getInt1Ty(mLLVMContext), mBoleanType.getLLVMType(mContext));
  EXPECT_STREQ("boolean", mBoleanType.getTypeName().c_str());
  EXPECT_EQ("%d", mBoleanType.getFormat());
}

TEST_F(BooleanTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(BooleanTypeTest, canCastTest) {
  EXPECT_FALSE(mBoleanType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mBoleanType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(BooleanTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type boolean to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sext i1 true to i8", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::BYTE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sext i1 true to i8", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sext i1 true to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i1 true to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv5 = sitofp i1 true to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mBoleanType.isPrimitive());
  EXPECT_FALSE(mBoleanType.isOwner());
  EXPECT_FALSE(mBoleanType.isReference());
  EXPECT_FALSE(mBoleanType.isArray());
  EXPECT_FALSE(mBoleanType.isFunction());
  EXPECT_FALSE(mBoleanType.isPackage());
  EXPECT_FALSE(mBoleanType.isNative());
  EXPECT_FALSE(mBoleanType.isPointer());
  EXPECT_FALSE(mBoleanType.isImmutable());
}

TEST_F(BooleanTypeTest, isObjectTest) {
  EXPECT_FALSE(mBoleanType.isController());
  EXPECT_FALSE(mBoleanType.isInterface());
  EXPECT_FALSE(mBoleanType.isModel());
  EXPECT_FALSE(mBoleanType.isNode());
}

TEST_F(BooleanTypeTest, createLocalVariableTest) {
  mBoleanType.createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %0 = alloca i1"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i1 false, i1* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanTypeTest, createFieldVariableTest) {
  mBoleanType.createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(BooleanTypeTest, createParameterVariableTest) {
  mBoleanType.createParameterVariable(mContext, "var", NULL, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(BooleanTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mBoleanType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type boolean is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
