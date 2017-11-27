//
//  TestObjectBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectBuilder}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "wisey/Field.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ObjectBuilderTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  ObjectBuilder* mObjectBuilder;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  ModelTypeSpecifier* mModelTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ObjectBuilderTest() :
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    vector<string> package;
    mModelTypeSpecifier = new ModelTypeSpecifier(package, "MShape");
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType *structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList arguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mWidth", arguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mHeight", arguments));
    mModel = Model::newModel(modelFullName, structType);
    mModel->setFields(fields, 1u);
    mContext.addModel(mModel);
    Value* fieldValue1 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(*mField1Expression, generateIR(_)).WillByDefault(Return(fieldValue1));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mField1Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument1));
    Value* fieldValue2 = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(fieldValue2));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mField2Expression, printToStream(_, _)).WillByDefault(Invoke(printBuilderArgument2));

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);

    string argumentSpecifier1("withWidth");
    ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
    string argumentSpecifier2("withHeight");
    ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
    ObjectBuilderArgumentList argumentList;
    argumentList.push_back(argument1);
    argumentList.push_back(argument2);
    mObjectBuilder = new ObjectBuilder(mModelTypeSpecifier, argumentList);

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
  
  ~ObjectBuilderTest() {
    delete mField1Expression;
    delete mField2Expression;
    delete mStringStream;
  }
  
  static void printBuilderArgument1(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
  
  static void printBuilderArgument2(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(ObjectBuilderTest, getVariableTest) {
  EXPECT_EQ(mObjectBuilder->getVariable(mContext), nullptr);
}

TEST_F(ObjectBuilderTest, releaseOwnershipTest) {
  mObjectBuilder->generateIR(mContext);
  string temporaryVariableName = IVariable::getTemporaryVariableName(mObjectBuilder);
  
  EXPECT_NE(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
  
  mObjectBuilder->releaseOwnership(mContext);

  *mStringStream << mBlock->getInstList().back();
  string expected = string() +
  "  store %systems.vos.wisey.compiler.tests.MShape* null, "
  "%systems.vos.wisey.compiler.tests.MShape** %3";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ObjectBuilderTest, testGetType) {
  ObjectBuilderArgumentList argumentList;
  ObjectBuilder objectBuilder(mModelTypeSpecifier, argumentList);

  EXPECT_EQ(objectBuilder.getType(mContext), mModel->getOwner());
}

TEST_F(ObjectBuilderTest, existsInOuterScopeTest) {
  mObjectBuilder->generateIR(mContext);

  EXPECT_FALSE(mObjectBuilder->existsInOuterScope(mContext));
}

TEST_F(ObjectBuilderTest, isConstantTest) {
  EXPECT_FALSE(mObjectBuilder->isConstant());
}

TEST_F(ObjectBuilderTest, printToStreamTest) {
  stringstream stringStream;
  mObjectBuilder->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("builder(systems.vos.wisey.compiler.tests.MShape)"
               ".withWidth(3).withHeight(5).build()",
               stringStream.str().c_str());
}
