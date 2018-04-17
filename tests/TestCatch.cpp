//
//  TestCatch.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Catch}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/Catch.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/Method.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct CatchTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  Catch* mCatch;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
public:
  
  CatchTest() : mLLVMContext(mContext.getLLVMContext()) {
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height", 0));
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* fooMethod = new Method(mModel,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType, 0);
    mModel->setMethods(methods);
    mModel->setFields(fields, 0u);
    mContext.addModel(mModel);

    ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier(NULL, "MSquare", 0);
    Block* block = new Block();
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    mCatch = new Catch(typeSpecifier, "mycatch", compoundStatement, 0);
  }

  ~CatchTest() {
    delete mCatch;
  }
};

TEST_F(CatchTest, getTypeTest) {
  EXPECT_EQ(mCatch->getType(mContext), (const ModelOwner*) mModel->getOwner());
}
