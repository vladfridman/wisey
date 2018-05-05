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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/TypeComparisionExpression.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct TypeComparisionExpressionTest : public Test {
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
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  TypeComparisionExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

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
    mSquareModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                   squareFullName,
                                   squareStructType,
                                   mContext.getImportProfile(),
                                   0);

    vector<IField*> squareFields;
    squareFields.push_back(new FixedField(PrimitiveTypes::INT, "width", 0));
    squareFields.push_back(new FixedField(PrimitiveTypes::INT, "height", 0));
    vector<const wisey::Argument*> methodArguments;
    vector<IMethod*> squareMethods;
    vector<const Model*> thrownExceptions;
    IMethod* fooMethod = new Method(mSquareModel,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    IMethod* barMethod = new Method(mSquareModel,
                                    "bar",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    squareMethods.push_back(fooMethod);
    squareMethods.push_back(barMethod);
    
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    VariableList subShapeInterfaceArguments;
    vector<IObjectElementDefinition*> subShapeInterfaceElements;
    vector<IModelTypeSpecifier*> subShapeThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    IObjectElementDefinition* methodFooSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     subShapeInterfaceArguments,
                                     subShapeThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    subShapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> subShapeParentInterfaces;
    mSubShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 subShapeFullName,
                                                 subShapeIinterfaceStructType,
                                                 subShapeParentInterfaces,
                                                 subShapeInterfaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
    mContext.addInterface(mSubShapeInterface, 0);
    mSubShapeInterface->buildMethods(mContext);
    
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    VariableList shapeInterfaceArguments;
    vector<IObjectElementDefinition*> shapeInterfaceElements;
    vector<IModelTypeSpecifier*> shapeThrownExceptions;
    MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
    methodFooSignature = new MethodSignatureDeclaration(intSpecifier,
                                                        "foo",
                                                        shapeInterfaceArguments,
                                                        shapeThrownExceptions,
                                                        methodQualifiers,
                                                        0);
    shapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* subShapeTypeSpecifier =
    new InterfaceTypeSpecifier(NULL, "ISubShape", 0);
    shapeParentInterfaces.push_back(subShapeTypeSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS, 
                                              shapeFullName,
                                              shapeIinterfaceStructType,
                                              shapeParentInterfaces,
                                              shapeInterfaceElements,
                                              mContext.getImportProfile(),
                                              0);
    mContext.addInterface(mShapeInterface, 0);
    mShapeInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceArguments;
    vector<IObjectElementDefinition*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    MethodSignatureDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "bar",
                                     objectInterfaceArguments,
                                     objectThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements,
                                               mContext.getImportProfile(),
                                               0);
    mContext.addInterface(mObjectInterface, 0);
    mObjectInterface->buildMethods(mContext);

    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    vector<IInterfaceTypeSpecifier*> carParentInterfaces;
    vector<IObjectElementDefinition*> carElements;
    mCarInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                            carFullName,
                                            carInterfaceStructType,
                                            carParentInterfaces,
                                            carElements,
                                            mContext.getImportProfile(),
                                            0);
    mContext.addInterface(mCarInterface, 0);
    mCarInterface->buildMethods(mContext);

    vector<Interface*> sqaureInterfaces;
    sqaureInterfaces.push_back(mShapeInterface);
    sqaureInterfaces.push_back(mObjectInterface);
    mSquareModel->setFields(mContext, squareFields, sqaureInterfaces.size());
    mSquareModel->setMethods(squareMethods);
    mSquareModel->setInterfaces(sqaureInterfaces);
    
    string circleFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, "MCircle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                   circleFullName,
                                   circleStructType,
                                   mContext.getImportProfile(),
                                   0);
  }
  
  ~TypeComparisionExpressionTest() {
    delete mStringStream;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "object";
  }
  
  static void printTypeSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "type";
  }
};

TEST_F(TypeComparisionExpressionTest, compareIdenticalPrimitiveTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareDifferntPrimitiveTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareDifferentModelTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSquareModel));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareModelAndInterfaceTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TypeComparisionExpressionTest, compareModelAndInterfaceTypesNotMatchTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mCarInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareInterfaceAndInterfaceTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mShapeInterface));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TypeComparisionExpressionTest, isConstantTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  
  EXPECT_FALSE(typeComparision.isConstant());
}

TEST_F(TypeComparisionExpressionTest, isAssignableTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  
  EXPECT_FALSE(typeComparision.isAssignable());
}

TEST_F(TypeComparisionExpressionTest, printToStreamTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier, 0);
  ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  ON_CALL(*typeSpecifier, printToStream(_, _)).WillByDefault(Invoke(printTypeSpecifier));

  stringstream stringStream;
  typeComparision.printToStream(mContext, stringStream);
  EXPECT_STREQ("object instanceof type", stringStream.str().c_str());
}

TEST_F(TestFileRunner, instanceOfTrivialMatchRunTest) {
  runFile("tests/samples/test_instanceof_trivial_match.yz", "2");
}

TEST_F(TestFileRunner, interfaceInstanceOfModelMatchRunTest) {
  runFile("tests/samples/test_interface_instanceof_model_match.yz", "1");
}

TEST_F(TestFileRunner, interfaceInstanceOfInterfaceMatchRunTest) {
  runFile("tests/samples/test_interface_instanceof_interface_match.yz", "1");
}

TEST_F(TestFileRunner, interfaceInstanceOfInterfaceNotMatchRunTest) {
  runFile("tests/samples/test_interface_instanceof_interface_notmatch.yz", "0");
}

TEST_F(TestFileRunner, interfaceInstanceControllersRunTest) {
  runFile("tests/samples/test_instanceof_controllers.yz", "1");
}

TEST_F(TestFileRunner, interfaceInstanceNodesRunTest) {
  runFile("tests/samples/test_instanceof_nodes.yz", "1");
}

TEST_F(TestFileRunner, interfaceInstanceOfWithNullRunTest) {
  runFile("tests/samples/test_instanceof_with_null.yz", "0");
}

TEST_F(TestFileRunner, instanceofWithArraysRunDeathTest) {
  expectFailCompile("tests/samples/test_instanceof_with_arrays.yz",
                    1,
                    "tests/samples/test_instanceof_with_arrays.yz\\(9\\): Error: Operator instanceof does not work with arrays");
}
