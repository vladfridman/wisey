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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ControllerOwner.hpp"
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
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                parentInterfaces,
                                                interfaceElements);

    string additorFullName = "systems.vos.wisey.compiler.tests.CAdditor";
    StructType *additorStructType = StructType::create(mLLVMContext, additorFullName);
    mAdditorController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                   additorFullName,
                                                   additorStructType);
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
                                                   interfaceElements);
    mContext.addInterface(mCalculatorInterface);
    mCalculatorInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               parentInterfaces,
                                               interfaceElements);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    string scienceCalculatorFullName = "systems.vos.wisey.compiler.tests.IScienceCalculator";
    StructType* scienceCalculatorIinterfaceStructType =
    StructType::create(mLLVMContext, scienceCalculatorFullName);
    vector<IInterfaceTypeSpecifier*> scienceCalculatorParentInterfaces;
    vector<IObjectElementDeclaration*> scienceCalculatorInterfaceElements;
    InterfaceTypeSpecifier* calculatorTypeSpecifier = new InterfaceTypeSpecifier(NULL,
                                                                                 "ICalculator");
    scienceCalculatorParentInterfaces.push_back(calculatorTypeSpecifier);
    mScienceCalculatorInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                          scienceCalculatorFullName,
                                                          scienceCalculatorIinterfaceStructType,
                                                          scienceCalculatorParentInterfaces,
                                                          scienceCalculatorInterfaceElements);
    mContext.addInterface(mScienceCalculatorInterface);
    mScienceCalculatorInterface->buildMethods(mContext);

    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);

    string multiplierFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType* structType = StructType::create(mLLVMContext, multiplierFullName);
    mMultiplierController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                      multiplierFullName,
                                                      structType);
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
  EXPECT_EQ(mMultiplierController->getOwner()->getObjectType(), mMultiplierController);
}

TEST_F(ControllerOwnerTest, getNameTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getTypeName(),
            "systems.vos.wisey.compiler.tests.CMultiplier*");
}

TEST_F(ControllerOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getLLVMType(mContext),
            mMultiplierController->getLLVMType(mContext));
}

TEST_F(ControllerOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getTypeKind(), CONTROLLER_OWNER_TYPE);
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

TEST_F(ControllerOwnerTest, isPrimitiveTest) {
  EXPECT_FALSE(mMultiplierController->getOwner()->isPrimitive());
}

TEST_F(ControllerOwnerTest, isOwnerTest) {
  EXPECT_TRUE(mMultiplierController->getOwner()->isOwner());
}

TEST_F(ControllerOwnerTest, isReferenceTest) {
  EXPECT_FALSE(mMultiplierController->getOwner()->isReference());
}

TEST_F(ControllerOwnerTest, isArrayTest) {
  EXPECT_FALSE(mMultiplierController->getOwner()->isArray());
}

TEST_F(ControllerOwnerTest, isFunctionTest) {
  EXPECT_FALSE(mMultiplierController->getOwner()->isFunction());
}

TEST_F(ControllerOwnerTest, isPackageTest) {
  EXPECT_FALSE(mMultiplierController->getOwner()->isPackage());
}

TEST_F(ControllerOwnerTest, isObjectTest) {
  EXPECT_TRUE(mMultiplierController->getOwner()->isController());
  EXPECT_FALSE(mMultiplierController->getOwner()->isInterface());
  EXPECT_FALSE(mMultiplierController->getOwner()->isModel());
  EXPECT_FALSE(mMultiplierController->getOwner()->isNode());
}

TEST_F(ControllerOwnerTest, getDestructorFunctionTest) {
  Function* result = mAdditorController->getOwner()->getDestructorFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);

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
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
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
  "\n  %1 = getelementptr i8, i8* %0, i64 16"
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
  "\n  %ownerDeclaration = alloca %systems.vos.wisey.compiler.tests.CMultiplier*"
  "\n  store %systems.vos.wisey.compiler.tests.CMultiplier* null, %systems.vos.wisey.compiler.tests.CMultiplier** %ownerDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  InjectionArgumentList injectionArgumentList;
  Field* field = new Field(FIXED_FIELD,
                           mMultiplierController->getOwner(),
                           NULL,
                           "mField",
                           injectionArgumentList);
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
  "\n  %parameterObjectPointer = alloca %systems.vos.wisey.compiler.tests.CMultiplier*"
  "\n  store %systems.vos.wisey.compiler.tests.CMultiplier* null, %systems.vos.wisey.compiler.tests.CMultiplier** %parameterObjectPointer\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, controllerPassModelReferenceRunTest) {
  runFile("tests/samples/test_controller_pass_model_reference.yz", "3");
}

