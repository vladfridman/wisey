//
//  TestModelOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelOwner}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ModelOwnerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  Model* mCircleModel;
  Model* mStarModel;
  Interface* mSubShapeInterface;
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mCarInterface;
  Method* mMethod;
  StructType* mStructType;
  FieldFixed* mWidthField;
  FieldFixed* mHeightField;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mField3Expression;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelOwnerTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()),
  mField3Expression(new NiceMock<MockExpression>()) {
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    map<string, IField*> fields;
    ExpressionList fieldArguments;
    mWidthField = new FieldFixed(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    mHeightField = new FieldFixed(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    fields["width"] = mWidthField;
    fields["height"] = mHeightField;
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method("foo",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL);
    vector<Method*> methods;
    methods.push_back(mMethod);
    Method* barMethod = new Method("bar",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL);
    methods.push_back(barMethod);
    
    vector<Type*> subShapeInterfaceTypes;
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    subShapeIinterfaceStructType->setBody(subShapeInterfaceTypes);
    vector<MethodArgument*> subShapeInterfaceMethodArguments;
    vector<MethodSignature*> subShapeInterfaceMethods;
    vector<const Model*> subShapeInterfaceThrownExceptions;
    MethodSignature* methodFooSignature = new MethodSignature("foo",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              subShapeInterfaceMethodArguments,
                                                              subShapeInterfaceThrownExceptions);
    subShapeInterfaceMethods.push_back(methodFooSignature);
    mSubShapeInterface = new Interface(subShapeFullName, subShapeIinterfaceStructType);
    vector<Interface*> subShapeParentInterfaces;
    mSubShapeInterface->setParentInterfacesAndMethodSignatures(subShapeParentInterfaces,
                                                               subShapeInterfaceMethods);
    
    vector<Type*> shapeInterfaceTypes;
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    shapeIinterfaceStructType->setBody(shapeInterfaceTypes);
    vector<MethodArgument*> shapeInterfaceMethodArguments;
    vector<MethodSignature*> shapeInterfaceMethods;
    vector<const Model*> shapeInterfaceThrownExceptions;
    methodFooSignature = new MethodSignature("foo",
                                             AccessLevel::PUBLIC_ACCESS,
                                             PrimitiveTypes::INT_TYPE,
                                             shapeInterfaceMethodArguments,
                                             shapeInterfaceThrownExceptions);
    shapeInterfaceMethods.push_back(methodFooSignature);
    vector<Interface*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(mSubShapeInterface);
    mShapeInterface = new Interface(shapeFullName, shapeIinterfaceStructType);
    mShapeInterface->setParentInterfacesAndMethodSignatures(shapeParentInterfaces,
                                                            shapeInterfaceMethods);
    
    vector<Type*> objectInterfaceTypes;
    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    objectInterfaceStructType->setBody(objectInterfaceTypes);
    vector<MethodArgument*> objectInterfaceMethodArguments;
    vector<MethodSignature*> objectInterfaceMethods;
    vector<const Model*> objectInterfaceThrownExceptions;
    MethodSignature* methodBarSignature = new MethodSignature("bar",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              objectInterfaceThrownExceptions);
    objectInterfaceMethods.push_back(methodBarSignature);
    mObjectInterface = new Interface(objectFullName, objectInterfaceStructType);
    vector<Interface*> objectParentInterfaces;
    mObjectInterface->setParentInterfacesAndMethodSignatures(objectParentInterfaces,
                                                             objectInterfaceMethods);
    
    vector<Type*> carInterfaceTypes;
    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    carInterfaceStructType->setBody(carInterfaceTypes);
    mCarInterface = new Interface(carFullName, carInterfaceStructType);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel = new Model(modelFullName, mStructType);
    mModel->setFields(fields);
    mModel->setMethods(methods);
    mModel->setInterfaces(interfaces);
    
    string cirlceFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, cirlceFullName);
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = new Model(cirlceFullName, circleStructType);
    Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, cirlceFullName + ".name");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       cirlceFullName + ".name");
    
    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    map<string, IField*> starFields;
    starFields["mBrightness"] = new FieldFixed(PrimitiveTypes::INT_TYPE,
                                               "mBrightness",
                                               0,
                                               fieldArguments);
    starFields["mWeight"] = new FieldFixed(PrimitiveTypes::INT_TYPE, "mWeight", 1, fieldArguments);
    mStarModel = new Model(starFullName, starStructType);
    mStarModel->setFields(starFields);
    mContext.addModel(mStarModel);
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field3Value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
    ON_CALL(*mField3Expression, generateIR(_)).WillByDefault(Return(field3Value));
    ON_CALL(*mField3Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
    
    IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
    IConcreteObjectType::generateVTable(mContext, mStarModel);
    
    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(function);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ModelOwnerTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
    delete mField3Expression;
  }
};

TEST_F(ModelOwnerTest, getObjectTest) {
  EXPECT_EQ(mModel->getOwner()->getObject(), mModel);
}

TEST_F(ModelOwnerTest, getNameTest) {
  EXPECT_STREQ(mModel->getOwner()->getName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare*");
}

TEST_F(ModelOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mModel->getOwner()->getLLVMType(mLLVMContext),
            mModel->getLLVMType(mLLVMContext)->getPointerElementType());
}

TEST_F(ModelOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mModel->getOwner()->getTypeKind(), MODEL_OWNER_TYPE);
}

TEST_F(ModelOwnerTest, canCastToTest) {
  EXPECT_FALSE(mModel->getOwner()->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mCircleModel->getOwner()));
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mCarInterface->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mModel->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mShapeInterface->getOwner()));

  EXPECT_FALSE(mModel->getOwner()->canCastTo(mCircleModel));
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mCarInterface));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mModel));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mShapeInterface));
}

TEST_F(ModelOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mCircleModel->getOwner()));
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mCarInterface->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mModel->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mShapeInterface->getOwner()));
  
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mCircleModel));
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mCarInterface));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mModel));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mShapeInterface));
}

TEST_F(ModelOwnerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mLLVMContext)->getPointerTo());
  mModel->getOwner()->castTo(mContext, pointer, mShapeInterface->getOwner());
  EXPECT_EQ(mBasicBlock->size(), 1u);
  
  *mStringStream << *mBasicBlock->begin();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare** null "
               "to %systems.vos.wisey.compiler.tests.IShape**");
  mStringBuffer.clear();
}

TEST_F(ModelOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->getOwner()->castTo(mContext, pointer, mSubShapeInterface->getOwner());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MSquare*, "
  "%systems.vos.wisey.compiler.tests.MSquare** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MSquare* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 8"
  "\n  %3 = alloca %systems.vos.wisey.compiler.tests.ISubShape*"
  "\n  %4 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.ISubShape*"
  "\n  store %systems.vos.wisey.compiler.tests.ISubShape* %4, "
  "%systems.vos.wisey.compiler.tests.ISubShape** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, ownerAssignToReferenceRunTest) {
  runFile("tests/samples/test_owner_assign_to_reference.yz", "3");
}
