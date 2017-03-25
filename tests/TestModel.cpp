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
#include <llvm/Support/raw_ostream.h>

#include "yazyk/MethodSignature.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct ModelTest : public Test {
  Model* mModel;
  Model* mCircleModel;
  Interface* mSubShapeInterface;
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mCarInterface;
  Method* mMethod;
  StructType* mStructType;
  ModelField* mWidthField;
  ModelField* mHeightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "MSquare");
    mStructType->setBody(types);
    map<string, ModelField*> fields;
    mWidthField = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    mHeightField = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    fields["width"] = mWidthField;
    fields["height"] = mHeightField;
    vector<MethodArgument*> methodArguments;
    mMethod = new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL);
    vector<Method*> methods;
    methods.push_back(mMethod);
    methods.push_back(new Method("bar", PrimitiveTypes::INT_TYPE, methodArguments, 1, NULL));
    
    vector<Type*> subShapeInterfaceTypes;
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, "ISubShape");
    subShapeIinterfaceStructType->setBody(subShapeInterfaceTypes);
    vector<MethodArgument*> subShapeInterfaceMethodArguments;
    vector<MethodSignature*> subShapeInterfaceMethods;
    MethodSignature* methodFooSignature = new MethodSignature("foo",
                                                              PrimitiveTypes::INT_TYPE,
                                                              subShapeInterfaceMethodArguments,
                                                              0);
    subShapeInterfaceMethods.push_back(methodFooSignature);
    vector<Interface*> subShapeParentInterfaces;
    mSubShapeInterface = new Interface("ISubShape",
                                       subShapeIinterfaceStructType,
                                       subShapeParentInterfaces,
                                       subShapeInterfaceMethods);
    
    vector<Type*> shapeInterfaceTypes;
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, "IShape");
    shapeIinterfaceStructType->setBody(shapeInterfaceTypes);
    vector<MethodArgument*> shapeInterfaceMethodArguments;
    vector<MethodSignature*> shapeInterfaceMethods;
    methodFooSignature = new MethodSignature("foo",
                                             PrimitiveTypes::INT_TYPE,
                                             shapeInterfaceMethodArguments,
                                             0);
    shapeInterfaceMethods.push_back(methodFooSignature);
    vector<Interface*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mSubShapeInterface);
    mShapeInterface = new Interface("IShape",
                                    shapeIinterfaceStructType,
                                    shapeParentInterfaces,
                                    shapeInterfaceMethods);
   
    vector<Type*> objectInterfaceTypes;
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, "IObject");
    objectInterfaceStructType->setBody(objectInterfaceTypes);
    vector<MethodArgument*> objectInterfaceMethodArguments;
    vector<MethodSignature*> objectInterfaceMethods;
    MethodSignature* methodBarSignature = new MethodSignature("bar",
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              0);
    objectInterfaceMethods.push_back(methodBarSignature);
    vector<Interface*> objectParentInterfaces;
    mObjectInterface = new Interface("IObject",
                                     objectInterfaceStructType,
                                     objectParentInterfaces,
                                     objectInterfaceMethods);
    
    vector<Type*> carInterfaceTypes;
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, "ICar");
    carInterfaceStructType->setBody(carInterfaceTypes);
    vector<MethodSignature*> carInterfaceMethods;
    vector<Interface*> carParentInterfaces;
    mCarInterface = new Interface("ICar",
                                  carInterfaceStructType,
                                  carParentInterfaces,
                                  carInterfaceMethods);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel = new Model("MSquare", mStructType, fields, methods, interfaces);

    StructType* circleStructType = StructType::create(mLLVMContext, "MCircle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    vector<Method*> circleMethods;
    map<string, ModelField*> circleFields;
    vector<Interface*> circleInterfaces;
    mCircleModel = new Model("MCircle",
                             circleStructType,
                             circleFields,
                             circleMethods,
                             circleInterfaces);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ModelTest() {
    delete mStringStream;
  }
};

