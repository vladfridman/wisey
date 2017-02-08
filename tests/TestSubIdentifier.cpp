//
//  TestSubIdentifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link SubIdentifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/SubIdentifier.hpp"

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

struct SubIdentifierTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mExpression;
  
  SubIdentifierTest() {
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType* structType = StructType::create(llvmContext, "Shape");
    structType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    (*fields)["width"] = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    (*fields)["height"] = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    Model* model = new Model("Shape", structType, fields);
    
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(model));
  }
  
  ~SubIdentifierTest() { }
};

TEST_F(SubIdentifierTest, TestSubIdentifierType) {
  SubIdentifier subIdentifier(mExpression, "width");
  
  EXPECT_EQ(subIdentifier.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(TestFileSampleRunner, ModelSubIdentifierTest) {
  runFile("tests/samples/test_model_subidentifier.yz", "7");
}

TEST_F(TestFileSampleRunner, ModelIncorrectSubIdentifierDeathTest) {
  expectFailIRGeneration("tests/samples/test_model_incorrect_subidentifier.yz",
                         1,
                         "Error: Field 'width' is not found in model 'Color'");
}
