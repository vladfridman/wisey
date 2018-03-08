//
//  TestNativeFunctionCall.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NativeFunctionCall}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NativeFunctionCall.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NativeFunctionCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  NativeFunctionCall* mNativeFunctionCall;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mNativeFunction;
  NiceMock<MockExpression>* mArgumentExpression;
  
public:
  
  NativeFunctionCallTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    FunctionType *typeIdType = TypeBuilder<void (types::i<8>*), false>::get(mLLVMContext);
    mNativeFunction = cast<Function>(mContext.getModule()->
                                     getOrInsertFunction("nativeFunction", typeIdType));
    vector<IExpression*> arguments;
    mArgumentExpression = new NiceMock<MockExpression>();
    ON_CALL(*mArgumentExpression, getType(_)).WillByDefault(Return(NullType::NULL_TYPE));
    Value* nullValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    ON_CALL(*mArgumentExpression, generateIR(_, _)).WillByDefault(Return(nullValue));
    arguments.push_back(mArgumentExpression);
    mNativeFunctionCall = new NativeFunctionCall(mNativeFunction, arguments);
  }
  
  ~NativeFunctionCallTest() {
    delete mNativeFunctionCall;
  }
};

TEST_F(NativeFunctionCallTest, generateIRTest) {
  mNativeFunctionCall->generateIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:"
  "\n  call void @nativeFunction(i8* null)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

