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
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

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
  NativeType* mNativeType;
  BasicBlock* mBasicBlock;
  FieldNativeVariable* mFieldNativeVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldNativeVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mNativeType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(mNativeType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.TObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    fields.push_back(new Field(STATE_FIELD, mNativeType, NULL, "mFoo", fieldArguments));
    mObject = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                objectFullName,
                                objectStructType);
    mObject->setFields(fields, 0u);
    
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
    
    mFieldNativeVariable = new FieldNativeVariable("mFoo", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldNativeVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldNativeVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mFieldNativeVariable->getName().c_str(), "mFoo");
  EXPECT_EQ(mFieldNativeVariable->getType(), mNativeType);
}

TEST_F(FieldNativeVariableTest, referenceFieldVariableGenerateIdentifierIRTest) {
  mFieldNativeVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.TObject, "
  "%systems.vos.wisey.compiler.tests.TObject* null, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
