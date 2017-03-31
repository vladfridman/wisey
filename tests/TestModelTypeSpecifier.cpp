//
//  TestModelTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(ModelTypeSpecifierTest, creationTest) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(llvmContext));
  types.push_back(Type::getInt32Ty(llvmContext));
  StructType* structType = StructType::create(llvmContext, "MSquare");
  structType->setBody(types);
  map<string, Field*> fields;
  ExpressionList fieldArguments;
  fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
  fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
  vector<MethodArgument*> methodArguments;
  vector<Method*> methods;
  methods.push_back(new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL));
  vector<Interface*> interfaces;
  Model* model = new Model("MSquare", structType, fields, methods, interfaces);
  context.addModel(model);

  ModelTypeSpecifier modelTypeSpecifier("MSquare");
  
  EXPECT_EQ(modelTypeSpecifier.getName(), "MSquare");
  EXPECT_EQ(modelTypeSpecifier.getType(context), model);
}
