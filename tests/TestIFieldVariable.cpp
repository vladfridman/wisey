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
  Model* mObject;
  Field* mField;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mLLVMContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    mField = new Field(PrimitiveTypes::INT_TYPE, "foo", 0, fieldArguments);
    fields["foo"] = mField;
    mObject = new Model(objectFullName, objectStructType);
    mObject->setFields(fields);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new HeapReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IFieldVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(IFieldVariableTest, checkAndFindFieldTest) {
  EXPECT_EQ(IFieldVariable::checkAndFindField(mContext, mObject, "foo"), mField);
}

TEST_F(IFieldVariableTest, getFieldPointerTest) {
  IFieldVariable::getFieldPointer(mContext, mObject, "foo");

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(IFieldVariableTest, checkAndFindFieldDeathTest) {
  EXPECT_EXIT(IFieldVariable::checkAndFindField(mContext, mObject, "bar"),
              ::testing::ExitedWithCode(1),
              "Error: Field bar is not found in object systems.vos.wisey.compiler.tests.MObject");
}
