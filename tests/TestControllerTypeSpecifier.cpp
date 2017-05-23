//
//  TestControllerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TEST(ControllerTypeSpecifierTest, creationTest) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(llvmContext));
  types.push_back(Type::getInt32Ty(llvmContext));
  string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
  StructType* structType = StructType::create(llvmContext, "CMultiplier");
  structType->setBody(types);
  vector<Field*> receivedFields;
  vector<Field*> injectedFields;
  vector<Field*> stateFields;
  ExpressionList fieldArguments;
  receivedFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "left", 0, fieldArguments));
  receivedFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "right", 1, fieldArguments));
  vector<MethodArgument*> methodArguments;
  vector<Method*> methods;
  vector<IType*> thrownExceptions;
  Method* multiplyMethod = new Method("multiply",
                                      AccessLevel::PUBLIC_ACCESS,
                                      PrimitiveTypes::INT_TYPE,
                                      methodArguments,
                                      thrownExceptions,
                                      NULL,
                                      0);
  methods.push_back(multiplyMethod);
  vector<Interface*> interfaces;
  Controller* controller = new Controller("CMultiplier",
                                          controllerFullName,
                                          structType,
                                          receivedFields,
                                          injectedFields,
                                          stateFields,
                                          methods,
                                          interfaces);
  context.addController(controller);
  
  vector<string> package;
  ControllerTypeSpecifier controllerTypeSpecifier(package, "CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getType(context), controller);
}
