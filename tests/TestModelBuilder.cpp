//
//  TestModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelBuilder}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelBuilder.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ModelBuilderTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  NiceMock<MockExpression> mFieldValue1;
  NiceMock<MockExpression> mFieldValue2;
  NiceMock<MockExpression> mFieldValue3;
  ModelTypeSpecifier mModelTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelBuilderTest() : mModelTypeSpecifier(ModelTypeSpecifier("MShape")) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType *structType = StructType::create(llvmContext, "MShape");
    structType->setBody(types);
    map<string, Field*> fields;
    fields["mWidth"] = new Field(PrimitiveTypes::INT_TYPE, "mWidth", 0);
    fields["mHeight"] = new Field(PrimitiveTypes::INT_TYPE, "mHeight", 1);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("MShape", structType, fields, methods, interfaces);
    mContext.addModel(mModel);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(mFieldValue1, generateIR(_)).WillByDefault(Return(fieldValue1));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(mFieldValue2, generateIR(_)).WillByDefault(Return(fieldValue2));
    Value* fieldValue3 = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
    ON_CALL(mFieldValue3, generateIR(_)).WillByDefault(Return(fieldValue3));

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ModelBuilderTest() {
    delete mStringStream;
  }
};

TEST_F(ModelBuilderTest, releaseOwnershipTest) {
  string argumentSpecifier1("withWidth");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mFieldValue2);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);
  modelBuilder.generateIR(mContext);
  ostringstream stream;
  stream << "__tmp" << (long) &modelBuilder;
  string temporaryVariableName = stream.str();
  
  EXPECT_NE(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
  
  modelBuilder.releaseOwnership(mContext);

  EXPECT_EQ(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
}

TEST_F(ModelBuilderTest, testGetType) {
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);

  EXPECT_EQ(modelBuilder.getType(mContext), mModel);
}
