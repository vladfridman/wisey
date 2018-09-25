//
//  TestInterfaceOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceOwner}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "InterfaceOwner.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ThreadExpression.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InterfaceOwnerTest : public Test {
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  StructType* mShapeStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  InterfaceOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> objectTypes;
    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(objectTypes);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectStructType,
                                               parentInterfaces,
                                               interfaceElements,
                                               mContext.getImportProfile(),
                                               0);
    
    vector<Type*> shapeTypes;
    string shapeFullName = "systems.vos.wisey.compiler.tests.IShape";
    mShapeStructType = StructType::create(mLLVMContext, shapeFullName);
    mShapeStructType->setBody(shapeTypes);
    vector<IObjectElementDefinition*> shapeMethodElements;
    vector<IInterfaceTypeSpecifier*> shapeParentInterfaces;
    InterfaceTypeSpecifier* objectInterfaceSpecifier =
    new InterfaceTypeSpecifier(NULL, "IObject", 0);
    shapeParentInterfaces.push_back(objectInterfaceSpecifier);
    mShapeInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                              shapeFullName,
                                              mShapeStructType,
                                              shapeParentInterfaces,
                                              shapeMethodElements,
                                              mContext.getImportProfile(),
                                              0);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InterfaceOwnerTest() {
    delete mStringStream;
  }
};

TEST_F(InterfaceOwnerTest, getObjectTest) {
  EXPECT_EQ(mShapeInterface, mShapeInterface->getOwner()->getReference());
}

TEST_F(InterfaceOwnerTest, getNameTest) {
  EXPECT_STREQ(mShapeInterface->getOwner()->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.IShape*");
}

TEST_F(InterfaceOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mShapeInterface->getOwner()->getLLVMType(mContext),
            mShapeInterface->getLLVMType(mContext));
}

TEST_F(InterfaceOwnerTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mContext, mObjectInterface));

  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mContext, mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mObjectInterface->getOwner()->canAutoCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canAutoCastTo(mContext, mObjectInterface));
  
  EXPECT_TRUE(mObjectInterface->getOwner()->canAutoCastTo(mContext, mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canAutoCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceOwnerTest, isTypeKindTest) {
  EXPECT_FALSE(mShapeInterface->getOwner()->isPrimitive());
  EXPECT_TRUE(mObjectInterface->getOwner()->isOwner());
  EXPECT_FALSE(mObjectInterface->getOwner()->isReference());
  EXPECT_FALSE(mObjectInterface->getOwner()->isArray());
  EXPECT_FALSE(mObjectInterface->getOwner()->isFunction());
  EXPECT_FALSE(mObjectInterface->getOwner()->isPackage());
  EXPECT_FALSE(mObjectInterface->getOwner()->isNative());
  EXPECT_FALSE(mObjectInterface->getOwner()->isPointer());
  EXPECT_FALSE(mObjectInterface->getOwner()->isImmutable());
}

TEST_F(InterfaceOwnerTest, isObjectTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->isController());
  EXPECT_TRUE(mObjectInterface->getOwner()->isInterface());
  EXPECT_FALSE(mObjectInterface->getOwner()->isModel());
  EXPECT_FALSE(mObjectInterface->getOwner()->isNode());
}

TEST_F(InterfaceOwnerTest, createLocalVariableTest) {
  mObjectInterface->getOwner()->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.IObject*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store %systems.vos.wisey.compiler.tests.IObject* null, %systems.vos.wisey.compiler.tests.IObject** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new ReceivedField(mObjectInterface->getOwner(), "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mObjectInterface->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(InterfaceOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mObjectInterface->getOwner()->getLLVMType(mContext));
  mObjectInterface->getOwner()->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %var = alloca %systems.vos.wisey.compiler.tests.IObject*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store %systems.vos.wisey.compiler.tests.IObject* null, %systems.vos.wisey.compiler.tests.IObject** %var\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceOwnerTest, injectTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.ITest";
  StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IInterfaceTypeSpecifier*> interfaceParentInterfaces;
  vector<IObjectElementDefinition*> interafaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 interfaceStructType,
                                                 interfaceParentInterfaces,
                                                 interafaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  mContext.addInterface(interface, 0);
  llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                interface->getTypeName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
  
  string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
  StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
  vector<Type*> controllerTypes;
  controllerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                            ->getPointerTo()->getPointerTo());
  controllerStructType->setBody(controllerTypes);
  Controller* controller = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                     controllerFullName,
                                                     controllerStructType,
                                                     mContext.getImportProfile(),
                                                     0);
  vector<Interface*> controllerParentInterfaces;
  controllerParentInterfaces.push_back(interface);
  controller->setInterfaces(controllerParentInterfaces);
  
  mContext.addController(controller, 0);
  mContext.bindInterfaceToController(interface->getTypeName(), controller->getTypeName(), 0);
  
  IConcreteObjectType::declareTypeNameGlobal(mContext, controller);
  IConcreteObjectType::declareVTable(mContext, controller);
  
  InjectionArgumentList arguments;
  interface->getOwner()->inject(mContext, arguments, 0);
  
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call %systems.vos.wisey.compiler.tests.ITest* @systems.vos.wisey.compiler.tests.ITest.inject(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, interfaceOwnerCastToModelOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_model_owner.yz", 5);
}

TEST_F(TestFileRunner, interfaceOwnerCastToInterfaceOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_interface_owner.yz", 5);
}

TEST_F(TestFileRunner, interfaceOwnerCastToAnotherInterfaceOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_another_interface_owner.yz", 5);
}

TEST_F(TestFileRunner, interfaceOnwerAutoCastRunTest) {
  runFile("tests/samples/test_interface_owner_autocast.yz", 5);
}

TEST_F(TestFileRunner, interfaceOwnerCearedAndThrowsNpeDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_owner_cleared_and_throws_npe.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_owner_cleared_and_throws_npe.yz:43)\n"
                               "Main thread ended without a result\n");
}
