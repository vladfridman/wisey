//
//  TestLocalHeapVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalHeapVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalHeapVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  
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

    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType* structType = StructType::create(llvmContext, "Shape");
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("Shape", structType, fields, methods, interfaces);}
};

TEST_F(LocalHeapVariableTest, heapVariableAssignmentTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  LocalHeapVariable* uninitializedHeapVariable =
    new LocalHeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  BitCastInst* bitCastInst = IRWriter::newBitCastInst(mContext, fooValue, fooValue->getType());
  LocalHeapVariable localHeapVariable("bar", PrimitiveTypes::INT_TYPE, bitCastInst);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(bitCastInst));
  
  localHeapVariable.generateAssignmentIR(mContext, expression);

  EXPECT_EQ(localHeapVariable.getValue(), bitCastInst);
  EXPECT_TRUE(BitCastInst::classof(localHeapVariable.getValue()));
}

TEST_F(LocalHeapVariableTest, heapVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  LocalHeapVariable localHeapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
 
  EXPECT_EQ(localHeapVariable.generateIdentifierIR(mContext, "bar"), fooValue);
}

TEST_F(LocalHeapVariableTest, uninitializedHeapVariableIdentifierDeathTest) {
  LocalHeapVariable localHeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_EXIT(localHeapVariable.generateIdentifierIR(mContext, "bar"),
              ::testing::ExitedWithCode(1),
              "Variable 'foo' is used before it has been initialized.");
}

TEST_F(LocalHeapVariableTest, freeTest) {
  Value* fooValue = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
  LocalHeapVariable localHeapVariable("foo", mModel, fooValue);

  EXPECT_EQ(mBlock->size(), 0u);

  localHeapVariable.free(mContext);
  
  EXPECT_EQ(mBlock->size(), 2u);
  BasicBlock::iterator iterator = mBlock->begin();
  EXPECT_TRUE(BitCastInst::classof(&(*iterator)));
  EXPECT_TRUE(CallInst::classof(&(*++iterator)));
}

TEST_F(TestFileSampleRunner, modelVariableAssignmentRunTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", "0");
}

TEST_F(TestFileSampleRunner, interfaceVariableAssignmentRunTest) {
  runFile("tests/samples/test_interface_variable_assignment.yz", "25");
}

TEST_F(TestFileSampleRunner, usingUninitializedHeapVariableRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_heap_variable_not_initialized.yz",
                         1,
                         "Error: Variable 'color' is used before it has been initialized.");
}

TEST_F(TestFileSampleRunner, IncompatableHeapVariableTypesInAssignmentRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_incompatible_heap_variable_types_in_assignment.yz",
                         1,
                         "Error: Incopatible types: can not cast from type 'MShape' to 'MColor'");
}
