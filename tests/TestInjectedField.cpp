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
#include "MockConcreteObjectType.hpp"
#include "MockObjectOwnerType.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "InjectedField.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ThreadExpression.hpp"

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
  NiceMock<MockVariable>* mThisVariable;
  const NiceMock<MockObjectOwnerType>* mObjectOwnerType;
  const NiceMock<MockConcreteObjectType>* mObjectType;
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
  mObjectType(new NiceMock<MockConcreteObjectType>()),
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
    mField = InjectedField::createDelayed(mType,
                                          mType,
                                          "mFoo",
                                          fieldArguments,
                                          "/tmp/source.yz",
                                          1);
    controllerFields.push_back(mField);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType,
                                            mContext.getImportProfile(),
                                            0);
    mController->setFields(mContext, controllerFields, 1u);
    mContext.addController(mController, 0);
    IConcreteObjectType::declareTypeNameGlobal(mContext, mController);
    IConcreteObjectType::declareVTable(mContext, mController);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
     
    PointerType* int8PointerType = Type::getInt8Ty(mLLVMContext)->getPointerTo();
    ON_CALL(*mType, getLLVMType(_)).WillByDefault(Return(int8PointerType));
    Value* null = ConstantPointerNull::get(int8PointerType);
    ON_CALL(*mType, inject(_, _, _)).WillByDefault(Return(null));

    mThisVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThisVariable, getName()).WillByDefault(Return(IObjectType::THIS));
    ON_CALL(*mThisVariable, getType()).WillByDefault(Return(mController));
    null = ConstantPointerNull::get(mController->getLLVMType(mContext));
    ON_CALL(*mThisVariable, generateIdentifierIR(_, _)).WillByDefault(Return(null));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~InjectedFieldTest() {
    delete mType;
    delete mExpression;
    delete mInjectedType;
    delete mObjectOwnerType;
    delete mObjectType;
    delete mThisVariable;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(InjectedFieldTest, fieldCreationTest) {
  InjectedField* field = InjectedField::createDelayed(mType,
                                                      mInjectedType,
                                                      mName,
                                                      mInjectionArgumentList,
                                                      "",
                                                      0);
  
  EXPECT_EQ(field->getType(), mType);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_TRUE(field->isAssignable(mObjectType));

  EXPECT_TRUE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

TEST_F(InjectedFieldTest, elementTypeTest) {
  InjectedField* field = InjectedField::createDelayed(mType,
                                                      mInjectedType,
                                                      mName,
                                                      mInjectionArgumentList,
                                                      "",
                                                      0);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  EXPECT_FALSE(field->isLLVMFunction());
}

TEST_F(InjectedFieldTest, fieldPrintToStreamTest) {
  InjectedField* field = InjectedField::createImmediate(mType,
                                                        mInjectedType,
                                                        mName,
                                                        mInjectionArgumentList,
                                                        "",
                                                        0);

  stringstream stringStream;
  field->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  inject immediate MInjectedObject* mField.withFoo(expression);\n",
               stringStream.str().c_str());
}

TEST_F(InjectedFieldTest, injectTest) {
  InjectedField* field = InjectedField::createDelayed(mType,
                                                      mInjectedType,
                                                      mName,
                                                      mInjectionArgumentList,
                                                      "",
                                                      0);
  EXPECT_CALL(*mInjectedType, inject(_, _, _));

  field->inject(mContext);
}

TEST_F(InjectedFieldTest, getValueForDelayedInjectedFieldTest) {
  mContext.getScopes().setVariable(mContext, mThisVariable);
  mField->declareInjectionFunction(mContext, mController);
  Value* null = ConstantPointerNull::get(mType->getLLVMType(mContext)->getPointerTo());
  mField->getValue(mContext, mController, null, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call i8* @systems.vos.wisey.compiler.tests.CController.mFoo.inject(%systems.vos.wisey.compiler.tests.CController* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8** null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InjectedFieldTest, getValueForImmediateInjectedFieldTest) {
  InjectionArgumentList fieldArguments;
  InjectedField* field = InjectedField::createImmediate(mType,
                                                        mType,
                                                        "mFoo",
                                                        fieldArguments,
                                                        "/tmp/source.yz",
                                                        1);

  Value* null = ConstantPointerNull::get(mType->getLLVMType(mContext)->getPointerTo());
  field->getValue(mContext, mController, null, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %mFoo = load i8*, i8** null\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InjectedFieldTest, declareInjectionFunctionTest) {
  mField->declareInjectionFunction(mContext, mController);
  
  Function* function = mContext.getModule()->
  getFunction(mField->getInjectionFunctionName(mController));
  
  EXPECT_NE(nullptr, function);
  
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  vector<Type*> functionArgumentTypes;
  functionArgumentTypes.push_back(mController->getLLVMType(mContext));
  functionArgumentTypes.push_back(threadInterface->getLLVMType(mContext));
  Controller* callstackController =
    mContext.getController(Names::getCallStackControllerFullName(), 0);
  functionArgumentTypes.push_back(callstackController->getLLVMType(mContext));
  functionArgumentTypes.push_back(mType->getLLVMType(mContext)->getPointerTo());
  FunctionType* functionType = FunctionType::get(mType->getLLVMType(mContext),
                                                 functionArgumentTypes,
                                                 false);
  
  EXPECT_EQ(functionType, function->getFunctionType());
}

TEST_F(InjectedFieldTest, defineInjectionFunctionTest) {
  mContext.getScopes().popScope(mContext, 0);
  
  mField->defineInjectionFunction(mContext, mController);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->
  getFunction(mField->getInjectionFunctionName(mController));
  
  EXPECT_NE(nullptr, function);
  
  *mStringStream << *function;
  string expected =
  "\ndefine i8* @systems.vos.wisey.compiler.tests.CController.mFoo.inject(%systems.vos.wisey.compiler.tests.CController* %this, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i8** %fieldPointer) {"
  "\ndeclarations:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declarations"
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
  vector<IField*> controllerFields;
  InjectionArgumentList fieldArguments;
  InjectedField* injectedField = InjectedField::createDelayed(mType,
                                                              mController->getOwner(),
                                                              "mFoo",
                                                              fieldArguments,
                                                              "/tmp/source.yz",
                                                              1);

  EXPECT_NO_THROW(injectedField->checkType(mContext));
}

TEST_F(InjectedFieldTest, checkReferenceTypeDeathTest) {
  vector<IField*> controllerFields;
  InjectionArgumentList fieldArguments;
  InjectedField* injectedField = InjectedField::createDelayed(mType,
                                                              mController,
                                                              "mFoo",
                                                              fieldArguments,
                                                              "/tmp/source.yz",
                                                              1);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(injectedField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Injected fields must have owner type denoted by '*' if the injected type is not scoped\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InjectedFieldTest, injectInterfaceNotBoundWithArgumentsDeathTest) {
  InjectedField* field = InjectedField::createDelayed(mType,
                                                      mObjectOwnerType,
                                                      mName,
                                                      mInjectionArgumentList,
                                                      "",
                                                      5);
  
  ON_CALL(*mObjectOwnerType, isOwner()).WillByDefault(Return(true));
  ON_CALL(*mObjectOwnerType, isInterface()).WillByDefault(Return(true));
  ON_CALL(*mObjectOwnerType, getReference()).WillByDefault(Return(mObjectType));
  ON_CALL(*mObjectType, isInterface()).WillByDefault(Return(true));
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(field->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Arguments are not allowed for injection of interfaces that are not bound to controllers\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, injectFieldNodeTypeRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_field_node_type.yz",
                    1,
                    "tests/samples/test_inject_field_node_type.yz\\(7\\): Error: "
                    "Only controllers, interfaces bound to controllers and arrays may be injected in fields");
}

TEST_F(TestFileRunner, injectFieldControllerScopedNotReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_field_controller_scoped_not_reference.yz",
                    1,
                    "tests/samples/test_inject_field_controller_scoped_not_reference.yz\\(7\\): Error: "
                    "Controller systems.vos.wisey.compiler.tests.CService is scoped and should have reference field type");
}

TEST_F(TestFileRunner, injectFieldControllerNotScopedNotOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_field_controller_not_scoped_not_owner.yz",
                    1,
                    "tests/samples/test_inject_field_controller_not_scoped_not_owner.yz\\(7\\): Error: "
                    "Injected fields must have owner type denoted by '\\*' if the injected type is not scoped");
}

TEST_F(TestFileRunner, injectInterfaceMissingArgumentsRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_missing_arguments.yz",
                    1,
                    "tests/samples/test_inject_interface_missing_arguments.yz\\(18\\): Error: "
                    "Received field mValue is not initialized");
}
