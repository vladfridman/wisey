//
//  TestControllerTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/ControllerTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(ControllerTypeSpecifierTest, creationTest) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(llvmContext));
  types.push_back(Type::getInt32Ty(llvmContext));
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
  methods.push_back(new Method("multiply", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL));
  vector<Interface*> interfaces;
  Controller* controller = new Controller("CMultiplier",
                                          structType,
                                          receivedFields,
                                          injectedFields,
                                          stateFields,
                                          methods,
                                          interfaces);
  context.addController(controller);
  
  ControllerTypeSpecifier controllerTypeSpecifier("CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getName(), "CMultiplier");
  EXPECT_EQ(controllerTypeSpecifier.getType(context), controller);
}
