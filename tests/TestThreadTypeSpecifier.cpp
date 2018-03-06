//
//  TestThreadTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThreadTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/FakeExpression.hpp"
#include "wisey/Method.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct ThreadTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Thread* mThread;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ThreadTypeSpecifierTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string threadFullName = "systems.vos.wisey.compiler.tests.TWorker";
    StructType* structType = StructType::create(llvmContext, threadFullName);
    structType->setBody(types);
    mThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS, threadFullName, structType);
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    Field* field = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mFrom", fieldArguments);
    fields.push_back(field);
    field = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mTo", fieldArguments);
    fields.push_back(field);
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* fooMethod = new Method(mThread,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    mThread->setFields(fields, 1u);
    mThread->setMethods(methods);
    mContext.addThread(mThread);
  }
};

TEST_F(ThreadTypeSpecifierTest, threadTypeSpecifierCreateTest) {
  vector<string> package;
  ThreadTypeSpecifier threadTypeSpecifier(NULL, "TWorker");
  
  EXPECT_EQ(threadTypeSpecifier.getType(mContext), mThread);
}

TEST_F(ThreadTypeSpecifierTest, threadTypeSpecifierCreateWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifier threadTypeSpecifier(packageExpression, "TWorker");
  
  EXPECT_EQ(threadTypeSpecifier.getType(mContext), mThread);
}

TEST_F(ThreadTypeSpecifierTest, printToStreamTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifier threadTypeSpecifier(packageExpression, "TWorker");
  
  stringstream stringStream;
  threadTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.TWorker", stringStream.str().c_str());
}

TEST_F(ThreadTypeSpecifierTest, threadTypeSpecifierSamePackageDeathTest) {
  ThreadTypeSpecifier threadTypeSpecifier(NULL, "TThread");
  
  EXPECT_EXIT(threadTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Thread systems.vos.wisey.compiler.tests.TThread is not defined");
}

TEST_F(ThreadTypeSpecifierTest, threadTypeSpecifierNotDefinedDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifier threadTypeSpecifier(packageExpression, "TThread");
  
  EXPECT_EXIT(threadTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Thread systems.vos.wisey.compiler.tests.TThread is not defined");
}

