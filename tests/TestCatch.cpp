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

#include "MockStatement.hpp"
#include "wisey/Catch.hpp"
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
  NiceMock<MockStatement>* mMockStatement;
  Model* mModel;
  Catch* mCatch;
  
public:
  
  CatchTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockStatement(new NiceMock<MockStatement>()) {
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList arguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", 0, arguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", 1, arguments));
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* fooMethod = new Method("foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL);
    methods.push_back(fooMethod);
    mModel = new Model(modelFullName, structType);
    mModel->setMethods(methods);
    mModel->setFields(fields);
    mContext.addModel(mModel);

    vector<string> package;
    ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier(package, "MSquare");
    mCatch = new Catch(typeSpecifier, "mycatch", mMockStatement);
  }

  ~CatchTest() {
    delete mCatch;
  }
};

TEST_F(CatchTest, getTypeTest) {
  EXPECT_EQ(mCatch->getType(mContext), (ModelOwner*) mModel->getOwner());
}
