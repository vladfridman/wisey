//
//  TestLocalPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalPrimitiveVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalPrimitiveVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalPrimitiveVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalPrimitiveVariableTest() {
    delete mStringStream;
  }
};

TEST_F(LocalPrimitiveVariableTest, basicFieldsTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");
  LocalPrimitiveVariable variable("foo", PrimitiveTypes::INT_TYPE, alloc);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(LocalPrimitiveVariableTest, generateAssignmentIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");

  LocalPrimitiveVariable variable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(assignValue));
  vector<const IExpression*> arrayIndices;

  variable.generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  ASSERT_EQ(2ul, mBlock->size());
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 5, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalPrimitiveVariableTest, generateAssignmentIRWithCastTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");
  LocalPrimitiveVariable variable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(assignValue));
  vector<const IExpression*> arrayIndices;

  variable.generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  ASSERT_EQ(3ul, mBlock->size());
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %conv = zext i1 true to i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 %conv, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalPrimitiveVariableTest, generateIdentifierIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");
  LocalPrimitiveVariable variable("foo", PrimitiveTypes::INT_TYPE, alloc);
  
  variable.generateIdentifierIR(mContext);

  ASSERT_EQ(2ul, mBlock->size());
  
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = load i32, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, assignmentWithAutocastRunTest) {
  runFile("tests/samples/test_assignment_with_autocast.yz", "1");
}