TEST_F(ModelTest, modelInstantiationTest) {
  EXPECT_STREQ(mModel->getName().c_str(), "MSquare");
  EXPECT_STREQ(mModel->getVTableName().c_str(), "model.MSquare.vtable");
  EXPECT_EQ(mModel->getTypeKind(), MODEL_TYPE);
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo());
  EXPECT_EQ(mModel->getInterfaces().size(), 2u);
}

TEST_F(ModelTest, findFeildTest) {
  EXPECT_EQ(mModel->findField("width"), mWidthField);
  EXPECT_EQ(mModel->findField("height"), mHeightField);
  EXPECT_EQ(mModel->findField("depth"), nullptr);
}

TEST_F(ModelTest, findMethodTest) {
  EXPECT_EQ(mModel->findMethod("foo"), mMethod);
  EXPECT_EQ(mModel->findMethod("get"), nullptr);
}

TEST_F(ModelTest, methodIndexesTest) {
  EXPECT_EQ(mModel->findMethod("foo")->getIndex(), 0u);
  EXPECT_EQ(mModel->findMethod("bar")->getIndex(), 1u);
}

TEST_F(ModelTest, getMissingFieldsTest) {
  set<string> givenFields;
  givenFields.insert("width");
  
  vector<string> missingFields = mModel->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "height");
}

TEST_F(ModelTest, canCastToTest) {
  EXPECT_FALSE(mModel->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canCastTo(mCircleModel));
  EXPECT_FALSE(mModel->canCastTo(mCarInterface));
  EXPECT_TRUE(mModel->canCastTo(mModel));
  EXPECT_TRUE(mModel->canCastTo(mShapeInterface));
}

TEST_F(ModelTest, canCastLosslessToTest) {
  EXPECT_FALSE(mModel->canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canCastLosslessTo(mCircleModel));
  EXPECT_FALSE(mModel->canCastLosslessTo(mCarInterface));
  EXPECT_TRUE(mModel->canCastLosslessTo(mModel));
  EXPECT_TRUE(mModel->canCastLosslessTo(mShapeInterface));
}

TEST_F(ModelTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get((PointerType*) mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mShapeInterface);
  ASSERT_EQ(mBasicBlock->size(), 1u);

  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = bitcast %MSquare* null to %IShape*");
  mStringBuffer.clear();
}

TEST_F(ModelTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get((PointerType*) mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mSubShapeInterface);
  ASSERT_EQ(mBasicBlock->size(), 3u);
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = bitcast %MSquare* null to i8*");
  mStringBuffer.clear();

  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %add.ptr = getelementptr i8, i8* %0, i64 8");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %1 = bitcast i8* %add.ptr to %ISubShape*");
  mStringBuffer.clear();
}

TEST_F(ModelTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mModel->getObjectNameGlobalVariableName().c_str(), "model.MSquare.name");
}

TEST_F(ModelTest, getTypeTableName) {
  ASSERT_STREQ(mModel->getTypeTableName().c_str(), "model.MSquare.typetable");
}

TEST_F(ModelTest, castToDeathTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);

  EXPECT_EXIT(mModel->castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'MSquare' to 'int'");
}

TEST_F(ModelTest, getFlattenedInterfaceHierarchyTest) {
  vector<Interface*> allInterfaces = mModel->getFlattenedInterfaceHierarchy();
  
  EXPECT_EQ(allInterfaces.size(), 3u);
  EXPECT_EQ(allInterfaces.at(0), mShapeInterface);
  EXPECT_EQ(allInterfaces.at(1), mSubShapeInterface);
  EXPECT_EQ(allInterfaces.at(2), mObjectInterface);
}

TEST_F(ModelTest, doesImplmentInterfaceTest) {
  EXPECT_TRUE(mModel->doesImplmentInterface(mSubShapeInterface));
  EXPECT_TRUE(mModel->doesImplmentInterface(mShapeInterface));
  EXPECT_TRUE(mModel->doesImplmentInterface(mObjectInterface));
  EXPECT_FALSE(mModel->doesImplmentInterface(mCarInterface));
}
