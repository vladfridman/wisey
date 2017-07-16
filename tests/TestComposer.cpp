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
    mModel = new Model(modelFullName, modelStructType);

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
  Value* value = ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
  
  Composer::checkNullAndThrowNPE(mContext, value, mModel);

  *mStringStream << *mMainFunction;
  string expected =
  "\ndefine internal i32 @main() {"
  "\nentry:"
  "\n  %cmp = icmp eq %systems.vos.wisey.compiler.tests.MMyModel* null, null"
  "\n  br i1 %cmp, label %if.then, label %if.end"
  "\n"
  "\nif.then:                                          ; preds = %entry"
  "\n  %malloccall = tail call i8* @malloc(i64 0)"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MNullPointerException*"
  "\n  %0 = alloca %wisey.lang.MNullPointerException*"
  "\n  store %wisey.lang.MNullPointerException* %buildervar, %wisey.lang.MNullPointerException** %0"
  "\n  %1 = bitcast %wisey.lang.MNullPointerException* %buildervar to i8*"
  "\n  %2 = bitcast { i8*, i8* }* @wisey.lang.MNullPointerException.rtti to i8*"
  "\n  %3 = getelementptr %wisey.lang.MNullPointerException, "
  "%wisey.lang.MNullPointerException* null, i32 1"
  "\n  %4 = ptrtoint %wisey.lang.MNullPointerException* %3 to i64"
  "\n  %5 = call i8* @__cxa_allocate_exception(i64 %4)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %1, i64 %4, i32 4, i1 false)"
  "\n  %variableObject = load %wisey.lang.MNullPointerException*, "
  "%wisey.lang.MNullPointerException** %0"
  "\n  %6 = bitcast %wisey.lang.MNullPointerException* %variableObject to i8*"
  "\n  tail call void @free(i8* %6)"
  "\n  call void @__cxa_throw(i8* %5, i8* %2, i8* null)"
  "\n  unreachable"
  "\n"
  "\nif.end:                                           ; preds = %entry"
  "\n}\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());

  mStringBuffer.clear();
}
