//
//  TestFieldOwnerArrayVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldOwnerArrayVariable}
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
#include "wisey/FieldOwnerArrayVariable.hpp"
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

struct FieldOwnerArrayVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mObject;
  BasicBlock* mBasicBlock;
  FieldOwnerArrayVariable* mFieldOwnerArrayVariable;
  wisey::ArrayType* mArrayType;
  Interface* mInterface;
  Node* mNode;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldOwnerArrayVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mArrayType = new wisey::ArrayType(mInterface->getOwner(), 5);
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
    
    mFieldOwnerArrayVariable = new FieldOwnerArrayVariable("mFoo", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldOwnerArrayVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldOwnerArrayVariableTest, basicFieldsTest) {
  EXPECT_STREQ("mFoo", mFieldOwnerArrayVariable->getName().c_str());
  EXPECT_EQ(mArrayType, mFieldOwnerArrayVariable->getType());
}

TEST_F(FieldOwnerArrayVariableTest, generateIdentifierIRTest) {
  mFieldOwnerArrayVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, "
  "%systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerArrayVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantPointerNull::get(mInterface->getLLVMType(mContext));
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mInterface->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  arrayIndices.push_back(new FakeExpression(one, PrimitiveTypes::INT_TYPE));
  
  mFieldOwnerArrayVariable->generateAssignmentIR(mContext,
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
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.IInterface(i8* %3)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* null, "
  "%systems.vos.wisey.compiler.tests.IInterface** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerArrayVariableTest, generateAssignmentWithAutoCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantPointerNull::get(mNode->getLLVMType(mContext));
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;
  llvm::Constant* one = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  arrayIndices.push_back(new FakeExpression(one, PrimitiveTypes::INT_TYPE));
  
  mFieldOwnerArrayVariable->generateAssignmentIR(mContext,
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
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.IInterface(i8* %6)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %4, "
  "%systems.vos.wisey.compiler.tests.IInterface** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

// TODO: make this work
//TEST_F(TestFileSampleRunner, fieldOwnerArrayRunTest) {
//  runFile("tests/samples/test_field_owner_array.yz", "2018");
//}

// TODO: make this work
//TEST_F(TestFileSampleRunner, fieldOwnerArrayDestructorsAreCalledRunTest) {
//  runFileCheckOutputWithDestructorDebug("tests/samples/test_field_owner_array.yz",
//                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
//                                        "destructor systems.vos.wisey.compiler.tests.CController\n"
//                                        "destructor systems.vos.wisey.compiler.tests.MCar\n",
//                                        "");
//}

TEST_F(TestFileSampleRunner, fieldArrayElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.getValue(tests/samples/test_field_array_element_is_nulled_on_owner_transfer.yz:21)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_element_is_nulled_on_owner_transfer.yz:31)\n");
}

TEST_F(TestFileSampleRunner, ownerVariableIsNulledOnOwnerTransferToFieldArrayRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_owner_transfer_to_field_array.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.setValue(tests/samples/test_owner_variable_is_nulled_on_owner_transfer_to_field_array.yz:17)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_owner_transfer_to_field_array.yz:26)\n");
}

TEST_F(TestFileSampleRunner, fieldOwnerArrayInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_owner_array_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_owner_array_initialized_to_null.yz:28)\n");
}

