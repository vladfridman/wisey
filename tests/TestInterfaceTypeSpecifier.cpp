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

#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(InterfaceTypeSpecifierTest, creationTest) {
  IRGenerationContext context;
  vector<Type*> types;
  StructType* structType = StructType::create(context.getLLVMContext(), "IShape");
  structType->setBody(types);
  vector<MethodArgument*> methodArguments;
  vector<MethodSignature*> methodSignatures;
  vector<IType*> methodExceptions;
  MethodSignature* methodSignature = new MethodSignature("foo",
                                                         AccessLevel::PUBLIC_ACCESS,
                                                         PrimitiveTypes::INT_TYPE,
                                                         methodArguments,
                                                         methodExceptions,
                                                         0);
  methodSignatures.push_back(methodSignature);
  vector<Interface*> parentInterfaces;
  Interface* interface = new Interface("IShape", structType, parentInterfaces, methodSignatures);
  context.addInterface(interface);
  
  InterfaceTypeSpecifier interfaceTypeSpecifier("IShape");
  
  EXPECT_EQ(interfaceTypeSpecifier.getName(), "IShape");
  EXPECT_EQ(interfaceTypeSpecifier.getType(context), interface);
}
