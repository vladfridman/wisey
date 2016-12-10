//
//  Tests {@link AddditiveMultiplicativeExpression}
//  Yazyk
//
//  Created by Vladimir Fridman on 12/9/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/AddditiveMultiplicativeExpression.hpp"
#include "yazyk/codegen.hpp"
#include "yazyk/ILLVMBridge.hpp"
#include "yazyk/node.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;

using namespace llvm;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_METHOD1(generateIR, Value* (IRGenerationContext&));
};

class MockLLVMBridge : public ILLVMBridge {
public:
  MOCK_CONST_METHOD5(createBinaryOperator, Value* (Instruction::BinaryOps, Value*, Value*, const Twine&, BasicBlock*));
};

TEST(AddditiveMultiplicativeExpressionTest, SimpleTest) {
  NiceMock<MockExpression> lhs;
  NiceMock<MockExpression> rhs;
  NiceMock<MockLLVMBridge> llvmBridge;

  IRGenerationContext context;
  BasicBlock * bblock = BasicBlock::Create(context.getLLVMContext(), "test");
  context.pushBlock(bblock);
  EXPECT_CALL(llvmBridge, createBinaryOperator(_, _, _, _, _));
  
  AddditiveMultiplicativeExpression expression(lhs, '+', rhs, &llvmBridge);
  expression.generateIR(context);
}
