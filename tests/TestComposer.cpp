//
//  TestComposer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Composer}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockReferenceVariable.hpp"
#include "TestPrefix.hpp"
#include "wisey/Composer.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/Method.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramFile.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ComposerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Model* mModel;
  Function* mMainFunction;
  ImportProfile* mImportProfile;
  string mPackage = "systems.vos.wisey.compiler.tests";

public:
  
  ComposerTest() : mLLVMContext(mContext.getLLVMContext()){
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             modelStructType,
                             mContext.getImportProfile(),
                             0);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mContext.getImportProfile()->setSourceFileName(mContext, "test.yz");

    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mModel->getTypeName());
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       mModel->getObjectNameGlobalVariableName());

    mContext.getScopes().pushScope();

    mContext.setObjectType(mModel);
    vector<const wisey::Argument*> arguments;
    vector<const Model*> exceptions;
    Method* method = new Method(mModel,
                                "foo",
                                PUBLIC_ACCESS,
                                PrimitiveTypes::VOID,
                                arguments,
                                exceptions,
                                NULL,
                                NULL,
                                5);
    mContext.setCurrentMethod(method);
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ComposerTest() {
    delete mStringStream;
  }
};

TEST_F(ComposerTest, pushCallStackTest) {
  Composer::pushCallStack(mContext, 5);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.threads.CCallStack.pushStack(%wisey.threads.CCallStack* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* getelementptr inbounds ([54 x i8], [54 x i8]* @\"systems.vos.wisey.compiler.tests.MMyModel.foo(test.yz\", i32 0, i32 0), i32 5)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, popCallStackTest) {
  Composer::popCallStack(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.threads.CCallStack.popStack(%wisey.threads.CCallStack* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, setLineNumberTestTest) {
  Composer::setLineNumber(mContext, 5);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %wisey.threads.CCallStack* null to %CCallStack*"
  "\n  %1 = getelementptr %CCallStack, %CCallStack* %0, i32 0, i32 3"
  "\n  %2 = load i32, i32* %1"
  "\n  %3 = sub i32 %2, 1"
  "\n  %4 = getelementptr %CCallStack, %CCallStack* %0, i32 0, i32 2"
  "\n  %5 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %4"
  "\n  %6 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* %5, i32 0, i32 3"
  "\n  %7 = getelementptr [0 x i32], [0 x i32]* %6, i32 0, i32 %3"
  "\n  store i32 5, i32* %7\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}
