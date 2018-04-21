//
//  TestControllerOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerOwner}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
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

struct ControllerOwnerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mMultiplierController;
  Controller* mAdditorController;
  Interface* mCalculatorInterface;
  Interface* mScienceCalculatorInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ControllerOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                parentInterfaces,
                                                interfaceElements,
                                                0);

    string additorFullName = "systems.vos.wisey.compiler.tests.CAdditor";
    StructType* additorStructType = StructType::create(mLLVMContext, additorFullName);
    vector<Type*> additorTypes;
    additorTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                           ->getPointerTo()->getPointerTo());
    additorStructType->setBody(additorTypes);
    mAdditorController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                   additorFullName,
                                                   additorStructType,
                                                   0);
    IConcreteObjectType::generateNameGlobal(mContext, mAdditorController);
    IConcreteObjectType::generateShortNameGlobal(mContext, mAdditorController);
    IConcreteObjectType::generateVTable(mContext, mAdditorController);

    string calculatorFullName = "systems.vos.wisey.compiler.tests.ICalculator";
    StructType* calculatorIinterfaceStructType = StructType::create(mLLVMContext,
                                                                    calculatorFullName);
    mCalculatorInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                   calculatorFullName,
                                                   calculatorIinterfaceStructType,
                                                   parentInterfaces,
                                                   interfaceElements,
                                                   0);
    mContext.addInterface(mCalculatorInterface);
    mCalculatorInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               parentInterfaces,
                                               interfaceElements,
                                               0);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    string scienceCalculatorFullName = "systems.vos.wisey.compiler.tests.IScienceCalculator";
    StructType* scienceCalculatorIinterfaceStructType =
    StructType::create(mLLVMContext, scienceCalculatorFullName);
    vector<IInterfaceTypeSpecifier*> scienceCalculatorParentInterfaces;
    vector<IObjectElementDefinition*> scienceCalculatorInterfaceElements;
    InterfaceTypeSpecifier* calculatorTypeSpecifier =
    new InterfaceTypeSpecifier(NULL, "ICalculator", 0);
    scienceCalculatorParentInterfaces.push_back(calculatorTypeSpecifier);
    mScienceCalculatorInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                          scienceCalculatorFullName,
                                                          scienceCalculatorIinterfaceStructType,
                                                          scienceCalculatorParentInterfaces,
                                                          scienceCalculatorInterfaceElements,
                                                          0);
    mContext.addInterface(mScienceCalculatorInterface);
    mScienceCalculatorInterface->buildMethods(mContext);

    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);

    string multiplierFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType* structType = StructType::create(mLLVMContext, multiplierFullName);
    mMultiplierController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                      multiplierFullName,
                                                      structType,
                                                      0);
    mMultiplierController->setInterfaces(interfaces);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ControllerOwnerTest, getObjectTest) {
  EXPECT_EQ(mMultiplierController, mMultiplierController->getOwner()->getReference());
}

TEST_F(ControllerOwnerTest, getNameTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getTypeName(),
            "systems.vos.wisey.compiler.tests.CMultiplier*");
}

TEST_F(ControllerOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getLLVMType(mContext),
            mMultiplierController->getLLVMType(mContext));
}

