//
//  TestFieldOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "FieldOwnerVariable.hpp"
#include "IExpression.hpp"
#include "IInterfaceTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "ParameterReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldOwnerVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mObject;
  Node* mNode;
  Interface* mInterface;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  Function* mFunction;
  FieldOwnerVariable* mFieldOwnerVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements,
                                         mContext.getImportProfile(),
                                         0);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string modelFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          modelFullName,
                          modelStructType,
                          mContext.getImportProfile(),
                          0);
    mNode->setInterfaces(interfaces);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mNode->getOwner()->getLLVMType(mContext));
    types.push_back(mInterface->getOwner()->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.NObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mNode->getOwner(), "foo", 0));
    fields.push_back(new StateField(mInterface->getOwner(), "bar", 0));
    mObject = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                            objectFullName,
                            objectStructType,
                            mContext.getImportProfile(),
                            0);
    mObject->setFields(mContext, fields, 1u);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mFieldOwnerVariable = new FieldOwnerVariable("foo", mObject, 0);
    
    mInterface->declareInterfaceTypeName(mContext);
    IConcreteObjectType::declareTypeNameGlobal(mContext, mNode);
    IConcreteObjectType::defineVTable(mContext, mNode);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldOwnerVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldOwnerVariable->getName().c_str());
  EXPECT_EQ(mNode->getOwner(), mFieldOwnerVariable->getType());
  EXPECT_TRUE(mFieldOwnerVariable->isField());
  EXPECT_FALSE(mFieldOwnerVariable->isStatic());
}

TEST_F(FieldOwnerVariableTest, generateIdentifierIRTest) {
  mFieldOwnerVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mEntryBlock;
  
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %foo = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, generateIdentifierReferenceIRTest) {
  mFieldOwnerVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mEntryBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  llvm::PointerType* llvmType = (llvm::PointerType*) mNode->getOwner()->getLLVMType(mContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  mFieldOwnerVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected = string() +
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.NNode* %1 to i8*"
  "\n  invoke void @systems.vos.wisey.compiler.tests.NNode.destructor(i8* %2, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %3 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %4 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %3, { i8*, i32 }* %4"
  "\n  %5 = getelementptr { i8*, i32 }, { i8*, i32 }* %4, i32 0, i32 0"
  "\n  %6 = load i8*, i8** %5"
  "\n  %7 = call i8* @__cxa_get_exception_ptr(i8* %6)"
  "\n  %8 = getelementptr i8, i8* %7, i64 8"
  "\n  %9 = icmp eq %systems.vos.wisey.compiler.tests.NObject* null, null"
  "\n  br i1 %9, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %cleanup.cont"
  "\n  resume { i8*, i32 } %3"
  "\n"
  "\nif.notnull:                                       ; preds = %cleanup.cont"
  "\n  %10 = bitcast %systems.vos.wisey.compiler.tests.NObject* null to i64*"
  "\n  %11 = getelementptr i64, i64* %10, i64 -1"
  "\n  %count = load i64, i64* %11"
  "\n  %12 = add i64 %count, -1"
  "\n  store i64 %12, i64* %11"
  "\n  br label %if.end"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, %systems.vos.wisey.compiler.tests.NNode** %0"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, generateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  llvm::PointerType* llvmType = mNode->getOwner()->getLLVMType(mContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  FieldOwnerVariable* ownerFieldVariable = new FieldOwnerVariable("bar", mObject, 0);
  ownerFieldVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  BranchInst::Create(mEntryBlock, mDeclareBlock);

  *mStringStream << *mFunction;
  string expected = string() +
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IInterface*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 2"
  "\n  %4 = load %systems.vos.wisey.compiler.tests.IInterface*, %systems.vos.wisey.compiler.tests.IInterface** %3"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %4 to i8*"
  "\n  invoke void @__destroyObjectOwnerFunction(i8* %5, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %6 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %7 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %6, { i8*, i32 }* %7"
  "\n  %8 = getelementptr { i8*, i32 }, { i8*, i32 }* %7, i32 0, i32 0"
  "\n  %9 = load i8*, i8** %8"
  "\n  %10 = call i8* @__cxa_get_exception_ptr(i8* %9)"
  "\n  %11 = getelementptr i8, i8* %10, i64 8"
  "\n  %12 = icmp eq %systems.vos.wisey.compiler.tests.NObject* null, null"
  "\n  br i1 %12, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %cleanup.cont"
  "\n  resume { i8*, i32 } %6"
  "\n"
  "\nif.notnull:                                       ; preds = %cleanup.cont"
  "\n  %13 = bitcast %systems.vos.wisey.compiler.tests.NObject* null to i64*"
  "\n  %14 = getelementptr i64, i64* %13, i64 -1"
  "\n  %count = load i64, i64* %14"
  "\n  %15 = add i64 %count, -1"
  "\n  store i64 %15, i64* %14"
  "\n  br label %if.end"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %2, %systems.vos.wisey.compiler.tests.IInterface** %3"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, setToNullTest) {
  mFieldOwnerVariable->setToNull(mContext, 0);
  
  *mStringStream << *mEntryBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, %systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, %systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, variableSetToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_object_field_variable_set_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_object_field_variable_set_to_null.yz:20)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, stateOwnerFieldSetToNullTest) {
  runFile("tests/samples/test_state_owner_field_set_to_null.yz", 1);
}

TEST_F(TestFileRunner, objectFieldVariableSetToAnotherTest) {
  runFile("tests/samples/test_object_field_variable_set_to_another.yz", 7);
}

TEST_F(TestFileRunner, assignFieldOwnerToParameterOwnerCompileTest) {
  compileFile("tests/samples/test_assign_field_owner_to_parameter_owner.yz");
}

TEST_F(TestFileRunner, assignFieldOwnerToNullCompileTest) {
  compileFile("tests/samples/test_assign_field_owner_to_null.yz");
}

TEST_F(TestFileRunner, objectFieldVariableNullComplicatedTest) {
  compileAndRunFileCheckOutput("tests/samples/test_object_field_variable_null_complicated.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.getValue(tests/samples/test_object_field_variable_null_complicated.yz:26)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_object_field_variable_null_complicated.yz:36)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, destructorCalledOnAssignFieldOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_destructor_called_on_assign_field_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "car is destoyed\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, fieldPointerOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_pointer_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mPointer\n"
                                        "destructor systems.vos.wisey.compiler.tests.MModel\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MModel\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, modelReturnsObjectOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_model_returns_object_owner.yz",
                    1,
                    "tests/samples/test_model_returns_object_owner.yz\\(12\\): Error: "
                    "Setting an unassignable owner field 'mPart' of object systems.vos.wisey.compiler.tests.MCar to null possibly by returning its value");
}
