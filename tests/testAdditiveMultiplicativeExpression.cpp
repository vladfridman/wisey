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
#include "yazyk/log.hpp"
#include "yazyk/node.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Property;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

extern int yyparse();
extern FILE* yyin;
extern Block* programBlock;

class MockExpression : public IExpression {
public:
  MOCK_METHOD1(generateIR, Value* (IRGenerationContext&));
};

class MockLLVMBridge : public ILLVMBridge {
public:
  MOCK_CONST_METHOD5(createBinaryOperator, Value* (Instruction::BinaryOps, Value*, Value*, const Twine&, BasicBlock*));
};

struct AddditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext context;
  NiceMock<MockExpression> lhs;
  NiceMock<MockExpression> rhs;
  NiceMock<MockLLVMBridge> llvmBridge;
  Value * lhsValue;
  Value * rhsValue;
  BasicBlock * bblock;
  
  AddditiveMultiplicativeExpressionTest() {
    lhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 3);
    rhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);

    bblock = BasicBlock::Create(context.getLLVMContext(), "test");
    context.pushBlock(bblock);

    ON_CALL(lhs, generateIR(_)).WillByDefault(Return(lhsValue));
    ON_CALL(rhs, generateIR(_)).WillByDefault(Return(rhsValue));
  }
};

TEST_F(AddditiveMultiplicativeExpressionTest, AdditionTest) {
  EXPECT_CALL(llvmBridge, createBinaryOperator(Instruction::Add,
                                               lhsValue,
                                               rhsValue,
                                               Property(&Twine::str, StrEq("add")),
                                               bblock));
  
  AddditiveMultiplicativeExpression expression(lhs, '+', rhs, &llvmBridge);
  expression.generateIR(context);
}

TEST_F(AddditiveMultiplicativeExpressionTest, SubtractionTest) {
  EXPECT_CALL(llvmBridge, createBinaryOperator(Instruction::Sub,
                                               lhsValue,
                                               rhsValue,
                                               Property(&Twine::str, StrEq("sub")),
                                               bblock));
  
  AddditiveMultiplicativeExpression expression(lhs, '-', rhs, &llvmBridge);
  expression.generateIR(context);
}

struct AddditiveMultiplicativeExpressionRunTest : Test {
  AddditiveMultiplicativeExpressionRunTest() {
    InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    
    Log::setLogLevel(ERRORLEVEL);
  }
  
  ~AddditiveMultiplicativeExpressionRunTest() {
    fclose(yyin);
  }
  
  void runFile(string fileName, string expectedResult) {
    yyin = fopen(fileName.c_str(), "r");
    if (yyin == NULL) {
      Log::e("Sample test " + fileName + " not found!\n");
      FAIL();
      return;
    }
    yyparse();
    
    IRGenerationContext context;
    context.generateIR(*programBlock);
    GenericValue result = context.runCode();
    string resultString = result.IntVal.toString(10, true);
    ASSERT_STREQ(resultString.c_str(), expectedResult.c_str());
  }
};

TEST_F(AddditiveMultiplicativeExpressionRunTest, AdditionRunTest) {
  runFile("tests/samples/test_addition.yz", "7");
}

TEST_F(AddditiveMultiplicativeExpressionRunTest, SubtractionRunTest) {
  runFile("tests/samples/test_subtraction.yz", "14");
}

TEST_F(AddditiveMultiplicativeExpressionRunTest, MultiplicationRunTest) {
  runFile("tests/samples/test_multiplication.yz", "50");
}

TEST_F(AddditiveMultiplicativeExpressionRunTest, DivisionRunTest) {
  runFile("tests/samples/test_division.yz", "5");
}
