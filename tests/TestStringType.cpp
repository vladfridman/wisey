//
//  TestStringType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StringType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StringTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  StringType mStringType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  StringTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    IField* field = new FixedField(&mStringType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StringTypeTest() {
    delete mStringStream;
  }
};

TEST_F(StringTypeTest, stringTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mStringType.getLLVMType(mContext));
  EXPECT_STREQ("string", mStringType.getTypeName().c_str());
  EXPECT_EQ("%s", mStringType.getFormat());
}

TEST_F(StringTypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
}

TEST_F(StringTypeTest, canCastTest) {
  EXPECT_TRUE(mStringType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mStringType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::VOID));
}

TEST_F(StringTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* expressionValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5));
  
  EXPECT_ANY_THROW(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 5));
  EXPECT_ANY_THROW(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 5));
  EXPECT_ANY_THROW(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 5));
  EXPECT_ANY_THROW(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 5));
  EXPECT_ANY_THROW(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'string' to 'void'\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'string' to 'char'\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'string' to 'int'\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'string' to 'long'\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'string' to 'float'\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type 'string' to 'double'\n",
               buffer.str().c_str());

  Value* one = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  EXPECT_EQ(one, mStringType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0));

  EXPECT_EQ(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::STRING, 5),
            expressionValue);
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(StringTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mStringType.isPrimitive());
  EXPECT_FALSE(mStringType.isOwner());
  EXPECT_FALSE(mStringType.isReference());
  EXPECT_FALSE(mStringType.isArray());
  EXPECT_FALSE(mStringType.isFunction());
  EXPECT_FALSE(mStringType.isPackage());
  EXPECT_FALSE(mStringType.isNative());
  EXPECT_FALSE(mStringType.isPointer());
}

TEST_F(StringTypeTest, isObjectTest) {
  EXPECT_FALSE(mStringType.isController());
  EXPECT_FALSE(mStringType.isInterface());
  EXPECT_FALSE(mStringType.isModel());
  EXPECT_FALSE(mStringType.isNode());
  EXPECT_FALSE(mStringType.isPointer());
}

TEST_F(StringTypeTest, createLocalVariableTest) {
  mStringType.createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i8*"
  "\n  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @__empty.str, i32 0, i32 0), i8** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(StringTypeTest, createFieldVariableTest) {
  mStringType.createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(StringTypeTest, createParameterVariableTest) {
  mStringType.createParameterVariable(mContext, "var", NULL, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(StringTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mStringType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type string is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, stringCastToBooleanRunTest) {
  runFile("tests/samples/test_string_cast_to_boolean.yz", "3");
}

TEST_F(TestFileRunner, mstringCreateFromStringRunTest) {
  runFile("tests/samples/test_mstring_create_from_string.yz", "7");
}

TEST_F(TestFileRunner, mstringCreateFromCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_char_array.yz",
                     "mstring: abc length: 3\n",
                     "");
}

TEST_F(TestFileRunner, mstringCreateFromImmutableCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_immutable_char_array.yz",
                     "mstring: def length: 3\n",
                     "");
}

TEST_F(TestFileRunner, mstringCreateFromEmptyCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_mstring_create_from_empty_char_array.yz",
                     "mstring:  length: 0\n",
                     "");
}

TEST_F(TestFileRunner, mstringCopyRunTest) {
  runFile("tests/samples/test_mstring_copy.yz", "7");
}

TEST_F(TestFileRunner, cstringCreateFromStringRunTest) {
  runFile("tests/samples/test_cstring_create_from_string.yz", "3");
}

TEST_F(TestFileRunner, cstringCopyRunTest) {
  runFile("tests/samples/test_cstring_copy.yz", "3");
}

TEST_F(TestFileRunner, cstringAppendStringRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_string.yz",
                     "Before cstring: 123456 capacity: 10\n"
                     "After cstring: 1234567 capacity: 10\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendStringLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_string_long.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 12\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendCharArrayLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_char_array.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 13\n",
                     "");
}

TEST_F(TestFileRunner, cstringAppendImmutableCharArrayLongRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_append_immutable_char_array.yz",
                     "Before cstring: 123 capacity: 6\n"
                     "After cstring: 1234567 capacity: 13\n",
                     "");
}

TEST_F(TestFileRunner, cstringToModelRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_to_model.yz",
                     "before: 123 length: 3 capacity: 6\n"
                     "after: 123 length: 3\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromModelRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_model.yz",
                     "mstring: 123 length: 3\n"
                     "cstring: 123 length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_char_array.yz",
                     "cstring: abc length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromImmutableCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_immutable_char_array.yz",
                     "cstring: def length: 3 capacity: 6\n",
                     "");
}

TEST_F(TestFileRunner, cstringCreateFromEmptyCharArrayRunTest) {
  runFileCheckOutput("tests/samples/test_cstring_create_from_empty_char_array.yz",
                     "cstring:  length: 0 capacity: 1\n",
                     "");
}
