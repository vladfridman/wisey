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

#include "MockExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

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
  Interface* mSubShapeInterface;
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  Interface* mCarInterface;
  Method* mMethod;
  StructType* mStructType;
  Field* mWidthField;
  Field* mHeightField;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  NiceMock<MockExpression>* mField3Expression;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()),
  mField3Expression(new NiceMock<MockExpression>()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    mWidthField = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    mHeightField = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    fields["width"] = mWidthField;
    fields["height"] = mHeightField;
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method("foo",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL,
                         0);
    vector<Method*> methods;
    methods.push_back(mMethod);
    Method* barMethod = new Method("bar",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL,
                                   1);
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
                                                              subShapeInterfaceThrownExceptions,
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
    vector<const Model*> shapeInterfaceThrownExceptions;
    methodFooSignature = new MethodSignature("foo",
                                             AccessLevel::PUBLIC_ACCESS,
                                             PrimitiveTypes::INT_TYPE,
                                             shapeInterfaceMethodArguments,
                                             shapeInterfaceThrownExceptions,
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
    vector<const Model*> objectInterfaceThrownExceptions;
    MethodSignature* methodBarSignature = new MethodSignature("bar",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              objectInterfaceThrownExceptions,
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
    map<string, Field*> starFields;
    starFields["mBrightness"] =
      new Field(PrimitiveTypes::INT_TYPE, "mBrightness", 0, fieldArguments);
    starFields["mWeight"] = new Field(PrimitiveTypes::INT_TYPE, "mWeight", 1, fieldArguments);
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
    
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ModelTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
    delete mField3Expression;
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
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo()->getPointerTo());
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

TEST_F(ModelTest, getSizeTest) {
  Value* value = mModel->getSize(mContext);
  IRWriter::createReturnInst(mContext, value);
  
  GenericValue result = mContext.runCode();
  
  EXPECT_EQ(result.IntVal, 8);
}

TEST_F(ModelTest, createRTTITest) {
  GlobalVariable* rtti = mContext.getModule()->
    getGlobalVariable(mCircleModel->getRTTIVariableName());
  ASSERT_EQ(rtti, nullptr);
  
  mCircleModel->createRTTI(mContext);
  rtti = mContext.getModule()->getGlobalVariable(mCircleModel->getRTTIVariableName());
  ASSERT_NE(rtti, nullptr);
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
  ASSERT_EQ(mBasicBlock->size(), 1u);

  *mStringStream << *mBasicBlock->begin();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %systems.vos.wisey.compiler.tests.MSquare** null "
               "to %systems.vos.wisey.compiler.tests.IShape**");
  mStringBuffer.clear();
}

TEST_F(ModelTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mSubShapeInterface);
  
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
  
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
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
  Mock::AllowLeak(mField3Expression);
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
  string argumentSpecifier1("withBrightness");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withWeight");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2, mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  Value* result = mStarModel->build(mContext, argumentList);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  ASSERT_EQ(6ul, mBasicBlock->size());
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 mul nuw (i64 ptrtoint "
  "(i32* getelementptr (i32, i32* null, i32 1) to i64), i64 2))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.MStar*"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.MStar, "
    "%systems.vos.wisey.compiler.tests.MStar* %buildervar, i32 0, i32 0"
  "\n  store i32 3, i32* %0"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MStar, "
    "%systems.vos.wisey.compiler.tests.MStar* %buildervar, i32 0, i32 1"
  "\n  store i32 5, i32* %1\n";
  
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
}

TEST_F(ModelTest, buildInvalidObjectBuilderArgumentsDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mField3Expression);
  
  string argumentSpecifier1("width");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withWeight");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2, mField2Expression);
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
  Mock::AllowLeak(mField3Expression);
  
  string argumentSpecifier1("withBrightness");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withWeight");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2, mField3Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              "Error: Model builder argument value for field mWeight does not match its type");
}

TEST_F(ModelTest, buildNotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mField3Expression);
  
  string argumentSpecifier1("withBrightness");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1, mField1Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  
  const char *expected =
  "Error: Field mWeight is not initialized"
  "\nError: Some fields of the model systems.vos.wisey.compiler.tests.MStar are not initialized.";
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}
