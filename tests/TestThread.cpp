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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/Constant.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedField.hpp"
#include "wisey/StateField.hpp"
#include "wisey/Thread.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/VariableDeclaration.hpp"
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
  LLVMFunction* mLLVMFunction;

  ThreadTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string workerFullName = "systems.vos.wisey.compiler.tests.TWorker";
    mStructType = StructType::create(mLLVMContext, workerFullName);
    mStructType->setBody(types);
    mThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS, workerFullName, mStructType, 9);
    vector<IField*> fields;
    mFromField = new StateField(PrimitiveTypes::INT_TYPE, "mFrom", 0);
    mToField = new StateField(PrimitiveTypes::INT_TYPE, "mTo", 0);
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
                         new MethodQualifiers(0),
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
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    
    IntConstant* intConstant = new IntConstant(5, 0);
    mConstant = wisey::Constant::newPublicConstant(PrimitiveTypes::INT_TYPE,
                                                   "MYCONSTANT",
                                                   intConstant);
    wisey::Constant* privateConstant = wisey::Constant::newPrivateConstant(PrimitiveTypes::INT_TYPE,
                                                                           "MYCONSTANT3",
                                                                           intConstant);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);
    constants.push_back(privateConstant);
    
    vector<const IType*> functionArgumentTypes;
    LLVMFunctionType* llvmFunctionType = new LLVMFunctionType(LLVMPrimitiveTypes::I8,
                                                              functionArgumentTypes);
    vector<const LLVMFunctionArgument*> llvmFunctionArguments;
    Block* functionBlock = new Block();
    CompoundStatement* functionCompoundStatement = new CompoundStatement(functionBlock, 0);
    mLLVMFunction = new LLVMFunction("myfunction",
                                     PUBLIC_ACCESS,
                                     llvmFunctionType,
                                     LLVMPrimitiveTypes::I8,
                                     llvmFunctionArguments,
                                     functionCompoundStatement,
                                     0);
    vector<LLVMFunction*> functions;
    functions.push_back(mLLVMFunction);

    mThread->setFields(fields, 1u);
    mThread->setMethods(methods);
    mThread->setInterfaces(interfaces);
    mThread->setConstants(constants);
    mThread->setLLVMFunctions(functions);
    mContext.addThread(mThread);
    
    vector<Type*> nonInjectableFieldThreadTypes;
    nonInjectableFieldThreadTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                                            ->getPointerTo()->getPointerTo());
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
    new InjectedField(PrimitiveTypes::INT_TYPE,
                      PrimitiveTypes::INT_TYPE,
                      "left",
                      fieldArguments,
                      mContext.getImportProfile()->getSourceFileName(),
                      3);
    nonInjectableFieldThreadFields.push_back(injectedField);
    mNonInjectableFieldThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                                  nonInjectableFieldThreadFullName,
                                                  nonInjectableFieldThreadStructType,
                                                  0);
    mNonInjectableFieldThread->setFields(nonInjectableFieldThreadFields, 1u);
    mContext.addThread(mNonInjectableFieldThread);
    
    vector<Type*> notWellFormedArgumentsThreadTypes;
    notWellFormedArgumentsThreadTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                                  true)
                                                ->getPointerTo()->getPointerTo());
    notWellFormedArgumentsThreadTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string notWellFormedArgumentsThreadFullName =
    "systems.vos.wisey.compiler.tests.TNotWellFormedArgumentsThread";
    StructType* notWellFormedArgumentsThreadStructType =
    StructType::create(mLLVMContext, notWellFormedArgumentsThreadFullName);
    notWellFormedArgumentsThreadStructType->setBody(notWellFormedArgumentsThreadTypes);
    vector<IField*> notWellFormedArgumentsThreadFields;
    notWellFormedArgumentsThreadFields.
    push_back(new ReceivedField(PrimitiveTypes::INT_TYPE, "mValue", 0));
    mNotWellFormedArgumentsThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                                      notWellFormedArgumentsThreadFullName,
                                                      notWellFormedArgumentsThreadStructType,
                                                      0);
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
    
    IConcreteObjectType::generateNameGlobal(mContext, mThread);
    IConcreteObjectType::generateShortNameGlobal(mContext, mThread);
    IConcreteObjectType::generateVTable(mContext, mThread);

    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ThreadTest() {
    delete mStringStream;
    delete mThreadVariable;
  }
};

TEST_F(ThreadTest, isPublicTest) {
  EXPECT_TRUE(mThread->isPublic());
}

TEST_F(ThreadTest, getLineTest) {
  EXPECT_EQ(9, mThread->getLine());
}

TEST_F(ThreadTest, getNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.TWorker",
               mThread->getTypeName().c_str());
}

TEST_F(ThreadTest, getShortNameTest) {
  EXPECT_STREQ("TWorker", mThread->getShortName().c_str());
}

