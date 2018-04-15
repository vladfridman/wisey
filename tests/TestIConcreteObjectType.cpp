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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

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
  Model* mConstellationModel;
  Model* mCarModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IConcreteObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    mContext.getScopes().pushScope();
    ON_CALL(mMockObject, getTypeName()).WillByDefault(Return("Object"));
    ON_CALL(mMockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("Object.name"));

    vector<Interface*> interfaces;
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    Interface* interface1 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                    "Interface1",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements);
    Interface* interface2 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                    "Interface2",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements);
    mInterface3 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                          "Interface3",
                                          NULL,
                                          parentInterfaces,
                                          interfaceElements);
    Interface* interface4 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                    "Interface4",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements);
    interfaces.push_back(interface1);
    interfaces.push_back(interface2);
    interfaces.push_back(mInterface3);
    interfaces.push_back(interface4);
    ON_CALL(mMockObject, getFlattenedInterfaceHierarchy()).WillByDefault(Return(interfaces));

    InjectionArgumentList fieldArguments;
    
    vector<Type*> starTypes;
    starTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType* starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<IField*> starFields;
    starFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mBrightness"));
    starFields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mWeight"));
    mStarModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, starFullName, starStructType);
    mStarModel->setFields(starFields, 1u);
    mContext.addModel(mStarModel);
    
    vector<Type*> galaxyTypes;
    galaxyTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                          ->getPointerTo()->getPointerTo());
    galaxyTypes.push_back(mStarModel->getOwner()->getLLVMType(mContext));
    string galaxyFullName = "systems.vos.wisey.compiler.tests.MGalaxy";
    StructType* galaxyStructType = StructType::create(mLLVMContext, galaxyFullName);
    galaxyStructType->setBody(galaxyTypes);
    vector<IField*> galaxyFields;
    galaxyFields.push_back(new FixedField(mStarModel->getOwner(), "mStar"));
    mGalaxyModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, galaxyFullName, galaxyStructType);
    mGalaxyModel->setFields(galaxyFields, 1u);
    mContext.addModel(mGalaxyModel);

    vector<Type*> constellationTypes;
    constellationTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                                 ->getPointerTo()->getPointerTo());
    constellationTypes.push_back(mStarModel->getLLVMType(mContext));
    string constellationFullName = "systems.vos.wisey.compiler.tests.MConstellation";
    StructType* constellationStructType = StructType::create(mLLVMContext, constellationFullName);
    constellationStructType->setBody(constellationTypes);
    vector<IField*> constellationFields;
    constellationFields.push_back(new FixedField(mStarModel, "mStar"));
    mConstellationModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                          constellationFullName,
                                          constellationStructType);
    mConstellationModel->setFields(constellationFields, 1u);
    mContext.addModel(mConstellationModel);
    
    vector<Type*> canNavigateTypes;
    canNavigateTypes.push_back(Type::getInt32Ty(mLLVMContext)->getPointerTo()->getPointerTo());
    string canNavigateFullName = "systems.vos.wisey.compiler.tests.ICanNavigate";
    StructType* canNavigateStructType = StructType::create(mLLVMContext, canNavigateFullName);
    vector<IInterfaceTypeSpecifier*> canNavigateParentInterfaces;
    vector<IObjectElementDefinition*> canNavigateElements;
    mCanNavigate = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                           canNavigateFullName,
                                           canNavigateStructType,
                                           canNavigateParentInterfaces,
                                           canNavigateElements);

    vector<Type*> carTypes;
    carTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                       ->getPointerTo()->getPointerTo());
    carTypes.push_back(mCanNavigate->getLLVMType(mContext));
    string carFullName = "systems.vos.wisey.compiler.tests.MCar";
    StructType* carStructType = StructType::create(mLLVMContext, carFullName);
    carStructType->setBody(carTypes);
    vector<IField*> carFields;
    carFields.push_back(new FixedField(mCanNavigate->getOwner(), "mNavigator"));
    mCarModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, carFullName, carStructType);
    mCarModel->setFields(carFields, 1u);
    mContext.addModel(mCarModel);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getNamedGlobal("Object.name"), nullptr);

  IConcreteObjectType::generateNameGlobal(mContext, &mMockObject);
  IConcreteObjectType::generateShortNameGlobal(mContext, &mMockObject);

  ASSERT_NE(mContext.getModule()->getNamedGlobal("Object.name"), nullptr);
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
  IConcreteObjectType::generateShortNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateVTable(mContext, mStarModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mStarModel);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->getFunction(mStarModel->getTypeName() + ".destructor");
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MStar.destructor(i8* %this) "
  "personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast i8* %this to %systems.vos.wisey.compiler.tests.MStar*"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MStar* %1 to i64*"
  "\n  %3 = getelementptr i64, i64* %2, i64 -1"
  "\n  %refCounter = load i64, i64* %3"
  "\n  %4 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %4, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %5 = getelementptr i8, i8* %this, i64 -8"
  "\n  tail call void @free(i8* %5)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %6 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %6"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithObjectOwnerFieldTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateShortNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateVTable(mContext, mStarModel);
  
  IConcreteObjectType::generateNameGlobal(mContext, mGalaxyModel);
  IConcreteObjectType::generateShortNameGlobal(mContext, mGalaxyModel);
  IConcreteObjectType::generateVTable(mContext, mGalaxyModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mGalaxyModel);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(mGalaxyModel->getTypeName() +
                                                         ".destructor");
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MGalaxy.destructor(i8* %this) "
  "personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast i8* %this to %systems.vos.wisey.compiler.tests.MGalaxy*"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MGalaxy, %systems.vos.wisey.compiler.tests.MGalaxy* %1, i32 0, i32 1"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.MStar*, %systems.vos.wisey.compiler.tests.MStar** %2"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MStar* %3 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %4)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MGalaxy* %1 to i64*"
  "\n  %6 = getelementptr i64, i64* %5, i64 -1"
  "\n  %refCounter = load i64, i64* %6"
  "\n  %7 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %7, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %8 = getelementptr i8, i8* %this, i64 -8"
  "\n  tail call void @free(i8* %8)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithObjectReferenceFieldTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mConstellationModel);
  IConcreteObjectType::generateShortNameGlobal(mContext, mConstellationModel);
  IConcreteObjectType::generateVTable(mContext, mConstellationModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mConstellationModel);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(mConstellationModel->getTypeName() +
                                                         ".destructor");
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MConstellation.destructor(i8* %this) "
  "personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast i8* %this to %systems.vos.wisey.compiler.tests.MConstellation*"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MConstellation, %systems.vos.wisey.compiler.tests.MConstellation* %1, i32 0, i32 1"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.MStar*, %systems.vos.wisey.compiler.tests.MStar** %2"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MStar* %3 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 -1)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MConstellation* %1 to i64*"
  "\n  %6 = getelementptr i64, i64* %5, i64 -1"
  "\n  %refCounter = load i64, i64* %6"
  "\n  %7 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %7, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %8 = getelementptr i8, i8* %this, i64 -8"
  "\n  tail call void @free(i8* %8)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}


