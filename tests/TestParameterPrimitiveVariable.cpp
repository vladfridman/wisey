//
//  TestParameterPrimitiveVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 11/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterPrimitiveVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "ParameterPrimitiveVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterPrimitiveVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterPrimitiveVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
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
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterPrimitiveVariableTest, basicFieldsTest) {
  Value* fooValueStore = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterPrimitiveVariable variable("foo", PrimitiveTypes::INT, fooValueStore, 0);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(PrimitiveTypes::INT, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isStatic());
}

TEST_F(ParameterPrimitiveVariableTest, generateIdentifierIRTest) {
  Value* fooValueStore = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterPrimitiveVariable variable("foo", PrimitiveTypes::INT, fooValueStore, 0);

  EXPECT_EQ(fooValueStore, variable.generateIdentifierIR(mContext, 0));
}

TEST_F(ParameterPrimitiveVariableTest, parameterReferenceVariableAssignmentDeathTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterPrimitiveVariable variable("foo", PrimitiveTypes::INT, fooValue, 0);
  vector<const IExpression*> arrayIndices;

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(variable.generateAssignmentIR(mContext, NULL, arrayIndices, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Assignment to method parameters is not allowed\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ParameterPrimitiveVariableTest, parameterReferenceVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ParameterPrimitiveVariable variable("foo", PrimitiveTypes::INT, fooValue, 0);

  EXPECT_EQ(variable.generateIdentifierIR(mContext, 0), fooValue);
}

TEST_F(TestFileRunner, methodAssignToPrimitiveArgumentDeathRunTest) {
  expectFailCompile("tests/samples/test_method_assign_to_primitive_argument.yz",
                    1,
                    "Error: Assignment to method parameters is not allowed");
}

