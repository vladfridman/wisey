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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Injector.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

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
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier(NULL, "IMyInterface");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType *controllerStructType = StructType::create(llvmContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<IField*> controllerFields;
    controllerFields.push_back(new StateField(PrimitiveTypes::INT_TYPE, "left"));
    controllerFields.push_back(new StateField(PrimitiveTypes::INT_TYPE, "right"));
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType);
    mController->setFields(controllerFields, 1u);
    mContext.addController(mController);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(llvmContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    mContext.addInterface(mInterface);
    mContext.bindInterfaceToController(mInterface, mController);
   
    IConcreteObjectType::generateNameGlobal(mContext, mController);
    IConcreteObjectType::generateShortNameGlobal(mContext, mController);
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

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "1";
  }
};

TEST_F(InjectorTest, getVariableTest) {
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);
  vector<const IExpression*> arrayIndices;

  EXPECT_EQ(injector.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(InjectorTest, getTypeTest) {
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);
  
  EXPECT_EQ(injector.getType(mContext), mInterface->getOwner());
}

TEST_F(InjectorTest, isConstantTest) {
  Injector injector(mInterfaceTypeSpecifier, mInjectionArgumentList, 0);

  EXPECT_FALSE(injector.isConstant());
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
  runFile("tests/samples/test_inline_controller_injection.yz", "5");
}

TEST_F(TestFileRunner, inlineInterfaceInjectionRunTest) {
  runFile("tests/samples/test_inline_interface_injection.yz", "7");
}
