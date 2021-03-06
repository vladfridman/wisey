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

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "TestFileRunner.hpp"
#include "IRWriter.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "Method.hpp"
#include "MethodSignature.hpp"
#include "MethodSignatureDeclaration.hpp"
#include "ModelOwner.hpp"
#include "ModelTypeSpecifier.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "ReceivedField.hpp"
#include "VariableDeclaration.hpp"
#include "WiseyModelOwnerType.hpp"
#include "WiseyModelType.hpp"
#include "WiseyObjectOwnerType.hpp"
#include "WiseyObjectType.hpp"

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
  ReceivedField* mWidthField;
  ReceivedField* mHeightField;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mField3Expression;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
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
    
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             mStructType,
                             mContext.getImportProfile(),
                             0);

    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "height", 0));
    vector<const wisey::Argument*> methodArguments;
    vector<const Model*> thrownExceptions;
    IMethod* method = new Method(mModel,
                                 "foo",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT,
                                 methodArguments,
                                 thrownExceptions,
                                 new MethodQualifiers(0),
                                 NULL,
                                 0);
    vector<IMethod*> methods;
    methods.push_back(method);
    IMethod* barMethod = new Method(mModel,
                                    "bar",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    methods.push_back(barMethod);
    
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    VariableList subShapeInterfaceArguments;
    vector<IObjectElementDefinition*> subShapeInterfaceElements;
    vector<IModelTypeSpecifier*> subShapeInterfaceThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    IObjectElementDefinition* methodFooSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     subShapeInterfaceArguments,
                                     subShapeInterfaceThrownExceptions,
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
    mSubShapeInterface->buildMethods(mContext);
    mContext.addInterface(mSubShapeInterface, 0);

    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    VariableList shapeInterfaceArguments;
    vector<IObjectElementDefinition*> shapeInterfaceElements;
    vector<IModelTypeSpecifier*> shapeInterfaceThrownExceptions;
    MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
    methodFooSignature = new MethodSignatureDeclaration(intSpecifier,
                                                        "foo",
                                                        shapeInterfaceArguments,
                                                        shapeInterfaceThrownExceptions,
                                                        methodQualifiers,
                                                        0);
    shapeInterfaceElements.push_back(methodFooSignature);
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* subShapeInterfaceTypeSpecifier =
      new InterfaceTypeSpecifier(NULL, "ISubShape", 0);
    shapeParentInterfaces.push_back(subShapeInterfaceTypeSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              shapeIinterfaceStructType,
                                              shapeParentInterfaces,
                                              shapeInterfaceElements,
                                              mContext.getImportProfile(),
                                              0);
    mShapeInterface->buildMethods(mContext);
    mContext.addInterface(mShapeInterface, 0);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceArguments;
    vector<IObjectElementDefinition*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectInterfaceThrownExceptions;
    IObjectElementDefinition* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "bar",
                                     objectInterfaceArguments,
                                     objectInterfaceThrownExceptions,
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
    mObjectInterface->buildMethods(mContext);
    mContext.addInterface(mObjectInterface, 0);

    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    vector<IInterfaceTypeSpecifier*> carParentInterfaces;
    vector<IObjectElementDefinition*> carInterfaceElements;
    mCarInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                            carFullName,
                                            carInterfaceStructType,
                                            carParentInterfaces,
                                            carInterfaceElements,
                                            mContext.getImportProfile(),
                                            0);
    mCarInterface->buildMethods(mContext);
    mContext.addInterface(mCarInterface, 0);

    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel->setFields(mContext, fields, interfaces.size());
    mModel->setMethods(methods);
    mModel->setInterfaces(interfaces);
    
    string cirlceFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, cirlceFullName);
    vector<Type*> circleTypes;
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                   cirlceFullName,
                                   circleStructType,
                                   mContext.getImportProfile(),
                                   0);
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  cirlceFullName + ".typename");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       cirlceFullName + ".typename");
    IConcreteObjectType::declareTypeNameGlobal(mContext, mCircleModel);
    IConcreteObjectType::declareVTable(mContext, mCircleModel);

    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<IField*> starFields;
    starFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mBrightness", 0));
    starFields.push_back(new ReceivedField(PrimitiveTypes::INT, "mWeight", 0));
    mStarModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 starFullName,
                                 starStructType,
                                 mContext.getImportProfile(),
                                 0);
    mStarModel->setFields(mContext, starFields, 1u);
    mContext.addModel(mStarModel, 0);
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    Value* field3Value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
    ON_CALL(*mField3Expression, generateIR(_, _)).WillByDefault(Return(field3Value));
    ON_CALL(*mField3Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
    
    IConcreteObjectType::declareTypeNameGlobal(mContext, mStarModel);
    IConcreteObjectType::declareVTable(mContext, mStarModel);
    
    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mDeclareBlock= BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
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
  EXPECT_EQ(mModel, mModel->getOwner()->getReference());
}

