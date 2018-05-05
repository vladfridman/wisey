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

#include "TestPrefix.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/Method.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct ModelTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Model* mModel;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  ModelTypeSpecifierTest() {
    TestPrefix::generateIR(mContext);
    
    LLVMContext& llvmContext = mContext.getLLVMContext();

    vector<Type*> types;
     types.push_back(FunctionType::get(Type::getInt32Ty(llvmContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT, "height", 0));
    vector<const wisey::Argument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* fooMethod = new Method(mModel,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    mModel->setFields(mContext, fields, 1u);
    mModel->setMethods(methods);
    mContext.addModel(mModel, 0);
  }
};

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierCreateTest) {
  vector<string> package;
  ModelTypeSpecifier modelTypeSpecifier(NULL, "MSquare", 0);
  
  EXPECT_EQ(modelTypeSpecifier.getType(mContext), mModel);
}

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierCreateWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier modelTypeSpecifier(packageExpression, "MSquare", 0);
  
  EXPECT_EQ(modelTypeSpecifier.getType(mContext), mModel);
}

TEST_F(ModelTypeSpecifierTest, printToStreamTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier modelTypeSpecifier(packageExpression, "MSquare", 0);

  stringstream stringStream;
  modelTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MSquare", stringStream.str().c_str());
}

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierSamePackageDeathTest) {
  ModelTypeSpecifier modelTypeSpecifier(NULL, "MCircle", 10);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(modelTypeSpecifier.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(10): Error: Model systems.vos.wisey.compiler.tests.MCircle is not defined\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ModelTypeSpecifierTest, modelTypeSpecifierNotDefinedDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifier modelTypeSpecifier(packageExpression, "MCircle", 15);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(modelTypeSpecifier.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(15): Error: Model systems.vos.wisey.compiler.tests.MCircle is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
