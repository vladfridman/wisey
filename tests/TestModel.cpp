//
//  TestModel.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Model}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/Target.h>

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/Constant.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/VariableDeclaration.hpp"
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/WiseyModelType.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ModelTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  Model* mCircleModel;
  Model* mStarModel;
  Model* mGalaxyModel;
  Model* mBirthdateModel;
  Interface* mSubShapeInterface;
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mCarInterface;
  IMethod* mMethod;
  StructType* mStructType;
  FixedField* mWidthField;
  FixedField* mHeightField;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  wisey::Constant* mConstant;
  BasicBlock *mBasicBlock;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  LLVMFunction* mLLVMFunction;

  ModelTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);

    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    vector<IField*> fields;
    mWidthField = new FixedField(PrimitiveTypes::INT_TYPE, "mWidth", 0);
    mHeightField = new FixedField(PrimitiveTypes::INT_TYPE, "mHeight", 0);
    fields.push_back(mWidthField);
    fields.push_back(mHeightField);
    vector<const wisey::Argument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, mStructType, 3);
    mMethod = new Method(mModel,
                         "foo",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         new MethodQualifiers(0),
                         NULL,
                         0);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    IMethod* barMethod = new Method(mModel,
                                    "bar",
                                    AccessLevel::PRIVATE_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
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
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
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
                                                 0);
    mContext.addInterface(mSubShapeInterface);
    mSubShapeInterface->buildMethods(mContext);
    
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
    InterfaceTypeSpecifier* subShapeInterfaceSpecifier =
      new InterfaceTypeSpecifier(NULL, "ISubShape", 0);
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(subShapeInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              shapeIinterfaceStructType,
                                              shapeParentInterfaces,
                                              shapeInterfaceElements,
                                              0);
    mContext.addInterface(mShapeInterface);
    mShapeInterface->buildMethods(mContext);

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
                                               0);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    vector<IInterfaceTypeSpecifier*> carParentInterfaces;
    vector<IObjectElementDefinition*> carInterfaceElements;
    mCarInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                            carFullName,
                                            carInterfaceStructType,
                                            carParentInterfaces,
                                            carInterfaceElements,
                                            0);
    mContext.addInterface(mCarInterface);
    mCarInterface->buildMethods(mContext);

    IntConstant* intConstant = new IntConstant(5, 0);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    "MYCONSTANT",
                                    intConstant);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);

    vector<const IType*> functionArgumentTypes;
    LLVMFunctionType* llvmFunctionType = new LLVMFunctionType(LLVMPrimitiveTypes::I8,
                                                              functionArgumentTypes);
    vector<const wisey::Argument*> llvmFunctionArguments;
    Block* functionBlock = new Block();
    CompoundStatement* functionCompoundStatement = new CompoundStatement(functionBlock, 0);
    mLLVMFunction = new LLVMFunction(mModel,
                                     "myfunction",
                                     PUBLIC_ACCESS,
                                     llvmFunctionType,
                                     LLVMPrimitiveTypes::I8,
                                     llvmFunctionArguments,
                                     functionCompoundStatement,
                                     0);
    vector<LLVMFunction*> functions;
    functions.push_back(mLLVMFunction);

    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel->setFields(mContext, fields, interfaces.size());
    mModel->setMethods(methods);
    mModel->setInterfaces(interfaces);
    mModel->setConstants(constants);
    mModel->setLLVMFunctions(functions);

    string cirlceFullName = "systems.vos.wisey.compiler.tests.MCircle";
    StructType* circleStructType = StructType::create(mLLVMContext, cirlceFullName);
    vector<Type*> circleTypes;
    circleTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                          ->getPointerTo()->getPointerTo());
    circleStructType->setBody(circleTypes);
    mCircleModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, cirlceFullName, circleStructType, 0);
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  cirlceFullName + ".name");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       cirlceFullName + ".name");

    vector<Type*> galaxyTypes;
    galaxyTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                          ->getPointerTo()->getPointerTo());
    string galaxyFullName = "systems.vos.wisey.compiler.tests.MGalaxy";
    StructType* galaxyStructType = StructType::create(mLLVMContext, galaxyFullName);
    galaxyStructType->setBody(galaxyTypes);
    mGalaxyModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, galaxyFullName, galaxyStructType, 0);
    mContext.addModel(mGalaxyModel);

    vector<Type*> birthdateTypes;
    birthdateTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                             ->getPointerTo()->getPointerTo());
    string birthdateFullName = "systems.vos.wisey.compiler.tests.MBirthdate";
    StructType* birthdateStructType = StructType::create(mLLVMContext, birthdateFullName);
    birthdateStructType->setBody(birthdateTypes);
    mBirthdateModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                      birthdateFullName,
                                      birthdateStructType,
                                      0);
    mContext.addModel(mBirthdateModel);
    
    vector<Type*> starTypes;
    starTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    starTypes.push_back(mBirthdateModel->getLLVMType(mContext));
    starTypes.push_back(mGalaxyModel->getLLVMType(mContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<IField*> starFields;
    starFields.push_back(new FixedField(mBirthdateModel->getOwner(), "mBirthdate", 0));
    starFields.push_back(new FixedField(mGalaxyModel, "mGalaxy", 0));
    mStarModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, starFullName, starStructType, 0);
    mStarModel->setFields(mContext, starFields, 1u);
    mContext.addModel(mStarModel);
    Value* field1Value = ConstantPointerNull::get(mBirthdateModel->getOwner()
                                                  ->getLLVMType(mContext));
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(mBirthdateModel->getOwner()));
    Value* field2Value = ConstantPointerNull::get(mGalaxyModel->getLLVMType(mContext));
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(mGalaxyModel));
    
    IConcreteObjectType::generateNameGlobal(mContext, mBirthdateModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mBirthdateModel);
    IConcreteObjectType::generateVTable(mContext, mBirthdateModel);

    IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mStarModel);
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
 
    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ModelTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
    delete mThreadVariable;
  }
};

