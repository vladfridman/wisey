//
//  TestCatch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Catch}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockStatement.hpp"
#include "yazyk/Catch.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct CatchTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockStatement> mMockStatement;
  Model* mModel;
  Catch* mCatch;
  
public:
  
  CatchTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    StructType* structType = StructType::create(mLLVMContext, "MSquare");
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
    mModel = new Model("MSquare", structType, fields, methods, interfaces);
    mContext.addModel(mModel);

    ModelTypeSpecifier typeSpecifier("MSquare");
    mCatch = new Catch(typeSpecifier, "mycatch", mMockStatement);
  }

  ~CatchTest() {
    delete mCatch;
  }
};

TEST_F(CatchTest, getTypeTest) {
  EXPECT_EQ(mCatch->getType(mContext), mModel);
}
