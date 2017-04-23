//
//  TestModel.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Model}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "yazyk/MethodSignature.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

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
  NiceMock<MockExpression> mField1Expression;
  NiceMock<MockExpression> mField2Expression;
  NiceMock<MockExpression> mField3Expression;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ModelTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "MSquare");
    mStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    mWidthField = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    mHeightField = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    fields["width"] = mWidthField;
    fields["height"] = mHeightField;
    vector<MethodArgument*> methodArguments;
    vector<IType*> thrownExceptions;
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
    StructType* subShapeIinterfaceStructType = StructType::create(mLLVMContext, "ISubShape");
    subShapeIinterfaceStructType->setBody(subShapeInterfaceTypes);
    vector<MethodArgument*> subShapeInterfaceMethodArguments;
    vector<MethodSignature*> subShapeInterfaceMethods;
    vector<IType*> subShapeInterfaceThrownExceptions;
    MethodSignature* methodFooSignature = new MethodSignature("foo",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              subShapeInterfaceMethodArguments,
                                                              subShapeInterfaceThrownExceptions,
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
    vector<IType*> shapeInterfaceThrownExceptions;
    methodFooSignature = new MethodSignature("foo",
                                             AccessLevel::PUBLIC_ACCESS,
                                             PrimitiveTypes::INT_TYPE,
                                             shapeInterfaceMethodArguments,
                                             shapeInterfaceThrownExceptions,
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
    vector<IType*> objectInterfaceThrownExceptions;
    MethodSignature* methodBarSignature = new MethodSignature("bar",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              objectInterfaceThrownExceptions,
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
    
    vector<Interface*> interfaces;
    interfaces.push_back(mShapeInterface);
    interfaces.push_back(mObjectInterface);
    mModel = new Model("MSquare", mStructType, fields, methods, interfaces);

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

    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    StructType *starStructType = StructType::create(mLLVMContext, "MStar");
    starStructType->setBody(starTypes);
    map<string, Field*> starFields;
    starFields["mBrightness"] =
      new Field(PrimitiveTypes::INT_TYPE, "mBrightness", 0, fieldArguments);
    starFields["mWeight"] = new Field(PrimitiveTypes::INT_TYPE, "mWeight", 1, fieldArguments);
    vector<Method*> starMethods;
    vector<Interface*> starInterfaces;
    mStarModel = new Model("MStar", starStructType, starFields, starMethods, starInterfaces);
    mContext.addModel(mStarModel);
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(mField1Expression, generateIR(_)).WillByDefault(Return(field1Value));
    ON_CALL(mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(mField2Expression, generateIR(_)).WillByDefault(Return(field2Value));
    ON_CALL(mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field3Value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
    ON_CALL(mField3Expression, generateIR(_)).WillByDefault(Return(field3Value));
    ON_CALL(mField3Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ModelTest() {
    delete mStringStream;
  }
};

TEST_F(ModelTest, modelInstantiationTest) {
  EXPECT_STREQ(mModel->getName().c_str(), "MSquare");
  EXPECT_STREQ(mModel->getVTableName().c_str(), "model.MSquare.vtable");
  EXPECT_EQ(mModel->getTypeKind(), MODEL_TYPE);
  EXPECT_EQ(mModel->getLLVMType(mLLVMContext), mStructType->getPointerTo());
  EXPECT_EQ(mModel->getInterfaces().size(), 2u);
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
}

TEST_F(ModelTest, canAutoCastToTest) {
  EXPECT_FALSE(mModel->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mModel->canAutoCastTo(mCircleModel));
  EXPECT_FALSE(mModel->canAutoCastTo(mCarInterface));
  EXPECT_TRUE(mModel->canAutoCastTo(mModel));
  EXPECT_TRUE(mModel->canAutoCastTo(mShapeInterface));
}

TEST_F(ModelTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get((PointerType*) mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mShapeInterface);
  ASSERT_EQ(mBasicBlock->size(), 1u);

  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = bitcast %MSquare* null to %IShape*");
  mStringBuffer.clear();
}

TEST_F(ModelTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get((PointerType*) mModel->getLLVMType(mLLVMContext));
  mModel->castTo(mContext, pointer, mSubShapeInterface);
  ASSERT_EQ(mBasicBlock->size(), 3u);
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = bitcast %MSquare* null to i8*");
  mStringBuffer.clear();

  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %add.ptr = getelementptr i8, i8* %0, i64 8");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %1 = bitcast i8* %add.ptr to %ISubShape*");
  mStringBuffer.clear();
}

TEST_F(ModelTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mModel->getObjectNameGlobalVariableName().c_str(), "model.MSquare.name");
}

TEST_F(ModelTest, getTypeTableName) {
  ASSERT_STREQ(mModel->getTypeTableName().c_str(), "model.MSquare.typetable");
}

TEST_F(ModelTest, castToDeathTest) {
  Mock::AllowLeak(&mField1Expression);
  Mock::AllowLeak(&mField2Expression);
  Mock::AllowLeak(&mField3Expression);
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);

  EXPECT_EXIT(mModel->castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'MSquare' to 'int'");
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
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withWeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mField2Expression);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  Value* result = mStarModel->build(mContext, argumentList);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  ASSERT_EQ(6ul, mBasicBlock->size());
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  string expected = string() +
  "  %malloccall = tail call i8* @malloc(i32 trunc (i64 mul nuw (i64 ptrtoint" +
  " (i32* getelementptr (i32, i32* null, i32 1) to i64), i64 2) to i32))";
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %buildervar = bitcast i8* %malloccall to %MStar*");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = getelementptr %MStar, %MStar* %buildervar, i32 0, i32 0");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 3, i32* %0");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %1 = getelementptr %MStar, %MStar* %buildervar, i32 0, i32 1");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 5, i32* %1");
  mStringBuffer.clear();
}

TEST_F(ModelTest, buildInvalidModelBuilderArgumentsDeathTest) {
  Mock::AllowLeak(&mField1Expression);
  Mock::AllowLeak(&mField2Expression);
  Mock::AllowLeak(&mField3Expression);
  
  string argumentSpecifier1("width");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withWeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mField2Expression);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  const char *expected =
  "Error: Model builder argument should start with 'with'. e.g. .withField\\(value\\)."
  "\nError: Some arguments for the model 'MStar' builder are not well formed";
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelTest, buildIncorrectArgumentTypeDeathTest) {
  Mock::AllowLeak(&mField1Expression);
  Mock::AllowLeak(&mField2Expression);
  Mock::AllowLeak(&mField3Expression);
  
  string argumentSpecifier1("withBrightness");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mField1Expression);
  string argumentSpecifier2("withWeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mField3Expression);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  const char *expected = "Error: Model builder argument value for field 'mWeight' "
    "does not match its type";
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(ModelTest, buildNotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(&mField1Expression);
  Mock::AllowLeak(&mField2Expression);
  Mock::AllowLeak(&mField3Expression);
  
  string argumentSpecifier1("withBrightness");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mField1Expression);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  
  const char *expected =
  "Error: Field 'mWeight' is not initialized"
  "\nError: Some fields of the model 'MStar' are not initialized.";
  
  EXPECT_EXIT(mStarModel->build(mContext, argumentList),
              ::testing::ExitedWithCode(1),
              expected);
}
