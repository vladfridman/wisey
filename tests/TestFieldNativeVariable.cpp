//
//  TestFieldNativeVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldNativeVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldNativeVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldNativeVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Thread* mObject;
  const LLVMPointerType* mPointerType;
  BasicBlock* mBasicBlock;
  FieldNativeVariable* mFieldNativeVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldNativeVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mPointerType = LLVMPrimitiveTypes::I8->getPointerType();
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mPointerType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.TObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mPointerType, "mFoo"));
    mObject = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
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
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldNativeVariable = new FieldNativeVariable("mFoo", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldNativeVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldNativeVariableTest, basicFieldsTest) {
  EXPECT_STREQ("mFoo", mFieldNativeVariable->getName().c_str());
  EXPECT_EQ(mPointerType, mFieldNativeVariable->getType());
  EXPECT_TRUE(mFieldNativeVariable->isField());
  EXPECT_FALSE(mFieldNativeVariable->isSystem());
}

TEST_F(FieldNativeVariableTest, generateIdentifierIRTest) {
  mFieldNativeVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.TObject, %systems.vos.wisey.compiler.tests.TObject* null, i32 0, i32 1"
  "\n  %1 = load i8*, i8** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldNativeVariableTest, generateIdentifierReferenceIRTest) {
  mFieldNativeVariable->generateIdentifierReferenceIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.TObject, %systems.vos.wisey.compiler.tests.TObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
