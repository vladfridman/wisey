//
//  TestThreadOwner.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThreadOwner}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ThreadOwnerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Thread* mThread;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ThreadOwnerTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    vector<Interface*> interfaces;
    
    string threadFullName = "systems.vos.wisey.compiler.tests.TWorker";
    StructType* structType = StructType::create(mLLVMContext, threadFullName);
    mThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                threadFullName,
                                structType);
    mThread->setInterfaces(interfaces);
 
    IConcreteObjectType::generateNameGlobal(mContext, mThread);
    IConcreteObjectType::generateShortNameGlobal(mContext, mThread);
    IConcreteObjectType::generateVTable(mContext, mThread);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ThreadOwnerTest, getObjectTest) {
  EXPECT_EQ(mThread->getOwner()->getObjectType(), mThread);
}

TEST_F(ThreadOwnerTest, getNameTest) {
  EXPECT_EQ(mThread->getOwner()->getTypeName(),
            "systems.vos.wisey.compiler.tests.TWorker*");
}

TEST_F(ThreadOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mThread->getOwner()->getLLVMType(mContext),
            mThread->getLLVMType(mContext));
}

TEST_F(ThreadOwnerTest, canCastTest) {
  const IObjectOwnerType* threadOwner = mThread->getOwner();
  
  EXPECT_FALSE(threadOwner->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(threadOwner->canCastTo(mContext, mThread));
  EXPECT_TRUE(threadOwner->canCastTo(mContext, mThread->getOwner()));
}

TEST_F(ThreadOwnerTest, canAutoCastTest) {
  const IObjectOwnerType* threadOwner = mThread->getOwner();
  
  EXPECT_FALSE(threadOwner->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(threadOwner->canAutoCastTo(mContext, mThread));
  EXPECT_TRUE(threadOwner->canAutoCastTo(mContext, mThread->getOwner()));
}

TEST_F(ThreadOwnerTest, castToItselfTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mThread->getLLVMType(mContext));
  Value* result = mThread->getOwner()->castTo(mContext, pointer, mThread->getOwner(), 0);

  EXPECT_EQ(result, pointer);
}

TEST_F(ThreadOwnerTest, isTypeKindTest) {
  EXPECT_FALSE(mThread->getOwner()->isPrimitive());
  EXPECT_TRUE(mThread->getOwner()->isOwner());
  EXPECT_FALSE(mThread->getOwner()->isReference());
  EXPECT_FALSE(mThread->getOwner()->isArray());
  EXPECT_FALSE(mThread->getOwner()->isFunction());
  EXPECT_FALSE(mThread->getOwner()->isPackage());
  EXPECT_FALSE(mThread->getOwner()->isNative());
}

TEST_F(ThreadOwnerTest, isObjectTest) {
  EXPECT_FALSE(mThread->getOwner()->isController());
  EXPECT_FALSE(mThread->getOwner()->isInterface());
  EXPECT_FALSE(mThread->getOwner()->isModel());
  EXPECT_FALSE(mThread->getOwner()->isNode());
  EXPECT_TRUE(mThread->getOwner()->isThread());
}

TEST_F(ThreadOwnerTest, getDestructorFunctionTest) {
  Function* result = mThread->getOwner()->getDestructorFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  EXPECT_EQ(functionType, result->getFunctionType());
}

TEST_F(ThreadOwnerTest, createLocalVariableTest) {
  mThread->getOwner()->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %ownerDeclaration = alloca %systems.vos.wisey.compiler.tests.TWorker*"
  "\n  store %systems.vos.wisey.compiler.tests.TWorker* null, %systems.vos.wisey.compiler.tests.TWorker** %ownerDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mThread->getOwner(), "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mThread->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ThreadOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mThread->getOwner()->getLLVMType(mContext));
  mThread->getOwner()->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %parameterObjectPointer = alloca %systems.vos.wisey.compiler.tests.TWorker*"
  "\n  store %systems.vos.wisey.compiler.tests.TWorker* null, %systems.vos.wisey.compiler.tests.TWorker** %parameterObjectPointer\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadOwnerTest, getPointerTypeTest) {
  const IType* pointerType = mThread->getOwner()->getPointerType();
  EXPECT_EQ(mThread->getOwner()->getLLVMType(mContext)->getPointerTo(),
            pointerType->getLLVMType(mContext));
}
