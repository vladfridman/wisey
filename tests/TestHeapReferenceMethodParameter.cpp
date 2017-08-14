//
//  TestHeapReferenceMethodParameter.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HeapReferenceMethodParameter}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/HeapReferenceMethodParameter.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct HeapReferenceMethodParameterTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  HeapReferenceMethodParameterTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

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

    mStringStream = new raw_string_ostream(mStringBuffer);
}
};

TEST_F(HeapReferenceMethodParameterTest, heapVariableAssignmentDeathTest) {
  HeapReferenceMethodParameter heapMethodParameter("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_EXIT(heapMethodParameter.generateAssignmentIR(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Assignment to method parameters is not allowed");
}

TEST_F(HeapReferenceMethodParameterTest, heapVariableIdentifierTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  HeapReferenceMethodParameter heapMethodParameter("foo", PrimitiveTypes::INT_TYPE, fooValue);
  
  EXPECT_EQ(heapMethodParameter.generateIdentifierIR(mContext, "bar"), fooValue);
}
