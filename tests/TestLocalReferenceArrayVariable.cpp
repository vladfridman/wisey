//
//  TestLocalReferenceArrayVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalReferenceArrayVariable}
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
#include "wisey/LocalReferenceArrayVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalReferenceArrayVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  const wisey::ArrayType* mArrayType;
  
public:
  
  LocalReferenceArrayVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~LocalReferenceArrayVariableTest() {
  }
};

TEST_F(LocalReferenceArrayVariableTest, generateIdentifierIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, mArrayType->getLLVMType(mContext), "foo");
  LocalReferenceArrayVariable variable("foo", mArrayType, alloc);
  
  EXPECT_EQ(alloc, variable.generateIdentifierIR(mContext));
}

TEST_F(TestFileSampleRunner, arrayOfModelReferencesRunTest) {
  runFile("tests/samples/test_array_of_model_references.yz", "2018");
}

TEST_F(TestFileSampleRunner, referenceCountDecrementsOnArrayElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_array_element_unassign.yz", "5");
}

TEST_F(TestFileSampleRunner, referenceCountIncrementsOnAssignToArrayElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_array_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_array_element.yz:17)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, referenceArrayInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_initialized_to_null.yz:16)\n");
}


