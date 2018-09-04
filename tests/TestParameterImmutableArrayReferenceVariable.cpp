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
  Function* mFunction;
  const wisey::ArrayType* mArrayType;
  const ImmutableArrayType* mImmutableArrayType;
  Value* mArrayPointer;
  ParameterImmutableArrayReferenceVariable* mVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterImmutableArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    mImmutableArrayType = mArrayType->getImmutable();
    
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
    mVariable = new ParameterImmutableArrayReferenceVariable("foo",
                                                             mImmutableArrayType,
                                                             mArrayPointer,
                                                             0);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterImmutableArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mImmutableArrayType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isStatic());
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, generateIdentifierIRTest) {
  EXPECT_EQ(mArrayPointer, mVariable->generateIdentifierIR(mContext, 0));
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, decrementReferenceCounterTest) {
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
  "\n  %2 = atomicrmw add i64* %1, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, generateAssignmentDathTest) {
  vector<const IExpression*> indices;
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVariable->generateAssignmentIR(mContext, NULL, indices, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Assignment to method parameters is not allowed\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ParameterImmutableArrayReferenceVariableTest, generateElementAssignmentDathTest) {
  vector<const IExpression*> indices;
  NiceMock<MockExpression> expression;
  indices.push_back(&expression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVariable->generateAssignmentIR(mContext, NULL, indices, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Attempting to change a value in an immutable array\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, parameterImmutableArrayReferenceRunTest) {
  runFile("tests/samples/test_parameter_immutable_array_reference.yz", 5);
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
