//
//  TestParameterImmutableArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterImmutableArrayReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/ParameterImmutableArrayReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterImmutableArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const ImmutableArrayType* mImmutableArrayType;
  Value* mArrayPointer;
  ParameterImmutableArrayReferenceVariable* mVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterImmutableArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    mImmutableArrayType = mArrayType->getImmutable();
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mArrayPointer = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
    mVariable = new ParameterImmutableArrayReferenceVariable("foo",
                                                             mImmutableArrayType,
                                                             mArrayPointer);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterImmutableArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mImmutableArrayType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isSystem());
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mArrayPointer, mVariable->generateIdentifierIR(mContext));
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, decrementReferenceCounterTest) {
  mVariable->decrementReferenceCounter(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  call void @__adjustReferenceCounterForImmutableArray(i8* %0, i64 -1)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, generateAssignmentDathTest) {
  vector<const IExpression*> indices;
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, indices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, generateElementAssignmentDathTest) {
  vector<const IExpression*> indices;
  NiceMock<MockExpression> expression;
  indices.push_back(&expression);

  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, indices, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, parameterImmutableArrayReferenceRunTest) {
  runFile("tests/samples/test_parameter_immutable_array_reference.yz", "5");
}

TEST_F(TestFileRunner, parameterImmutableArrayReferenceElementAssignRunDeathTest) {
  expectFailCompile("tests/samples/test_parameter_immutable_array_reference_element_assign.yz",
                    1,
                    "tests/samples/test_parameter_immutable_array_reference_element_assign.yz\\(8\\): "
                    "Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, parameterImmutableArrayReferenceCastFromArrayReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_parameter_immutable_array_reference_assign.yz",
                    1,
                    "tests/samples/test_parameter_immutable_array_reference_assign.yz\\(10\\): "
                    "Error: Assignment to method parameters is not allowed");
}
