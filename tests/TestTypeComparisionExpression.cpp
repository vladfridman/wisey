//
//  TestTypeComparisionExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link TypeComparisionExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockTypeSpecifier.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/TypeComparisionExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct TestTypeComparisionExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  Value* mTrueValue;
  Value* mFalseValue;
  Model* mCircleModel;
  Model* mSquareModel;
  Interface* mSubShapeInterface;
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mCarInterface;
  NiceMock<MockExpression>* mExpression;
  
  TestTypeComparisionExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mTrueValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
    mFalseValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 0);
    
    vector<Type*> squareTypes;
    squareTypes.push_back(Type::getInt32Ty(mLLVMContext));
    squareTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string squareFullName = "systems.vos.wisey.compiler.tests.MSquare";
    StructType* squareStructType = StructType::create(mLLVMContext, squareFullName);
    squareStructType->setBody(squareTypes);
    map<string, Field*> squareFields;
    ExpressionList fieldArguments;
    squareFields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    squareFields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    vector<MethodArgument*> methodArguments;
    vector<Method*> squareMethods;
    vector<const Model*> thrownExceptions;
    Method* fooMethod = new Method("foo",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL,
                                   0);
    Method* barMethod = new Method("bar",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL,
                                   1);
    squareMethods.push_back(fooMethod);
    squareMethods.push_back(barMethod);
    
    vector<Type*> subShapeInterfaceTypes;
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    subShapeIinterfaceStructType->setBody(subShapeInterfaceTypes);
    vector<MethodArgument*> subShapeInterfaceMethodArguments;
    vector<MethodSignature*> subShapeInterfaceMethods;
    vector<const Model*> subShapeThrownExceptions;
    MethodSignature* methodFooSignature = new MethodSignature("foo",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              subShapeInterfaceMethodArguments,
                                                              subShapeThrownExceptions,
                                                              0);
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
    vector<const Model*> shapeThrownExceptions;
    methodFooSignature = new MethodSignature("foo",
                                             AccessLevel::PUBLIC_ACCESS,
                                             PrimitiveTypes::INT_TYPE,
                                             shapeInterfaceMethodArguments,
                                             shapeThrownExceptions,
                                             0);
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
    vector<const Model*> objectThrownExceptions;
    MethodSignature* methodBarSignature = new MethodSignature("bar",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              objectThrownExceptions,
                                                              0);
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
    
    vector<Interface*> sqaureInterfaces;
    sqaureInterfaces.push_back(mShapeInterface);
    sqaureInterfaces.push_back(mObjectInterface);
    mSquareModel = new Model(squareFullName, squareStructType);
    mSquareModel->setFields(squareFields);
    mSquareModel->setMethods(squareMethods);
    mSquareModel->setInterfaces(sqaureInterfaces);
    
    string circleFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, "MCircle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = new Model(circleFullName, circleStructType);
  }
  
  ~TestTypeComparisionExpressionTest() {
    delete mStringStream;
  }
};

TEST_F(TestTypeComparisionExpressionTest, compareIdenticalPrimiteveTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareDifferntPrimiteveTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareIdenticalModelTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mCircleModel));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareDifferentModelTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSquareModel));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareModelAndInterfaceTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareModelAndInterfaceTypesNotMatchTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mCarInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareInterfaceAndInterfaceTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mShapeInterface));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, existsInOuterScopeTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  
  EXPECT_FALSE(typeComparision.existsInOuterScope(mContext));
}

TEST_F(TestTypeComparisionExpressionTest, releaseOwnershipDeathTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(typeSpecifier);
  
  EXPECT_EXIT(typeComparision.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Can not release ownership of an instanceof operation result, "
              "it is not a heap pointer");
}

TEST_F(TestTypeComparisionExpressionTest, addReferenceToOwnerDeathTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(typeSpecifier);
  
  EXPECT_EXIT(typeComparision.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to an instanceof expression");
}

TEST_F(TestFileSampleRunner, instanceOfTrivialMatchRunTest) {
  runFile("tests/samples/test_instanceof_trivial_match.yz", "2");
}

TEST_F(TestFileSampleRunner, interfaceInstanceOfModelMatchRunTest) {
  runFile("tests/samples/test_interface_instanceof_model_match.yz", "1");
}

TEST_F(TestFileSampleRunner, interfaceInstanceOfInterfaceMatchRunTest) {
  runFile("tests/samples/test_interface_instanceof_interface_match.yz", "1");
}

TEST_F(TestFileSampleRunner, interfaceInstanceOfInterfaceNotMatchRunTest) {
  runFile("tests/samples/test_interface_instanceof_interface_notmatch.yz", "0");
}

TEST_F(TestFileSampleRunner, interfaceInstanceControllersRunTest) {
  runFile("tests/samples/test_instanceof_controllers.yz", "1");
}

TEST_F(TestFileSampleRunner, interfaceInstanceNodesRunTest) {
  runFile("tests/samples/test_instanceof_nodes.yz", "1");
}