TEST_F(ControllerOwnerTest, canCastTest) {
  const IObjectOwnerType* controllerOwner = mMultiplierController->getOwner();

  EXPECT_FALSE(controllerOwner->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(controllerOwner->canCastTo(mContext, mAdditorController));
  EXPECT_FALSE(controllerOwner->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(controllerOwner->canCastTo(mContext, mMultiplierController));
  EXPECT_TRUE(controllerOwner->canCastTo(mContext, mCalculatorInterface));

  EXPECT_FALSE(controllerOwner->canCastTo(mContext, mAdditorController->getOwner()));
  EXPECT_FALSE(controllerOwner->canCastTo(mContext, mVehicleInterface->getOwner()));
  EXPECT_TRUE(controllerOwner->canCastTo(mContext, mMultiplierController->getOwner()));
  EXPECT_TRUE(controllerOwner->canCastTo(mContext, mCalculatorInterface->getOwner()));
  EXPECT_FALSE(controllerOwner->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(controllerOwner->
               canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(controllerOwner->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(controllerOwner->
               canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ControllerOwnerTest, canAutoCastTest) {
  const IObjectOwnerType* controllerOwner = mMultiplierController->getOwner();
  
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mContext, mAdditorController));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mContext, mMultiplierController));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mContext, mCalculatorInterface));
  
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mContext, mAdditorController->getOwner()));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mContext, mVehicleInterface->getOwner()));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mContext, mMultiplierController->getOwner()));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mContext, mCalculatorInterface->getOwner()));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(controllerOwner->
               canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(controllerOwner->
              canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ControllerOwnerTest, castToItselfTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  Value* result = mMultiplierController->getOwner()->castTo(mContext,
                                                            pointer,
                                                            mMultiplierController->getOwner(),
                                                            0);
  
  EXPECT_EQ(result, pointer);
}

TEST_F(ControllerOwnerTest, isTypeKindTest) {
  EXPECT_FALSE(mMultiplierController->getOwner()->isPrimitive());
  EXPECT_TRUE(mMultiplierController->getOwner()->isOwner());
  EXPECT_FALSE(mMultiplierController->getOwner()->isReference());
  EXPECT_FALSE(mMultiplierController->getOwner()->isArray());
  EXPECT_FALSE(mMultiplierController->getOwner()->isFunction());
  EXPECT_FALSE(mMultiplierController->getOwner()->isPackage());
  EXPECT_FALSE(mMultiplierController->getOwner()->isNative());
  EXPECT_FALSE(mMultiplierController->getOwner()->isPointer());
  EXPECT_FALSE(mMultiplierController->getOwner()->isImmutable());
}

TEST_F(ControllerOwnerTest, isObjectTest) {
  EXPECT_TRUE(mMultiplierController->getOwner()->isController());
  EXPECT_FALSE(mMultiplierController->getOwner()->isInterface());
  EXPECT_FALSE(mMultiplierController->getOwner()->isModel());
  EXPECT_FALSE(mMultiplierController->getOwner()->isNode());
  EXPECT_FALSE(mMultiplierController->getOwner()->isThread());
}

TEST_F(ControllerOwnerTest, getDestructorFunctionTest) {
  Function* result = mAdditorController->getOwner()->getDestructorFunction(mContext, 0);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argumentTypes, false);

  EXPECT_EQ(functionType, result->getFunctionType());
}

TEST_F(ControllerOwnerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->getOwner()->castTo(mContext,
                                            pointer,
                                            mScienceCalculatorInterface->getOwner(),
                                            0);
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IScienceCalculator*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->getOwner()->castTo(mContext,
                                            pointer,
                                            mCalculatorInterface->getOwner(),
                                            0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.ICalculator*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerOwnerTest, createLocalVariableTest) {
  mMultiplierController->getOwner()->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.CMultiplier*"
  "\n  store %systems.vos.wisey.compiler.tests.CMultiplier* null, %systems.vos.wisey.compiler.tests.CMultiplier** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mMultiplierController->getOwner(), "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mMultiplierController->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");

  EXPECT_NE(variable, nullptr);
}

TEST_F(ControllerOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mMultiplierController->getOwner()->getLLVMType(mContext));
  mMultiplierController->getOwner()->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %var = alloca %systems.vos.wisey.compiler.tests.CMultiplier*"
  "\n  store %systems.vos.wisey.compiler.tests.CMultiplier* null, %systems.vos.wisey.compiler.tests.CMultiplier** %var\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerOwnerTest, injectTest) {
  InjectionArgumentList injectionArguments;
  Value* result = mAdditorController->getOwner()->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CAdditor.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CAdditor.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* %injectvar, i32 0, i32 1"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.CAdditor* %1 to i8*"
  "\n  %3 = getelementptr i8, i8* %2, i64 0"
  "\n  %4 = bitcast i8* %3 to i32 (...)***"
  "\n  %5 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CAdditor.vtable, i32 0, i32 0, i32 0"
  "\n  %6 = bitcast i8** %5 to i32 (...)**"
  "\n  store i32 (...)** %6, i32 (...)*** %4"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, controllerPassModelReferenceRunTest) {
  runFile("tests/samples/test_controller_pass_model_reference.yz", "3");
}

