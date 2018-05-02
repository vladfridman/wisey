//
//  TestObjectBuilderArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectBuilderArgument}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/ObjectBuilderArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ObjectBuilderArgumentTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mFieldExpression;
  Model* mModel;
  Value* mValue;
  
  ObjectBuilderArgumentTest() : mFieldExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(llvmContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    InjectionArgumentList arguments;
    fields.push_back(new FixedField(PrimitiveTypes::INT, "mFieldA", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);
    
    mValue = ConstantFP::get(Type::getFloatTy(llvmContext), 2.5);
    ON_CALL(*mFieldExpression, generateIR(_, _)).WillByDefault(Return(mValue));
    ON_CALL(*mFieldExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
    ON_CALL(*mFieldExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ObjectBuilderArgumentTest, validObjectBuilderArgumentTest) {
  string argumentSpecifier("withFieldA");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_TRUE(argument.checkArgument(mModel));
}

TEST_F(ObjectBuilderArgumentTest, invalidObjectBuilderArgumentTest) {
  string argumentSpecifier("mFieldA");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mModel));
  EXPECT_STREQ("Error: Object builder argument should start with 'with'. e.g. .withField(value).\n",
               errorBuffer.str().c_str());

  cerr.rdbuf(streamBuffer);
}

TEST_F(ObjectBuilderArgumentTest, misspelledObjectBuilderArgumentTest) {
  string argumentSpecifier("withFielda");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mModel));
  EXPECT_STREQ("Error: Object builder could not find field mFielda in "
               "object systems.vos.wisey.compiler.tests.MModel\n",
               errorBuffer.str().c_str());
  
  cerr.rdbuf(streamBuffer);
}

TEST_F(ObjectBuilderArgumentTest, getValueTest) {
  string argumentSpecifier("withFieldA");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getValue(mContext, PrimitiveTypes::VOID), mValue);
}

TEST_F(ObjectBuilderArgumentTest, getTypeTest) {
  string argumentSpecifier("withFieldA");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getType(mContext), PrimitiveTypes::FLOAT);
}

TEST_F(ObjectBuilderArgumentTest, printToStreamTest) {
  string argumentSpecifier("withFieldA");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  stringstream stringStream;
  argument.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("withFieldA(expression)", stringStream.str().c_str());
}
