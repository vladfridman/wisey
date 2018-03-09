//
//  TestThread.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Thread}
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/Constant.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ReceivedField.hpp"
#include "wisey/StateField.hpp"
#include "wisey/Thread.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ThreadTest : public Test {
  Thread* mThread;
  Thread* mNonInjectableFieldThread;
  Thread* mNotWellFormedArgumentsThread;
  IMethod* mMethod;
  StructType* mStructType;
  StateField* mFromField;
  StateField* mToField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock *mBasicBlock;
  wisey::Constant* mConstant;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ThreadTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string workerFullName = "systems.vos.wisey.compiler.tests.TWorker";
    mStructType = StructType::create(mLLVMContext, workerFullName);
    mStructType->setBody(types);
    mThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS, workerFullName, mStructType);
    vector<IField*> fields;
    mFromField = new StateField(PrimitiveTypes::INT_TYPE, "mFrom");
    mToField = new StateField(PrimitiveTypes::INT_TYPE, "mTo");
    fields.push_back(mFromField);
    fields.push_back(mToField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method(mThread,
                         "work",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL,
                         0);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    IMethod* fooMethod = new Method(mThread,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    
    IntConstant* intConstant = new IntConstant(5);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    "MYCONSTANT",
                                    intConstant);
    wisey::Constant* privateConstant = new wisey::Constant(PRIVATE_ACCESS,
                                                           PrimitiveTypes::INT_TYPE,
                                                           "MYCONSTANT3",
                                                           intConstant);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);
    constants.push_back(privateConstant);
    
    mThread->setFields(fields, interfaces.size() + 1);
    mThread->setMethods(methods);
    mThread->setInterfaces(interfaces);
    mThread->setConstants(constants);
    mContext.addThread(mThread);
    
    vector<Type*> nonInjectableFieldThreadTypes;
    nonInjectableFieldThreadTypes.push_back(Type::getInt64Ty(mLLVMContext));
    nonInjectableFieldThreadTypes.push_back(Type::getInt32Ty(mLLVMContext));
    nonInjectableFieldThreadTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string nonInjectableFieldThreadFullName =
    "systems.vos.wisey.compiler.tests.TNonInjectableFieldThread";
    StructType* nonInjectableFieldThreadStructType =
    StructType::create(mLLVMContext, nonInjectableFieldThreadFullName);
    nonInjectableFieldThreadStructType->setBody(nonInjectableFieldThreadTypes);
    vector<IField*> nonInjectableFieldThreadFields;
    InjectionArgumentList fieldArguments;
    InjectedField* injectedField =
    new InjectedField(PrimitiveTypes::INT_TYPE, NULL, "left", fieldArguments);
    nonInjectableFieldThreadFields.push_back(injectedField);
    mNonInjectableFieldThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                                  nonInjectableFieldThreadFullName,
                                                  nonInjectableFieldThreadStructType);
    mNonInjectableFieldThread->setFields(nonInjectableFieldThreadFields, 1u);
    mContext.addThread(mNonInjectableFieldThread);
    
    vector<Type*> notWellFormedArgumentsThreadTypes;
    notWellFormedArgumentsThreadTypes.push_back(Type::getInt64Ty(mLLVMContext));
    notWellFormedArgumentsThreadTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string notWellFormedArgumentsThreadFullName =
    "systems.vos.wisey.compiler.tests.TNotWellFormedArgumentsThread";
    StructType* notWellFormedArgumentsThreadStructType =
    StructType::create(mLLVMContext, notWellFormedArgumentsThreadFullName);
    notWellFormedArgumentsThreadStructType->setBody(notWellFormedArgumentsThreadTypes);
    vector<IField*> notWellFormedArgumentsThreadFields;
    notWellFormedArgumentsThreadFields.
    push_back(new ReceivedField(PrimitiveTypes::INT_TYPE, "mValue"));
    mNotWellFormedArgumentsThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                                      notWellFormedArgumentsThreadFullName,
                                                      notWellFormedArgumentsThreadStructType);
    mNotWellFormedArgumentsThread->setFields(notWellFormedArgumentsThreadFields, 1u);
    mContext.addThread(mNotWellFormedArgumentsThread);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
    Value* threadObject = ConstantPointerNull::get(threadController->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadController));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ThreadTest() {
    delete mStringStream;
    delete mThreadVariable;
  }
};

TEST_F(ThreadTest, getAccessLevelTest) {
  EXPECT_EQ(mThread->getAccessLevel(), AccessLevel::PUBLIC_ACCESS);
}

