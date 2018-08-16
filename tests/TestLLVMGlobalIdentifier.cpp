//
//  TestLLVMGlobalIdentifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMGlobalIdentifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMGlobalIdentifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMGlobalIdentifierTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mName;
  BasicBlock* mEntryBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  LLVMGlobalIdentifierTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    mName = "myglobal";
    
    new GlobalVariable(*mContext.getModule(),
                       Type::getInt8Ty(mLLVMContext),
                       true,
                       GlobalValue::ExternalLinkage,
                       nullptr,
                       mName);
    mContext.setLLVMGlobalVariable(LLVMPrimitiveTypes::I8, mName);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LLVMGlobalIdentifierTest() {
  }
};

TEST_F(LLVMGlobalIdentifierTest, generateIRTest) {
  LLVMGlobalIdentifier identifier(mName, 0);
  identifier.generateIR(mContext, PrimitiveTypes::VOID);

  *mStringStream << *mEntryBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = load i8, i8* @myglobal\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMGlobalIdentifierTest, undeclaredVariableDeathTest) {
  LLVMGlobalIdentifier identifier("foo", 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(identifier.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: LLVM global 'foo' is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(LLVMGlobalIdentifierTest, getTypeTest) {
  LLVMGlobalIdentifier identifier(mName, 0);

  EXPECT_EQ(LLVMPrimitiveTypes::I8, identifier.getType(mContext));
}

TEST_F(LLVMGlobalIdentifierTest, isConstantTest) {
  LLVMGlobalIdentifier identifier(mName, 0);
  
  EXPECT_TRUE(identifier.isConstant());
}

TEST_F(LLVMGlobalIdentifierTest, isAssignableTest) {
  LLVMGlobalIdentifier identifier(mName, 0);

  EXPECT_FALSE(identifier.isAssignable());
}

TEST_F(LLVMGlobalIdentifierTest, printToStreamTest) {
  LLVMGlobalIdentifier identifier(mName, 0);

  stringstream stringStream;
  identifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::global::myglobal", stringStream.str().c_str());
}
