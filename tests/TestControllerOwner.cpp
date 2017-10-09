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

#include "TestFileSampleRunner.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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
  
  ControllerOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mVehicleInterface = Interface::newInterface(vehicleFullName,
                                                vehicleInterfaceStructType,
                                                parentInterfaces,
                                                interfaceMethods);

    string additorFullName = "systems.vos.wisey.compiler.tests.CAdditor";
    StructType *additorStructType = StructType::create(mLLVMContext, additorFullName);
    mAdditorController = Controller::newController(additorFullName, additorStructType);
    
    string calculatorFullName = "systems.vos.wisey.compiler.tests.ICalculator";
    StructType* calculatorIinterfaceStructType = StructType::create(mLLVMContext,
                                                                    calculatorFullName);
    mCalculatorInterface = Interface::newInterface(calculatorFullName,
                                                   calculatorIinterfaceStructType,
                                                   parentInterfaces,
                                                   interfaceMethods);
    mContext.addInterface(mCalculatorInterface);
    mCalculatorInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    mObjectInterface = Interface::newInterface(objectFullName,
                                               objectInterfaceStructType,
                                               parentInterfaces,
                                               interfaceMethods);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    string scienceCalculatorFullName = "systems.vos.wisey.compiler.tests.IScienceCalculator";
    StructType* scienceCalculatorIinterfaceStructType =
    StructType::create(mLLVMContext, scienceCalculatorFullName);
    vector<InterfaceTypeSpecifier*> scienceCalculatorParentInterfaces;
    vector<MethodSignatureDeclaration*> scienceCalculatorInterfaceMethods;
    vector<string> package;
    InterfaceTypeSpecifier* calculatorTypeSpecifier = new InterfaceTypeSpecifier(package,
                                                                                 "ICalculator");
    scienceCalculatorParentInterfaces.push_back(calculatorTypeSpecifier);
    mScienceCalculatorInterface = Interface::newInterface(scienceCalculatorFullName,
                                                          scienceCalculatorIinterfaceStructType,
                                                          scienceCalculatorParentInterfaces,
                                                          scienceCalculatorInterfaceMethods);
    mContext.addInterface(mScienceCalculatorInterface);
    mScienceCalculatorInterface->buildMethods(mContext);

    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);

    string multiplierFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType* structType = StructType::create(mLLVMContext, multiplierFullName);
    mMultiplierController = Controller::newController(multiplierFullName, structType);
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
  EXPECT_EQ(mMultiplierController->getOwner()->getObject(), mMultiplierController);
}

TEST_F(ControllerOwnerTest, getNameTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getName(),
            "systems.vos.wisey.compiler.tests.CMultiplier*");
}

TEST_F(ControllerOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getLLVMType(mLLVMContext),
            mMultiplierController->getLLVMType(mLLVMContext)->getPointerElementType());
}

TEST_F(ControllerOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mMultiplierController->getOwner()->getTypeKind(), CONTROLLER_OWNER_TYPE);
}

TEST_F(ControllerOwnerTest, canCastTest) {
  const IObjectOwnerType* controllerOwner = mMultiplierController->getOwner();

  EXPECT_FALSE(controllerOwner->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(controllerOwner->canCastTo(mAdditorController));
  EXPECT_FALSE(controllerOwner->canCastTo(mVehicleInterface));
  EXPECT_TRUE(controllerOwner->canCastTo(mMultiplierController));
  EXPECT_TRUE(controllerOwner->canCastTo(mCalculatorInterface));

  EXPECT_FALSE(controllerOwner->canCastTo(mAdditorController->getOwner()));
  EXPECT_FALSE(controllerOwner->canCastTo(mVehicleInterface->getOwner()));
  EXPECT_TRUE(controllerOwner->canCastTo(mMultiplierController->getOwner()));
  EXPECT_TRUE(controllerOwner->canCastTo(mCalculatorInterface->getOwner()));
}

TEST_F(ControllerOwnerTest, canAutoCastTest) {
  const IObjectOwnerType* controllerOwner = mMultiplierController->getOwner();
  
  EXPECT_FALSE(controllerOwner->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mAdditorController));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mVehicleInterface));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mMultiplierController));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mCalculatorInterface));
  
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mAdditorController->getOwner()));
  EXPECT_FALSE(controllerOwner->canAutoCastTo(mVehicleInterface->getOwner()));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mMultiplierController->getOwner()));
  EXPECT_TRUE(controllerOwner->canAutoCastTo(mCalculatorInterface->getOwner()));
}

TEST_F(ControllerOwnerTest, castToItselfTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mLLVMContext));
  Value* result = mMultiplierController->getOwner()->castTo(mContext,
                                                            pointer,
                                                            mMultiplierController->getOwner());
  
  EXPECT_EQ(result, pointer);
}

TEST_F(ControllerOwnerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mMultiplierController->getLLVMType(mLLVMContext));
  mMultiplierController->getOwner()->castTo(mContext,
                                            pointer,
                                            mScienceCalculatorInterface->getOwner());
  EXPECT_EQ(mBasicBlock->size(), 1u);
  
  *mStringStream << *mBasicBlock->begin();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier** null "
               "to %systems.vos.wisey.compiler.tests.IScienceCalculator**");
  mStringBuffer.clear();
}

TEST_F(ControllerOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mLLVMContext));
  mMultiplierController->getOwner()->castTo(mContext,
                                            pointer,
                                            mCalculatorInterface->getOwner());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.CMultiplier*, "
  "%systems.vos.wisey.compiler.tests.CMultiplier** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 8"
  "\n  %3 = alloca %systems.vos.wisey.compiler.tests.ICalculator*"
  "\n  %4 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.ICalculator*"
  "\n  store %systems.vos.wisey.compiler.tests.ICalculator* %4, "
  "%systems.vos.wisey.compiler.tests.ICalculator** %3\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, controllerPassModelReferenceRunTest) {
  runFile("tests/samples/test_controller_pass_model_reference.yz", "3");
}

