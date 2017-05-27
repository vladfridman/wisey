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

struct ModelTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Model* mModel;
  vector<string> mPackage;

  ModelTypeSpecifierTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
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
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
    mModel->setMethods(methods);
    mContext.addModel(mModel);

    mPackage.push_back("systems");
    mPackage.push_back("vos");
    mPackage.push_back("wisey");
    mPackage.push_back("compiler");
    mPackage.push_back("tests");
  }
};

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierCreateTest) {
  vector<string> package;
  ModelTypeSpecifier modelTypeSpecifier(package, "MSquare");
  
  EXPECT_EQ(modelTypeSpecifier.getType(mContext), mModel);
}

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierCreateWithPackageTest) {
  ModelTypeSpecifier modelTypeSpecifier(mPackage, "MSquare");
  
  EXPECT_EQ(modelTypeSpecifier.getType(mContext), mModel);
}

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierNoImportDeathTest) {
  vector<string> package;
  ModelTypeSpecifier modelTypeSpecifier(package, "MCircle");
  
  EXPECT_EXIT(modelTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Could not find definition for MCircle");
}

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierNotDefinedDeathTest) {
  ModelTypeSpecifier modelTypeSpecifier(mPackage, "MCircle");
  
  EXPECT_EXIT(modelTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Model systems.vos.wisey.compiler.tests.MCircle is not defined");
}
