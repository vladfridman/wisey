//
//  TestModelBuilderArgument.cpp
//  Yazyk
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

#include "TestFileSampleRunner.hpp"
#include "yazyk/ModelBuilderArgument.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using ::testing::NiceMock;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct ModelBuilderArgumentTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mFieldValue;
  Model* mModel;
  
  ModelBuilderArgumentTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType *structType = StructType::create(llvmContext, "MModel");
    structType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    ModelField* fieldA = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    (*fields)["mFieldA"] = fieldA;
    map<string, Method*>* methods = new map<string, Method*>();
    mModel = new Model("MModel", structType, fields, methods);
  }
};

TEST_F(ModelBuilderArgumentTest, TestValidModelBuilderArgument) {
  string argumentSpecifier("withFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldValue);
  
  EXPECT_TRUE(argument.checkArgument(mModel));
}

TEST_F(ModelBuilderArgumentTest, TestInvalidModelBuilderArgument) {
  string argumentSpecifier("mFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldValue);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mModel));
  EXPECT_STREQ("Error: Model builder argument should start with 'with'. e.g. .withField(value).\n",
               errorBuffer.str().c_str());

  cerr.rdbuf(streamBuffer);
}

TEST_F(ModelBuilderArgumentTest, TestMisspelledModelBuilderArgument) {
  string argumentSpecifier("withFielda");
  ModelBuilderArgument argument(argumentSpecifier, mFieldValue);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mModel));
  EXPECT_STREQ("Error: Model could not find field 'mFielda' in MODEL 'MModel'\n",
               errorBuffer.str().c_str());
  
  cerr.rdbuf(streamBuffer);
}
