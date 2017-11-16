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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
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

struct FieldOwnerVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mObject;
  Node* mNode;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  FieldOwnerVariable* mFieldOwnerVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface(interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string modelFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mNode = Node::newNode(modelFullName, modelStructType);
    mNode->setInterfaces(interfaces);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(mNode->getOwner()->getLLVMType(mLLVMContext));
    types.push_back(mInterface->getOwner()->getLLVMType(mLLVMContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.NObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(STATE_FIELD, mNode->getOwner(), "foo", fieldArguments));
    fields.push_back(new Field(STATE_FIELD, mInterface->getOwner(), "bar", fieldArguments));
    mObject = Node::newNode(objectFullName, objectStructType);
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
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new ParameterReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldOwnerVariable = new FieldOwnerVariable("foo", mObject);
    
    vector<Type*> argumentTypes;
    argumentTypes.push_back(mNode->getLLVMType(mLLVMContext));
    ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
    FunctionType* destructorFunctionType = FunctionType::get(Type::getVoidTy(mLLVMContext),
                                                             argTypesArray,
                                                             false);
    Function::Create(destructorFunctionType,
                     GlobalValue::InternalLinkage,
                     "destructor." + mNode->getName(),
                     mContext.getModule());

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldOwnerVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mFieldOwnerVariable->getName().c_str(), "foo");
  EXPECT_EQ(mFieldOwnerVariable->getType(), mNode->getOwner());
}

TEST_F(FieldOwnerVariableTest, ownerFieldVariableGenerateIdentifierIRTest) {
  mFieldOwnerVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, ownerFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = (PointerType*) mNode->getOwner()->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mFieldOwnerVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** %0"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.NNode("
  "%systems.vos.wisey.compiler.tests.NNode* %1)"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, ownerFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = mNode->getOwner()->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  FieldOwnerVariable* ownerFieldVariable = new FieldOwnerVariable("bar", mObject);
  ownerFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IInterface*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 2"
  "\n  %4 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %3"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.IInterface("
  "%systems.vos.wisey.compiler.tests.IInterface* %4)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %2, "
  "%systems.vos.wisey.compiler.tests.IInterface** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, setToNullTest) {
  mFieldOwnerVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldOwnerVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mFieldOwnerVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToNullTest) {
  expectFailCompile("tests/samples/test_object_field_variable_set_to_null.yz",
                    1,
                    "Error: Variable 'mModel' was previously cleared and can not be used");
}

TEST_F(TestFileSampleRunner, stateOwnerFieldSetToNullTest) {
  runFile("tests/samples/test_state_owner_field_set_to_null.yz", "1");
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToAnotherTest) {
  runFile("tests/samples/test_object_field_variable_set_to_another.yz", "7");
}

TEST_F(TestFileSampleRunner, assignFieldOwnerToParameterOwnerCompileTest) {
  compileFile("tests/samples/test_assign_field_owner_to_parameter_owner.yz");
}

TEST_F(TestFileSampleRunner, assignFieldOwnerToNullCompileTest) {
  compileFile("tests/samples/test_assign_field_owner_to_null.yz");
}

TEST_F(TestFileSampleRunner, objectFieldVariableNullComplicatedTest) {
  compileAndRunFileCheckOutput("tests/samples/test_object_field_variable_null_complicated.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.getValue(tests/samples/test_object_field_variable_null_complicated.yz:26)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_object_field_variable_null_complicated.yz:36)\n"
                               "  at wisey.lang.CProgramRunner.run(wisey/lang/CProgramRunner.yz:13)\n");
}

TEST_F(TestFileSampleRunner, destructorCalledOnAssignFieldOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_destructor_called_on_assign_field_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "car is destoyed\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
