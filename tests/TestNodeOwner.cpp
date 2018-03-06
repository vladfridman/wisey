//
//  TestNodeOwner.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeOwner}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/NodeOwner.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NodeOwnerTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mComplicatedNode;
  Node* mSimpleNode;
  Node* mSimplerNode;
  Interface* mComplicatedElementInterface;
  Interface* mElementInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  StructType* mStructType;
  Field* mLeftField;
  Field* mRightField;
  Field* mAreaField;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  NodeOwnerTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);

    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    VariableList elementInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> elementObjectElements;
    vector<IModelTypeSpecifier*> elementThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* getElementSignature =
    new MethodSignatureDeclaration(intSpecifier,
                                   "getElement",
                                   elementInterfaceMethodArguments,
                                   elementThrownExceptions);
    elementObjectElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> elementParentInterfaces;
    mElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                elementInterfaceFullName,
                                                elementInterfaceStructType,
                                                elementParentInterfaces,
                                                elementObjectElements);
    mContext.addInterface(mElementInterface);
    mElementInterface->buildMethods(mContext);
    
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    vector<IObjectElementDeclaration*> complicatedElementObjectElements;
    complicatedElementObjectElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> complicatedElementParentInterfaces;
    InterfaceTypeSpecifier* elementInterfaceTypeSpecifier = new InterfaceTypeSpecifier(NULL,
                                                                                       "IElement");
    complicatedElementParentInterfaces.push_back(elementInterfaceTypeSpecifier);
    mComplicatedElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                           complicatedElementFullName,
                                                           complicatedElementIinterfaceStructType,
                                                           complicatedElementParentInterfaces,
                                                           complicatedElementObjectElements);
    mContext.addInterface(mComplicatedElementInterface);
    mComplicatedElementInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> objectElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceMethodArguments,
                                     objectThrownExceptions);
    objectElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectElements);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string complicatedNodeFullName = "systems.vos.wisey.compiler.tests.NComplicatedNode";
    mStructType = StructType::create(mLLVMContext, complicatedNodeFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    mLeftField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mLeft", fieldArguments);
    mRightField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mRight", fieldArguments);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    IMethod* method = new Method(mComplicatedNode,
                                 "getElement",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 thrownExceptions,
                                 NULL,
                                 0);
    vector<IMethod*> methods;
    methods.push_back(method);
    IMethod* fooMethod = new Method(mComplicatedNode,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    mComplicatedNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                     complicatedNodeFullName,
                                     mStructType);
    mComplicatedNode->setFields(fields, interfaces.size() + 1);
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(Type::getInt64Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<Field*> simpleNodeFields;
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         PrimitiveTypes::INT_TYPE,
                                         NULL,
                                         "mLeft",
                                          fieldArguments));
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         PrimitiveTypes::INT_TYPE,
                                         NULL,
                                         "mRight",
                                         fieldArguments));
    mAreaField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mArea", fieldArguments);
    simpleNodeFields.push_back(mAreaField);
    mSimpleNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                simpleNodeFullName,
                                simpleNodeStructType);
    mSimpleNode->setFields(simpleNodeFields, 1u);
    mContext.addNode(mSimpleNode);
    
    vector<Type*> simplerNodeTypes;
    simplerNodeTypes.push_back(Type::getInt64Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simplerNodeFullName = "systems.vos.wisey.compiler.tests.NSimplerNode";
    StructType* simplerNodeStructType = StructType::create(mLLVMContext, simplerNodeFullName);
    simplerNodeStructType->setBody(simplerNodeTypes);
    vector<Field*> simplerNodeFields;
    simplerNodeFields.push_back(new Field(FIXED_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          NULL,
                                          "mLeft",
                                          fieldArguments));
    simplerNodeFields.push_back(new Field(FIXED_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          NULL, 
                                          "mRight",
                                          fieldArguments));
    mSimplerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                 simplerNodeFullName,
                                 simplerNodeStructType);
    mSimplerNode->setFields(simplerNodeFields, 1u);
    mContext.addNode(mSimplerNode);
    IConcreteObjectType::generateNameGlobal(mContext, mSimplerNode);
    IConcreteObjectType::generateShortNameGlobal(mContext, mSimplerNode);
    IConcreteObjectType::generateVTable(mContext, mSimplerNode);

    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDeclaration*> vehicleObjectElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleObjectElements);
    
    IConcreteObjectType::generateNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateShortNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    
    mVehicleInterface->buildMethods(mContext);
    
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
  
  ~NodeOwnerTest() {
    delete mField1Expression;
    delete mField2Expression;
  }
};

