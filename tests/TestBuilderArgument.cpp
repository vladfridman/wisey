//
//  TestBuilderArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BuilderArgument}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "BuilderArgument.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct BuilderArgumentTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mFieldExpression;
  Model* mModel;
  Value* mValue;
  
  BuilderArgumentTest() : mFieldExpression(new NiceMock<MockExpression>()) {
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
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "mFieldA", 0));
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

TEST_F(BuilderArgumentTest, validBuilderArgumentTest) {
  string argumentSpecifier("withFieldA");
  BuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_TRUE(argument.checkArgument(mContext, mModel, 0));
}

TEST_F(BuilderArgumentTest, invalidBuilderArgumentTest) {
  string argumentSpecifier("mFieldA");
  BuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mContext, mModel, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Object builder argument should start with 'with'. e.g. .withField(value).\n",
               errorBuffer.str().c_str());

  cerr.rdbuf(streamBuffer);
}

TEST_F(BuilderArgumentTest, misspelledBuilderArgumentTest) {
  string argumentSpecifier("withFielda");
  BuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument(mContext, mModel, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Object builder could not find field mFielda in object systems.vos.wisey.compiler.tests.MModel\n",
               errorBuffer.str().c_str());
  
  cerr.rdbuf(streamBuffer);
}

TEST_F(BuilderArgumentTest, getValueTest) {
  string argumentSpecifier("withFieldA");
  BuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getValue(mContext, PrimitiveTypes::VOID), mValue);
}

TEST_F(BuilderArgumentTest, getTypeTest) {
  string argumentSpecifier("withFieldA");
  BuilderArgument argument(argumentSpecifier, mFieldExpression);
  
  EXPECT_EQ(argument.getType(mContext), PrimitiveTypes::FLOAT);
}

TEST_F(BuilderArgumentTest, printToStreamTest) {
  string argumentSpecifier("withFieldA");
  BuilderArgument argument(argumentSpecifier, mFieldExpression);
  stringstream stringStream;
  argument.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("withFieldA(expression)", stringStream.str().c_str());
}