TEST_F(ThreadTest, getVTableNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.TWorker.vtable",
               mThread->getVTableName().c_str());
}

TEST_F(ThreadTest, getLLVMTypeTest) {
  EXPECT_EQ(mStructType->getPointerTo(), mThread->getLLVMType(mContext));
}

TEST_F(ThreadTest, getInterfacesTest) {
  EXPECT_EQ(0u, mThread->getInterfaces().size());
}

TEST_F(ThreadTest, getVTableSizeTest) {
  EXPECT_EQ(1u, mThread->getVTableSize());
}

TEST_F(ThreadTest, getFieldsTest) {
  EXPECT_EQ(2u, mThread->getFields().size());
}

TEST_F(ThreadTest, getOwnerTest) {
  ASSERT_NE(nullptr, mThread->getOwner());
  EXPECT_EQ(mThread, mThread->getOwner()->getReference());
}

TEST_F(ThreadTest, getFieldIndexTest) {
  EXPECT_EQ(1u, mThread->getFieldIndex(mFromField));
  EXPECT_EQ(2u, mThread->getFieldIndex(mToField));
}

TEST_F(ThreadTest, findFeildTest) {
  EXPECT_EQ(mFromField, mThread->findField("mFrom"));
  EXPECT_EQ(mToField, mThread->findField("mTo"));
  EXPECT_EQ(nullptr, mThread->findField("mFoo"));
}

TEST_F(ThreadTest, findMethodTest) {
  EXPECT_EQ(mMethod, mThread->findMethod("work"));
  EXPECT_EQ(nullptr, mThread->findMethod("bar"));
}

TEST_F(ThreadTest, findConstantTest) {
  EXPECT_EQ(mConstant, mThread->findConstant("MYCONSTANT"));
}

TEST_F(ThreadTest, findConstantDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mThread->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Thread systems.vos.wisey.compiler.tests.TWorker "
              "does not have constant named MYCONSTANT2");
}

TEST_F(ThreadTest, findLLVMFunctionTest) {
  EXPECT_EQ(mLLVMFunction, mThread->findLLVMFunction("myfunction"));
}

TEST_F(ThreadTest, findLLVMFunctionDeathTest) {
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mThread->findLLVMFunction("nonexistingfunction"),
              ::testing::ExitedWithCode(1),
              "LLVM function nonexistingfunction not found in object systems.vos.wisey.compiler.tests.TWorker");
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
  EXPECT_FALSE(mThread->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mThread->canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mThread->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mThread->canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ThreadTest, canAutoCastToTest) {
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mThread->canAutoCastTo(mContext, mThread));
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mThread->canAutoCastTo(mContext, mThread));
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mThread->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mThread->canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ThreadTest, isTypeKindTest) {
  EXPECT_FALSE(mThread->isPrimitive());
  EXPECT_FALSE(mThread->isOwner());
  EXPECT_TRUE(mThread->isReference());
  EXPECT_FALSE(mThread->isArray());
  EXPECT_FALSE(mThread->isFunction());
  EXPECT_FALSE(mThread->isPackage());
  EXPECT_FALSE(mThread->isNative());
  EXPECT_FALSE(mThread->isPointer());
}

TEST_F(ThreadTest, isObjectTest) {
  EXPECT_FALSE(mThread->isController());
  EXPECT_FALSE(mThread->isInterface());
  EXPECT_FALSE(mThread->isModel());
  EXPECT_FALSE(mThread->isNode());
  EXPECT_TRUE(mThread->isThread());
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
  "\n  %1 = getelementptr i64, i64* %0, i64 -1"
  "\n  %refCounter = load i64, i64* %1\n";
  
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

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.TWorker.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TWorker.refCounter, %systems.vos.wisey.compiler.tests.TWorker.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.TWorker.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TWorker.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.TWorker.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TWorker.refCounter, %systems.vos.wisey.compiler.tests.TWorker.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.TWorker.refCounter, %systems.vos.wisey.compiler.tests.TWorker.refCounter* %injectvar, i32 0, i32 1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.TWorker, %systems.vos.wisey.compiler.tests.TWorker* %1, i32 0, i32 1"
  "\n  store i32 0, i32* %2"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.TWorker, %systems.vos.wisey.compiler.tests.TWorker* %1, i32 0, i32 2"
  "\n  store i32 5, i32* %3"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.TWorker* %1 to i8*"
  "\n  %5 = getelementptr i8, i8* %4, i64 0"
  "\n  %6 = bitcast i8* %5 to i32 (...)***"
  "\n  %7 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.TWorker.vtable, i32 0, i32 0, i32 0"
  "\n  %8 = bitcast i8** %7 to i32 (...)**"
  "\n  store i32 (...)** %8, i32 (...)*** %6"
  "\n";
  
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
              "Error: Injector argumet value for field 'mTo' "
              "does not match its type");
}

