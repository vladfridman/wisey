//
//  TestInjectionArgument.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InjectionArgument}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Field.hpp"
#include "wisey/InjectionArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct InjectionArgumentTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mFieldExpression;
  Controller* mController;
  Value* mValue;
  
  InjectionArgumentTest() : mFieldExpression(new NiceMock<MockExpression>()) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList arguments;
    Field* field = new Field(RECEIVED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mFieldA", arguments);
    fields.push_back(field);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mController->setFields(fields, 1u);
    
    mValue = ConstantFP::get(Type::getFloatTy(llvmContext), 2.5);
    ON_CALL(*mFieldExpression, generateIR(_, _)).WillByDefault(Return(mValue));
    ON_CALL(*mFieldExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
    ON_CALL(*mFieldExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(InjectionArgumentTest, validObjectBuilderArgumentTest) {
  string argumentSpecifier("withFieldA");
  InjectionArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_TRUE(argument.checkArgument(mController));
}

TEST_F(InjectionArgumentTest, invalidObjectBuilderArgumentTest) {
  string argumentSpecifier("mFieldA");
  InjectionArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mController));
  EXPECT_STREQ("Error: Injection argument should start with 'with'. e.g. .withField(value).\n",
               errorBuffer.str().c_str());
  
  cerr.rdbuf(streamBuffer);
}

TEST_F(InjectionArgumentTest, misspelledObjectBuilderArgumentTest) {
  string argumentSpecifier("withFielda");
  InjectionArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mController));
  EXPECT_STREQ("Error: Injector could not find field mFielda in object "
               "systems.vos.wisey.compiler.tests.CController\n",
               errorBuffer.str().c_str());
  
  cerr.rdbuf(streamBuffer);
}

TEST_F(InjectionArgumentTest, getValueTest) {
  string argumentSpecifier("withFieldA");
  InjectionArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getValue(mContext, PrimitiveTypes::VOID_TYPE), mValue);
}

TEST_F(InjectionArgumentTest, getTypeTest) {
  string argumentSpecifier("withFieldA");
  InjectionArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getType(mContext), PrimitiveTypes::FLOAT_TYPE);
}

TEST_F(InjectionArgumentTest, printToStreamTest) {
  string argumentSpecifier("withFieldA");
  ObjectBuilderArgument argument(argumentSpecifier, mFieldExpression);
  stringstream stringStream;
  argument.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("withFieldA(expression)", stringStream.str().c_str());
}
