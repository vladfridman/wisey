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

    vector<string> package;
    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier(package, "IMyInterface");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType *controllerStructType = StructType::create(llvmContext, controllerFullName);
    controllerStructType->setBody(types);
    vector<Field*> controllerReceivedFields;
    vector<Field*> controllerInjectedFields;
    vector<Field*> controllerStateFields;
    ExpressionList fieldArguments;
    Field* fieldLeft = new Field(PrimitiveTypes::INT_TYPE, "left", 0, fieldArguments);
    Field* fieldRight = new Field(PrimitiveTypes::INT_TYPE, "right", 1, fieldArguments);
    controllerStateFields.push_back(fieldLeft);
    controllerStateFields.push_back(fieldRight);
    vector<Method*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = new Controller(controllerFullName,
                                 controllerStructType,
                                 controllerReceivedFields,
                                 controllerInjectedFields,
                                 controllerStateFields,
                                 controllerMethods,
                                 controllerInterfaces);
    mContext.addController(mController);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(llvmContext, interfaceFullName);
    vector<Interface*> interfaceParentInterfaces;
    vector<MethodSignature*> interfaceMethodSignatures;
    mInterface = new Interface(interfaceFullName,
                               interfaceStructType,
                               interfaceParentInterfaces,
                               interfaceMethodSignatures);
    mContext.addInterface(mInterface);
    mContext.bindInterfaceToController(mInterface, mController);
   
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

TEST_F(InterfaceInjectorTest, releaseOwnershipTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  interfaceInjector.generateIR(mContext);
  
  ostringstream stream;
  stream << "__tmp" << (long) &interfaceInjector;
  string temporaryVariableName = stream.str();
  
  EXPECT_NE(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
  
  interfaceInjector.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable(temporaryVariableName), nullptr);
}

TEST_F(InterfaceInjectorTest, getTypeTest) {
  InterfaceInjector interfaceInjector(mInterfaceTypeSpecifier);
  
  EXPECT_EQ(interfaceInjector.getType(mContext), mController);
}
