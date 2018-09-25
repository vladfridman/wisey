//
//  TestStringFormatType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StringFormatType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "StringFormatType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct StringFormatTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  StringFormatType mStringFormatType;
  
public:
  
  StringFormatTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StringFormatTypeTest() {
    delete mStringStream;
  }
};

TEST_F(StringFormatTypeTest, stringFormatTypeTest) {
  EXPECT_STREQ("stringformat", mStringFormatType.getTypeName().c_str());
}

TEST_F(StringFormatTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_FALSE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_TRUE(mStringFormatType.canAutoCastTo(mContext, PrimitiveTypes::STRING_FORMAT));
}

TEST_F(StringFormatTypeTest, canCastTest) {
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_FALSE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_TRUE(mStringFormatType.canCastTo(mContext, PrimitiveTypes::STRING_FORMAT));
}

TEST_F(StringFormatTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mStringFormatType.isPrimitive());
  EXPECT_FALSE(mStringFormatType.isOwner());
  EXPECT_FALSE(mStringFormatType.isReference());
  EXPECT_FALSE(mStringFormatType.isArray());
  EXPECT_FALSE(mStringFormatType.isFunction());
  EXPECT_FALSE(mStringFormatType.isPackage());
  EXPECT_FALSE(mStringFormatType.isNative());
  EXPECT_FALSE(mStringFormatType.isPointer());
}

TEST_F(StringFormatTypeTest, isObjectTest) {
  EXPECT_FALSE(mStringFormatType.isController());
  EXPECT_FALSE(mStringFormatType.isInterface());
  EXPECT_FALSE(mStringFormatType.isModel());
  EXPECT_FALSE(mStringFormatType.isNode());
  EXPECT_FALSE(mStringFormatType.isPointer());
}

TEST_F(StringFormatTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mStringFormatType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type stringformat is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
