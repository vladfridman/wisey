//
//  TestHeapReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HeapReferenceVariable}
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
#include "wisey/HeapReferenceVariable.hpp"
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

struct HeapReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
 
public:
  
  HeapReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
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

TEST_F(HeapReferenceVariableTest, heapVariableAssignmentTest) {
  Type* llvmType = mModel->getLLVMType(mLLVMContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  
  IVariable* uninitializedHeapVariable = new HeapReferenceVariable("foo", mModel, fooValue);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((PointerType*) llvmType);
  HeapReferenceVariable heapReferenceVariable("bar", mModel, NULL);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(barValue));
  
  EXPECT_EQ(uninitializedHeapVariable->generateAssignmentIR(mContext, &expression), barValue);
}

TEST_F(HeapReferenceVariableTest, heapVariableIdentifierTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  HeapReferenceVariable heapReferenceVariable("foo", mModel, fooValue);
  
  EXPECT_EQ(heapReferenceVariable.generateIdentifierIR(mContext, "fooVal"), fooValue);
}

TEST_F(HeapReferenceVariableTest, existsInOuterScopeTest) {
  HeapReferenceVariable heapReferenceVariable("foo", mModel, NULL);

  EXPECT_FALSE(heapReferenceVariable.existsInOuterScope());
}

TEST_F(TestFileSampleRunner, modelVariableAssignmentRunTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", "0");
}

TEST_F(TestFileSampleRunner, interfaceVariableAssignmentRunTest) {
  runFile("tests/samples/test_interface_variable_assignment.yz", "25");
}

TEST_F(TestFileSampleRunner, usingUninitializedHeapVariableRunDeathTest) {
  compileAndRunFile("tests/samples/test_heap_variable_not_initialized.yz", 11);
}

TEST_F(TestFileSampleRunner, incompatableHeapVariableTypesInAssignmentRunDeathTest) {
  expectFailCompile("tests/samples/test_incompatible_heap_variable_types_in_assignment.yz",
                    1,
                    "Error: Incompatible types: can not cast from type "
                    "'systems.vos.wisey.compiler.tests.MShape\\*' to "
                    "'systems.vos.wisey.compiler.tests.MColor\\*'");
}
