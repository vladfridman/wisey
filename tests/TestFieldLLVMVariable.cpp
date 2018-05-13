//
//  TestFieldLLVMVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldLLVMVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldLLVMVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldLLVMVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mObject;
  const ILLVMType* mType;
  BasicBlock* mBasicBlock;
  FieldLLVMVariable* mFieldLLVMVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldLLVMVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mType = LLVMPrimitiveTypes::I16;
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mType, "mFoo", 0));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType,
                                        mContext.getImportProfile(),
                                        0);
    mObject->setFields(mContext, fields, 1u);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mFieldLLVMVariable = new FieldLLVMVariable("mFoo", mObject, 0);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldLLVMVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldLLVMVariableTest, basicFieldsTest) {
  EXPECT_STREQ("mFoo", mFieldLLVMVariable->getName().c_str());
  EXPECT_EQ(mType, mFieldLLVMVariable->getType());
  EXPECT_TRUE(mFieldLLVMVariable->isField());
  EXPECT_FALSE(mFieldLLVMVariable->isSystem());
}

TEST_F(FieldLLVMVariableTest, generateIdentifierIRTest) {
  mFieldLLVMVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %mFoo = load i16, i16* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldLLVMVariableTest, generateIdentifierReferenceIRTest) {
  mFieldLLVMVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldLLVMVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(LLVMPrimitiveTypes::I16));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  
  mFieldLLVMVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  store i16 3, i16* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, fieldLLVMVariableRunTest) {
  runFile("tests/samples/test_field_llvm_variable.yz", "7");
}
