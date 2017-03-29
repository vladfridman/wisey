//
//  TestFieldReference.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldReference}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/FieldReference.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldReferenceTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mExpression;
  
  FieldReferenceTest() {
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType* structType = StructType::create(llvmContext, "MShape");
    structType->setBody(types);
    map<string, Field*> fields;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    Model* model = new Model("MShape", structType, fields, methods, interfaces);
    
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(model));
  }
  
  ~FieldReferenceTest() { }
};

TEST_F(FieldReferenceTest, fieldReferenceTypeTest) {
  FieldReference fieldReference(mExpression, "width");
  
  EXPECT_EQ(fieldReference.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(FieldReferenceTest, referenceFieldInPrimitiveTypeDeathTest) {
  Mock::AllowLeak(&mExpression);
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  FieldReference fieldReference(mExpression, "width");

  EXPECT_EXIT(fieldReference.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Attempt to reference field 'width' in a primitive type expression");
}

TEST_F(FieldReferenceTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(&mExpression);
  FieldReference fieldReference(mExpression, "width");
  
  EXPECT_EXIT(fieldReference.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Model fields can not be references and "
              "objects they reference can not be released");
}

TEST_F(TestFileSampleRunner, modelFieldReferenceRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_model_field_reference.yz",
                         1,
                         "Error: All model fields are private. "
                         "Implement getters and setters to use fields");
}

TEST_F(TestFileSampleRunner, incorrectModelFieldReferenceRunDeathTest) {
  expectFailIRGeneration("tests/samples/test_model_incorrect_field_reference.yz",
                         1,
                         "Error: Field 'mWidth' is not found in model 'MColor'");
}
