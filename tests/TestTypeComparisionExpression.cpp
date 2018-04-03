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
    mSquareModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, squareFullName, squareStructType);

    vector<IField*> squareFields;
    squareFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width"));
    squareFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height"));
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> squareMethods;
    vector<const Model*> thrownExceptions;
    IMethod* fooMethod = new Method(mSquareModel,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL,
                                    0);
    IMethod* barMethod = new Method(mSquareModel,
                                    "bar",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL,
                                    0);
    squareMethods.push_back(fooMethod);
    squareMethods.push_back(barMethod);
    
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    VariableList subShapeInterfaceMethodArguments;
    vector<IObjectElementDefinition*> subShapeInterfaceElements;
    vector<IModelTypeSpecifier*> subShapeThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    IObjectElementDefinition* methodFooSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     subShapeInterfaceMethodArguments,
                                     subShapeThrownExceptions);
    subShapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> subShapeParentInterfaces;
    mSubShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 subShapeFullName,
                                                 subShapeIinterfaceStructType,
                                                 subShapeParentInterfaces,
                                                 subShapeInterfaceElements);
    mContext.addInterface(mSubShapeInterface);
    mSubShapeInterface->buildMethods(mContext);
    
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    VariableList shapeInterfaceMethodArguments;
    vector<IObjectElementDefinition*> shapeInterfaceElements;
    vector<IModelTypeSpecifier*> shapeThrownExceptions;
    methodFooSignature = new MethodSignatureDeclaration(intSpecifier,
                                                        "foo",
                                                        shapeInterfaceMethodArguments,
                                                        shapeThrownExceptions);
    shapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* subShapeTypeSpecifier = new InterfaceTypeSpecifier(NULL, "ISubShape");
    shapeParentInterfaces.push_back(subShapeTypeSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS, 
                                              shapeFullName,
                                              shapeIinterfaceStructType,
                                              shapeParentInterfaces,
                                              shapeInterfaceElements);
    mContext.addInterface(mShapeInterface);
    mShapeInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDefinition*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    MethodSignatureDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "bar",
                                     objectInterfaceMethodArguments,
                                     objectThrownExceptions);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    vector<IInterfaceTypeSpecifier*> carParentInterfaces;
    vector<IObjectElementDefinition*> carElements;
    mCarInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                            carFullName,
                                            carInterfaceStructType,
                                            carParentInterfaces,
                                            carElements);
    mContext.addInterface(mCarInterface);
    mCarInterface->buildMethods(mContext);

    vector<Interface*> sqaureInterfaces;
    sqaureInterfaces.push_back(mShapeInterface);
    sqaureInterfaces.push_back(mObjectInterface);
    mSquareModel->setFields(squareFields, sqaureInterfaces.size());
    mSquareModel->setMethods(squareMethods);
    mSquareModel->setInterfaces(sqaureInterfaces);
    
    string circleFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, "MCircle");
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, circleFullName, circleStructType);
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

TEST_F(TypeComparisionExpressionTest, getVariableTest) {
  TypeComparisionExpression typeComparision(mExpression, NULL);
  vector<const IExpression*> arrayIndices;

  EXPECT_EQ(typeComparision.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(TypeComparisionExpressionTest, compareIdenticalPrimiteveTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TypeComparisionExpressionTest, compareDifferntPrimiteveTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareIdenticalModelTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mCircleModel));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TypeComparisionExpressionTest, compareDifferentModelTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mCircleModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSquareModel));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareModelAndInterfaceTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TypeComparisionExpressionTest, compareModelAndInterfaceTypesNotMatchTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mSquareModel));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mCarInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mFalseValue);
}

TEST_F(TypeComparisionExpressionTest, compareInterfaceAndInterfaceTypesTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mShapeInterface));
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  ON_CALL(*typeSpecifier, getType(_)).WillByDefault(Return(mSubShapeInterface));
  
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
  Value* value = typeComparision.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  EXPECT_EQ(value, mTrueValue);
}

TEST_F(TypeComparisionExpressionTest, isConstantTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);

  EXPECT_FALSE(typeComparision.isConstant());
}

TEST_F(TypeComparisionExpressionTest, printToStreamTest) {
  NiceMock<MockTypeSpecifier>* typeSpecifier = new NiceMock<MockTypeSpecifier>();
  TypeComparisionExpression typeComparision(mExpression, typeSpecifier);
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
