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
  Model* mCircleModel;
  Interface* mInterface;
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
    mStructType = StructType::create(mLLVMContext, "Square");
    mStructType->setBody(types);
    map<string, ModelField*> fields;
    mWidthField = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    mHeightField = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    fields["width"] = mWidthField;
    fields["height"] = mHeightField;
    vector<MethodArgument*> methodArguments;
    mMethod = new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments);
    map<string, Method*> methods;
    methods["foo"] = mMethod;
    
    vector<Type*> interfaceTypes;
    StructType* interfaceStructType = StructType::create(mLLVMContext, "Shape");
    interfaceStructType->setBody(interfaceTypes);
    vector<MethodArgument*> interfaceMethodArguments;
    map<string, Method*> interfaceMethods;
    interfaceMethods["foo"] = new Method("foo", PrimitiveTypes::INT_TYPE, interfaceMethodArguments);
    mInterface = new Interface("Shape", interfaceStructType, interfaceMethods);
   
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);
    mModel = new Model("Square", mStructType, fields, methods, interfaces);

    StructType* circleStructType = StructType::create(mLLVMContext, "Circle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    map<string, Method*> circleMethods;
    map<string, ModelField*> circleFields;
    vector<Interface*> circleInterfaces;
    mCircleModel = new Model("Circle",
                             circleStructType,
                             circleFields,
                             circleMethods,
                             circleInterfaces);
}
  
  ~ModelTest() { }
};

TEST_F(ModelTest, TestModelInstantiation) {
  EXPECT_STREQ(mModel->getName().c_str(), "Square");
  EXPECT_STREQ(mModel->getVTableName().c_str(), "model.Square.vtable");
  EXPECT_EQ(mModel->getTypeKind(), MODEL_TYPE);
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo());
  EXPECT_EQ(mModel->getInterfaces().size(), 1u);
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
  EXPECT_FALSE(mModel->canCastTo(mCircleModel));
  EXPECT_TRUE(mModel->canCastTo(mModel));
  EXPECT_TRUE(mModel->canCastTo(mInterface));
}

TEST_F(ModelTest, CanCastLosslessToTest) {
  EXPECT_FALSE(mModel->canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canCastLosslessTo(mCircleModel));
  EXPECT_TRUE(mModel->canCastLosslessTo(mModel));
  EXPECT_TRUE(mModel->canCastLosslessTo(mInterface));
}

TEST_F(ModelTest, CastTest) {
  
}

TEST_F(ModelTest, CastToDeathTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);

  EXPECT_EXIT(mModel->castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'Square' to 'int'");
}
