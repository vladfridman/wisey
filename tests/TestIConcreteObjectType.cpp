//
//  TestIConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/31/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IConcreteObjectType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IConcreteObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockConcreteObjectType> mMockObject;
  Interface* mInterface3;
  Model* mStarModel;
  
  IConcreteObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mContext.getScopes().pushScope();
    ON_CALL(mMockObject, getName()).WillByDefault(Return("Object"));
    ON_CALL(mMockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("Object.name"));

    vector<Interface*> interfaces;
    Interface* interface1 = new Interface("Interface1", NULL);
    Interface* interface2 = new Interface("Interface2", NULL);
    mInterface3 = new Interface("Interface3", NULL);
    Interface* interface4 = new Interface("Interface4", NULL);
    interfaces.push_back(interface1);
    interfaces.push_back(interface2);
    interfaces.push_back(mInterface3);
    interfaces.push_back(interface4);
    ON_CALL(mMockObject, getFlattenedInterfaceHierarchy()).WillByDefault(Return(interfaces));

    ExpressionList fieldArguments;
    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    map<string, Field*> starFields;
    starFields["mBrightness"] =
    new Field(PrimitiveTypes::INT_TYPE, "mBrightness", 0, fieldArguments);
    starFields["mWeight"] = new Field(PrimitiveTypes::INT_TYPE, "mWeight", 1, fieldArguments);
    mStarModel = new Model(starFullName, starStructType);
    mStarModel->setFields(starFields);
    mContext.addModel(mStarModel);
}
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);

  IConcreteObjectType::generateNameGlobal(mContext, &mMockObject);

  ASSERT_NE(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);
}

TEST_F(IConcreteObjectTypeTest, getInterfaceIndexTest) {
  ASSERT_EQ(IConcreteObjectType::getInterfaceIndex(&mMockObject, mInterface3), 2);
}

TEST_F(IConcreteObjectTypeTest, declareFieldVariablesTest) {
  EXPECT_EQ(mContext.getScopes().getVariable("mBrightness"), nullptr);

  IConcreteObjectType::declareFieldVariables(mContext, mStarModel);

  EXPECT_NE(mContext.getScopes().getVariable("mBrightness"), nullptr);
}
