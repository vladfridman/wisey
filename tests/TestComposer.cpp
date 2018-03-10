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
#include "wisey/ProgramPrefix.hpp"
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
  ImportProfile* mImportProfile;
  string mPackage = "systems.vos.wisey.compiler.tests";

public:
  
  ComposerTest() : mLLVMContext(mContext.getLLVMContext()), mMethodName("foo") {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, modelStructType);
    mModel->setImportProfile(mImportProfile);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    string sourceFile = "test.yz";
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, sourceFile);
    GlobalVariable* global = new GlobalVariable(*mContext.getModule(),
                                                stringConstant->getType(),
                                                true,
                                                GlobalValue::InternalLinkage,
                                                stringConstant,
                                                ProgramFile::getSourceFileConstantName(sourceFile));
    ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 0);
    Value* Idx[2];
    Idx[0] = zeroInt32;
    Idx[1] = zeroInt32;
    Type* elementType = global->getType()->getPointerElementType();
    mContext.getImportProfile()->
    setSourceFileNamePointer(ConstantExpr::getGetElementPtr(elementType, global, Idx));

    stringConstant = ConstantDataArray::getString(mLLVMContext, mModel->getTypeName());
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

    Thread* mainThread = mContext.getThread(Names::getMainThreadFullName());
    Value* threadObject = ConstantPointerNull::get(mainThread->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockReferenceVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(mainThread));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().pushScope();
    mContext.getScopes().setVariable(mThreadVariable);
    mContext.setObjectType(mModel);

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ComposerTest() {
    delete mStringStream;
    delete mThreadVariable;
  }
};

TEST_F(ComposerTest, pushCallStackTest) {
  Composer::pushCallStack(mContext, 5);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.lang.TMainThread.pushStack("
    "%wisey.lang.TMainThread* null, "
    "%wisey.lang.TMainThread* null, "
    "i8* getelementptr inbounds ([42 x i8], [42 x i8]* "
    "@systems.vos.wisey.compiler.tests.MMyModel.name, i32 0, i32 0), "
    "i8* getelementptr inbounds ([4 x i8], [4 x i8]* @methodname.foo, i32 0, i32 0), "
    "i8* getelementptr inbounds ([8 x i8], [8 x i8]* @sourcefile.test.yz, i32 0, i32 0), "
    "i32 5)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, popCallStackTest) {
  Composer::popCallStack(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.lang.TMainThread.popStack("
  "%wisey.lang.TMainThread* null, "
  "%wisey.lang.TMainThread* null)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}
