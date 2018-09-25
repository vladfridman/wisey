//
//  TestLLVMPointerOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPointerOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPointerOwnerType.hpp"
#include "LLVMPointerType.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMPointerOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  const LLVMPointerOwnerType* mLLVMPointerOwnerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMPointerOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = llvm::BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    StructType* structType = StructType::create(mLLVMContext, "mystruct");
    
    ON_CALL(mConcreteObjectType, isReference()).WillByDefault(Return(true));
    ON_CALL(mConcreteObjectType, isNative()).WillByDefault(Return(false));
    ON_CALL(mConcreteObjectType, getLLVMType(_)).WillByDefault(Return(structType->getPointerTo()));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I8);
    mLLVMPointerOwnerType = pointerType->getOwner();
  }
  
  ~LLVMPointerOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMPointerOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mLLVMPointerOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::i8::pointer*", mLLVMPointerOwnerType->getTypeName().c_str());
}

TEST_F(LLVMPointerOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  const LLVMPointerOwnerType* anotherPointerOwnerType =
  LLVMPointerType::create(LLVMPrimitiveTypes::I32)->getOwner();
  EXPECT_TRUE(mLLVMPointerOwnerType->canAutoCastTo(mContext, anotherPointerOwnerType));
  EXPECT_TRUE(mLLVMPointerOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMPointerOwnerTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMPointerOwnerType->canCastTo(mContext, anotherPointerType));
  EXPECT_TRUE(mLLVMPointerOwnerType->canCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMPointerOwnerTypeTest, castToTest) {
  LLVMPointerType* anotherPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mLLVMPointerOwnerType->getLLVMType(mContext));
  Value* result = mLLVMPointerOwnerType->castTo(mContext, value, anotherPointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerOwnerTypeTest, castToObjectTest) {
  Value* value = ConstantPointerNull::get(mLLVMPointerOwnerType->getLLVMType(mContext));
  Value* result = mLLVMPointerOwnerType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to %mystruct*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->isPrimitive());
  EXPECT_TRUE(mLLVMPointerOwnerType->isOwner());
  EXPECT_FALSE(mLLVMPointerOwnerType->isReference());
  EXPECT_FALSE(mLLVMPointerOwnerType->isArray());
  EXPECT_FALSE(mLLVMPointerOwnerType->isFunction());
  EXPECT_FALSE(mLLVMPointerOwnerType->isPackage());
  EXPECT_TRUE(mLLVMPointerOwnerType->isNative());
  EXPECT_TRUE(mLLVMPointerOwnerType->isPointer());
  EXPECT_FALSE(mLLVMPointerOwnerType->isImmutable());
}

TEST_F(LLVMPointerOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->isController());
  EXPECT_FALSE(mLLVMPointerOwnerType->isInterface());
  EXPECT_FALSE(mLLVMPointerOwnerType->isModel());
  EXPECT_FALSE(mLLVMPointerOwnerType->isNode());
}

TEST_F(LLVMPointerOwnerTypeTest, createLocalVariableTest) {
  mLLVMPointerOwnerType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %temp = alloca i8*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i8* null, i8** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerOwnerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mLLVMPointerOwnerType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMPointerOwnerType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMPointerOwnerTypeTest, createParameterVariableTest) {
  llvm::Constant* null = ConstantPointerNull::get(mLLVMPointerOwnerType->getLLVMType(mContext));
  mLLVMPointerOwnerType->createParameterVariable(mContext, "foo", null, 0);
  IVariable* variable = mContext.getScopes().getVariable("foo");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMPointerOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMPointerOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer*", stringStream.str().c_str());
}

TEST_F(LLVMPointerOwnerTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMPointerOwnerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Both constructor and destructor must be provided for pointer injection\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, customConstructorDestructorRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_custom_constructor_destructor.yz",
                                        "Custom constructor\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mPointerFied\n"
                                        "Custom destructor\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
