//
//  TestLocalHeapVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalHeapVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct LocalHeapVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  
public:
  
  LocalHeapVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
  }
};

TEST_F(LocalHeapVariableTest, HeapVariableAssignmentTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  LocalHeapVariable *uninitializedHeapVariable =
    new LocalHeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  BitCastInst* bitCastInst = new BitCastInst(fooValue,
                                             fooValue->getType(),
                                             "foo",
                                             mContext.getBasicBlock());
  LocalHeapVariable localHeapVariable("bar", PrimitiveTypes::INT_TYPE, bitCastInst);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(bitCastInst));
  
  localHeapVariable.generateAssignmentIR(mContext, expression);

  EXPECT_EQ(mContext.getScopes().getVariable("foo"), nullptr);
  EXPECT_NE(localHeapVariable.getValue(), bitCastInst);
  EXPECT_TRUE(BitCastInst::classof(localHeapVariable.getValue()));
  EXPECT_STREQ(localHeapVariable.getValue()->getName().str().c_str(), "bar");
}

TEST_F(LocalHeapVariableTest, HeapVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  LocalHeapVariable localHeapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
 
  EXPECT_EQ(localHeapVariable.generateIdentifierIR(mContext, "bar"), fooValue);
}

TEST_F(LocalHeapVariableTest, UninitializedHeapVariableIdentifierDeathTest) {
  LocalHeapVariable localHeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_EXIT(localHeapVariable.generateIdentifierIR(mContext, "bar"),
              ::testing::ExitedWithCode(1),
              "Variable 'foo' is used before it has been initialized.");
}

TEST_F(LocalHeapVariableTest, FreeTest) {
  Value* fooValue = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
  LocalHeapVariable localHeapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);

  EXPECT_EQ(mBlock->size(), 0u);

  localHeapVariable.free(mContext);
  
  EXPECT_EQ(mBlock->size(), 2u);
  BasicBlock::iterator iterator = mBlock->begin();
  EXPECT_TRUE(BitCastInst::classof(&(*iterator)));
  EXPECT_TRUE(CallInst::classof(&(*++iterator)));
}

TEST_F(TestFileSampleRunner, ModelVariableAssignmentTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", "0");
}

TEST_F(TestFileSampleRunner, UsingUninitializedHeapVariableDeathTest) {
  expectFailIRGeneration("tests/samples/test_heap_variable_not_initialized.yz",
                         1,
                         "Error: Variable 'color' is used before it has been initialized.");
}

TEST_F(TestFileSampleRunner, IncompatableHeapVariableTypesInAssignmentDeathTest) {
  expectFailIRGeneration("tests/samples/test_incompatible_heap_variable_types_in_assignment.yz",
                         1,
                         "Error: Incopatible types: can not cast from type 'MShape' to 'MColor'");
}
