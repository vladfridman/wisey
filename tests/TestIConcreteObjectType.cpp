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
#include "wisey/InterfaceTypeSpecifier.hpp"
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
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    Interface* interface1 = Interface::newInterface("Interface1",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements);
    Interface* interface2 = Interface::newInterface("Interface2",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements);
    mInterface3 = Interface::newInterface("Interface3",
                                          NULL,
                                          parentInterfaces,
                                          interfaceElements);
    Interface* interface4 = Interface::newInterface("Interface4",
                                                    NULL,
                                                    parentInterfaces,
                                                    interfaceElements);
    interfaces.push_back(interface1);
    interfaces.push_back(interface2);
    interfaces.push_back(mInterface3);
    interfaces.push_back(interface4);
    ON_CALL(mMockObject, getFlattenedInterfaceHierarchy()).WillByDefault(Return(interfaces));

    ExpressionList fieldArguments;
    
    vector<Type*> starTypes;
    starTypes.push_back(Type::getInt64Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    starTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string starFullName = "systems.vos.wisey.compiler.tests.MStar";
    StructType* starStructType = StructType::create(mLLVMContext, starFullName);
    starStructType->setBody(starTypes);
    vector<Field*> starFields;
    starFields.push_back(new Field(FIXED_FIELD,
                                   PrimitiveTypes::INT_TYPE,
                                   "mBrightness",
                                   fieldArguments));
    starFields.push_back(new Field(FIXED_FIELD,
                                   PrimitiveTypes::INT_TYPE,
                                   "mWeight",
                                   fieldArguments));
    mStarModel = Model::newModel(starFullName, starStructType);
    mStarModel->setFields(starFields, 1u);
    mContext.addModel(mStarModel);
    
    vector<Type*> galaxyTypes;
    galaxyTypes.push_back(Type::getInt64Ty(mLLVMContext));
    galaxyTypes.push_back(mStarModel->getLLVMType(mLLVMContext)->getPointerElementType());
    string galaxyFullName = "systems.vos.wisey.compiler.tests.MGalaxy";
    StructType* galaxyStructType = StructType::create(mLLVMContext, galaxyFullName);
    galaxyStructType->setBody(galaxyTypes);
    vector<Field*> galaxyFields;
    galaxyFields.push_back(new Field(FIXED_FIELD,
                                     mStarModel->getOwner(),
                                     "mStar",
                                     fieldArguments));
    mGalaxyModel = Model::newModel(galaxyFullName, galaxyStructType);
    mGalaxyModel->setFields(galaxyFields, 1u);
    mContext.addModel(mGalaxyModel);

    
    vector<Type*> canNavigateTypes;
    canNavigateTypes.push_back(Type::getInt32Ty(mLLVMContext)->getPointerTo()->getPointerTo());
    string canNavigateFullName = "systems.vos.wisey.compiler.tests.ICanNavigate";
    StructType* canNavigateStructType = StructType::create(mLLVMContext, canNavigateFullName);
    vector<InterfaceTypeSpecifier*> canNavigateParentInterfaces;
    vector<IObjectElementDeclaration*> canNavigateElements;
    mCanNavigate = Interface::newInterface(canNavigateFullName,
                                           canNavigateStructType,
                                           canNavigateParentInterfaces,
                                           canNavigateElements);

    vector<Type*> carTypes;
    carTypes.push_back(Type::getInt64Ty(mLLVMContext));
    carTypes.push_back(mCanNavigate->getLLVMType(mLLVMContext)->getPointerElementType());
    string carFullName = "systems.vos.wisey.compiler.tests.MCar";
    StructType* carStructType = StructType::create(mLLVMContext, carFullName);
    carStructType->setBody(carTypes);
    vector<Field*> carFields;
    carFields.push_back(new Field(FIXED_FIELD,
                                  mCanNavigate->getOwner(),
                                  "mNavigator",
                                  fieldArguments));
    mCarModel = Model::newModel(carFullName, carStructType);
    mCarModel->setFields(carFields, 1u);
    mContext.addModel(mCarModel);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getNamedGlobal("Object.name"), nullptr);

  IConcreteObjectType::generateNameGlobal(mContext, &mMockObject);

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
  IConcreteObjectType::generateVTable(mContext, mStarModel);
  IConcreteObjectType::composeDestructorBody(mContext, mStarModel);
  
  Function* function = mContext.getModule()->getFunction("destructor." + mStarModel->getName());
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @destructor.systems.vos.wisey.compiler.tests.MStar("
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
  "\n  tail call void @free(i8* %2)"
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
  "\ndefine void @destructor.systems.vos.wisey.compiler.tests.MGalaxy("
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
  "%systems.vos.wisey.compiler.tests.MGalaxy* %0, i32 0, i32 1"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MStar("
  "%systems.vos.wisey.compiler.tests.MStar** %2)"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MGalaxy* %0 to i8*"
  "\n  tail call void @free(i8* %3)"
  "\n  ret void"
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
  "\ndefine void @destructor.systems.vos.wisey.compiler.tests.MCar("
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
  "%systems.vos.wisey.compiler.tests.MCar* %0, i32 0, i32 1"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.ICanNavigate("
  "%systems.vos.wisey.compiler.tests.ICanNavigate** %2)"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MCar* %0 to i8*"
  "\n  tail call void @free(i8* %3)"
  "\n  ret void"
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

TEST_F(IConcreteObjectTypeTest, addInterfaceAndItsParentsTest) {
  mContext.setPackage("some.package");
  
  vector<InterfaceTypeSpecifier*> interfaceTypeSpecifiers;
  vector<IObjectElementDeclaration *> objectElements;

  vector<Interface*> flattenedInterfaces;

  vector<string> package;
  package.push_back("some");
  package.push_back("package");
  StructType* grandChildStructType = StructType::create(mLLVMContext, "some.package.IGrandChild");
  Interface* grandChild = Interface::newInterface("some.package.IGrandChild",
                                                  grandChildStructType,
                                                  interfaceTypeSpecifiers,
                                                  objectElements);

  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(package, "IGrandChild"));
  StructType* child1StructType = StructType::create(mLLVMContext, "some.package.IChild1");
  Interface* child1 = Interface::newInterface("some.package.IChild1",
                                             child1StructType,
                                             interfaceTypeSpecifiers,
                                             objectElements);
  interfaceTypeSpecifiers.clear();
  StructType* child2StructType = StructType::create(mLLVMContext, "some.package.IChild2");
  Interface* child2 = Interface::newInterface("some.package.IChild2",
                                              child2StructType,
                                              interfaceTypeSpecifiers,
                                              objectElements);

  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(package, "IChild1"));
  interfaceTypeSpecifiers.push_back(new InterfaceTypeSpecifier(package, "IChild2"));
  StructType* parentStructType = StructType::create(mLLVMContext, "some.package.IParent");
  Interface* parent = Interface::newInterface("some.package.IParent",
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

TEST_F(TestFileSampleRunner, freeingFieldVariablesRunTest) {
  runFile("tests/samples/test_freeing_field_variables.yz", "3");
}

TEST_F(TestFileSampleRunner, freeingFieldInterfaceOwnersRunTest) {
  runFile("tests/samples/test_freeing_field_interface_owners.yz", "3");
}

