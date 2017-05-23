//
//  TestModelBuilder.cpp
//  Wisey
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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ModelBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ModelBuilderTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  NiceMock<MockExpression> mField1Expression;
  NiceMock<MockExpression> mField2Expression;
  ModelTypeSpecifier* mModelTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelBuilderTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    vector<string> package;
    mModelTypeSpecifier = new ModelTypeSpecifier(package, "MShape");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["mWidth"] = new Field(PrimitiveTypes::INT_TYPE, "mWidth", 0, fieldArguments);
    fields["mHeight"] = new Field(PrimitiveTypes::INT_TYPE, "mHeight", 1, fieldArguments);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("MShape", modelFullName, structType, fields, methods, interfaces);
    mContext.addModel(mModel);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(mField1Expression, generateIR(_)).WillByDefault(Return(fieldValue1));
    ON_CALL(mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(mField2Expression, generateIR(_)).WillByDefault(Return(fieldValue2));
    ON_CALL(mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));

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
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mField2Expression);
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
