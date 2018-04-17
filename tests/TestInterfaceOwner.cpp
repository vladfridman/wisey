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
#include "TestFileRunner.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/InterfaceOwner.hpp"
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

struct InterfaceOwnerTest : public Test {
  Interface* mShapeInterface;
  Interface* mObjectInterface;
  StructType* mShapeStructType;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  InterfaceOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
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
                                              0);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
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

TEST_F(InterfaceOwnerTest, getDestructorFunctionTest) {
  Function* result = mObjectInterface->getOwner()->getDestructorFunction(mContext, 0);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  EXPECT_EQ(functionType, result->getFunctionType());
}

TEST_F(InterfaceOwnerTest, canCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mContext, mShapeInterface));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mContext, mObjectInterface));

  EXPECT_TRUE(mObjectInterface->getOwner()->canCastTo(mContext, mShapeInterface->getOwner()));
  EXPECT_TRUE(mShapeInterface->getOwner()->canCastTo(mContext, mObjectInterface->getOwner()));
}

TEST_F(InterfaceOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
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
}

TEST_F(InterfaceOwnerTest, isObjectTest) {
  EXPECT_FALSE(mObjectInterface->getOwner()->isController());
  EXPECT_TRUE(mObjectInterface->getOwner()->isInterface());
  EXPECT_FALSE(mObjectInterface->getOwner()->isModel());
  EXPECT_FALSE(mObjectInterface->getOwner()->isNode());
  EXPECT_FALSE(mObjectInterface->getOwner()->isThread());
}

TEST_F(InterfaceOwnerTest, createLocalVariableTest) {
  mObjectInterface->getOwner()->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.IObject*"
  "\n  store %systems.vos.wisey.compiler.tests.IObject* null, %systems.vos.wisey.compiler.tests.IObject** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(InterfaceOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mObjectInterface->getOwner(), "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mObjectInterface->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(InterfaceOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mObjectInterface->getOwner()->getLLVMType(mContext));
  mObjectInterface->getOwner()->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %var = alloca %systems.vos.wisey.compiler.tests.IObject*"
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
                                                 0);
  mContext.addInterface(interface);
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
                                                     0);
  vector<Interface*> controllerParentInterfaces;
  controllerParentInterfaces.push_back(interface);
  controller->setInterfaces(controllerParentInterfaces);
  
  mContext.addController(controller);
  mContext.bindInterfaceToController(interface, controller);
  
  IConcreteObjectType::generateNameGlobal(mContext, controller);
  IConcreteObjectType::generateShortNameGlobal(mContext, controller);
  IConcreteObjectType::generateVTable(mContext, controller);
  
  InjectionArgumentList arguments;
  interface->getOwner()->inject(mContext, arguments, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CController.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CController.refCounter, %systems.vos.wisey.compiler.tests.CController.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CController.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CController.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CController.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CController.refCounter, %systems.vos.wisey.compiler.tests.CController.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CController.refCounter, %systems.vos.wisey.compiler.tests.CController.refCounter* %injectvar, i32 0, i32 1"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.CController* %1 to i8*"
  "\n  %3 = getelementptr i8, i8* %2, i64 0"
  "\n  %4 = bitcast i8* %3 to i32 (...)***"
  "\n  %5 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CController.vtable, i32 0, i32 0, i32 0"
  "\n  %6 = bitcast i8** %5 to i32 (...)**"
  "\n  store i32 (...)** %6, i32 (...)*** %4"
  "\n  %7 = bitcast %systems.vos.wisey.compiler.tests.CController* %1 to i8*"
  "\n  %8 = getelementptr i8, i8* %7, i64 0"
  "\n  %9 = bitcast i8* %8 to %systems.vos.wisey.compiler.tests.ITest*"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, interfaceOwnerCastToModelOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_model_owner.yz", "5");
}

TEST_F(TestFileRunner, interfaceOwnerCastToInterfaceOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_interface_owner.yz", "5");
}

TEST_F(TestFileRunner, interfaceOwnerCastToAnotherInterfaceOwnerRunTest) {
  runFile("tests/samples/test_interface_owner_cast_to_another_interface_owner.yz", "5");
}

TEST_F(TestFileRunner, interfaceOnwerAutoCastRunTest) {
  runFile("tests/samples/test_interface_owner_autocast.yz", "5");
}

TEST_F(TestFileRunner, interfaceOwnerCearedAndThrowsNpeDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_interface_owner_cleared_and_throws_npe.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_interface_owner_cleared_and_throws_npe.yz:43)\n"
                               "Main thread ended without a result\n");
}
