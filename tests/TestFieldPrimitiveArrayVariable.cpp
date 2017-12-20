//
//  TestFieldPrimitiveArrayVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldPrimitiveArrayVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FieldPrimitiveArrayVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldPrimitiveArrayVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mObject;
  BasicBlock* mBasicBlock;
  FieldPrimitiveArrayVariable* mFieldPrimitiveArrayVariable;
  wisey::ArrayType* mArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldPrimitiveArrayVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = new wisey::ArrayType(PrimitiveTypes::INT_TYPE, 5);
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(mArrayType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList arguments;
    fields.push_back(new Field(STATE_FIELD, mArrayType, "mFoo", arguments));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType);
    mObject->setFields(fields, 1u);
    
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
    IVariable* thisVariable = new ParameterReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldPrimitiveArrayVariable = new FieldPrimitiveArrayVariable("mFoo", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldPrimitiveArrayVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldPrimitiveArrayVariableTest, basicFieldsTest) {
  EXPECT_STREQ("mFoo", mFieldPrimitiveArrayVariable->getName().c_str());
  EXPECT_EQ(mArrayType, mFieldPrimitiveArrayVariable->getType());
}

TEST_F(FieldPrimitiveArrayVariableTest, primitiveFieldVariableGenerateIdentifierIRTest) {
  mFieldPrimitiveArrayVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldPrimitiveArrayVariableTest, primitiveFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  arrayIndices.push_back(new FakeExpression(one, PrimitiveTypes::INT_TYPE));
  
  mFieldPrimitiveArrayVariable->generateAssignmentIR(mContext,
                                                     &assignToExpression,
                                                     arrayIndices,
                                                     0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %1 = getelementptr [5 x i32], [5 x i32]* %0, i32 0, i32 1"
  "\n  store i32 3, i32* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldPrimitiveArrayVariableTest, primitiveFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(PrimitiveTypes::CHAR_TYPE->getLLVMType(mContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  arrayIndices.push_back(new FakeExpression(one, PrimitiveTypes::INT_TYPE));

  mFieldPrimitiveArrayVariable->generateAssignmentIR(mContext,
                                                     &assignToExpression,
                                                     arrayIndices,
                                                     0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %1 = getelementptr [5 x i32], [5 x i32]* %0, i32 0, i32 1"
  "\n  %conv = zext i16 3 to i32"
  "\n  store i32 %conv, i32* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileSampleRunner, fieldPrimitiveArrayVariableRunTest) {
  runFile("tests/samples/test_field_primitive_array_variable.yz", "3");
}

TEST_F(TestFileSampleRunner, fieldPrimitiveArrayVariableWithAutocastRunTest) {
  runFile("tests/samples/test_field_primitive_array_variable_with_autocast.yz", "77");
}

