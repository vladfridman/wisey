//
//  TestModel.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Model}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>

#include "yazyk/Model.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct ModelTest : public Test {
  Model* mModel;
  Method* mMethod;
  StructType* mStructType;
  ModelField* mWidthField;
  ModelField* mHeightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  ModelTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "Shape");
    mStructType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    mWidthField = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    mHeightField = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    (*fields)["width"] = mWidthField;
    (*fields)["height"] = mHeightField;
    vector<MethodArgument*> methodArguments;
    mMethod = new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments);
    map<string, Method*>* methods = new map<string, Method*>();
    (*methods)["foo"] = mMethod;
    vector<Interface*> interfaces;
    mModel = new Model("Shape", mStructType, fields, methods, interfaces);
  }
  
  ~ModelTest() { }
};

TEST_F(ModelTest, TestModelInstantiation) {
  EXPECT_STREQ(mModel->getName().c_str(), "Shape");
  EXPECT_STREQ(mModel->getVTableName().c_str(), "model.Shape.vtable");
  EXPECT_EQ(mModel->getTypeKind(), MODEL_TYPE);
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo());
}

TEST_F(ModelTest, TestFindFeild) {
  EXPECT_EQ(mModel->findField("width"), mWidthField);
  EXPECT_EQ(mModel->findField("height"), mHeightField);
  EXPECT_EQ(mModel->findField("depth"), nullptr);
}

TEST_F(ModelTest, TestFindMethod) {
  EXPECT_EQ(mModel->findMethod("foo"), mMethod);
  EXPECT_EQ(mModel->findMethod("bar"), nullptr);
}

TEST_F(ModelTest, TestGetMissingFields) {
  set<string> givenFields;
  givenFields.insert("width");
  
  vector<string> missingFields = mModel->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "height");
}

TEST_F(ModelTest, CanCastToTest) {
  EXPECT_FALSE(mModel->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mModel->canCastTo(mModel));
}

TEST_F(ModelTest, CanCastLosslessToTest) {
  EXPECT_FALSE(mModel->canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mModel->canCastLosslessTo(mModel));
}

TEST_F(ModelTest, CastToTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);

  EXPECT_EXIT(mModel->castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'Shape' to 'int'");
}
