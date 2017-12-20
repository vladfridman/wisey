//
//  TestFieldReferenceArrayVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldReferenceArrayVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FieldReferenceArrayVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Interface.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
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

struct FieldReferenceArrayVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mObject;
  BasicBlock* mBasicBlock;
  FieldReferenceArrayVariable* mFieldReferenceArrayVariable;
  wisey::ArrayType* mArrayType;
  Interface* mInterface;
  Node* mNode;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldReferenceArrayVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);
    
    string nodeFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, nodeFullName);
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS, nodeFullName, nodeStructType);
    mNode->setInterfaces(interfaces);

    mArrayType = new wisey::ArrayType(mInterface, 5);
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(mArrayType->getLLVMType(mContext));
    
    string objectFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList arguments;
    fields.push_back(new Field(STATE_FIELD, mArrayType, "mFoo", arguments));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType);
    mObject->setFields(fields, 1u);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldReferenceArrayVariable = new FieldReferenceArrayVariable("mFoo", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldReferenceArrayVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldReferenceArrayVariableTest, basicFieldsTest) {
  EXPECT_STREQ("mFoo", mFieldReferenceArrayVariable->getName().c_str());
  EXPECT_EQ(mArrayType, mFieldReferenceArrayVariable->getType());
}

TEST_F(FieldReferenceArrayVariableTest, generateIdentifierIRTest) {
  mFieldReferenceArrayVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceArrayVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantPointerNull::get(mInterface->getLLVMType(mContext));
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mInterface));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  arrayIndices.push_back(new FakeExpression(one, PrimitiveTypes::INT_TYPE));
  
  mFieldReferenceArrayVariable->generateAssignmentIR(mContext,
                                                     &assignToExpression,
                                                     arrayIndices,
                                                     0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %1 = getelementptr [5 x %systems.vos.wisey.compiler.tests.IInterface*], "
  "[5 x %systems.vos.wisey.compiler.tests.IInterface*]* %0, i32 0, i32 1"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %1"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %2 to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %3, i64 -1)"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.IInterface* null to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %4, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* null, "
  "%systems.vos.wisey.compiler.tests.IInterface** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceArrayVariableTest, generateAssignmentWithAutoCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantPointerNull::get(mNode->getLLVMType(mContext));
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  arrayIndices.push_back(new FakeExpression(one, PrimitiveTypes::INT_TYPE));
  
  mFieldReferenceArrayVariable->generateAssignmentIR(mContext,
                                                     &assignToExpression,
                                                     arrayIndices,
                                                     0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %1 = getelementptr [5 x %systems.vos.wisey.compiler.tests.IInterface*], "
  "[5 x %systems.vos.wisey.compiler.tests.IInterface*]* %0, i32 0, i32 1"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  %3 = getelementptr i8, i8* %2, i64 8"
  "\n  %4 = bitcast i8* %3 to %systems.vos.wisey.compiler.tests.IInterface*"
  "\n  %5 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %1"
  "\n  %6 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %5 to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %6, i64 -1)"
  "\n  %7 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %4 to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %7, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %4, "
  "%systems.vos.wisey.compiler.tests.IInterface** %1\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileSampleRunner, fieldReferenceArrayRunTest) {
  runFile("tests/samples/test_field_reference_array.yz", "2018");
}

TEST_F(TestFileSampleRunner, fieldReferenceArrayRefCountDecrementedInDestructorRunTest) {
  runFile("tests/samples/test_field_reference_array_ref_count_decremented_in_destructor.yz",
          "2018");
}

TEST_F(TestFileSampleRunner, referenceCountIncrementsOnAssignToFieldArrayElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_field_array_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_field_array_element.yz:30)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, fieldReferenceArrayInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_reference_array_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_reference_array_initialized_to_null.yz:23)\n");
}


