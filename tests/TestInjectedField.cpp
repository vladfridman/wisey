//
//  TestInjectedField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InjectedField}
//

#include <gtest/gtest.h>

#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockObjectOwnerType.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InjectedFieldTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockType>* mType;
  NiceMock<MockType>* mInjectedType;
  NiceMock<MockVariable>* mThreadVariable;
  const NiceMock<MockObjectOwnerType>* mObjectOwnerType;
  const NiceMock<MockObjectType>* mObjectType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  InjectionArgumentList mInjectionArgumentList;
  InjectionArgument* mInjectionArgument;
  Controller* mController;
  Function* mFunction;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  InjectedField* mField;

public:
  
  InjectedFieldTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mType(new NiceMock<MockType>()),
  mInjectedType(new NiceMock<MockType>()),
  mObjectOwnerType(new NiceMock<MockObjectOwnerType>()),
  mObjectType(new NiceMock<MockObjectType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    mInjectionArgument = new InjectionArgument("withFoo", mExpression);
    mInjectionArgumentList.push_back(mInjectionArgument);
    ON_CALL(*mType, getTypeName()).WillByDefault(Return("MObject*"));
    ON_CALL(*mInjectedType, getTypeName()).WillByDefault(Return("MInjectedObject*"));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    EXPECT_CALL(*mType, die());
    EXPECT_CALL(*mInjectedType, die());

    vector<Type*> controllerTypes;
    string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    controllerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                              ->getPointerTo()->getPointerTo());
    controllerStructType->setBody(controllerTypes);
    vector<IField*> controllerFields;
    InjectionArgumentList fieldArguments;
    mField = new InjectedField(mType, mType, "mFoo", fieldArguments, "/tmp/source.yz", 1);
    controllerFields.push_back(mField);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType,
                                            mContext.getImportProfile(),
                                            0);
    mController->setFields(mContext, controllerFields, 1u);
    mContext.addController(mController, 0);
    IConcreteObjectType::generateNameGlobal(mContext, mController);
    IConcreteObjectType::generateShortNameGlobal(mContext, mController);
    IConcreteObjectType::generateVTable(mContext, mController);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    PointerType* int8PointerType = Type::getInt8Ty(mLLVMContext)->getPointerTo();
    ON_CALL(*mType, getLLVMType(_)).WillByDefault(Return(int8PointerType));
    Value* null = ConstantPointerNull::get(int8PointerType);
    ON_CALL(*mType, inject(_, _, _)).WillByDefault(Return(null));

    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_, _)).WillByDefault(Return(threadObject));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InjectedFieldTest() {
    delete mType;
    delete mExpression;
    delete mInjectedType;
    delete mObjectOwnerType;
    delete mObjectType;
    delete mThreadVariable;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(InjectedFieldTest, fieldCreationTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList, "", 0);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_TRUE(field.isAssignable());

  EXPECT_FALSE(field.isFixed());
  EXPECT_TRUE(field.isInjected());
  EXPECT_FALSE(field.isReceived());
  EXPECT_FALSE(field.isState());
}

TEST_F(InjectedFieldTest, elementTypeTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList, "", 0);

  EXPECT_FALSE(field.isConstant());
  EXPECT_TRUE(field.isField());
  EXPECT_FALSE(field.isMethod());
  EXPECT_FALSE(field.isStaticMethod());
  EXPECT_FALSE(field.isMethodSignature());
  EXPECT_FALSE(field.isLLVMFunction());
}

TEST_F(InjectedFieldTest, fieldPrintToStreamTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList, "", 0);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  inject MInjectedObject* mField.withFoo(expression);\n",
               stringStream.str().c_str());
}

TEST_F(InjectedFieldTest, injectTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList, "", 0);
  EXPECT_CALL(*mInjectedType, inject(_, _, _));

  field.inject(mContext);
}

TEST_F(InjectedFieldTest, declareInjectionFunctionTest) {
  mField->declareInjectionFunction(mContext, mController);
  
  Function* function = mContext.getModule()->
  getFunction(mField->getInjectionFunctionName(mController));
  
  EXPECT_NE(nullptr, function);
  
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  vector<Type*> functionArgumentTypes;
  functionArgumentTypes.push_back(threadInterface->getLLVMType(mContext));
  functionArgumentTypes.push_back(mType->getLLVMType(mContext)->getPointerTo());
  FunctionType* functionType = FunctionType::get(mType->getLLVMType(mContext),
                                                 functionArgumentTypes,
                                                 false);
  
  EXPECT_EQ(functionType, function->getFunctionType());
}

TEST_F(InjectedFieldTest, callInjectFunctionTest) {
  mContext.getScopes().setVariable(mContext, mThreadVariable);
  mField->declareInjectionFunction(mContext, mController);
  Value* null = ConstantPointerNull::get(mType->getLLVMType(mContext)->getPointerTo());
  mField->callInjectFunction(mContext, mController, null, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = call i8* @systems.vos.wisey.compiler.tests.CController.mFoo.inject(%wisey.threads.IThread* null, i8** null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InjectedFieldTest, defineInjectionFunctionTest) {
  mField->defineInjectionFunction(mContext, mController);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->
  getFunction(mField->getInjectionFunctionName(mController));
  
  EXPECT_NE(nullptr, function);
  
  *mStringStream << *function;
  string expected =
  "\ndefine i8* @systems.vos.wisey.compiler.tests.CController.mFoo.inject(%wisey.threads.IThread* %thread, i8** %fieldPointer) {"
  "\nentry:"
  "\n  %0 = load i8*, i8** %fieldPointer"
  "\n  %isNull = icmp eq i8* %0, null"
  "\n  br i1 %isNull, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  store i8* null, i8** %fieldPointer"
  "\n  ret i8* null"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  ret i8* %0"
  "\n}"
  "\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InjectedFieldTest, checkOwnerTypeTest) {
  ON_CALL(*mType, isOwner()).WillByDefault(Return(true));
  ON_CALL(*mType, isReference()).WillByDefault(Return(false));

  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(InjectedFieldTest, checkReferenceTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  ON_CALL(*mType, isOwner()).WillByDefault(Return(false));
  ON_CALL(*mType, isReference()).WillByDefault(Return(true));

  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Injected fields must have owner type denoted by '*'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InjectedFieldTest, injectInterfaceNotBoundWithArgumentsDeathTest) {
  InjectedField field(mObjectOwnerType, mObjectOwnerType, mName, mInjectionArgumentList, "", 5);
  ON_CALL(*mObjectOwnerType, isOwner()).WillByDefault(Return(true));
  ON_CALL(*mObjectOwnerType, isInterface()).WillByDefault(Return(true));
  ON_CALL(*mObjectOwnerType, getReference()).WillByDefault(Return(mObjectType));
  ON_CALL(*mObjectType, isInterface()).WillByDefault(Return(true));
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(field.checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Arguments are not allowed for injection of interfaces that are not bound to controllers\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
