//
//  TestParameterSystemReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterSystemReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterSystemReferenceVariable.hpp"
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

struct ParameterSystemReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterSystemReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width"));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height"));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 1u);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterSystemReferenceVariableTest, basicFieldsTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterSystemReferenceVariable variable("foo", mModel, fooValue);
  
  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mModel, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_TRUE(variable.isSystem());
}

TEST_F(ParameterSystemReferenceVariableTest, variableAssignmentDeathTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterSystemReferenceVariable variable("foo", mModel, fooValue);
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(variable.generateAssignmentIR(mContext, NULL, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(ParameterSystemReferenceVariableTest, variableIdentifierTest) {
  Value* fooValueStore = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterSystemReferenceVariable variable("foo", mModel, fooValueStore);
  
  EXPECT_EQ(fooValueStore, variable.generateIdentifierIR(mContext));
}

TEST_F(ParameterSystemReferenceVariableTest, decrementReferenceCounterTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterSystemReferenceVariable variable("foo", mModel, fooValue);
  
  variable.decrementReferenceCounter(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MShape* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 -1)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
