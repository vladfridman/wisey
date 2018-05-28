//
//  TestFieldPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldPrimitiveVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
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

struct FieldPrimitiveVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mObject;
  BasicBlock* mBasicBlock;
  FieldPrimitiveVariable* mFieldPrimitiveVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldPrimitiveVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(PrimitiveTypes::INT->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(PrimitiveTypes::INT, "foo", 0));
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
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mFieldPrimitiveVariable = new FieldPrimitiveVariable("foo", mObject, 0);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldPrimitiveVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldPrimitiveVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldPrimitiveVariable->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::INT, mFieldPrimitiveVariable->getType());
  EXPECT_TRUE(mFieldPrimitiveVariable->isField());
  EXPECT_FALSE(mFieldPrimitiveVariable->isSystem());
}

TEST_F(FieldPrimitiveVariableTest, generateIdentifierIRTest) {
  mFieldPrimitiveVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %foo = load i32, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldPrimitiveVariableTest, generateIdentifierReferenceIRTest) {
  mFieldPrimitiveVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldPrimitiveVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  mFieldPrimitiveVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  store i32 3, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldPrimitiveVariableTest, generateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(PrimitiveTypes::CHAR->getLLVMType(mContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  mFieldPrimitiveVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %conv = zext i8 3 to i32"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  store i32 %conv, i32* %0\n";
 
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, objectFieldSetRunTest) {
  runFile("tests/samples/test_object_field_set.yz", 7);
}

TEST_F(TestFileRunner, objectFieldSetWithAutocastRunTest) {
  runFile("tests/samples/test_object_field_set_with_autocast.yz", 1);
}
