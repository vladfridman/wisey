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

#include "wisey/Composer.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ComposerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Model* mModel;
  Function* mMainFunction;

public:
  
  ComposerTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = Model::newModel(modelFullName, modelStructType);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

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

