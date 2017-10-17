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

#include "TestPrefix.hpp"
#include "wisey/Composer.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/IMethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ProgramFile.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

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
  Controller* mThreadController;
  Value* mThreadObject;

public:
  
  ComposerTest() : mLLVMContext(mContext.getLLVMContext()), mMethodName("foo") {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::run(mContext);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = Model::newModel(modelFullName, modelStructType);

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
    Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, sourceFile);
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
    mContext.setSourceFileNamePointer(ConstantExpr::getGetElementPtr(elementType, global, Idx));

    stringConstant = ConstantDataArray::getString(mLLVMContext, mModel->getName());
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

    mThreadController = mContext.getController(Names::getThreadControllerFullName());
    mThreadObject = ConstantPointerNull::get(mThreadController->getLLVMType(mLLVMContext));

    mStringStream = new raw_string_ostream(mStringBuffer);
}
  
  ~ComposerTest() {
    delete mStringStream;
  }
};

TEST_F(ComposerTest, checkNullAndThrowNPETest) {
  BasicBlock* landingPadBlock = BasicBlock::Create(mLLVMContext,
                                                   "cleanup.landing.pad",
                                                   mMainFunction);
  FinallyBlock* finallyBlock = new FinallyBlock();
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(landingPadBlock, NULL, finallyBlock, catchList);
  mContext.getScopes().setTryCatchInfo(tryCatchInfo);

  Value* value = ConstantPointerNull::get((PointerType*) mModel->getLLVMType(mLLVMContext)
                                          ->getPointerElementType());
  
  Composer::checkNullAndThrowNPE(mContext, value);

  *mStringStream << *mMainFunction;
  string expected =
  "\ndefine internal i32 @main() {"
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MMyModel* null to i8*"
  "\n  invoke void @__checkForNullAndThrow(i8* %0)"
  "\n          to label %invoke.continue unwind label %cleanup.landing.pad"
  "\n"
  "\ncleanup.landing.pad:                              ; preds = %entry"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n}\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());

  mStringBuffer.clear();
}

TEST_F(ComposerTest, pushCallStackTest) {
  Composer::pushCallStack(mContext, mThreadObject, mModel, 5);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.lang.CThread.pushStack("
  "%wisey.lang.CThread** null, "
  "%wisey.lang.CThread** null, "
  "i8* getelementptr inbounds ([8 x i8], [8 x i8]* @sourcefile.test.yz, i32 0, i32 0), "
  "i32 5)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, setNextOnCallStackTest) {
  Value* modelObject = ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  Composer::setNextOnCallStack(mContext, mThreadObject, mModel, modelObject, mMethodName);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.lang.CThread.setObjectAndMethod("
  "%wisey.lang.CThread** null, "
  "%wisey.lang.CThread** null, "
  "i8* getelementptr inbounds ([42 x i8], [42 x i8]* "
  "@systems.vos.wisey.compiler.tests.MMyModel.name, i32 0, i32 0), "
  "i8* getelementptr inbounds ([4 x i8], [4 x i8]* @methodname.foo, i32 0, i32 0))\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}

TEST_F(ComposerTest, popCallStackTest) {
  Composer::popCallStack(mContext, mThreadObject, mModel);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @wisey.lang.CThread.popStack("
  "%wisey.lang.CThread** null, "
  "%wisey.lang.CThread** null)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  
  mStringBuffer.clear();
}
