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

#include "TestFileSampleRunner.hpp"
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

struct ParameterReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", fieldArguments));
    mModel = Model::newModel(modelFullName, structType);
    mModel->setFields(fields, 1u);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterReferenceVariableTest, heapReferenceMethodParameterVariableAssignmentDeathTest) {
  ParameterReferenceVariable heapMethodParameter("foo", mModel, NULL);
  
  EXPECT_EXIT(heapMethodParameter.generateAssignmentIR(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Assignment to method parameters is not allowed");
}

TEST_F(ParameterReferenceVariableTest, heapReferenceMethodParameterVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ParameterReferenceVariable heapMethodParameter("foo", mModel, fooValue);
  
  EXPECT_EQ(heapMethodParameter.generateIdentifierIR(mContext, "bar"), fooValue);
}

TEST_F(TestFileSampleRunner, assignParameterReferenceToLocalOwnerCompileTest) {
  compileFile("tests/samples/test_assign_parameter_reference_local_owner.yz");
}

TEST_F(TestFileSampleRunner, assignParameterReferenceToLocalReferenceCompileTest) {
  compileFile("tests/samples/test_assign_parameter_reference_to_local_reference.yz");
}

TEST_F(TestFileSampleRunner, assignParameterReferenceToNullCompileTest) {
  compileFile("tests/samples/test_assign_parameter_reference_to_null.yz");
}
