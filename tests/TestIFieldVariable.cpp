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

#include "wisey/Field.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LocalReferenceVariable.hpp"
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
  Controller* mController;
  Field* mStateField;
  Field* mInjectedField;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ExpressionList fieldArguments;
    vector<Type*> types;
    types.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mLLVMContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<Field*> controllerFields;
    mInjectedField = new Field(INJECTED_FIELD, PrimitiveTypes::INT_TYPE, "foo", fieldArguments);
    mStateField = new Field(STATE_FIELD, PrimitiveTypes::INT_TYPE, "bar", fieldArguments);
    controllerFields.push_back(mInjectedField);
    controllerFields.push_back(mStateField);
    mController = Controller::newController(controllerFullName, controllerStructType);
    mController->setFields(controllerFields, 0u);
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mController->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new LocalReferenceVariable("this", mController, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IFieldVariableTest() {
    delete mStringStream;
    delete mController;
  }
};

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentTest) {
  EXPECT_EQ(IFieldVariable::checkAndFindFieldForAssignment(mContext, mController, "bar"),
            mStateField);
}

TEST_F(IFieldVariableTest, getFieldPointerTest) {
  IFieldVariable::getFieldPointer(mContext, mController, "foo");

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.CController*, "
  "%systems.vos.wisey.compiler.tests.CController** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentDoesNotExistDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindFieldForAssignment(mContext, mController, "var"),
              ::testing::ExitedWithCode(1),
              "Error: Field var is not found in object "
              "systems.vos.wisey.compiler.tests.CController");
}

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentNotAssignableDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindFieldForAssignment(mContext, mController, "foo"),
              ::testing::ExitedWithCode(1),
              "Error: Can not assign to field foo");
}
