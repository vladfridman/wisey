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
#include "wisey/Names.hpp"
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
  string mMethodName;
  NiceMock<MockReferenceVariable>* mThreadVariable;
  NiceMock<MockReferenceVariable>* mCallStackVariable;
  ImportProfile* mImportProfile;
  string mPackage = "systems.vos.wisey.compiler.tests";

public:
  
  ComposerTest() : mLLVMContext(mContext.getLLVMContext()), mMethodName("foo") {
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

    stringConstant = ConstantDataArray::getString(mLLVMContext, mMethodName);
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       IMethodCall::getMethodNameConstantName(mMethodName));
    IConcreteObjectType::defineCurrentObjectNameVariable(mContext, mModel);
    IMethod::defineCurrentMethodNameVariable(mContext, mMethodName);

    mContext.getScopes().pushScope();

    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockReferenceVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_, _)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mContext, mThreadVariable);

    Controller* callStack = mContext.getController(Names::getCallStackControllerFullName(), 0);
    Value* callStackValue = ConstantPointerNull::get(callStack->getLLVMType(mContext));
    mCallStackVariable = new NiceMock<MockReferenceVariable>();
    ON_CALL(*mCallStackVariable, getName()).WillByDefault(Return(ThreadExpression::CALL_STACK));
    ON_CALL(*mCallStackVariable, getType()).WillByDefault(Return(callStack));
    ON_CALL(*mCallStackVariable, generateIdentifierIR(_, _)).WillByDefault(Return(callStackValue));
    mContext.getScopes().setVariable(mContext, mCallStackVariable);

    mContext.setObjectType(mModel);
    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ComposerTest() {
    delete mStringStream;
    delete mThreadVariable;
    delete mCallStackVariable;
  }
};

TEST_F(ComposerTest, pushCallStackTest) {
  Composer::pushCallStack(mContext, 5);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.threads.CCallStack.pushStack(%wisey.threads.CCallStack* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* getelementptr inbounds ([42 x i8], [42 x i8]* @systems.vos.wisey.compiler.tests.MMyModel.typename, i32 0, i32 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @methodname.foo, i32 0, i32 0), i8* getelementptr inbounds ([8 x i8], [8 x i8]* @sourcefile.test.yz, i32 0, i32 0), i32 5)\n";
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
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i32 5)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}
