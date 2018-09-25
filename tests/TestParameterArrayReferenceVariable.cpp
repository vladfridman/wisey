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

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "ParameterArrayReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"

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
  Function* mFunction;
  const wisey::ArrayType* mArrayType;
  Value* mArrayPointer;
  ParameterArrayReferenceVariable* mVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mArrayPointer = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
    mVariable = new ParameterArrayReferenceVariable("foo", mArrayType, mArrayPointer, 0);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mArrayType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isStatic());
}

TEST_F(ParameterArrayReferenceVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mArrayPointer, mVariable->generateIdentifierIR(mContext, 0));
}

TEST_F(ParameterArrayReferenceVariableTest, decrementReferenceCounterTest) {
  mVariable->decrementReferenceCounter(mContext);
  
  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = icmp eq { i64, i64, i64, [0 x i32] }* null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* null to i64*"
  "\n  %count = load i64, i64* %1"
  "\n  %2 = add i64 %count, -1"
  "\n  store i64 %2, i64* %1"
  "\n  br label %if.end"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayReferenceVariableTest, generateAssignmentDeathTest) {
  vector<const IExpression*> indices;
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVariable->generateAssignmentIR(mContext, NULL, indices, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Assignment to method parameters is not allowed\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, parameterArrayReferenceOfIntsRunTest) {
  runFile("tests/samples/test_parameter_array_reference_of_ints.yz", 5);
}

TEST_F(TestFileRunner, assignToParameterArrayReferenceVariableElementRunTest) {
  runFile("tests/samples/test_assign_to_parameter_array_reference_variable_element.yz", 7);
}

TEST_F(TestFileRunner, parameterArrayReferenceOfIntsIncrementsReferenceCountRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_array_reference_of_ints_increments_reference_count.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_array_reference_of_ints_increments_reference_count.yz:20)\n"
                               "Details: Object referenced by expression of type int[][]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}