TEST_F(NodeOwnerTest, getObjectTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getObjectType(), mComplicatedNode);
}

TEST_F(NodeOwnerTest, getNameTest) {
  EXPECT_STREQ(mComplicatedNode->getOwner()->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode*");
}

TEST_F(NodeOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getLLVMType(mContext),
            mComplicatedNode->getLLVMType(mContext));
}

TEST_F(NodeOwnerTest, getDestructorFunctionTest) {
  Function* result = mSimplerNode->getOwner()->getDestructorFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(mLLVMContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  EXPECT_EQ(functionType, result->getFunctionType());
}

TEST_F(NodeOwnerTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mContext, mElementInterface));
}

TEST_F(NodeOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mContext, mElementInterface));
}

TEST_F(NodeOwnerTest, castToFirstInterfaceTest) {
  PointerType* type = mComplicatedNode->getOwner()->getLLVMType(mContext);
  ConstantPointerNull* pointer = ConstantPointerNull::get(type);
  mComplicatedNode->getOwner()->castTo(mContext,
                                       pointer,
                                       mComplicatedElementInterface->getOwner(),
                                       0);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IComplicatedElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));

  mComplicatedNode->getOwner()->castTo(mContext, pointer, mElementInterface->getOwner(), 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 16"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, isPrimitiveTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isPrimitive());
}

TEST_F(NodeOwnerTest, isOwnerTest) {
  EXPECT_TRUE(mSimpleNode->getOwner()->isOwner());
}

TEST_F(NodeOwnerTest, isReferenceTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isReference());
}

TEST_F(NodeOwnerTest, isArrayTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isArray());
}

TEST_F(NodeOwnerTest, isFunctionTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isFunction());
}

TEST_F(NodeOwnerTest, isPackageTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isPackage());
}

TEST_F(NodeOwnerTest, isObjectTest) {
  EXPECT_FALSE(mSimpleNode->getOwner()->isController());
  EXPECT_FALSE(mSimpleNode->getOwner()->isInterface());
  EXPECT_FALSE(mSimpleNode->getOwner()->isModel());
  EXPECT_TRUE(mSimpleNode->getOwner()->isNode());
  EXPECT_FALSE(mSimpleNode->getOwner()->isThread());
}

TEST_F(NodeOwnerTest, createLocalVariableTest) {
  mComplicatedNode->getOwner()->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %ownerDeclaration = alloca %systems.vos.wisey.compiler.tests.NComplicatedNode*"
  "\n  store %systems.vos.wisey.compiler.tests.NComplicatedNode* null, %systems.vos.wisey.compiler.tests.NComplicatedNode** %ownerDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  InjectionArgumentList injectionArgumentList;
  Field* field = new Field(FIXED_FIELD,
                           mComplicatedNode->getOwner(),
                           NULL,
                           "mField",
                           injectionArgumentList);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mComplicatedNode->getOwner()->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(NodeOwnerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mComplicatedNode->getOwner()->getLLVMType(mContext));
  mComplicatedNode->getOwner()->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %parameterObjectPointer = alloca %systems.vos.wisey.compiler.tests.NComplicatedNode*"
  "\n  store %systems.vos.wisey.compiler.tests.NComplicatedNode* null, %systems.vos.wisey.compiler.tests.NComplicatedNode** %parameterObjectPointer\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