TEST_F(ModelTest, isPublicTest) {
  EXPECT_TRUE(mModel->isPublic());
}

TEST_F(ModelTest, getLineTest) {
  EXPECT_EQ(3, mModel->getLine());
}

TEST_F(ModelTest, getNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MSquare", mModel->getTypeName().c_str());
}

TEST_F(ModelTest, getShortNameTest) {
  EXPECT_STREQ("MSquare", mModel->getShortName().c_str());
}

TEST_F(ModelTest, getVTableNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MSquare.vtable", mModel->getVTableName().c_str());
}

TEST_F(ModelTest, getLLVMTypeTest) {
  EXPECT_EQ(mStructType->getPointerTo(), mModel->getLLVMType(mContext));
}

TEST_F(ModelTest, getInterfacesTest) {
  EXPECT_EQ(2u, mModel->getInterfaces().size());
}

TEST_F(ModelTest, getVTableSizeTest) {
  EXPECT_EQ(3u, mModel->getVTableSize());
}

TEST_F(ModelTest, getFieldsTest) {
  EXPECT_EQ(2u, mModel->getFields().size());
}

TEST_F(ModelTest, getOwnerTest) {
  ASSERT_NE(nullptr, mModel->getOwner());
  EXPECT_EQ(mModel, mModel->getOwner()->getReference());
}

TEST_F(ModelTest, createRTTITest) {
  GlobalVariable* rtti = mContext.getModule()->getNamedGlobal(mCircleModel->getRTTIVariableName());
  ASSERT_EQ(rtti, nullptr);
  
  mCircleModel->createRTTI(mContext);
  rtti = mContext.getModule()->getNamedGlobal(mCircleModel->getRTTIVariableName());
  ASSERT_NE(rtti, nullptr);
}

TEST_F(ModelTest, findFeildTest) {
  EXPECT_EQ(mWidthField, mModel->findField("mWidth"));
  EXPECT_EQ(mHeightField, mModel->findField("mHeight"));
  EXPECT_EQ(nullptr, mModel->findField("mDepth"));
}

TEST_F(ModelTest, getFieldIndexTest) {
  EXPECT_EQ(2u, mModel->getFieldIndex(mWidthField));
  EXPECT_EQ(3u, mModel->getFieldIndex(mHeightField));
}

TEST_F(ModelTest, findMethodTest) {
  EXPECT_EQ(mMethod, mModel->findMethod("foo"));
  EXPECT_EQ(nullptr, mModel->findMethod("get"));
}

TEST_F(ModelTest, findConstantTest) {
  EXPECT_EQ(mConstant, mModel->findConstant("MYCONSTANT"));
}

