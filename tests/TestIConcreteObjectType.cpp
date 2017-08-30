//
//  TestIConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/31/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IConcreteObjectType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
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

struct IConcreteObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockConcreteObjectType> mMockObject;
  Interface* mInterface3;
  Interface* mCanNavigate;
  Model* mStarModel;
  Model* mGalaxyModel;
  Model* mCarModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IConcreteObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.getScopes().pushScope();
    ON_CALL(mMockObject, getName()).WillByDefault(Return("Object"));
    ON_CALL(mMockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("Object.name"));

    vector<Interface*> interfaces;
    Interface* interface1 = new Interface("Interface1", NULL);
    Interface* interface2 = new Interface("Interface2", NULL);
    mInterface3 = new Interface("Interface3", NULL);
    Interface* interface4 = new Interface("Interface4", NULL);
    interfaces.push_back(interface1);
    interfaces.push_back(interface2);
    interfaces.push_back(mInterface3);
    interfaces.push_back(interface4);
    ON_CALL(mMockObject, getFlattenedInterfaceHierarchy()).WillByDefault(Return(interfaces));

    ExpressionList fieldArguments;
    
    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType *starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    map<string, IField*> starFields;
    starFields["mBrightness"] = new FieldFixed(PrimitiveTypes::INT_TYPE,
                                               "mBrightness",
                                               0,
                                               fieldArguments);
    starFields["mWeight"] = new FieldFixed(PrimitiveTypes::INT_TYPE, "mWeight", 1, fieldArguments);
    mStarModel = new Model(starFullName, starStructType);
    mStarModel->setFields(starFields);
    mContext.addModel(mStarModel);
    
    vector<Type*> galaxyTypes;
    galaxyTypes.push_back(mStarModel->getLLVMType(mLLVMContext)->getPointerElementType());
    string galaxyFullName = "systems.vos.wisey.compiler.tests.MGalaxy";
    StructType* galaxyStructType = StructType::create(mLLVMContext, galaxyFullName);
    galaxyStructType->setBody(galaxyTypes);
    map<string, IField*> galaxyFields;
    galaxyFields["mStar"] = new FieldFixed(mStarModel->getOwner(), "mStar", 0, fieldArguments);
    mGalaxyModel = new Model(galaxyFullName, galaxyStructType);
    mGalaxyModel->setFields(galaxyFields);
    mContext.addModel(mGalaxyModel);

    
    vector<Type*> canNavigateTypes;
    canNavigateTypes.push_back(Type::getInt32Ty(mLLVMContext)->getPointerTo()->getPointerTo());
    string canNavigateFullName = "systems.vos.wisey.compiler.tests.ICanNavigate";
    StructType* canNavigateStructType = StructType::create(mLLVMContext, canNavigateFullName);
    mCanNavigate = new Interface(canNavigateFullName, canNavigateStructType);

    vector<Type*> carTypes;
    carTypes.push_back(mCanNavigate->getLLVMType(mLLVMContext)->getPointerElementType());
    string carFullName = "systems.vos.wisey.compiler.tests.MCar";
    StructType* carStructType = StructType::create(mLLVMContext, carFullName);
    carStructType->setBody(carTypes);
    map<string, IField*> carFields;
    carFields["mNavigator"] = new FieldFixed(mCanNavigate->getOwner(), "mNavigator", 0, fieldArguments);
    mCarModel = new Model(carFullName, carStructType);
    mCarModel->setFields(carFields);
    mContext.addModel(mCarModel);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);

  IConcreteObjectType::generateNameGlobal(mContext, &mMockObject);

  ASSERT_NE(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);
}

TEST_F(IConcreteObjectTypeTest, getInterfaceIndexTest) {
  ASSERT_EQ(IConcreteObjectType::getInterfaceIndex(&mMockObject, mInterface3), 2);
}

TEST_F(IConcreteObjectTypeTest, declareFieldVariablesTest) {
  EXPECT_EQ(mContext.getScopes().getVariable("mBrightness"), nullptr);

  IConcreteObjectType::declareFieldVariables(mContext, mStarModel);

  EXPECT_NE(mContext.getScopes().getVariable("mBrightness"), nullptr);
}

