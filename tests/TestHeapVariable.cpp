//
//  TestHeapVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HeapVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct HeapVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
 
public:
  
  HeapVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(HeapVariableTest, heapVariableAssignmentTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  HeapVariable* uninitializedHeapVariable =
    new HeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  BitCastInst* bitCastInst = IRWriter::newBitCastInst(mContext, fooValue, fooValue->getType());
  HeapVariable heapVariable("bar", PrimitiveTypes::INT_TYPE, bitCastInst);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(bitCastInst));
  
  heapVariable.generateAssignmentIR(mContext, &expression);

  EXPECT_EQ(heapVariable.getValue(), bitCastInst);
  EXPECT_TRUE(BitCastInst::classof(heapVariable.getValue()));
}

TEST_F(HeapVariableTest, heapVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  HeapVariable heapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
 
  EXPECT_EQ(heapVariable.generateIdentifierIR(mContext, "bar"), fooValue);
}

TEST_F(HeapVariableTest, existsInOuterScopeTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  HeapVariable heapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);

  EXPECT_FALSE(heapVariable.existsInOuterScope());
}

TEST_F(HeapVariableTest, uninitializedHeapVariableIdentifierDeathTest) {
  HeapVariable heapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_EXIT(heapVariable.generateIdentifierIR(mContext, "bar"),
              ::testing::ExitedWithCode(1),
              "Variable 'foo' is used before it has been initialized.");
}

TEST_F(HeapVariableTest, freeTest) {
  Value* fooValue = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext)->getPointerTo());
  HeapVariable heapVariable("foo", mModel->getOwner(), fooValue);

  EXPECT_EQ(mBlock->size(), 0u);

  heapVariable.free(mContext);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %variableObject = load i32*, i32** null"
  "\n  %0 = bitcast i32* %variableObject to i8*"
  "\n  tail call void @free(i8* %0)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, modelVariableAssignmentRunTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", "0");
}

TEST_F(TestFileSampleRunner, interfaceVariableAssignmentRunTest) {
  runFile("tests/samples/test_interface_variable_assignment.yz", "25");
}

TEST_F(TestFileSampleRunner, nullPointerExceptionRunTest) {
  compileAndRunFile("tests/samples/test_heap_owner_variable_not_initialized.yz", 11);
}

TEST_F(TestFileSampleRunner, usingUninitializedHeapVariableRunDeathTest) {
  expectFailCompile("tests/samples/test_heap_variable_not_initialized.yz",
                    1,
                    "Error: Variable 'color' is used before it has been initialized.");
}

TEST_F(TestFileSampleRunner, incompatableHeapVariableTypesInAssignmentRunDeathTest) {
  expectFailCompile("tests/samples/test_incompatible_heap_variable_types_in_assignment.yz",
                    1,
                    "Error: Incompatible types: can not cast from type "
                    "'systems.vos.wisey.compiler.tests.MShape\\*' to "
                    "'systems.vos.wisey.compiler.tests.MColor\\*'");
}