TEST_F(ModelTest, findConstantDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mModel->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Model systems.vos.wisey.compiler.tests.MSquare "
              "does not have constant named MYCONSTANT2");
}

TEST_F(ModelTest, findLLVMFunctionTest) {
  EXPECT_EQ(mLLVMFunction, mModel->findLLVMFunction("myfunction"));
}

TEST_F(ModelTest, getMissingFieldsTest) {
  set<string> givenFields;
  givenFields.insert("mWidth");
  
  vector<string> missingFields = mModel->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "mHeight");
}

TEST_F(ModelTest, innerObjectsTest) {
  NiceMock<MockObjectType> innerObject1;
  ON_CALL(innerObject1, getShortName()).WillByDefault(Return("MObject1"));
  NiceMock<MockObjectType> innerObject2;
  ON_CALL(innerObject2, getShortName()).WillByDefault(Return("MObject2"));
  
  mModel->addInnerObject(&innerObject1);
  mModel->addInnerObject(&innerObject2);
  
  EXPECT_EQ(mModel->getInnerObject("MObject1"), &innerObject1);
  EXPECT_EQ(mModel->getInnerObject("MObject2"), &innerObject2);
  EXPECT_EQ(mModel->getInnerObjects().size(), 2u);
}

TEST_F(ModelTest, markAsInnerTest) {
  EXPECT_FALSE(mModel->isInner());
  mModel->markAsInner();
  EXPECT_TRUE(mModel->isInner());
}

TEST_F(ModelTest, getReferenceAdjustmentFunctionTest) {
  Function* result = mModel->getReferenceAdjustmentFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(result, AdjustReferenceCounterForConcreteObjectSafelyFunction::get(mContext));
}

TEST_F(ModelTest, canCastToTest) {
  EXPECT_FALSE(mModel->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canCastTo(mContext, mCircleModel));
  EXPECT_FALSE(mModel->canCastTo(mContext, mCarInterface));
  EXPECT_TRUE(mModel->canCastTo(mContext, mModel));
  EXPECT_TRUE(mModel->canCastTo(mContext, mShapeInterface));
  EXPECT_FALSE(mModel->canCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mModel->canCastTo(mContext, mModel));
  EXPECT_TRUE(mModel->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mModel->canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mModel->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mModel->canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ModelTest, canAutoCastToTest) {
  EXPECT_FALSE(mModel->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canAutoCastTo(mContext, mCircleModel));
  EXPECT_FALSE(mModel->canAutoCastTo(mContext, mCarInterface));
  EXPECT_TRUE(mModel->canAutoCastTo(mContext, mModel));
  EXPECT_TRUE(mModel->canAutoCastTo(mContext, mShapeInterface));
  EXPECT_FALSE(mModel->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mModel->canAutoCastTo(mContext, mModel));
  EXPECT_TRUE(mModel->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mModel->canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mModel->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mModel->canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ModelTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->castTo(mContext, pointer, mShapeInterface, 0);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IShape*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->castTo(mContext, pointer, mSubShapeInterface, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.ISubShape*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, isTypeKindTest) {
  EXPECT_FALSE(mModel->isPrimitive());
  EXPECT_FALSE(mModel->isOwner());
  EXPECT_TRUE(mModel->isReference());
  EXPECT_FALSE(mModel->isArray());
  EXPECT_FALSE(mModel->isFunction());
  EXPECT_FALSE(mModel->isPackage());
  EXPECT_FALSE(mModel->isNative());
  EXPECT_FALSE(mModel->isPointer());
}

TEST_F(ModelTest, isObjectTest) {
  EXPECT_FALSE(mModel->isController());
  EXPECT_FALSE(mModel->isInterface());
  EXPECT_TRUE(mModel->isModel());
  EXPECT_FALSE(mModel->isNode());
  EXPECT_FALSE(mModel->isThread());
}

TEST_F(ModelTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, getReferenceCountTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i64*"
  "\n  %1 = getelementptr i64, i64* %0, i64 -1"
  "\n  %refCounter = load i64, i64* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mModel->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.name");
}

TEST_F(ModelTest, getObjectShortNameGlobalVariableNameTest) {
  ASSERT_STREQ(mModel->getObjectShortNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.shortname");
}

TEST_F(ModelTest, getTypeTableNameTest) {
  ASSERT_STREQ(mModel->getTypeTableName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.typetable");
}

TEST_F(ModelTest, getRTTIVariableNameTest) {
  ASSERT_STREQ(mModel->getRTTIVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.rtti");
}

TEST_F(ModelTest, castToDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);

  EXPECT_EXIT(mModel->castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: can not cast from "
              "type 'systems.vos.wisey.compiler.tests.MSquare' to 'int'");
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

TEST_F(ModelTest, buildTest) {
  string argumentSpecifier1("withBirthdate");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withGalaxy");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  Value* result = mStarModel->build(mContext, argumentList, 0);
  
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.MStar.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.MStar.refCounter, %systems.vos.wisey.compiler.tests.MStar.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.MStar.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MStar.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.MStar.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.MStar.refCounter, %systems.vos.wisey.compiler.tests.MStar.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MStar.refCounter, %systems.vos.wisey.compiler.tests.MStar.refCounter* %buildervar, i32 0, i32 1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MStar, %systems.vos.wisey.compiler.tests.MStar* %1, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.MBirthdate* null, %systems.vos.wisey.compiler.tests.MBirthdate** %2"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.MStar, %systems.vos.wisey.compiler.tests.MStar* %1, i32 0, i32 2"
  "\n  store %systems.vos.wisey.compiler.tests.MGalaxy* null, %systems.vos.wisey.compiler.tests.MGalaxy** %3"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MGalaxy* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 1)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MStar* %1 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.MStar.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, buildInvalidObjectBuilderArgumentsDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  string argumentSpecifier1("width");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withGalaxy");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  const char *expected =
  "Error: Object builder argument should start with 'with'. e.g. .withField\\(value\\)."
  "\nError: Some arguments for the model systems.vos.wisey.compiler.tests.MStar "
  "builder are not well formed";
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList, 0),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelTest, buildIncorrectArgumentTypeDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  string argumentSpecifier1("withBirthdate");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField2Expression);
  string argumentSpecifier2("withGalaxy");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField1Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList, 0),
              ::testing::ExitedWithCode(1),
              "Error: Model builder argument value for field mBirthdate does not match its type");
}

