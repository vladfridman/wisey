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
  NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier;
  const LLVMFunctionType* mLLVMFunctionType;
  LLVMFunction* mLLVMFunction;
  Function* mFunction;
  
  LLVMFunctionIdentifierTest() : mLLVMContext(mContext.getLLVMContext()) {
    mContext.getScopes().pushScope();
    mObject = new NiceMock<MockObjectType>();
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
    mLLVMFunction = new LLVMFunction("myfunction",
                                     PUBLIC_ACCESS,
                                     mLLVMFunctionType,
                                     LLVMPrimitiveTypes::I16,
                                     arguments,
                                     compoundStatement,
                                     0);
    vector<Type*> functionArgumentTypes;
    llvm::FunctionType* functionType = llvm::FunctionType::get(Type::getVoidTy(mLLVMContext),
                                                               functionArgumentTypes,
                                                               false);
    mFunction = llvm::Function::Create(functionType,
                                       GlobalValue::InternalLinkage,
                                       "systems.vos.wisey.tests.IObject.myfunction",
                                       mContext.getModule());
    ON_CALL(*mObjectSpecifier, getType(_)).WillByDefault(Return(mObject));
    ON_CALL(*mObject, findLLVMFunction("myfunction")).WillByDefault(Return(mLLVMFunction));
    ON_CALL(*mObject, getTypeName()).WillByDefault(Return("systems.vos.wisey.tests.IObject"));
    ON_CALL(*mObjectSpecifier, printToStream(_, _)).WillByDefault(Invoke(printObjectSpecifier));
    
    ImportProfile* importProfile = new ImportProfile("systems.vos.wisey.tests");
    importProfile->setSourceFileName(mContext, "/temp/mysource.yz");
    mContext.setImportProfile(importProfile);
  }
  
  ~LLVMFunctionIdentifierTest() {
    delete mObject;
    delete mLLVMFunctionType;
  }
  
  static void printObjectSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "systems.vos.wisey.tests.IObject";
  }
  
};

TEST_F(LLVMFunctionIdentifierTest, generateIRTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "myfunction", 0);
  EXPECT_EQ(mFunction, llvmFunctionIdentifier.generateIR(mContext, PrimitiveTypes::VOID_TYPE));
}

TEST_F(LLVMFunctionIdentifierTest, generateIRDeathTest) {
  ::Mock::AllowLeak(mObject);
  ::Mock::AllowLeak(mObjectSpecifier);
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "foo", 11);

  EXPECT_EXIT(llvmFunctionIdentifier.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "/temp/mysource.yz\\(11\\): Error: LLVMFunction 'foo' not found in object systems.vos.wisey.tests.IObject");
}

TEST_F(LLVMFunctionIdentifierTest, getTypeTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "myfunction", 0);
  EXPECT_EQ(mLLVMFunctionType, llvmFunctionIdentifier.getType(mContext));
}

TEST_F(LLVMFunctionIdentifierTest, isConstantTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "myfunction", 0);

  EXPECT_FALSE(llvmFunctionIdentifier.isConstant());
}

TEST_F(LLVMFunctionIdentifierTest, printToStreamTest) {
  LLVMFunctionIdentifier llvmFunctionIdentifier(mObjectSpecifier, "myfunction", 0);

  stringstream stringStream;
  llvmFunctionIdentifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.tests.IObject.myfunction", stringStream.str().c_str());
}
