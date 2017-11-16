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

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Constant.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/VariableDeclaration.hpp"

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
  Field* mWidthField;
  Field* mHeightField;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  wisey::Constant* mConstant;
  BasicBlock *mBasicBlock;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList arguments;
    mWidthField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mWidth", arguments);
    mHeightField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mHeight", arguments);
    fields.push_back(mWidthField);
    fields.push_back(mHeightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method("foo",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    IMethod* barMethod = new Method("bar",
                                    AccessLevel::PRIVATE_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL);
    methods.push_back(barMethod);
    
    string subShapeFullName = "systems.vos.wisey.compiler.tests.ISubShape";
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, subShapeFullName);
    VariableList subShapeInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> subShapeInterfaceElements;
    vector<ModelTypeSpecifier*> subShapeInterfaceThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* methodFooSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     subShapeInterfaceMethodArguments,
                                     subShapeInterfaceThrownExceptions);
    subShapeInterfaceElements.push_back(methodFooSignature);
    vector<InterfaceTypeSpecifier*> subShapeParentInterfaces;
    mSubShapeInterface = Interface::newInterface(subShapeFullName,
                                                 subShapeIinterfaceStructType,
                                                 subShapeParentInterfaces,
                                                 subShapeInterfaceElements);
    mContext.addInterface(mSubShapeInterface);
    mSubShapeInterface->buildMethods(mContext);
    
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* shapeIinterfaceStructType = StructType::create(mLLVMContext, shapeFullName);
    VariableList shapeInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> shapeInterfaceElements;
    vector<ModelTypeSpecifier*> shapeInterfaceThrownExceptions;
    methodFooSignature = new MethodSignatureDeclaration(intSpecifier,
                                                        "foo",
                                                        shapeInterfaceMethodArguments,
                                                        shapeInterfaceThrownExceptions);
    shapeInterfaceElements.push_back(methodFooSignature);
    vector<string> package;
    InterfaceTypeSpecifier* subShapeInterfaceSpecifier =
      new InterfaceTypeSpecifier(package, "ISubShape");
    vector<InterfaceTypeSpecifier*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(subShapeInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(shapeFullName,
                                              shapeIinterfaceStructType,
                                              shapeParentInterfaces,
                                              shapeInterfaceElements);
    mContext.addInterface(mShapeInterface);
    mShapeInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> objectInterfaceElements;
    vector<ModelTypeSpecifier*> objectInterfaceThrownExceptions;
    IObjectElementDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "bar",
                                     objectInterfaceMethodArguments,
                                     objectInterfaceThrownExceptions);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<InterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    string carFullName = "systems.vos.wisey.compiler.tests.ICar";
    StructType* carInterfaceStructType = StructType::create(mLLVMContext, carFullName);
    vector<InterfaceTypeSpecifier*> carParentInterfaces;
    vector<IObjectElementDeclaration*> carInterfaceElements;
    mCarInterface = Interface::newInterface(carFullName,
                                            carInterfaceStructType,
                                            carParentInterfaces,
                                            carInterfaceElements);
    mContext.addInterface(mCarInterface);
    mCarInterface->buildMethods(mContext);

    IntConstant* intConstant = new IntConstant(5);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    "MYCONSTANT",
                                    intConstant);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);

    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel = Model::newModel(modelFullName, mStructType);
    mModel->setFields(fields, interfaces.size() + 1);
    mModel->setMethods(methods);
    mModel->setInterfaces(interfaces);
    mModel->setConstants(constants);

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

    vector<Type*> galaxyTypes;
    galaxyTypes.push_back(Type::getInt64Ty(mLLVMContext));
    string galaxyFullName = "systems.vos.wisey.compiler.tests.MGalaxy";
    StructType* galaxyStructType = StructType::create(mLLVMContext, galaxyFullName);
    galaxyStructType->setBody(galaxyTypes);
    mGalaxyModel = Model::newModel(galaxyFullName, galaxyStructType);
    mContext.addModel(mGalaxyModel);

    vector<Type*> birthdateTypes;
    birthdateTypes.push_back(Type::getInt64Ty(mLLVMContext));
    string birthdateFullName = "systems.vos.wisey.compiler.tests.MBirthdate";
    StructType* birthdateStructType = StructType::create(mLLVMContext, birthdateFullName);
    birthdateStructType->setBody(birthdateTypes);
    mBirthdateModel = Model::newModel(birthdateFullName, birthdateStructType);
    mContext.addModel(mBirthdateModel);
    
    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt64Ty(mLLVMContext));
    starTypes.push_back(mBirthdateModel->getLLVMType(mLLVMContext));
    starTypes.push_back(mGalaxyModel->getLLVMType(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<Field*> starFields;
    starFields.push_back(new Field(FIXED_FIELD,
                                   mBirthdateModel->getOwner(),
                                   "mBirthdate",
                                   arguments));
    starFields.push_back(new Field(FIXED_FIELD, mGalaxyModel, "mGalaxy", arguments));
    mStarModel = Model::newModel(starFullName, starStructType);
    mStarModel->setFields(starFields, 1u);
    mContext.addModel(mStarModel);
    Value* field1Value = ConstantPointerNull::get(mBirthdateModel->getOwner()
                                                  ->getLLVMType(mLLVMContext));
    ON_CALL(*mField1Expression, generateIR(_)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(mBirthdateModel->getOwner()));
    Value* field2Value = ConstantPointerNull::get(mGalaxyModel->getLLVMType(mLLVMContext));
    ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(mGalaxyModel));
    
    IConcreteObjectType::generateNameGlobal(mContext, mBirthdateModel);
    IConcreteObjectType::generateVTable(mContext, mBirthdateModel);
    IConcreteObjectType::composeDestructorBody(mContext, mBirthdateModel);

    IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
    IConcreteObjectType::generateVTable(mContext, mStarModel);
    IConcreteObjectType::composeDestructorBody(mContext, mStarModel);

    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(function);
 
    Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
    Value* threadObject = ConstantPointerNull::get(threadController->getLLVMType(mLLVMContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadController));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);
    
    vector<Catch*> catchList;
    FinallyBlock* emptyBlock = new FinallyBlock();
    TryCatchInfo* tryCatchInfo = new TryCatchInfo(mBasicBlock, emptyBlock, catchList);
    mContext.getScopes().setTryCatchInfo(tryCatchInfo);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ModelTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
    delete mThreadVariable;
  }
};

