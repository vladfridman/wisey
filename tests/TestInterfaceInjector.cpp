//
//  TestInterfaceInjector.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceInjector}
//

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "wisey/InterfaceInjector.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InterfaceInjectorTest : Test {
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
  
  InterfaceInjectorTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mContext.setPackage("systems.vos.wisey.compiler.tests");

    vector<string> package;
    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier(package, "IMyInterface");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType *controllerStructType = StructType::create(llvmContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<Field*> controllerFields;
    ExpressionList arguments;
    Field* fieldLeft = new Field(STATE_FIELD, PrimitiveTypes::INT_TYPE, "left", 0, arguments);
    Field* fieldRight = new Field(STATE_FIELD, PrimitiveTypes::INT_TYPE, "right", 1, arguments);
    controllerFields.push_back(fieldLeft);
    controllerFields.push_back(fieldRight);
    mController = Controller::newController(controllerFullName, controllerStructType);
    mController->setFields(controllerFields);
    mContext.addController(mController);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(llvmContext, interfaceFullName);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mInterface = Interface::newInterface(interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceMethods);
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
  
  ~InterfaceInjectorTest() {
    delete mStringStream;
  }
};

TEST_F(InterfaceInjectorTest, getVariableTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  
  EXPECT_EQ(interfaceInjector.getVariable(mContext), nullptr);
}

TEST_F(InterfaceInjectorTest, releaseOwnershipTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  interfaceInjector.generateIR(mContext);
  
  string temporaryVariableName = IVariable::getTemporaryVariableName(&interfaceInjector);
  
  EXPECT_NE(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
  
  interfaceInjector.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
}

TEST_F(InterfaceInjectorTest, addReferenceToOwnerDeathTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  interfaceInjector.generateIR(mContext);
  
  NiceMock<MockVariable> referenceVariable;
  ON_CALL(referenceVariable, getName()).WillByDefault(Return("bar"));
  ON_CALL(referenceVariable, getType()).WillByDefault(Return(mInterface));

  string temporaryVariableName = IVariable::getTemporaryVariableName(&interfaceInjector);

  interfaceInjector.addReferenceToOwner(mContext, &referenceVariable);
  
  map<string, IVariable*> owners = mContext.getScopes().getOwnersForReference(&referenceVariable);
  EXPECT_EQ(owners.size(), 1u);
  EXPECT_EQ(owners.begin()->second, mContext.getScopes().getVariable(temporaryVariableName));
}

TEST_F(InterfaceInjectorTest, getTypeTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  
  EXPECT_EQ(interfaceInjector.getType(mContext), mController->getOwner());
}

TEST_F(InterfaceInjectorTest, existsInOuterScopeTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  interfaceInjector.generateIR(mContext);
  
  EXPECT_FALSE(interfaceInjector.existsInOuterScope(mContext));
}

TEST_F(InterfaceInjectorTest, printToStreamTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);

  stringstream stringStream;
  interfaceInjector.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("inject(systems.vos.wisey.compiler.tests.IMyInterface)", stringStream.str().c_str());
}
