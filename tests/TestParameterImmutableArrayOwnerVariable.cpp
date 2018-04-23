//
//  TestParameterImmutableArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterImmutableArrayOwnerVariable}
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
#include "wisey/ParameterImmutableArrayOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterImmutableArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const ImmutableArrayOwnerType* mImmutableArrayOwnerType;
  ParameterImmutableArrayOwnerVariable* mVariable;
  AllocaInst* mArrayAlloc;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterImmutableArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    mImmutableArrayOwnerType = mArrayType->getImmutable()->getOwner();
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
    mArrayAlloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
    mVariable = new ParameterImmutableArrayOwnerVariable("foo",
                                                         mImmutableArrayOwnerType,
                                                         mArrayAlloc);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterImmutableArrayOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mImmutableArrayOwnerType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isSystem());
}

TEST_F(ParameterImmutableArrayOwnerVariableTest, generateIdentifierIRTest) {
  mVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterImmutableArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  EXPECT_EQ(mArrayAlloc, mVariable->generateIdentifierReferenceIR(mContext));
}

TEST_F(ParameterImmutableArrayOwnerVariableTest, freeTest) {
  mVariable->free(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i64*"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* %1, i64 1)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterImmutableArrayOwnerVariableTest, setToNullTest) {
  mVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterImmutableArrayOwnerVariableTest, generateAssignmentDeathTest) {
  vector<const IExpression*> indices;
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, indices, 1),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(1\\): Error: Assignment to method parameters is not allowed");
}

TEST_F(ParameterImmutableArrayOwnerVariableTest, generateElementAssignmentDeathTest) {
  vector<const IExpression*> indices;
  NiceMock<MockExpression> expression;
  indices.push_back(&expression);
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, indices, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, parameterImmutableArrayOwnerRunTest) {
  runFile("tests/samples/test_parameter_immutable_array_owner.yz", "1");
}

TEST_F(TestFileRunner, parameterImmutableArrayOwnerRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_immutable_array_owner_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_immutable_array_owner_rce.yz:15)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, parameterImmutableArrayOwnerNullsPreviousOwnerRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_immutable_array_owner_nulls_previous_owner.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_immutable_array_owner_nulls_previous_owner.yz:15)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, parameterImmutableArrayOwnerElementAssignRunDeathTest) {
  expectFailCompile("tests/samples/test_parameter_immutable_array_owner_element_assign.yz",
                    1,
                    "tests/samples/test_parameter_immutable_array_owner_element_assign.yz\\(8\\): "
                    "Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, parameterImmutableArrayOwnerCastFromArrayReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_parameter_immutable_array_owner_assign.yz",
                    1,
                    "tests/samples/test_parameter_immutable_array_owner_assign.yz\\(8\\): "
                    "Error: Assignment to method parameters is not allowed");
}