//
//  TestLocalArrayVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalArrayVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalArrayVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalArrayVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  const wisey::ArrayType* mArrayType;
  
public:
  
  LocalArrayVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 3u);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
  }
  
  ~LocalArrayVariableTest() {
  }
};

TEST_F(LocalArrayVariableTest, generateIdentifierIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, mArrayType->getLLVMType(mContext), "foo");
  LocalArrayVariable variable("foo", mArrayType, alloc);

  EXPECT_EQ(alloc, variable.generateIdentifierIR(mContext));
}

TEST_F(LocalArrayVariableTest, generateAssignmentIRDeathTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, mArrayType->getLLVMType(mContext), "foo");
  LocalArrayVariable variable("foo", mArrayType, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  NiceMock<MockExpression> expression;
  ::Mock::AllowLeak(&expression);
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(assignValue));
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(variable.generateAssignmentIR(mContext, &expression, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Expression does not reference an array element");
}

TEST_F(TestFileSampleRunner, intArrayRunTest) {
  runFile("tests/samples/test_int_array.yz", "5");
}

TEST_F(TestFileSampleRunner, arrayOfModelsRunTest) {
  runFile("tests/samples/test_array_of_models.yz", "2018");
}

TEST_F(TestFileSampleRunner, referenceArrayInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_initialized_to_null.yz:16)\n");
}