TEST_F(ThreadTest, getNameTest) {
  EXPECT_STREQ(mThread->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.TWorker");
}

TEST_F(ThreadTest, getShortNameTest) {
  EXPECT_STREQ(mThread->getShortName().c_str(), "TWorker");
}

TEST_F(ThreadTest, getVTableNameTest) {
  EXPECT_STREQ(mThread->getVTableName().c_str(),
               "systems.vos.wisey.compiler.tests.TWorker.vtable");
}

TEST_F(ThreadTest, getLLVMTypeTest) {
  EXPECT_EQ(mThread->getLLVMType(mContext), mStructType->getPointerTo());
}

TEST_F(ThreadTest, getInterfacesTest) {
  EXPECT_EQ(mThread->getInterfaces().size(), 0u);
}

TEST_F(ThreadTest, getVTableSizeTest) {
  EXPECT_EQ(mThread->getVTableSize(), 0u);
}

TEST_F(ThreadTest, getFieldsTest) {
  EXPECT_EQ(mThread->getFields().size(), 2u);
}

TEST_F(ThreadTest, getOwnerTest) {
  ASSERT_NE(mThread->getOwner(), nullptr);
  EXPECT_EQ(mThread->getOwner()->getObjectType(), mThread);
}

TEST_F(ThreadTest, getObjectTypeTest) {
  EXPECT_EQ(mThread, mThread->getObjectType());
}

TEST_F(ThreadTest, getFieldIndexTest) {
  EXPECT_EQ(mThread->getFieldIndex(mFromField), 1u);
  EXPECT_EQ(mThread->getFieldIndex(mToField), 2u);
}

TEST_F(ThreadTest, findFeildTest) {
  EXPECT_EQ(mThread->findField("mFrom"), mFromField);
  EXPECT_EQ(mThread->findField("mTo"), mToField);
  EXPECT_EQ(mThread->findField("mFoo"), nullptr);
}

TEST_F(ThreadTest, findMethodTest) {
  EXPECT_EQ(mThread->findMethod("work"), mMethod);
  EXPECT_EQ(mThread->findMethod("bar"), nullptr);
}

TEST_F(ThreadTest, findConstantTest) {
  EXPECT_EQ(mThread->findConstant("MYCONSTANT"), mConstant);
}

TEST_F(ThreadTest, findConstantDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mThread->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Thread systems.vos.wisey.compiler.tests.TWorker "
              "does not have constant named MYCONSTANT2");
}

TEST_F(ThreadTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mThread->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.TWorker.name");
}

TEST_F(ThreadTest, getObjectShortNameGlobalVariableNameTest) {
  ASSERT_STREQ(mThread->getObjectShortNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.TWorker.shortname");
}

TEST_F(ThreadTest, getTypeTableNameTest) {
  ASSERT_STREQ(mThread->getTypeTableName().c_str(),
               "systems.vos.wisey.compiler.tests.TWorker.typetable");
}

TEST_F(ThreadTest, innerObjectsTest) {
  NiceMock<MockObjectType> innerObject1;
  ON_CALL(innerObject1, getShortName()).WillByDefault(Return("MObject1"));
  NiceMock<MockObjectType> innerObject2;
  ON_CALL(innerObject2, getShortName()).WillByDefault(Return("MObject2"));
  
  mThread->addInnerObject(&innerObject1);
  mThread->addInnerObject(&innerObject2);
  
  EXPECT_EQ(mThread->getInnerObject("MObject1"), &innerObject1);
  EXPECT_EQ(mThread->getInnerObject("MObject2"), &innerObject2);
  EXPECT_EQ(mThread->getInnerObjects().size(), 2u);
}

TEST_F(ThreadTest, markAsInnerTest) {
  EXPECT_FALSE(mThread->isInner());
  mThread->markAsInner();
  EXPECT_TRUE(mThread->isInner());
}

TEST_F(ThreadTest, getReferenceAdjustmentFunctionTest) {
  Function* result = mThread->getReferenceAdjustmentFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(result, AdjustReferenceCounterForConcreteObjectSafelyFunction::get(mContext));
}

TEST_F(ThreadTest, canCastToTest) {
  EXPECT_FALSE(mThread->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mThread->canCastTo(mContext, mThread));
  EXPECT_FALSE(mThread->canCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mThread->canCastTo(mContext, mThread));
}

TEST_F(ThreadTest, canAutoCastToTest) {
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mThread->canAutoCastTo(mContext, mThread));
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mThread->canAutoCastTo(mContext, mThread));
}

TEST_F(ThreadTest, isPrimitiveTest) {
  EXPECT_FALSE(mThread->isPrimitive());
}

TEST_F(ThreadTest, isOwnerTest) {
  EXPECT_FALSE(mThread->isOwner());
}

