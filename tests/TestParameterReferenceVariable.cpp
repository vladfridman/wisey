//
//  TestParameterReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
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

struct ParameterReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

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
    fields.push_back(new FixedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterReferenceVariableTest, basicFieldsTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterReferenceVariable variable("foo", mModel, fooValue);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mModel, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(ParameterReferenceVariableTest, parameterReferenceVariableAssignmentDeathTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterReferenceVariable parameterReferenceVariable("foo", mModel, fooValue);
  vector<const IExpression*> arrayIndices;

  EXPECT_EXIT(parameterReferenceVariable.generateAssignmentIR(mContext, NULL, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(ParameterReferenceVariableTest, generateIdentifierIRTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterReferenceVariable parameterReferenceVariable("foo", mModel, fooValue);
  
  EXPECT_EQ(fooValue, parameterReferenceVariable.generateIdentifierIR(mContext));
}

TEST_F(ParameterReferenceVariableTest, decrementReferenceCounterTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ParameterReferenceVariable parameterReferenceVariable("foo", mModel, fooValue);
  
  parameterReferenceVariable.decrementReferenceCounter(mContext);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MShape* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 -1)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, assignParameterReferenceToLocalOwnerCompileTest) {
  compileFile("tests/samples/test_assign_parameter_reference_local_owner.yz");
}

TEST_F(TestFileRunner, assignParameterReferenceToLocalReferenceCompileTest) {
  compileFile("tests/samples/test_assign_parameter_reference_to_local_reference.yz");
}

TEST_F(TestFileRunner, assignParameterReferenceToNullCompileTest) {
  compileFile("tests/samples/test_assign_parameter_reference_to_null.yz");
}
