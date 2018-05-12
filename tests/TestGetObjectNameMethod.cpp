//
//  TestGetObjectNameMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//
//  Tests {@link GetObjectNameMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/GetObjectNameMethod.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct GetObjectNameMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  const GetObjectNameMethod* mGetObjectNameMethod;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  GetObjectNameMethodTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mGetObjectNameMethod = GetObjectNameMethod::GET_OBJECT_NAME_METHOD;
  }
  
  ~GetObjectNameMethodTest() {
    delete mStringStream;
  }
};

TEST_F(GetObjectNameMethodTest, getNameTest) {
  EXPECT_STREQ("getObjectName", mGetObjectNameMethod->getName().c_str());
}

TEST_F(GetObjectNameMethodTest, getLLVMTypeTest) {
  EXPECT_EQ(FunctionType::get(Type::getInt8Ty(mLLVMContext)->getPointerTo(), false),
            mGetObjectNameMethod->getLLVMType(mContext));
}

TEST_F(GetObjectNameMethodTest, generateIRTest) {
  Value* null = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  GetObjectNameMethod::generateIR(mContext, new FakeExpression(null, PrimitiveTypes::STRING));
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = call i8* @__getOriginalObjectName(i8* null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(GetObjectNameMethodTest, isTypeKindTest) {
  EXPECT_FALSE(mGetObjectNameMethod->isPrimitive());
  EXPECT_FALSE(mGetObjectNameMethod->isOwner());
  EXPECT_FALSE(mGetObjectNameMethod->isReference());
  EXPECT_FALSE(mGetObjectNameMethod->isArray());
  EXPECT_TRUE(mGetObjectNameMethod->isFunction());
  EXPECT_FALSE(mGetObjectNameMethod->isPackage());
  EXPECT_FALSE(mGetObjectNameMethod->isNative());
  EXPECT_FALSE(mGetObjectNameMethod->isPointer());
  EXPECT_FALSE(mGetObjectNameMethod->isImmutable());
}

TEST_F(GetObjectNameMethodTest, isObjectTest) {
  EXPECT_FALSE(mGetObjectNameMethod->isController());
  EXPECT_FALSE(mGetObjectNameMethod->isInterface());
  EXPECT_FALSE(mGetObjectNameMethod->isModel());
  EXPECT_FALSE(mGetObjectNameMethod->isNode());
}

TEST_F(GetObjectNameMethodTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mGetObjectNameMethod->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type <object>.getObjectName is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, getObjectNameOnThisRunTest) {
  runFileCheckOutput("tests/samples/test_get_object_name_on_this.yz",
                     "systems.vos.wisey.compiler.tests.CProgram\n",
                     "");
}

TEST_F(TestFileRunner, getObjectNameOnThreadRunTest) {
  runFileCheckOutput("tests/samples/test_get_object_name_on_thread.yz",
                     "wisey.threads.CWorkerThread\n",
                     "");
}
