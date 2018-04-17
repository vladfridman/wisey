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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadOwner.hpp"
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/WiseyModelType.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"

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
    TestPrefix::generateIR(mContext);
    
    vector<Interface*> interfaces;
    
    string threadFullName = "systems.vos.wisey.compiler.tests.TWorker";
    StructType* structType = StructType::create(mLLVMContext, threadFullName);
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    structType->setBody(types);
    mThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                threadFullName,
                                structType,
                                0);
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
  EXPECT_EQ(mThread->getOwner()->getReference(), mThread);
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
  EXPECT_FALSE(threadOwner->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(threadOwner->canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(threadOwner->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(threadOwner->canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ThreadOwnerTest, canAutoCastTest) {
  const IObjectOwnerType* threadOwner = mThread->getOwner();
  
  EXPECT_FALSE(threadOwner->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(threadOwner->canAutoCastTo(mContext, mThread));
  EXPECT_TRUE(threadOwner->canAutoCastTo(mContext, mThread->getOwner()));
  EXPECT_FALSE(threadOwner->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(threadOwner->canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(threadOwner->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(threadOwner->canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
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
  EXPECT_FALSE(mThread->getOwner()->isPointer());
}

TEST_F(ThreadOwnerTest, isObjectTest) {
  EXPECT_FALSE(mThread->getOwner()->isController());
  EXPECT_FALSE(mThread->getOwner()->isInterface());
  EXPECT_FALSE(mThread->getOwner()->isModel());
  EXPECT_FALSE(mThread->getOwner()->isNode());
  EXPECT_TRUE(mThread->getOwner()->isThread());
}

TEST_F(ThreadOwnerTest, getDestructorFunctionTest) {
  Function* result = mThread->getOwner()->getDestructorFunction(mContext, 0);
  
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
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.TWorker*"
  "\n  store %systems.vos.wisey.compiler.tests.TWorker* null, %systems.vos.wisey.compiler.tests.TWorker** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mThread->getOwner(), "mField", 0);
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
  "\n  %var = alloca %systems.vos.wisey.compiler.tests.TWorker*"
  "\n  store %systems.vos.wisey.compiler.tests.TWorker* null, %systems.vos.wisey.compiler.tests.TWorker** %var\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadOwnerTest, injectTest) {
  InjectionArgumentList injectionArguments;
  Value* result = mThread->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.TWorker.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TWorker.refCounter, %systems.vos.wisey.compiler.tests.TWorker.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.TWorker.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.TWorker.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TWorker.refCounter, %systems.vos.wisey.compiler.tests.TWorker.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.TWorker.refCounter, %systems.vos.wisey.compiler.tests.TWorker.refCounter* %injectvar, i32 0, i32 1"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.TWorker* %1 to i8*"
  "\n  %3 = getelementptr i8, i8* %2, i64 0"
  "\n  %4 = bitcast i8* %3 to i32 (...)***"
  "\n  %5 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.TWorker.vtable, i32 0, i32 0, i32 0"
  "\n  %6 = bitcast i8** %5 to i32 (...)**"
  "\n  store i32 (...)** %6, i32 (...)*** %4"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
