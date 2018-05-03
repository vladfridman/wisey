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

#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

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
  Model* mModel;
  StateField* mStateField;
  FixedField* mFixedField;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(PrimitiveTypes::INT->getLLVMType(mContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<IField*> controllerFields;
    mStateField = new StateField(PrimitiveTypes::INT, "bar", 0);
    controllerFields.push_back(mStateField);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType,
                                            mContext.getImportProfile(),
                                            0);
    mController->setFields(mContext, controllerFields, 1u);
    
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             modelStructType,
                             mContext.getImportProfile(),
                             0);
    mFixedField = new FixedField(PrimitiveTypes::INT, "foo", 0);
    vector<IField*> modelFields;
    modelFields.push_back(mFixedField);
    mModel->setFields(mContext, modelFields, 1u);
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mController->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mController,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IFieldVariableTest() {
    delete mStringStream;
    delete mController;
  }
};

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentTest) {
  EXPECT_EQ(IFieldVariable::checkAndFindFieldForAssignment(mContext, mController, "bar", 0),
            mStateField);
}

TEST_F(IFieldVariableTest, getFieldPointerTest) {
  IFieldVariable::getFieldPointer(mContext, mController, "bar", 0);

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentDoesNotExistDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindFieldForAssignment(mContext, mController, "var", 1),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(1\\): Error: Field var is not found in object "
              "systems.vos.wisey.compiler.tests.CController");
}

TEST_F(IFieldVariableTest, checkAndFindFieldForAssignmentNotAssignableDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindFieldForAssignment(mContext, mModel, "foo", 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: Can not assign to field foo");
}
