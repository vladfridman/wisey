//
//  TestModelTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TEST(ModelTypeSpecifierTest, creationTest) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(llvmContext));
  types.push_back(Type::getInt32Ty(llvmContext));
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(llvmContext, modelFullName);
  structType->setBody(types);
  map<string, Field*> fields;
  ExpressionList fieldArguments;
  fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
  fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
  vector<MethodArgument*> methodArguments;
  vector<Method*> methods;
  vector<IType*> thrownExceptions;
  Method* fooMethod = new Method("foo",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 thrownExceptions,
                                 NULL,
                                 0);
  methods.push_back(fooMethod);
  vector<Interface*> interfaces;
  Model* model = new Model("MSquare", modelFullName, structType, fields, methods, interfaces);
  context.addModel(model);

  ModelTypeSpecifier modelTypeSpecifier("MSquare");
  
  EXPECT_EQ(modelTypeSpecifier.getType(context), model);
}