TEST_F(ThreadTest, injectNonInjectableTypeDeathTest) {
  InjectionArgumentList injectionArguments;
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mNonInjectableFieldThread->inject(mContext, injectionArguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type int is not injectable");
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
              "Error: Some injection arguments for injected object "
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
  childTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                       ->getPointerTo()->getPointerTo());
  childStructType->setBody(childTypes);
  vector<IField*> childFields;
  Thread* childThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                          childFullName,
                                          childStructType,
                                          0);
  childThread->setFields(childFields, 1u);
  mContext.addThread(childThread);
  
  vector<Type*> parentTypes;
  parentTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
  parentTypes.push_back(childThread->getLLVMType(mContext));
  string parentFullName = "systems.vos.wisey.compiler.tests.TParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  parentStructType->setBody(parentTypes);
  vector<IField*> parentFields;
  InjectionArgumentList fieldArguments;
  parentFields.push_back(new InjectedField(childThread->getOwner(),
                                           childThread->getOwner(),
                                           "mChild",
                                           fieldArguments,
                                           mContext.getImportProfile()->getSourceFileName(),
                                           3));
  Thread* parentThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS,
                                           parentFullName,
                                           parentStructType,
                                           0);
  parentThread->setFields(parentFields, 1u);
  mContext.addThread(parentThread);
  
  IConcreteObjectType::generateNameGlobal(mContext, childThread);
  IConcreteObjectType::generateShortNameGlobal(mContext, childThread);
  IConcreteObjectType::generateVTable(mContext, childThread);

  IConcreteObjectType::generateNameGlobal(mContext, parentThread);
  IConcreteObjectType::generateShortNameGlobal(mContext, parentThread);
  IConcreteObjectType::generateVTable(mContext, parentThread);

  InjectionArgumentList injectionArguments;
  Value* result = parentThread->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.TParent.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TParent.refCounter, %systems.vos.wisey.compiler.tests.TParent.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.TParent.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.TParent.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.TParent.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TParent.refCounter, %systems.vos.wisey.compiler.tests.TParent.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.TParent.refCounter, %systems.vos.wisey.compiler.tests.TParent.refCounter* %injectvar, i32 0, i32 1"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.TChild.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TChild.refCounter, %systems.vos.wisey.compiler.tests.TChild.refCounter* null, i32 1) to i64))"
  "\n  %injectvar2 = bitcast i8* %malloccall1 to %systems.vos.wisey.compiler.tests.TChild.refCounter*"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.TChild.refCounter* %injectvar2 to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.TChild.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.TChild.refCounter, %systems.vos.wisey.compiler.tests.TChild.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.TChild.refCounter, %systems.vos.wisey.compiler.tests.TChild.refCounter* %injectvar2, i32 0, i32 1"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.TChild* %3 to i8*"
  "\n  %5 = getelementptr i8, i8* %4, i64 0"
  "\n  %6 = bitcast i8* %5 to i32 (...)***"
  "\n  %7 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.TChild.vtable, i32 0, i32 0, i32 0"
  "\n  %8 = bitcast i8** %7 to i32 (...)**"
  "\n  store i32 (...)** %8, i32 (...)*** %6"
  "\n  %9 = getelementptr %systems.vos.wisey.compiler.tests.TParent, %systems.vos.wisey.compiler.tests.TParent* %1, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.TChild* %3, %systems.vos.wisey.compiler.tests.TChild** %9"
  "\n  %10 = bitcast %systems.vos.wisey.compiler.tests.TParent* %1 to i8*"
  "\n  %11 = getelementptr i8, i8* %10, i64 0"
  "\n  %12 = bitcast i8* %11 to i32 (...)***"
  "\n  %13 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.TParent.vtable, i32 0, i32 0, i32 0"
  "\n  %14 = bitcast i8** %13 to i32 (...)**"
  "\n  store i32 (...)** %14, i32 (...)*** %12"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ThreadTest, printToStreamTest) {
  stringstream stringStream;
  Model* innerPublicModel = Model::newModel(PUBLIC_ACCESS, "MInnerPublicModel", NULL, 0);
  vector<IField*> fields;
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField1", 0));
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField2", 0));
  innerPublicModel->setFields(fields, 1u);
  
  vector<MethodArgument*> methodArguments;
  vector<const Model*> thrownExceptions;
  Method* method = new Method(innerPublicModel,
                              "bar",
                              AccessLevel::PUBLIC_ACCESS,
                              PrimitiveTypes::INT_TYPE,
                              methodArguments,
                              thrownExceptions,
                              new MethodQualifiers(0),
                              NULL,
                              0);
  vector<IMethod*> methods;
  methods.push_back(method);
  innerPublicModel->setMethods(methods);
  
  Model* innerPrivateModel = Model::newModel(PRIVATE_ACCESS, "MInnerPrivateModel", NULL, 0);
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
               "}\n"
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
  IField* field = new StateField(mThread, "mField", 0);
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

