//
//  TestController.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Controller}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/Controller.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct ControllerTest : public Test {
  Controller* mController;
  Method* mMethod;
  StructType* mStructType;
  Field* mLeftField;
  Field* mRightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ControllerTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "CMultiplier");
    mStructType->setBody(types);
    vector<Field*> receivedFields;
    vector<Field*> injectedFields;
    vector<Field*> stateFields;
    mLeftField = new Field(PrimitiveTypes::INT_TYPE, "left", 0);
    mRightField = new Field(PrimitiveTypes::INT_TYPE, "right", 1);
    receivedFields.push_back(mLeftField);
    receivedFields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    mMethod = new Method("calculate", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL);
    vector<Method*> methods;
    methods.push_back(mMethod);
    methods.push_back(new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments, 1, NULL));
    
    vector<Interface*> interfaces;
    mController = new Controller("CMultiplier",
                                 mStructType,
                                 receivedFields,
                                 injectedFields,
                                 stateFields,
                                 methods,
                                 interfaces);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ControllerTest() {
    delete mStringStream;
  }
};

TEST_F(ControllerTest, modelInstantiationTest) {
  EXPECT_STREQ(mController->getName().c_str(), "CMultiplier");
  EXPECT_EQ(mController->getTypeKind(), CONTROLLER_TYPE);
  EXPECT_EQ(mController->getLLVMType(mLLVMContext), mStructType->getPointerTo());
  EXPECT_EQ(mController->getInterfaces().size(), 0u);
}

TEST_F(ControllerTest, findFeildTest) {
  EXPECT_EQ(mController->findField("left"), mLeftField);
  EXPECT_EQ(mController->findField("right"), mRightField);
  EXPECT_EQ(mController->findField("depth"), nullptr);
}

TEST_F(ControllerTest, findMethodTest) {
  EXPECT_EQ(mController->findMethod("calculate"), mMethod);
  EXPECT_EQ(mController->findMethod("bar"), nullptr);
}

TEST_F(ControllerTest, methodIndexesTest) {
  EXPECT_EQ(mController->findMethod("calculate")->getIndex(), 0u);
  EXPECT_EQ(mController->findMethod("foo")->getIndex(), 1u);
}

TEST_F(ControllerTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mController->getObjectNameGlobalVariableName().c_str(),
               "controller.CMultiplier.name");
}