TEST_F(ModelTest, buildNotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  string argumentSpecifier1("withBirthdate");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  
  const char *expected =
  "Error: Field mGalaxy is not initialized"
  "\nError: Some fields of the model systems.vos.wisey.compiler.tests.MStar are not initialized.";
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList, 0),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelTest, printToStreamTest) {
  stringstream stringStream;
  Model* innerPublicModel = Model::newModel(PUBLIC_ACCESS, "MInnerPublicModel", NULL, 0);
  vector<IField*> fields;
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField1", 0));
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField2", 0));
  innerPublicModel->setFields(mContext, fields, 0);

  vector<const wisey::Argument*> methodArguments;
  vector<const Model*> thrownExceptions;
  Method* method = new Method(innerPublicModel,
                              "bar",
                              AccessLevel::PUBLIC_ACCESS,
                              PrimitiveTypes::INT_TYPE,
                              methodArguments,
                              thrownExceptions,
                              new MethodQualifiers(0),
                              NULL,
                              0);
  vector<IMethod*> methods;
  methods.push_back(method);
  innerPublicModel->setMethods(methods);

  Model* innerPrivateModel = Model::newModel(PRIVATE_ACCESS, "MInnerPrivateModel", NULL, 0);
  innerPrivateModel->setFields(mContext, fields, 0);

  mModel->addInnerObject(innerPublicModel);
  mModel->addInnerObject(innerPrivateModel);
  mModel->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external model systems.vos.wisey.compiler.tests.MSquare\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IShape,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  fixed int mWidth;\n"
               "  fixed int mHeight;\n"
               "\n"
               "  int foo();\n"
               "}\n"
               "\n"
               "external model MInnerPrivateModel {\n"
               "}\n"
               "\n"
               "external model MInnerPublicModel {\n"
               "\n"
               "  fixed int mField1;\n"
               "  fixed int mField2;\n"
               "\n"
               "  int bar();\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(ModelTest, createLocalVariableTest) {
  mModel->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %referenceDeclaration = alloca %systems.vos.wisey.compiler.tests.MSquare*"
  "\n  store %systems.vos.wisey.compiler.tests.MSquare* null, %systems.vos.wisey.compiler.tests.MSquare** %referenceDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  FixedField* field = new FixedField(mModel, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mModel->createFieldVariable(mContext, "temp", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ModelTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  mModel->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, injectDeathTest) {
  ::Mock::AllowLeak(mField1Expression);
  ::Mock::AllowLeak(mField2Expression);
  ::Mock::AllowLeak(mThreadVariable);
  InjectionArgumentList arguments;
  EXPECT_EXIT(mModel->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type systems.vos.wisey.compiler.tests.MSquare "
              "is not injectable");
}

