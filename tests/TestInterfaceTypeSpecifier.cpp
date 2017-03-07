//
//  TestInterfaceTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/InterfaceTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(InterfaceTypeSpecifierTest, CreationTest) {
  IRGenerationContext context;
  vector<Type*> types;
  StructType* structType = StructType::create(context.getLLVMContext(), "IShape");
  structType->setBody(types);
  vector<MethodArgument*> methodArguments;
  map<string, Method*> methods;
  methods["foo"] = new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments);
  Interface* interface = new Interface("IShape", structType, methods);
  context.addInterface(interface);
  
  InterfaceTypeSpecifier interfaceTypeSpecifier("IShape");
  
  EXPECT_EQ(interfaceTypeSpecifier.getName(), "IShape");
  EXPECT_EQ(interfaceTypeSpecifier.getType(context), interface);
}