TEST_F(ModelOwnerTest, getNameTest) {
  EXPECT_STREQ(mModel->getOwner()->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare*");
}

TEST_F(ModelOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mModel->getOwner()->getLLVMType(mContext), mModel->getLLVMType(mContext));
}

TEST_F(ModelOwnerTest, canCastToTest) {
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mContext, mCircleModel->getOwner()));
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mContext, mCarInterface->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mContext, mModel->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mContext, mShapeInterface->getOwner()));

  EXPECT_FALSE(mModel->getOwner()->canCastTo(mContext, mCircleModel));
  EXPECT_FALSE(mModel->getOwner()->canCastTo(mContext, mCarInterface));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mContext, mModel));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_TRUE(mModel->getOwner()->
               canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mModel->getOwner()->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mModel->getOwner()->
              canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ModelOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mContext, mCircleModel->getOwner()));
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mContext, mCarInterface->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mContext, mModel->getOwner()));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mContext, mShapeInterface->getOwner()));
  
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mContext, mCircleModel));
  EXPECT_FALSE(mModel->getOwner()->canAutoCastTo(mContext, mCarInterface));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mContext, mModel));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_TRUE(mModel->getOwner()->
              canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mModel->getOwner()->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mModel->getOwner()->
              canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ModelOwnerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mContext));
  mModel->getOwner()->castTo(mContext, pointer, mShapeInterface->getOwner(), 0);
  
  *mStringStream << *mEntryBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IShape*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->getOwner()->castTo(mContext, pointer, mSubShapeInterface->getOwner(), 0);
  
  *mStringStream << *mEntryBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.ISubShape*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelOwnerTest, isTypeKindTest) {
  EXPECT_FALSE(mModel->getOwner()->isPrimitive());
  EXPECT_TRUE(mModel->getOwner()->isOwner());
  EXPECT_FALSE(mModel->getOwner()->isReference());
  EXPECT_FALSE(mModel->getOwner()->isArray());
  EXPECT_FALSE(mModel->getOwner()->isFunction());
  EXPECT_FALSE(mModel->getOwner()->isPackage());
  EXPECT_FALSE(mModel->getOwner()->isNative());
  EXPECT_FALSE(mModel->getOwner()->isPointer());
  EXPECT_TRUE(mModel->getOwner()->isImmutable());
}

TEST_F(ModelOwnerTest, isObjectTest) {
  EXPECT_FALSE(mModel->getOwner()->isController());
  EXPECT_FALSE(mModel->getOwner()->isInterface());
  EXPECT_TRUE(mModel->getOwner()->isModel());
  EXPECT_FALSE(mModel->getOwner()->isNode());
}

TEST_F(ModelOwnerTest, createLocalVariableTest) {
  mModel->getOwner()->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.MSquare*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store %systems.vos.wisey.compiler.tests.MSquare* null, %systems.vos.wisey.compiler.tests.MSquare** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new ReceivedField(mModel->getOwner(), "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mModel->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ModelOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mContext));
  mModel->getOwner()->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %var = alloca %systems.vos.wisey.compiler.tests.MSquare*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store %systems.vos.wisey.compiler.tests.MSquare* null, %systems.vos.wisey.compiler.tests.MSquare** %var\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelOwnerTest, injectDeathTest) {
  ::Mock::AllowLeak(mField1Expression);
  ::Mock::AllowLeak(mField2Expression);
  ::Mock::AllowLeak(mField3Expression);
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mModel->getOwner()->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type systems.vos.wisey.compiler.tests.MSquare* is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, ownerAssignToReferenceRunTest) {
  runFile("tests/samples/test_owner_assign_to_reference.yz", 3);
}