TEST_F(TestFileRunner, modelBuilderObjectArgumentAutocastRunTest) {
  runFile("tests/samples/test_model_builder_object_argument_autocast.yz", "2017");
}

TEST_F(TestFileRunner, modelBuilderPrimitiveArgumentAutocastRunTest) {
  runFile("tests/samples/test_model_builder_primitive_argument_autocast.yz", "1");
}

TEST_F(TestFileRunner, controllerWithInnerModelRunTest) {
  runFile("tests/samples/test_controller_with_inner_model.yz", "3");
}

TEST_F(TestFileRunner, controllerWithInnerModelShortSpecifiersRunTest) {
  runFile("tests/samples/test_controller_with_inner_model_short_specifiers.yz", "5");
}

TEST_F(TestFileRunner, controllerWithInnerModelMediumSpecifiersRunTest) {
  runFile("tests/samples/test_controller_with_inner_model_medium_specifiers.yz", "7");
}

TEST_F(TestFileRunner, innerObjectDefinedInDifferentObjectRunTest) {
  runFile("tests/samples/test_inner_object_defined_in_different_object.yz", "7");
}

TEST_F(TestFileRunner, innerObjectPrivateAccessableRunTest) {
  runFile("tests/samples/test_inner_object_private_accessable.yz", "3");
}

TEST_F(TestFileRunner, modelWithPrimitiveArrayRunTest) {
  runFile("tests/samples/test_model_with_primitive_array.yz", "5");
}

TEST_F(TestFileRunner, modelWithModelArrayRunTest) {
  runFile("tests/samples/test_model_with_model_array.yz", "3");
}

TEST_F(TestFileRunner, modelWithArrayReturnArrayOwnerRunTest) {
  runFile("tests/samples/test_model_with_array_return_array_owner.yz", "5");
}

TEST_F(TestFileRunner, innerObjectWrongTypeSpecifierDeathRunTest) {
  expectFailCompile("tests/samples/test_inner_object_wrong_type_specifier.yz",
                    1,
                    "Error: Model systems.vos.wisey.compiler.tests.MMyModel is not defined");
}

TEST_F(TestFileRunner, innerObjectPrivateInaccessableDeathRunTest) {
  expectFailCompile("tests/samples/test_inner_object_private_inaccessable.yz",
                    1,
                    "Error: Object systems.vos.wisey.compiler.tests.MOuterModel.MMyModel "
                    "is not accessable from object systems.vos.wisey.compiler.tests.CProgram");
}

TEST_F(TestFileRunner, modelWithNodeFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_node_field.yz",
                    1,
                    "tests/samples/test_model_with_node_field.yz\\(7\\): Error: "
                    "Model fields can only be of primitive, model or array owner type");
}

TEST_F(TestFileRunner, modelWithControllerFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_controller_field.yz",
                    1,
                    "tests/samples/test_model_with_controller_field.yz\\(7\\): Error: "
                    "Model fields can only be of primitive, model or array owner type");
}

TEST_F(TestFileRunner, modelWithNodeArrayDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_node_array.yz",
                    1,
                    "tests/samples/test_model_with_node_array.yz\\(14\\): Error: "
                    "Array fields in models can only be of primitive or model base type");
}

TEST_F(TestFileRunner, modelWithArrayReturnArrayReferenceDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_array_return_array_reference.yz",
                    1,
                    "tests/samples/test_model_with_array_return_array_reference.yz\\(8\\): Error: "
                    "Not allowed to return array reference from a model, "
                    "only returning array owner type is allowed");
}

TEST_F(TestFileRunner, modelWithPrimitiveArrayRceDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_model_with_primitive_array_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_model_with_primitive_array_rce.yz:19)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, modelWithModelArrayRceDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_model_with_model_array_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_model_with_model_array_rce.yz:27)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, modelWithArrayReturnArrayOwnerRceDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_model_with_array_return_array_owner_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_model_with_array_return_array_owner_rce.yz:34)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}
