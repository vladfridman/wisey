//
//  TestByteType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ByteType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ByteType.hpp"
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

struct ByteTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ByteType mByteType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  ByteTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    IField* field = new ReceivedField(&mByteType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ByteTypeTest() {
    delete mStringStream;
  }
};

TEST_F(ByteTypeTest, ByteTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext), mByteType.getLLVMType(mContext));
  EXPECT_STREQ("byte", mByteType.getTypeName().c_str());
  EXPECT_EQ("%d", mByteType.getFormat());
}

TEST_F(ByteTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mByteType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(ByteTypeTest, canCastTest) {
  EXPECT_FALSE(mByteType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mByteType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mByteType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(ByteTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt8Ty(mLLVMContext), 'a');
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mByteType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type byte to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = icmp ne i8 97, 0", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::BYTE, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sext i8 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sext i8 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sitofp i8 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mByteType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i8 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ByteTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mByteType.isPrimitive());
  EXPECT_FALSE(mByteType.isOwner());
  EXPECT_FALSE(mByteType.isReference());
  EXPECT_FALSE(mByteType.isArray());
  EXPECT_FALSE(mByteType.isFunction());
  EXPECT_FALSE(mByteType.isPackage());
  EXPECT_FALSE(mByteType.isNative());
  EXPECT_FALSE(mByteType.isPointer());
  EXPECT_FALSE(mByteType.isPointer());
}

TEST_F(ByteTypeTest, isObjectTest) {
  EXPECT_FALSE(mByteType.isController());
  EXPECT_FALSE(mByteType.isInterface());
  EXPECT_FALSE(mByteType.isModel());
  EXPECT_FALSE(mByteType.isNode());
}

TEST_F(ByteTypeTest, createLocalVariableTest) {
  mByteType.createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %0 = alloca i8"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i8 0, i8* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ByteTypeTest, createFieldVariableTest) {
  mByteType.createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ByteTypeTest, createParameterVariableTest) {
  mByteType.createParameterVariable(mContext, "var", NULL, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ByteTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mByteType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type byte is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
