//
//  TestModelFieldReference.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelFieldReference}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/ModelFieldReference.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct ModelFieldReferenceTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mExpression;
  
  ModelFieldReferenceTest() {
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType* structType = StructType::create(llvmContext, "Shape");
    structType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    (*fields)["width"] = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    (*fields)["height"] = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    map<string, Method*>* methods = new map<string, Method*>();
    Model* model = new Model("Shape", structType, fields, methods);
    
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(model));
  }
  
  ~ModelFieldReferenceTest() { }
};

TEST_F(ModelFieldReferenceTest, TestModelFieldReferenceType) {
  ModelFieldReference modelFieldReference(mExpression, "width");
  
  EXPECT_EQ(modelFieldReference.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(ModelFieldReferenceTest, TestReferenceFieldInPrimitiveTypeDeathTest) {
  Mock::AllowLeak(&mExpression);
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  ModelFieldReference modelFieldReference(mExpression, "width");

  EXPECT_EXIT(modelFieldReference.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Attempt to reference field 'width' in a primitive type expression");
}

TEST_F(TestFileSampleRunner, ModelModelFieldReferenceTest) {
  runFile("tests/samples/test_model_field_reference.yz", "7");
}

TEST_F(TestFileSampleRunner, ModelIncorrectModelFieldReferenceDeathTest) {
  expectFailIRGeneration("tests/samples/test_model_incorrect_field_reference.yz",
                         1,
                         "Error: Field 'mWidth' is not found in model 'MColor'");
}
