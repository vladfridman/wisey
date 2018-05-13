//
//  TestGetTypeNameMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//
//  Tests {@link GetTypeNameMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/GetTypeNameMethod.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct GetTypeNameMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  const GetTypeNameMethod* mGetTypeNameMethod;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  GetTypeNameMethodTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mGetTypeNameMethod = GetTypeNameMethod::GET_TYPE_NAME_METHOD;
  }
  
  ~GetTypeNameMethodTest() {
    delete mStringStream;
  }
};

TEST_F(GetTypeNameMethodTest, basicTest) {
  EXPECT_STREQ("getTypeName", mGetTypeNameMethod->getName().c_str());
  EXPECT_TRUE(mGetTypeNameMethod->isStatic());
  EXPECT_TRUE(mGetTypeNameMethod->isPublic());
}

TEST_F(GetTypeNameMethodTest, getLLVMTypeTest) {
  EXPECT_EQ(FunctionType::get(Type::getInt8Ty(mLLVMContext)->getPointerTo(), false),
            mGetTypeNameMethod->getLLVMType(mContext));
}

TEST_F(GetTypeNameMethodTest, generateIRTest) {
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  
  Value* result = GetTypeNameMethod::generateIR(mContext, threadInterface);
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(IObjectType::getObjectNamePointer(threadInterface, mContext), result);
}

TEST_F(GetTypeNameMethodTest, isTypeKindTest) {
  EXPECT_FALSE(mGetTypeNameMethod->isPrimitive());
  EXPECT_FALSE(mGetTypeNameMethod->isOwner());
  EXPECT_FALSE(mGetTypeNameMethod->isReference());
  EXPECT_FALSE(mGetTypeNameMethod->isArray());
  EXPECT_TRUE(mGetTypeNameMethod->isFunction());
  EXPECT_FALSE(mGetTypeNameMethod->isPackage());
  EXPECT_FALSE(mGetTypeNameMethod->isNative());
  EXPECT_FALSE(mGetTypeNameMethod->isPointer());
  EXPECT_FALSE(mGetTypeNameMethod->isImmutable());
}

TEST_F(GetTypeNameMethodTest, isObjectTest) {
  EXPECT_FALSE(mGetTypeNameMethod->isController());
  EXPECT_FALSE(mGetTypeNameMethod->isInterface());
  EXPECT_FALSE(mGetTypeNameMethod->isModel());
  EXPECT_FALSE(mGetTypeNameMethod->isNode());
}

TEST_F(GetTypeNameMethodTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mGetTypeNameMethod->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type getTypeName is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, getObjectNameOnThisRunTest) {
  runFileCheckOutput("tests/samples/test_get_type_name_on_interface.yz",
                     "wisey.lang.IProgram\n",
                     "");
}

TEST_F(TestFileRunner, getObjectNameOnThreadRunTest) {
  runFileCheckOutput("tests/samples/test_get_object_name_on_controller.yz",
                     "systems.vos.wisey.compiler.tests.CProgram\n",
                     "");
}
