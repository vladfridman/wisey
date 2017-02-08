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

#include <llvm/IR/LLVMContext.h>

#include "yazyk/Model.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct ModelTest : public Test {
  Model* mModel;
  StructType* mStructType;
  ModelField* mWidthField;
  ModelField* mHeightField;
  LLVMContext mLLVMContext;
  
  ModelTest() {
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
    mModel = new Model("Shape", mStructType, fields);
  }
  
  ~ModelTest() { }
};

TEST_F(ModelTest, TestModelInstantiation) {
  EXPECT_STREQ(mModel->getName().c_str(), "Shape");
  EXPECT_EQ(mModel->getTypeKind(), MODEL_TYPE);
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo());
}

TEST_F(ModelTest, TestFindFeild) {
  EXPECT_EQ(mModel->findField("width"), mWidthField);
  EXPECT_EQ(mModel->findField("height"), mHeightField);
  EXPECT_EQ(mModel->findField("depth"), nullptr);
}

TEST_F(ModelTest, TestGetMissingFields) {
  set<string> givenFields;
  givenFields.insert("width");
  
  vector<string> missingFields = mModel->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "height");
}
