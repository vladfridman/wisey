//
//  TestLLVMFunctionIdentifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunctionIdentifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockObjectType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "wisey/Block.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMFunctionArgument.hpp"
#include "wisey/LLVMFunctionIdentifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMFunctionIdentifierTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockObjectType>* mObject;
  NiceMock<MockObjectType>* mAnotherObject;
  NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier;
  const LLVMFunctionType* mLLVMFunctionType;
  LLVMFunction* mPublicLLVMFunction;
  LLVMFunction* mPrivateLLVMFunction;
  Function* mPublicFunction;
  
  LLVMFunctionIdentifierTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mContext.getScopes().pushScope();
    mObject = new NiceMock<MockObjectType>();
    mAnotherObject = new NiceMock<MockObjectType>();
    mObjectSpecifier = new NiceMock<MockObjectTypeSpecifier>();
    LLVMFunctionArgument* llvmFunctionArgument =
    new LLVMFunctionArgument(LLVMPrimitiveTypes::I64, "input");
    vector<const LLVMFunctionArgument*> arguments;
    arguments.push_back(llvmFunctionArgument);
    Block* block = new Block();
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    vector<const IType*> argumentTypes;
    argumentTypes.push_back(LLVMPrimitiveTypes::I64);
    mLLVMFunctionType = new LLVMFunctionType(LLVMPrimitiveTypes::I16, argumentTypes);
    mPublicLLVMFunction = LLVMFunction::newPublicLLVMFunction("publicFunction",
                                                              mLLVMFunctionType,
                                                              LLVMPrimitiveTypes::I16,
                                                              arguments,
                                                              compoundStatement,
                                                              0);
    mPrivateLLVMFunction = LLVMFunction::newPrivateLLVMFunction("privateFunction",
                                                                mLLVMFunctionType,
                                                                LLVMPrimitiveTypes::I16,
                                                                arguments,
                                                                compoundStatement,
                                                                0);
    vector<Type*> functionArgumentTypes;
    llvm::FunctionType* functionType = llvm::FunctionType::get(Type::getVoidTy(mLLVMContext),
                                                               functionArgumentTypes,
                                                               false);
    mPublicFunction = llvm::Function::Create(functionType,
                                             GlobalValue::InternalLinkage,
                                             "systems.vos.wisey.tests.IObject.publicFunction",
                                             mContext.getModule());
    llvm::Function::Create(functionType,
                           GlobalValue::InternalLinkage,
                           "systems.vos.wisey.tests.IObject.privateFunction",
                           mContext.getModule());
    ON_CALL(*mObjectSpecifier, getType(_)).WillByDefault(Return(mObject));
    ON_CALL(*mObject, findLLVMFunction("publicFunction")).
    WillByDefault(Return(mPublicLLVMFunction));
    ON_CALL(*mObject, findLLVMFunction("privateFunction")).
    WillByDefault(Return(mPrivateLLVMFunction));
    ON_CALL(*mObject, getTypeName()).WillByDefault(Return("systems.vos.wisey.tests.IObject"));
    ON_CALL(*mObjectSpecifier, printToStream(_, _)).WillByDefault(Invoke(printObjectSpecifier));
    
    ON_CALL(*mAnotherObject, getTypeName())
    .WillByDefault(Return("systems.vos.wisey.tests.CAnotherObject"));
    mContext.setObjectType(mAnotherObject);
  }
  
  ~LLVMFunctionIdentifierTest() {
    delete mObject;
    delete mAnotherObject;
    delete mLLVMFunctionType;
  }
  
  static void printObjectSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "systems.vos.wisey.tests.IObject";
  }
  
};

TEST_F(LLVMFunctionIdentifierTest, generateIRTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "publicFunction", 0);
  EXPECT_EQ(mPublicFunction,
            llvmFunctionIdentifier.generateIR(mContext, PrimitiveTypes::VOID_TYPE));
}

TEST_F(LLVMFunctionIdentifierTest, functionNotFoundDeathTest) {
  ::Mock::AllowLeak(mObject);
  ::Mock::AllowLeak(mAnotherObject);
  ::Mock::AllowLeak(mObjectSpecifier);
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "foo", 11);

  EXPECT_EXIT(llvmFunctionIdentifier.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: LLVMFunction 'foo' not found "
              "in object systems.vos.wisey.tests.IObject");
}

TEST_F(LLVMFunctionIdentifierTest, functionNotAccessableDeathTest) {
  ::Mock::AllowLeak(mObject);
  ::Mock::AllowLeak(mAnotherObject);
  ::Mock::AllowLeak(mObjectSpecifier);

  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "privateFunction", 3);
  
  EXPECT_EXIT(llvmFunctionIdentifier.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: LLVMFunction 'privateFunction' in "
              "systems.vos.wisey.tests.IObject is private and can not be accessed "
              "from systems.vos.wisey.tests.CAnotherObject");
}

TEST_F(LLVMFunctionIdentifierTest, getTypeTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "publicFunction", 0);
  EXPECT_EQ(mLLVMFunctionType, llvmFunctionIdentifier.getType(mContext));
}

TEST_F(LLVMFunctionIdentifierTest, isConstantTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "publicFunction", 0);

  EXPECT_FALSE(llvmFunctionIdentifier.isConstant());
}

TEST_F(LLVMFunctionIdentifierTest, printToStreamTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "publicFunction", 0);

  stringstream stringStream;
  llvmFunctionIdentifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.tests.IObject.publicFunction", stringStream.str().c_str());
}
