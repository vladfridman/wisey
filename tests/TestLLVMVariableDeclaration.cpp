//
//  TestLLVMVariableDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMVariableDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "Identifier.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPointerTypeSpecifier.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "LLVMPrimitiveTypeSpecifier.hpp"
#include "LLVMVariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMVariableDeclarationTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  Identifier* mIdentifier;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  LLVMVariableDeclarationTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIdentifier(new Identifier("foo", 0)) {
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LLVMVariableDeclarationTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMVariableDeclarationTest, stackLLVMVariableDeclarationWithoutAssignmentTest) {
  const ILLVMTypeSpecifier* typeSpecifier = LLVMPrimitiveTypes::I8->newTypeSpecifier(0);
  const LLVMPointerTypeSpecifier* pointerSpecifier = new LLVMPointerTypeSpecifier(typeSpecifier, 0);
  LLVMVariableDeclaration* declaration =
  LLVMVariableDeclaration::create(pointerSpecifier, mIdentifier, 0);
  
  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca i8*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i8* null, i8** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}
