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
    stateFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "left", 0));
    stateFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "right", 1));
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

TEST_F(ControllerInjectorTest, validInjectorTest) {
  ControllerInjector controllerInjector(mControllerTypeSpecifier);
  Value* result = controllerInjector.generateIR(mContext);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  EXPECT_EQ(2ul, mBlock->size());
  
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  string expected = "  %malloccall = tail call i8* @malloc(i32 trunc (i64 mul nuw (i64 ptrtoint"
    " (i32* getelementptr (i32, i32* null, i32 1) to i64), i64 2) to i32))";
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %injectvar = bitcast i8* %malloccall to %CMultiplier*");
  mStringBuffer.clear();
}

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