TEST_F(IConcreteObjectTypeTest, composeDestructorBodyTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateVTable(mContext, mStarModel);
  IConcreteObjectType::composeDestructorBody(mContext, mStarModel);
  
  Function* function = mContext.getModule()->getFunction("destructor." + mStarModel->getName());
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @destructor.systems.vos.wisey.compiler.tests.MStar("
  "%systems.vos.wisey.compiler.tests.MStar** %this) {"
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MStar*, "
  "%systems.vos.wisey.compiler.tests.MStar** %this"
  "\n  %1 = icmp eq %systems.vos.wisey.compiler.tests.MStar* %0, null"
  "\n  br i1 %1, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MStar* %0 to i8*"
  "\n  call void @__freeIfNotNull(i8* %2)"
  "\n  ret void"
  "\n}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithObjectOwnerFieldsTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateVTable(mContext, mStarModel);
  
  IConcreteObjectType::generateNameGlobal(mContext, mGalaxyModel);
  IConcreteObjectType::generateVTable(mContext, mGalaxyModel);
  IConcreteObjectType::composeDestructorBody(mContext, mGalaxyModel);

  Function* function = mContext.getModule()->getFunction("destructor." + mGalaxyModel->getName());
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @destructor.systems.vos.wisey.compiler.tests.MGalaxy("
  "%systems.vos.wisey.compiler.tests.MGalaxy** %this) {"
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MGalaxy*, "
  "%systems.vos.wisey.compiler.tests.MGalaxy** %this"
  "\n  %1 = icmp eq %systems.vos.wisey.compiler.tests.MGalaxy* %0, null"
  "\n  br i1 %1, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MGalaxy, "
  "%systems.vos.wisey.compiler.tests.MGalaxy* %0, i32 0, i32 0"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.MStar*, "
  "%systems.vos.wisey.compiler.tests.MStar** %2"
  "\n  %4 = icmp eq %systems.vos.wisey.compiler.tests.MStar* %3, null"
  "\n  br i1 %4, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %if.notnull, %if.this.notnull"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MGalaxy* %0 to i8*"
  "\n  call void @__freeIfNotNull(i8* %5)"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %if.this.notnull"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MStar("
  "%systems.vos.wisey.compiler.tests.MStar** %2)"
  "\n  br label %if.null"
  "\n}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithInterfaceOwnerFieldsTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mCarModel);
  IConcreteObjectType::generateVTable(mContext, mCarModel);
  IConcreteObjectType::composeDestructorBody(mContext, mCarModel);
  
  Function* function = mContext.getModule()->getFunction("destructor." + mCarModel->getName());
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @destructor.systems.vos.wisey.compiler.tests.MCar("
  "%systems.vos.wisey.compiler.tests.MCar** %this) {"
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MCar*, "
  "%systems.vos.wisey.compiler.tests.MCar** %this"
  "\n  %1 = icmp eq %systems.vos.wisey.compiler.tests.MCar* %0, null"
  "\n  br i1 %1, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MCar, "
  "%systems.vos.wisey.compiler.tests.MCar* %0, i32 0, i32 0"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.ICanNavigate*, "
  "%systems.vos.wisey.compiler.tests.ICanNavigate** %2"
  "\n  %4 = icmp eq %systems.vos.wisey.compiler.tests.ICanNavigate* %3, null"
  "\n  br i1 %4, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %if.notnull, %if.this.notnull"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MCar* %0 to i8*"
  "\n  call void @__freeIfNotNull(i8* %5)"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %if.this.notnull"
  "\n  %6 = load %systems.vos.wisey.compiler.tests.ICanNavigate*, "
  "%systems.vos.wisey.compiler.tests.ICanNavigate** %2"
  "\n  %7 = bitcast %systems.vos.wisey.compiler.tests.ICanNavigate* %6 to i8***"
  "\n  %vtable = load i8**, i8*** %7"
  "\n  %8 = getelementptr i8*, i8** %vtable, i64 0"
  "\n  %unthunkbypointer = load i8*, i8** %8"
  "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
  "\n  %9 = bitcast %systems.vos.wisey.compiler.tests.ICanNavigate* %6 to i8*"
  "\n  %10 = getelementptr i8, i8* %9, i64 %unthunkby"
  "\n  %11 = bitcast i8* %10 to void (%systems.vos.wisey.compiler.tests.ICanNavigate**)***"
  "\n  %vtable1 = load void (%systems.vos.wisey.compiler.tests.ICanNavigate**)**, "
  "void (%systems.vos.wisey.compiler.tests.ICanNavigate**)*** %11"
  "\n  %12 = getelementptr void (%systems.vos.wisey.compiler.tests.ICanNavigate**)*, "
  "void (%systems.vos.wisey.compiler.tests.ICanNavigate**)** %vtable1, i64 2"
  "\n  %13 = load void (%systems.vos.wisey.compiler.tests.ICanNavigate**)*, "
  "void (%systems.vos.wisey.compiler.tests.ICanNavigate**)** %12"
  "\n  %14 = bitcast i8* %10 to %systems.vos.wisey.compiler.tests.ICanNavigate*"
  "\n  %15 = alloca %systems.vos.wisey.compiler.tests.ICanNavigate*"
  "\n  store %systems.vos.wisey.compiler.tests.ICanNavigate* %14, "
  "%systems.vos.wisey.compiler.tests.ICanNavigate** %15"
  "\n  call void %13(%systems.vos.wisey.compiler.tests.ICanNavigate** %15)"
  "\n  br label %if.null"
  "\n}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorCallTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateVTable(mContext, mStarModel);

  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "main",
                                        mContext.getModule());
  
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.setMainFunction(function);

  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mStarModel->getLLVMType(mLLVMContext));
  IConcreteObjectType::composeDestructorCall(mContext, mStarModel, pointer);
  
  *mStringStream << *basicBlock;
  string expected =
  "\nentry:"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MStar("
  "%systems.vos.wisey.compiler.tests.MStar** null)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, freeingFieldVariablesRunTest) {
  runFile("tests/samples/test_freeing_field_variables.yz", "3");
}

TEST_F(TestFileSampleRunner, freeingFieldInterfaceOwnersRunTest) {
  runFile("tests/samples/test_freeing_field_interface_owners.yz", "3");
}

