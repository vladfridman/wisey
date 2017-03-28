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
  map<string, Field*> fields;
  fields["left"] = new Field(PrimitiveTypes::INT_TYPE, 0);
  fields["right"] = new Field(PrimitiveTypes::INT_TYPE, 1);
  vector<MethodArgument*> methodArguments;
  vector<Method*> methods;
  methods.push_back(new Method("multiply", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL));
  vector<Interface*> interfaces;
  Controller* controller = new Controller("CMultiplier", structType, fields, methods, interfaces);
  context.addController(controller);
  
  ControllerTypeSpecifier controllerTypeSpecifier("CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getName(), "CMultiplier");
  EXPECT_EQ(controllerTypeSpecifier.getType(context), controller);
}
