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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "Injector.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"
#include "StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
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
  InjectionArgumentList mInjectionArgumentList;
  
  InjectorTest() {
    TestPrefix::generateIR(mContext);
    
    LLVMContext& llvmContext = mContext.getLLVMContext();

    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier(NULL, "IMyInterface", 0);
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType *controllerStructType = StructType::create(llvmContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<IField*> controllerFields;
    controllerFields.push_back(new StateField(PrimitiveTypes::INT, "left", 0));
    controllerFields.push_back(new StateField(PrimitiveTypes::INT, "right", 0));
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType,
                                            mContext.getImportProfile(),
                                            0);
    mController->setFields(mContext, controllerFields, 1u);
    mContext.addController(mController, 0);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(llvmContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements,
                                         mContext.getImportProfile(),
                                         0);
    mContext.addInterface(mInterface, 0);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);
    mController->setInterfaces(interfaces);
    mContext.bindInterfaceToController(mInterface->getTypeName(), mController->getTypeName(), 0);
   
    IConcreteObjectType::declareTypeNameGlobal(mContext, mController);
    IConcreteObjectType::declareVTable(mContext, mController);
    
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

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "1";
  }
};

TEST_F(InjectorTest, getTypeTest) {
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);
  
  EXPECT_EQ(injector.getType(mContext), mInterface->getOwner());
}

TEST_F(InjectorTest, isConstantTest) {
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);

  EXPECT_FALSE(injector.isConstant());
}

TEST_F(InjectorTest, isAssignableTest) {
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);

  EXPECT_FALSE(injector.isAssignable());
}

TEST_F(InjectorTest, printToStreamTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  mInjectionArgumentList.push_back(new InjectionArgument("withFoo", mockExpression));
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);

  stringstream stringStream;
  injector.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("injector(systems.vos.wisey.compiler.tests.IMyInterface).withFoo(1).inject()",
               stringStream.str().c_str());
}

TEST_F(TestFileRunner, inlineControllerInjectionRunTest) {
  runFile("tests/samples/test_inline_controller_injection.yz", 5);
}

TEST_F(TestFileRunner, injectControllerReferenceInlineRunTest) {
  runFile("tests/samples/test_inject_controller_reference_inline.yz", 3);
}

TEST_F(TestFileRunner, inlineInterfaceInjectionRunTest) {
  runFile("tests/samples/test_inline_interface_injection.yz", 7);
}
