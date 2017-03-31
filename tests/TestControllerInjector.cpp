//
//  TestControllerInjector.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerInjector}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "yazyk/ControllerInjector.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ControllerInjectorTest : Test {
  IRGenerationContext mContext;
  Controller* mController;
  NiceMock<MockExpression> mFieldValue1;
  NiceMock<MockExpression> mFieldValue2;
  NiceMock<MockExpression> mFieldValue3;
  ControllerTypeSpecifier mControllerTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ControllerInjectorTest() : mControllerTypeSpecifier(ControllerTypeSpecifier("CMultiplier")) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType *structType = StructType::create(llvmContext, "CMultiplier");
    structType->setBody(types);
    vector<Field*> receivedFields;
    vector<Field*> injectedFields;
    vector<Field*> stateFields;
    ExpressionList fieldArguments;
    stateFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "left", 0, fieldArguments));
    stateFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "right", 1, fieldArguments));
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mController = new Controller("CMultiplier",
                                 structType,
                                 receivedFields,
                                 injectedFields,
                                 stateFields,
                                 methods,
                                 interfaces);
    mContext.addController(mController);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ControllerInjectorTest() {
    delete mStringStream;
  }
};

TEST_F(ControllerInjectorTest, releaseOwnershipTest) {
  ControllerInjector controllerInjector(mControllerTypeSpecifier);
  controllerInjector.generateIR(mContext);
  
  ostringstream stream;
  stream << "__tmp" << (long) &controllerInjector;
  string temporaryVariableName = stream.str();
  
  EXPECT_NE(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
  
  controllerInjector.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
}

TEST_F(ControllerInjectorTest, getTypeTest) {
  ControllerInjector controllerInjector(mControllerTypeSpecifier);
  
  EXPECT_EQ(controllerInjector.getType(mContext), mController);
}
