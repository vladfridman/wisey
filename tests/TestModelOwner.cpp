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
#include "TestPrefix.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

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
  StructType* mStructType;
  Field* mWidthField;
  Field* mHeightField;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mField3Expression;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ModelOwnerTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()),
  mField3Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList arguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", arguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", arguments));
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    IMethod* method = new Method("foo",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 thrownExceptions,
                                 NULL,
                                 0);
    vector<IMethod*> methods;
    methods.push_back(method);
    IMethod* barMethod = new Method("bar",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL,
                                    0);
    methods.push_back(barMethod);
    
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    VariableList subShapeInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> subShapeInterfaceElements;
    vector<IModelTypeSpecifier*> subShapeInterfaceThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* methodFooSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     subShapeInterfaceMethodArguments,
                                     subShapeInterfaceThrownExceptions);
    subShapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> subShapeParentInterfaces;
    mSubShapeInterface = Interface::newInterface(subShapeFullName,
                                                 subShapeIinterfaceStructType,
                                                 subShapeParentInterfaces,
                                                 subShapeInterfaceElements);
    mSubShapeInterface->buildMethods(mContext);
    mContext.addInterface(mSubShapeInterface);

    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    VariableList shapeInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> shapeInterfaceElements;
    vector<IModelTypeSpecifier*> shapeInterfaceThrownExceptions;
    methodFooSignature = new MethodSignatureDeclaration(intSpecifier,
                                                        "foo",
                                                        shapeInterfaceMethodArguments,
                                                        shapeInterfaceThrownExceptions);
    shapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* subShapeInterfaceTypeSpecifier =
      new InterfaceTypeSpecifier("", "ISubShape");
    shapeParentInterfaces.push_back(subShapeInterfaceTypeSpecifier);
    mShapeInterface = Interface::newInterface(shapeFullName,
                                              shapeIinterfaceStructType,
                                              shapeParentInterfaces,
                                              shapeInterfaceElements);
    mShapeInterface->buildMethods(mContext);
    mContext.addInterface(mShapeInterface);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectInterfaceThrownExceptions;
    IObjectElementDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "bar",
                                     objectInterfaceMethodArguments,
                                     objectInterfaceThrownExceptions);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements);
    mObjectInterface->buildMethods(mContext);
    mContext.addInterface(mObjectInterface);

    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    vector<IInterfaceTypeSpecifier*> carParentInterfaces;
    vector<IObjectElementDeclaration*> carInterfaceElements;
    mCarInterface = Interface::newInterface(carFullName,
                                            carInterfaceStructType,
                                            carParentInterfaces,
                                            carInterfaceElements);
    mCarInterface->buildMethods(mContext);
    mContext.addInterface(mCarInterface);

    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel = Model::newModel(modelFullName, mStructType);
    mModel->setFields(fields, interfaces.size() + 1);
    mModel->setMethods(methods);
    mModel->setInterfaces(interfaces);
    
    string cirlceFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, cirlceFullName);
    vector<Type*> circleTypes;
    circleTypes.push_back(Type::getInt64Ty(mLLVMContext));
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(cirlceFullName, circleStructType);
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  cirlceFullName + ".name");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       cirlceFullName + ".name");
    
    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt64Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<Field*> starFields;
    starFields.push_back(new Field(FIXED_FIELD,
                                   PrimitiveTypes::INT_TYPE,
                                   "mBrightness",
                                   arguments));
    starFields.push_back(new Field(FIXED_FIELD,
                                   PrimitiveTypes::INT_TYPE,
                                   "mWeight",
                                   arguments));
    mStarModel = Model::newModel(starFullName, starStructType);
    mStarModel->setFields(starFields, 1u);
    mContext.addModel(mStarModel);
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field3Value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
    ON_CALL(*mField3Expression, generateIR(_, _)).WillByDefault(Return(field3Value));
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
            mModel->getLLVMType(mLLVMContext));
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
  ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mLLVMContext));
  mModel->getOwner()->castTo(mContext, pointer, mShapeInterface->getOwner());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IShape*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->getOwner()->castTo(mContext, pointer, mSubShapeInterface->getOwner());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 16"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.ISubShape*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, ownerAssignToReferenceRunTest) {
  runFile("tests/samples/test_owner_assign_to_reference.yz", "3");
}
