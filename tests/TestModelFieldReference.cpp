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

#include "MockExpression.hpp"
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
    map<string, Field*> fields;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, 0);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, 1);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    Model* model = new Model("Shape", structType, fields, methods, interfaces);
    
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(model));
  }
  
  ~ModelFieldReferenceTest() { }
};

TEST_F(ModelFieldReferenceTest, modelFieldReferenceTypeTest) {
  ModelFieldReference modelFieldReference(mExpression, "width");
  
  EXPECT_EQ(modelFieldReference.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(ModelFieldReferenceTest, referenceFieldInPrimitiveTypeDeathTest) {
  Mock::AllowLeak(&mExpression);
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  ModelFieldReference modelFieldReference(mExpression, "width");

  EXPECT_EXIT(modelFieldReference.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Attempt to reference field 'width' in a primitive type expression");
}

TEST_F(ModelFieldReferenceTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(&mExpression);
  ModelFieldReference modelFieldReference(mExpression, "width");
  
  EXPECT_EXIT(modelFieldReference.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Model fields can not be references and "
              "objects they reference can not be released");
}

TEST_F(TestFileSampleRunner, modelModelFieldReferenceRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_model_field_reference.yz",
                         1,
                         "Error: All model fields are private. "
                         "Implement getters and setters to use fields");
}

TEST_F(TestFileSampleRunner, modelIncorrectModelFieldReferenceRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_model_incorrect_field_reference.yz",
                         1,
                         "Error: Field 'mWidth' is not found in model 'MColor'");
}
