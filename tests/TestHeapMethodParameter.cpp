//
//  TestHeapMethodParameter.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HeapMethodParameter}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/HeapMethodParameter.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct HeapMethodParameterTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  
public:
  
  HeapMethodParameterTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
  }
};

TEST_F(HeapMethodParameterTest, heapVariableAssignmentDeathTest) {
  HeapMethodParameter heapMethodParameter("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_EXIT(heapMethodParameter.generateAssignmentIR(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Assignment to method parameters is not allowed");
}

TEST_F(HeapMethodParameterTest, heapVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  HeapMethodParameter heapMethodParameter("foo", PrimitiveTypes::INT_TYPE, fooValue);
  
  EXPECT_EQ(heapMethodParameter.generateIdentifierIR(mContext, "bar"), fooValue);
}

TEST_F(HeapMethodParameterTest, freeTest) {
  Value* fooValue = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
  HeapMethodParameter heapMethodParameter("foo", mModel->getOwner(), fooValue);
  
  EXPECT_EQ(mBlock->size(), 0u);
  
  heapMethodParameter.free(mContext);
  
  EXPECT_EQ(mBlock->size(), 2u);
  BasicBlock::iterator iterator = mBlock->begin();
  EXPECT_TRUE(BitCastInst::classof(&(*iterator)));
  EXPECT_TRUE(CallInst::classof(&(*++iterator)));
}