TEST_F(ThreadTest, isReferenceTest) {
  EXPECT_TRUE(mThread->isReference());
}

TEST_F(ThreadTest, isArrayTest) {
  EXPECT_FALSE(mThread->isArray());
}

TEST_F(ThreadTest, isFunctionTest) {
  EXPECT_FALSE(mThread->isFunction());
}

TEST_F(ThreadTest, isPackageTest) {
  EXPECT_FALSE(mThread->isPackage());
}

TEST_F(ThreadTest, isObjectTest) {
  EXPECT_FALSE(mThread->isController());
  EXPECT_FALSE(mThread->isInterface());
  EXPECT_FALSE(mThread->isModel());
  EXPECT_FALSE(mThread->isNode());
  EXPECT_TRUE(mThread->isThread());
  EXPECT_FALSE(mThread->isNative());
}

TEST_F(ThreadTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mThread->getLLVMType(mContext));
  mThread->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mThread->getLLVMType(mContext));
  mThread->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 -1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mThread->getLLVMType(mContext));
  mThread->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker* null to i64*"
  "\n  %refCounter = load i64, i64* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, injectTest) {
  InjectionArgumentList injectionArguments;
  
  NiceMock<MockExpression> fromExpression;
  Value* fromValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 0);
  ON_CALL(fromExpression, generateIR(_, _)).WillByDefault(Return(fromValue));
  ON_CALL(fromExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  NiceMock<MockExpression> toExpression;
  Value* toValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  ON_CALL(toExpression, generateIR(_, _)).WillByDefault(Return(toValue));
  ON_CALL(toExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  InjectionArgument* ownerArgument = new InjectionArgument("withFrom", &fromExpression);
  InjectionArgument* referenceArgument = new InjectionArgument("withTo", &toExpression);
  
  injectionArguments.push_back(ownerArgument);
  injectionArguments.push_back(referenceArgument);
  
  Value* result = mThread->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.TWorker* getelementptr (%systems.vos.wisey.compiler.tests.TWorker, %systems.vos.wisey.compiler.tests.TWorker* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.TWorker*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.TWorker* getelementptr (%systems.vos.wisey.compiler.tests.TWorker, %systems.vos.wisey.compiler.tests.TWorker* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.TWorker, %systems.vos.wisey.compiler.tests.TWorker* %injectvar, i32 0, i32 1"
  "\n  store i32 0, i32* %1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.TWorker, %systems.vos.wisey.compiler.tests.TWorker* %injectvar, i32 0, i32 2"
  "\n  store i32 5, i32* %2\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, injectWrongTypeOfArgumentDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  NiceMock<MockExpression>* injectArgument2Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(injectArgument2Expression);
  Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(*injectArgument1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
  ON_CALL(*injectArgument1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  Value* field2Value = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*injectArgument2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
  ON_CALL(*injectArgument2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  InjectionArgument* injectionArgument1 = new InjectionArgument("withFrom",
                                                                injectArgument1Expression);
  InjectionArgument* injectionArgument2 = new InjectionArgument("withTo",
                                                                injectArgument2Expression);
  
  injectionArguments.push_back(injectionArgument1);
  injectionArguments.push_back(injectionArgument2);
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mThread->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Thread injector argumet value for field 'mTo' "
              "does not match its type");
}

TEST_F(ThreadTest, injectNonInjectableTypeDeathTest) {
  InjectionArgumentList injectionArguments;
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mNonInjectableFieldThread->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Attempt to inject a variable that is not of injectable type");
}

TEST_F(ThreadTest, notWellFormedInjectionArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(mThreadVariable);
  InjectionArgument* injectionArgument = new InjectionArgument("value",
                                                               injectArgument1Expression);
  injectionArguments.push_back(injectionArgument);
  
  EXPECT_EXIT(mNotWellFormedArgumentsThread->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Injection argument should start with 'with'. e.g. .withField\\(value\\).\n"
              "Error: Some injection arguments for thread "
              "systems.vos.wisey.compiler.tests.TNotWellFormedArgumentsThread are not well formed");
}

TEST_F(ThreadTest, injectTooFewArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mNotWellFormedArgumentsThread->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Received field mValue is not initialized");
}

TEST_F(ThreadTest, injectTooManyArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  NiceMock<MockExpression>* injectArgument2Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(injectArgument2Expression);
  Mock::AllowLeak(mThreadVariable);
  InjectionArgument* injectionArgument1 = new InjectionArgument("withValue",
                                                                injectArgument1Expression);
  InjectionArgument* injectionArgument2 = new InjectionArgument("withOwner",
                                                                injectArgument2Expression);
  injectionArguments.push_back(injectionArgument1);
  injectionArguments.push_back(injectionArgument2);
  
  EXPECT_EXIT(mNotWellFormedArgumentsThread->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Injector could not find field mOwner in object "
              "systems.vos.wisey.compiler.tests.TNotWellFormedArgumentsThread");
}

TEST_F(ThreadTest, injectFieldTest) {
  vector<Type*> childTypes;
  string childFullName = "systems.vos.wisey.compiler.tests.TChild";
  StructType* childStructType = StructType::create(mLLVMContext, childFullName);
  childTypes.push_back(Type::getInt64Ty(mLLVMContext));
  childStructType->setBody(childTypes);
  vector<IField*> childFields;
  Thread* childThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                          childFullName,
                                          childStructType);
  childThread->setFields(childFields, 1u);
  mContext.addThread(childThread);
  
  vector<Type*> parentTypes;
  parentTypes.push_back(Type::getInt64Ty(mLLVMContext));
  parentTypes.push_back(childThread->getLLVMType(mContext));
  string parentFullName = "systems.vos.wisey.compiler.tests.TParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  parentStructType->setBody(parentTypes);
  vector<IField*> parentFields;
  InjectionArgumentList fieldArguments;
  parentFields.push_back(new InjectedField(childThread->getOwner(),
                                           NULL,
                                           "mChild",
                                           fieldArguments));
  Thread* parentThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                           parentFullName,
                                           parentStructType);
  parentThread->setFields(parentFields, 1u);
  mContext.addThread(parentThread);
  
  InjectionArgumentList injectionArguments;
  Value* result = parentThread->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
  "(%systems.vos.wisey.compiler.tests.TParent* getelementptr "
  "(%systems.vos.wisey.compiler.tests.TParent, "
  "%systems.vos.wisey.compiler.tests.TParent* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.TParent*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TParent* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint ("
  "%systems.vos.wisey.compiler.tests.TParent* getelementptr ("
  "%systems.vos.wisey.compiler.tests.TParent, "
  "%systems.vos.wisey.compiler.tests.TParent* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (i64* getelementptr "
  "(i64, i64* null, i32 1) to i64))"
  "\n  %injectvar2 = bitcast i8* %malloccall1 to %systems.vos.wisey.compiler.tests.TChild*"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.TChild* %injectvar2 to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 ptrtoint ("
  "i64* getelementptr (i64, i64* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.TParent, "
  "%systems.vos.wisey.compiler.tests.TParent* %injectvar, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.TChild* "
  "%injectvar2, %systems.vos.wisey.compiler.tests.TChild** %2\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, printToStreamTest) {
  stringstream stringStream;
  Model* innerPublicModel = Model::newModel(PUBLIC_ACCESS, "MInnerPublicModel", NULL);
  vector<IField*> fields;
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField1"));
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField2"));
  innerPublicModel->setFields(fields, 0);
  
  vector<MethodArgument*> methodArguments;
  vector<const Model*> thrownExceptions;
  Method* method = new Method(innerPublicModel,
                              "bar",
                              AccessLevel::PUBLIC_ACCESS,
                              PrimitiveTypes::INT_TYPE,
                              methodArguments,
                              thrownExceptions,
                              NULL,
                              0);
  vector<IMethod*> methods;
  methods.push_back(method);
  innerPublicModel->setMethods(methods);
  
  Model* innerPrivateModel = Model::newModel(PRIVATE_ACCESS, "MInnerPrivateModel", NULL);
  innerPrivateModel->setFields(fields, 0);
  
  mThread->addInnerObject(innerPublicModel);
  mThread->addInnerObject(innerPrivateModel);
  mThread->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external thread systems.vos.wisey.compiler.tests.TWorker {\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  state int mFrom;\n"
               "  state int mTo;\n"
               "\n"
               "  int work();\n"
               "  int foo();\n"
               "\n"
               "external model MInnerPrivateModel {\n"
               "}\n"
               "\n"
               "external model MInnerPublicModel {\n"
               "\n"
               "  fixed int mField1;\n"
               "  fixed int mField2;\n"
               "\n"
               "  int bar();\n"
               "}\n"
               "\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(ThreadTest, createLocalVariableTest) {
  mThread->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %referenceDeclaration = alloca %systems.vos.wisey.compiler.tests.TWorker*"
  "\n  store %systems.vos.wisey.compiler.tests.TWorker* null, %systems.vos.wisey.compiler.tests.TWorker** %referenceDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mThread, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mThread->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ThreadTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mThread->getLLVMType(mContext));
  mThread->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

