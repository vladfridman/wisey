//
//  TestModelBuilderArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelBuilderArgument}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ModelBuilderArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ModelBuilderArgumentTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mFieldExpression;
  Model* mModel;
  Value* mValue;
  
  ModelBuilderArgumentTest() : mFieldExpression(new NiceMock<MockExpression>()) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["mFieldA"] = new Field(PrimitiveTypes::INT_TYPE, "mFieldA", 0, fieldArguments);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
    
    mValue = ConstantFP::get(Type::getFloatTy(llvmContext), 2.5);
    ON_CALL(*mFieldExpression, generateIR(_)).WillByDefault(Return(mValue));
    ON_CALL(*mFieldExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  }
};

TEST_F(ModelBuilderArgumentTest, validModelBuilderArgumentTest) {
  string argumentSpecifier("withFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_TRUE(argument.checkArgument(mModel));
}

TEST_F(ModelBuilderArgumentTest, invalidModelBuilderArgumentTest) {
  string argumentSpecifier("mFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mModel));
  EXPECT_STREQ("Error: Model builder argument should start with 'with'. e.g. .withField(value).\n",
               errorBuffer.str().c_str());

  cerr.rdbuf(streamBuffer);
}

TEST_F(ModelBuilderArgumentTest, misspelledModelBuilderArgumentTest) {
  string argumentSpecifier("withFielda");
  ModelBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mModel));
  EXPECT_STREQ("Error: Model builder could not find field mFielda in "
               "model systems.vos.wisey.compiler.tests.MModel\n",
               errorBuffer.str().c_str());
  
  cerr.rdbuf(streamBuffer);
}

TEST_F(ModelBuilderArgumentTest, getValueTest) {
  string argumentSpecifier("withFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getValue(mContext), mValue);
}

TEST_F(ModelBuilderArgumentTest, getTypeTest) {
  string argumentSpecifier("withFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getType(mContext), PrimitiveTypes::FLOAT_TYPE);
}
