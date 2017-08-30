//
//  TestIFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IFieldVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/FieldFixed.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IFieldVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mNode;
  FieldState* mFieldState;
  FieldFixed* mFieldFixed;
  
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ExpressionList fieldArguments;
    vector<Type*> types;
    types.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mLLVMContext));
    string nodeFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, nodeFullName);
    nodeStructType->setBody(types);
    vector<FieldFixed*> nodeFixedFields;
    vector<FieldState*> nodeStateFields;
    mFieldFixed = new FieldFixed(PrimitiveTypes::INT_TYPE, "foo", 0, fieldArguments);
    mFieldState = new FieldState(PrimitiveTypes::INT_TYPE, "bar", 1, fieldArguments);
    nodeFixedFields.push_back(mFieldFixed);
    nodeStateFields.push_back(mFieldState);
    mNode = new Node(nodeFullName, nodeStructType);
    mNode->setFields(nodeFixedFields, nodeStateFields);
    
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mNode->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new HeapReferenceVariable("this", mNode, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IFieldVariableTest() {
    delete mStringStream;
    delete mNode;
  }
};

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentTest) {
  EXPECT_EQ(IFieldVariable::checkAndFindFieldForAssignment(mContext, mNode, "bar"), mFieldState);
}

TEST_F(IFieldVariableTest, getFieldPointerTest) {
  IFieldVariable::getFieldPointer(mContext, mNode, "foo");

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NNode, "
  "%systems.vos.wisey.compiler.tests.NNode* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentDoesNotExistDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindFieldForAssignment(mContext, mNode, "var"),
              ::testing::ExitedWithCode(1),
              "Error: Field var is not found in object systems.vos.wisey.compiler.tests.NNode");
}

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentNotAssignableDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindFieldForAssignment(mContext, mNode, "foo"),
              ::testing::ExitedWithCode(1),
              "Error: Can not assign to field foo");
}