TEST_F(IConcreteObjectTypeTest, composeDestructorForObjectWithInterfaceOwnerFieldsTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mCarModel);
  IConcreteObjectType::generateShortNameGlobal(mContext, mCarModel);
  IConcreteObjectType::generateVTable(mContext, mCarModel);
  IConcreteObjectType::scheduleDestructorBodyComposition(mContext, mCarModel);
  mContext.runComposingCallbacks();

  Function* function = mContext.getModule()->getFunction(mCarModel->getTypeName() +
                                                         ".destructor");
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MCar.destructor(i8* %this) "
  "personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %this, null"
  "\n  br i1 %0, label %if.this.null, label %if.this.notnull"
  "\n"
  "\nif.this.null:                                     ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.this.notnull:                                  ; preds = %entry"
  "\n  %1 = bitcast i8* %this to %systems.vos.wisey.compiler.tests.MCar*"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MCar, %systems.vos.wisey.compiler.tests.MCar* %1, i32 0, i32 1"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.ICanNavigate*, %systems.vos.wisey.compiler.tests.ICanNavigate** %2"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.ICanNavigate* %3 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %4)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.MCar* %1 to i64*"
  "\n  %6 = getelementptr i64, i64* %5, i64 -1"
  "\n  %refCounter = load i64, i64* %6"
  "\n  %7 = icmp eq i64 %refCounter, 0"
  "\n  br i1 %7, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.this.notnull"
  "\n  %8 = getelementptr i8, i8* %this, i64 -8"
  "\n  tail call void @free(i8* %8)"
  "\n  ret void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.this.notnull"
  "\n  invoke void @__throwReferenceCountException(i64 %refCounter)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, composeDestructorCallTest) {
  IConcreteObjectType::generateNameGlobal(mContext, mStarModel);
  IConcreteObjectType::generateShortNameGlobal(mContext, mStarModel);
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

  ConstantPointerNull* pointer = ConstantPointerNull::get(mCarModel->getLLVMType(mContext));
  IConcreteObjectType::composeDestructorCall(mContext, pointer);
  
  *mStringStream << *basicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MCar* null to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %0)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IConcreteObjectTypeTest, addInterfaceAndItsParentsTest) {
  vector<IInterfaceTypeSpecifier*> interfaceTypeSpecifiers;
  vector<IObjectElementDefinition *> objectElements;

  vector<Interface*> flattenedInterfaces;

  string package = "some.package";
  StructType* grandChildStructType = StructType::create(mLLVMContext, "some.package.IGrandChild");
  Interface* grandChild = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                  "some.package.IGrandChild",
                                                  grandChildStructType,
                                                  interfaceTypeSpecifiers,
                                                  objectElements);

  PackageType* packageType = new PackageType(package);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(packageExpression,
                                                               "IGrandChild",
                                                               0));
  StructType* child1StructType = StructType::create(mLLVMContext, "some.package.IChild1");
  Interface* child1 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              "some.package.IChild1",
                                              child1StructType,
                                              interfaceTypeSpecifiers,
                                              objectElements);
  interfaceTypeSpecifiers.clear();
  StructType* child2StructType = StructType::create(mLLVMContext, "some.package.IChild2");
  Interface* child2 = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              "some.package.IChild2",
                                              child2StructType,
                                              interfaceTypeSpecifiers,
                                              objectElements);

  packageExpression = new FakeExpression(NULL, packageType);
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(packageExpression, "IChild1", 0));
  packageExpression = new FakeExpression(NULL, packageType);
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(packageExpression, "IChild2", 0));
  StructType* parentStructType = StructType::create(mLLVMContext, "some.package.IParent");
  Interface* parent = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              "some.package.IParent",
                                              parentStructType,
                                              interfaceTypeSpecifiers,
                                              objectElements);
  mContext.addInterface(parent);
  mContext.addInterface(child1);
  mContext.addInterface(child2);
  mContext.addInterface(grandChild);

  parent->buildMethods(mContext);

  IConcreteObjectType::addInterfaceAndItsParents(parent, flattenedInterfaces);
  
  EXPECT_EQ(flattenedInterfaces.size(), 4u);
  EXPECT_EQ(flattenedInterfaces.at(0), parent);
  EXPECT_EQ(flattenedInterfaces.at(1), child1);
  EXPECT_EQ(flattenedInterfaces.at(2), grandChild);
  EXPECT_EQ(flattenedInterfaces.at(3), child2);
}

TEST_F(IConcreteObjectTypeTest, getDestructorFunctionTypeTest) {
  Type* int8Pointer = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(int8Pointer);
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  
  Type* expextedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  Type* actualType = IConcreteObjectType::getDestructorFunctionType(mContext);
  
  EXPECT_EQ(expextedType, actualType);
}

TEST_F(TestFileRunner, freeingFieldVariablesRunTest) {
  runFile("tests/samples/test_freeing_field_variables.yz", "3");
}

TEST_F(TestFileRunner, freeingFieldInterfaceOwnersRunTest) {
  runFile("tests/samples/test_freeing_field_interface_owners.yz", "3");
}

TEST_F(TestFileRunner, compareObjectToNullRunTest) {
  runFile("tests/samples/test_compare_object_to_null.yz", "1");
}
