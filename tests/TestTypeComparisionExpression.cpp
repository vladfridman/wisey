//
//  TestTypeComparisionExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link TestTypeComparisionExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockTypeSpecifier.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"
#include "yazyk/TypeComparisionExpression.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
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
  
  TestTypeComparisionExpressionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    StructType* squareStructType = StructType::create(mLLVMContext, "MSquare");
    squareStructType->setBody(squareTypes);
    map<string, Field*> squareFields;
    ExpressionList fieldArguments;
    squareFields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    squareFields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    vector<MethodArgument*> methodArguments;
    vector<Method*> squareMethods;
    squareMethods.push_back(new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL));
    squareMethods.push_back(new Method("bar", PrimitiveTypes::INT_TYPE, methodArguments, 1, NULL));
    
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
    
    vector<Interface*> sqaureInterfaces;
    sqaureInterfaces.push_back(mShapeInterface);
    sqaureInterfaces.push_back(mObjectInterface);
    mSquareModel = new Model("MSquare",
                             squareStructType,
                             squareFields,
                             squareMethods,
                             sqaureInterfaces);
    
    StructType* circleStructType = StructType::create(mLLVMContext, "MCircle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    vector<Method*> circleMethods;
    map<string, Field*> circleFields;
    vector<Interface*> circleInterfaces;
    mCircleModel = new Model("MCircle",
                             circleStructType,
                             circleFields,
                             circleMethods,
                             circleInterfaces);
  }
  
  ~TestTypeComparisionExpressionTest() {
    delete mStringStream;
  }
};

TEST_F(TestTypeComparisionExpressionTest, compareIdenticalPrimiteveTypesTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareDifferntPrimiteveTypesTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareIdenticalModelTypesTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(mCircleModel));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareDifferentModelTypesTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(mSquareModel));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareModelAndInterfaceTypesTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareModelAndInterfaceTypesNotMatchTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(mCarInterface));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TestTypeComparisionExpressionTest, compareInterfaceAndInterfaceTypesTest) {
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mShapeInterface));
  NiceMock<MockTypeSpecifier> typeSpecifier;
  ON_CALL(typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TestTypeComparisionExpressionTest, releaseOwnershipDeathTest) {
  NiceMock<MockExpression> expression;
  NiceMock<MockTypeSpecifier> typeSpecifier;
  TypeComparisionExpression typeComparision(expression, typeSpecifier);
  
  EXPECT_EXIT(typeComparision.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Can not release ownership of an instanceof operation result, "
              "it is not a heap pointer");
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

