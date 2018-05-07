//
//  TestIntType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IntType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IntTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  IntType mIntType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  IntTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    IField* field = new FixedField(&mIntType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IntTypeTest() {
    delete mStringStream;
  }
};

TEST_F(IntTypeTest, intTypeTest) {
  EXPECT_EQ(Type::getInt32Ty(mLLVMContext), mIntType.getLLVMType(mContext));
  EXPECT_STREQ("int", mIntType.getTypeName().c_str());
  EXPECT_EQ("%d", mIntType.getFormat());
}

TEST_F(IntTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(IntTypeTest, canCastTest) {
  EXPECT_FALSE(mIntType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mIntType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(IntTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mIntType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'int' to 'void'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = icmp ne i32 5, 0", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i32 5 to i8", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sitofp i32 5 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IntTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mIntType.isPrimitive());
  EXPECT_FALSE(mIntType.isOwner());
  EXPECT_FALSE(mIntType.isReference());
  EXPECT_FALSE(mIntType.isArray());
  EXPECT_FALSE(mIntType.isFunction());
  EXPECT_FALSE(mIntType.isPackage());
  EXPECT_FALSE(mIntType.isNative());
  EXPECT_FALSE(mIntType.isPointer());
  EXPECT_FALSE(mIntType.isPointer());
}

TEST_F(IntTypeTest, isObjectTest) {
  EXPECT_FALSE(mIntType.isController());
  EXPECT_FALSE(mIntType.isInterface());
  EXPECT_FALSE(mIntType.isModel());
  EXPECT_FALSE(mIntType.isNode());
}

TEST_F(IntTypeTest, createLocalVariableTest) {
  mIntType.createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i32"
  "\n  store i32 0, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IntTypeTest, createFieldVariableTest) {
  mIntType.createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(IntTypeTest, createParameterVariableTest) {
  mIntType.createParameterVariable(mContext, "var", NULL, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(IntTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mIntType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type int is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, intCastToBooleanRunTest) {
  runFile("tests/samples/test_int_cast_to_boolean.yz", "3");
}