TEST_F(ModelTest, getNameTest) {
  EXPECT_STREQ(mModel->getName().c_str(), "systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(ModelTest, getShortNameTest) {
  EXPECT_STREQ(mModel->getShortName().c_str(), "MSquare");
}

TEST_F(ModelTest, getVTableNameTest) {
  EXPECT_STREQ(mModel->getVTableName().c_str(), "systems.vos.wisey.compiler.tests.MSquare.vtable");
}

TEST_F(ModelTest, getTypeKindTest) {
  EXPECT_EQ(mModel->getTypeKind(), MODEL_TYPE);
}

TEST_F(ModelTest, getLLVMTypeTest) {
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo());
}

TEST_F(ModelTest, getInterfacesTest) {
  EXPECT_EQ(mModel->getInterfaces().size(), 2u);
}

TEST_F(ModelTest, getVTableSizeTest) {
  EXPECT_EQ(mModel->getVTableSize(), 3u);
}

TEST_F(ModelTest, getFieldsTest) {
  EXPECT_EQ(mModel->getFields().size(), 2u);
}

TEST_F(ModelTest, getOwnerTest) {
  ASSERT_NE(mModel->getOwner(), nullptr);
  EXPECT_EQ(mModel->getOwner()->getObject(), mModel);
}

TEST(ModelGetSizeTest, getSizeTest) {
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> types;
  types.push_back(Type::getInt64Ty(llvmContext));
  types.push_back(Type::getInt32Ty(llvmContext));
  types.push_back(Type::getInt32Ty(llvmContext));
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(llvmContext, modelFullName);
  structType->setBody(types);

  Model* model = Model::newModel(modelFullName, structType);
  context.addModel(model);
  
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(llvmContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::ExternalLinkage,
                                        "main",
                                        context.getModule());
  
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function);
  context.setBasicBlock(basicBlock);
  context.getScopes().pushScope();
  context.setMainFunction(function);

  Value* value = model->getSize(context);
  IRWriter::createReturnInst(context, value);
  GenericValue result = context.runCode();
  
  EXPECT_EQ(result.IntVal, 16);
}

