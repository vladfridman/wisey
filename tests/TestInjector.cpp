//
//  TestInjector.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Injector}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Injector.hpp"
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

struct InjectorTest : Test {
  IRGenerationContext mContext;
  Controller* mController;
  Interface* mInterface;
  NiceMock<MockExpression> mFieldValue1;
  NiceMock<MockExpression> mFieldValue2;
  NiceMock<MockExpression> mFieldValue3;
  InterfaceTypeSpecifier* mInterfaceTypeSpecifier;
  BasicBlock *mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  InjectorTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier("", "IMyInterface");
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType *controllerStructType = StructType::create(llvmContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<Field*> controllerFields;
    ExpressionList arguments;
    Field* fieldLeft = new Field(STATE_FIELD, PrimitiveTypes::INT_TYPE, "left", arguments);
    Field* fieldRight = new Field(STATE_FIELD, PrimitiveTypes::INT_TYPE, "right", arguments);
     controllerFields.push_back(fieldLeft);
    controllerFields.push_back(fieldRight);
    mController = Controller::newController(controllerFullName, controllerStructType);
    mController->setFields(controllerFields, 1u);
    mContext.addController(mController);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(llvmContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface(interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    mContext.addInterface(mInterface);
    mContext.bindInterfaceToController(mInterface, mController);
   
    IConcreteObjectType::generateNameGlobal(mContext, mController);
    IConcreteObjectType::generateVTable(mContext, mController);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InjectorTest() {
    delete mStringStream;
  }
};

TEST_F(InjectorTest, getVariableTest) {
  Injector injector(mInterfaceTypeSpecifier, 0);
  
  EXPECT_EQ(injector.getVariable(mContext), nullptr);
}

TEST_F(InjectorTest, getTypeTest) {
  Injector injector(mInterfaceTypeSpecifier, 0);
  
  EXPECT_EQ(injector.getType(mContext), mController->getOwner());
}

TEST_F(InjectorTest, isConstantTest) {
  Injector injector(mInterfaceTypeSpecifier, 0);

  EXPECT_FALSE(injector.isConstant());
}

TEST_F(InjectorTest, printToStreamTest) {
  Injector injector(mInterfaceTypeSpecifier, 0);

  stringstream stringStream;
  injector.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("inject(systems.vos.wisey.compiler.tests.IMyInterface)", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, inlineControllerInjectionRunTest) {
  runFile("tests/samples/test_inline_controller_injection.yz", "5");
}

TEST_F(TestFileSampleRunner, inlineInterfaceInjectionRunTest) {
  runFile("tests/samples/test_inline_interface_injection.yz", "7");
}
