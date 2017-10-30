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
  
  InjectorTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mContext.setPackage("systems.vos.wisey.compiler.tests");

    vector<string> package;
    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier(package, "IMyInterface");
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
    vector<InterfaceTypeSpecifier*> parentInterfaces;
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
  Injector injector(mInterfaceTypeSpecifier);
  
  EXPECT_EQ(injector.getVariable(mContext), nullptr);
}

TEST_F(InjectorTest, releaseOwnershipTest) {
  Injector injector(mInterfaceTypeSpecifier);
  injector.generateIR(mContext);
  
  string temporaryVariableName = IVariable::getTemporaryVariableName(&injector);
  
  EXPECT_NE(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
  
  injector.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
}

TEST_F(InjectorTest, addReferenceToOwnerDeathTest) {
  Injector injector(mInterfaceTypeSpecifier);
  injector.generateIR(mContext);
  
  NiceMock<MockVariable> referenceVariable;
  ON_CALL(referenceVariable, getName()).WillByDefault(Return("bar"));
  ON_CALL(referenceVariable, getType()).WillByDefault(Return(mInterface));

  string temporaryVariableName = IVariable::getTemporaryVariableName(&injector);

  injector.addReferenceToOwner(mContext, &referenceVariable);
  
  map<string, IVariable*> owners = mContext.getScopes().getOwnersForReference(&referenceVariable);
  EXPECT_EQ(owners.size(), 1u);
  EXPECT_EQ(owners.begin()->second, mContext.getScopes().getVariable(temporaryVariableName));
}

TEST_F(InjectorTest, getTypeTest) {
  Injector injector(mInterfaceTypeSpecifier);
  
  EXPECT_EQ(injector.getType(mContext), mController->getOwner());
}

TEST_F(InjectorTest, existsInOuterScopeTest) {
  Injector injector(mInterfaceTypeSpecifier);
  injector.generateIR(mContext);
  
  EXPECT_FALSE(injector.existsInOuterScope(mContext));
}

TEST_F(InjectorTest, isConstantTest) {
  Injector injector(mInterfaceTypeSpecifier);

  EXPECT_FALSE(injector.isConstant());
}

TEST_F(InjectorTest, printToStreamTest) {
  Injector injector(mInterfaceTypeSpecifier);

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
