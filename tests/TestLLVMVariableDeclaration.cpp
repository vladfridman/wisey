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
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMVariableDeclaration.hpp"

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
  BasicBlock* mBasicBlock;
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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    
    mContext.setBasicBlock(mBasicBlock);
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
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca i8*"
  "\n  store i8* null, i8** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}