TEST_F(ModelTest, createRTTITest) {
  GlobalVariable* rtti = mContext.getModule()->getNamedGlobal(mCircleModel->getRTTIVariableName());
  ASSERT_EQ(rtti, nullptr);
  
  mCircleModel->createRTTI(mContext);
  rtti = mContext.getModule()->getNamedGlobal(mCircleModel->getRTTIVariableName());
  ASSERT_NE(rtti, nullptr);
}

TEST_F(ModelTest, findFeildTest) {
  EXPECT_EQ(mModel->findField("mWidth"), mWidthField);
  EXPECT_EQ(mModel->findField("mHeight"), mHeightField);
  EXPECT_EQ(mModel->findField("mDepth"), nullptr);
}

TEST_F(ModelTest, getFieldIndexTest) {
  EXPECT_EQ(mModel->getFieldIndex(mWidthField), 3u);
  EXPECT_EQ(mModel->getFieldIndex(mHeightField), 4u);
}

TEST_F(ModelTest, findMethodTest) {
  EXPECT_EQ(mModel->findMethod("foo"), mMethod);
  EXPECT_EQ(mModel->findMethod("get"), nullptr);
}

TEST_F(ModelTest, findConstantTest) {
  EXPECT_EQ(mModel->findConstant("MYCONSTANT"), mConstant);
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

TEST_F(ModelTest, getMissingFieldsTest) {
  set<string> givenFields;
  givenFields.insert("mWidth");
  
  vector<string> missingFields = mModel->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "mHeight");
}

TEST_F(ModelTest, canCastToTest) {
  EXPECT_FALSE(mModel->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canCastTo(mCircleModel));
  EXPECT_FALSE(mModel->canCastTo(mCarInterface));
  EXPECT_TRUE(mModel->canCastTo(mModel));
  EXPECT_TRUE(mModel->canCastTo(mShapeInterface));
  EXPECT_FALSE(mModel->canCastTo(NullType::NULL_TYPE));
}

TEST_F(ModelTest, canAutoCastToTest) {
  EXPECT_FALSE(mModel->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canAutoCastTo(mCircleModel));
  EXPECT_FALSE(mModel->canAutoCastTo(mCarInterface));
  EXPECT_TRUE(mModel->canAutoCastTo(mModel));
  EXPECT_TRUE(mModel->canAutoCastTo(mShapeInterface));
  EXPECT_FALSE(mModel->canAutoCastTo(NullType::NULL_TYPE));
}

TEST_F(ModelTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mShapeInterface);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IShape*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mSubShapeInterface);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 16"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.ISubShape*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, incremenetReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->incremenetReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, decremenetReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->decremenetReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare* null to i64*"
  "\n  %refCounter = load i64, i64* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mModel->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.name");
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

  EXPECT_EXIT(mModel->castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from "
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
  
  EXPECT_CALL(*mField1Expression, releaseOwnership(_)).Times(1);
  
  Value* result = mStarModel->build(mContext, argumentList);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint ("
  "%systems.vos.wisey.compiler.tests.MStar* getelementptr ("
  "%systems.vos.wisey.compiler.tests.MStar, %systems.vos.wisey.compiler.tests.MStar* null, i32 1) "
  "to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.MStar*"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.MStar, "
    "%systems.vos.wisey.compiler.tests.MStar* %buildervar, i32 0, i32 0"
  "\n  store i64 0, i64* %0"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MStar, "
    "%systems.vos.wisey.compiler.tests.MStar* %buildervar, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.MBirthdate* null, "
  "%systems.vos.wisey.compiler.tests.MBirthdate** %1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MStar, "
  "%systems.vos.wisey.compiler.tests.MStar* %buildervar, i32 0, i32 2"
  "\n  store %systems.vos.wisey.compiler.tests.MGalaxy* null, "
  "%systems.vos.wisey.compiler.tests.MGalaxy** %2"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MGalaxy* null to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %3, i64 1)\n";

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
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
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
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
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
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelTest, printToStreamTest) {
  stringstream stringStream;
  mModel->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external model systems.vos.wisey.compiler.tests.MSquare\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IShape,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  fixed int mWidth;\n"
               "  fixed int mHeight;\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  int foo();\n"
                "}\n",
               stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, modelBuilderObjectArgumentAutocast) {
  runFile("tests/samples/test_model_builder_object_argument_autocast.yz", "2017");
}

TEST_F(TestFileSampleRunner, modelBuilderPrimitiveArgumentAutocast) {
  runFile("tests/samples/test_model_builder_primitive_argument_autocast.yz", "1");
}
