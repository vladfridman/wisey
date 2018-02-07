//
//  TestParameterArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterArrayReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterArrayReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  Value* mArrayPointer;
  ParameterArrayReferenceVariable* mVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mArrayPointer = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
    mVariable = new ParameterArrayReferenceVariable("foo", mArrayType, mArrayPointer);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterArrayReferenceVariableTest, getNameTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
}

TEST_F(ParameterArrayReferenceVariableTest, getTypeTest) {
  EXPECT_EQ(mArrayType, mVariable->getType());
}

TEST_F(ParameterArrayReferenceVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mArrayPointer, mVariable->generateIdentifierIR(mContext));
}

TEST_F(ParameterArrayReferenceVariableTest, decrementReferenceCounterTest) {
  mVariable->decrementReferenceCounter(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 -1)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayReferenceVariableTest, generateAssignmentDathTest) {
  vector<const IExpression*> indices;
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, indices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(TestFileSampleRunner, parameterArrayReferenceOfIntsRunTest) {
  runFile("tests/samples/test_parameter_array_reference_of_ints.yz", "5");
}

TEST_F(TestFileSampleRunner, assignToParameterArrayReferenceVariableElementRunTest) {
  runFile("tests/samples/test_assign_to_parameter_array_reference_variable_element.yz", "7");
}

TEST_F(TestFileSampleRunner, parameterArrayReferenceOfIntsIncrementsReferenceCountRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_array_reference_of_ints_increments_reference_count.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_array_reference_of_ints_increments_reference_count.yz:20)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}
