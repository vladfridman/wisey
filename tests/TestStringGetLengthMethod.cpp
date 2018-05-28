//
//  TestStringGetLengthMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StringGetLengthMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/StringGetLengthMethod.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StringGetLengthMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  const StringGetLengthMethod* mStringGetLengthMethod;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  StringGetLengthMethodTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mStringGetLengthMethod = StringGetLengthMethod::STRING_GET_LENGTH_METHOD;
  }
  
  ~StringGetLengthMethodTest() {
    delete mStringStream;
  }
};

TEST_F(StringGetLengthMethodTest, getNameTest) {
  EXPECT_STREQ("getLength", mStringGetLengthMethod->getName().c_str());
}

TEST_F(StringGetLengthMethodTest, getLLVMTypeTest) {
  EXPECT_EQ(FunctionType::get(Type::getInt64Ty(mLLVMContext), false),
            mStringGetLengthMethod->getLLVMType(mContext));
}

TEST_F(StringGetLengthMethodTest, generateIRTest) {
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  StringGetLengthMethod::generateIR(mContext, new FakeExpression(null, PrimitiveTypes::STRING));
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call i64 @strlen(i8* null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(StringGetLengthMethodTest, isTypeKindTest) {
  EXPECT_FALSE(mStringGetLengthMethod->isPrimitive());
  EXPECT_FALSE(mStringGetLengthMethod->isOwner());
  EXPECT_FALSE(mStringGetLengthMethod->isReference());
  EXPECT_FALSE(mStringGetLengthMethod->isArray());
  EXPECT_TRUE(mStringGetLengthMethod->isFunction());
  EXPECT_FALSE(mStringGetLengthMethod->isPackage());
  EXPECT_FALSE(mStringGetLengthMethod->isNative());
  EXPECT_FALSE(mStringGetLengthMethod->isPointer());
  EXPECT_FALSE(mStringGetLengthMethod->isImmutable());
}

TEST_F(StringGetLengthMethodTest, isObjectTest) {
  EXPECT_FALSE(mStringGetLengthMethod->isController());
  EXPECT_FALSE(mStringGetLengthMethod->isInterface());
  EXPECT_FALSE(mStringGetLengthMethod->isModel());
  EXPECT_FALSE(mStringGetLengthMethod->isNode());
}

TEST_F(StringGetLengthMethodTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mStringGetLengthMethod->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type string.getLength is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, stringLengthRunTest) {
  runFile("tests/samples/test_string_length.yz", 7);
}
