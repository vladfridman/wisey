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
  
  NodeOwnerTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::run(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    VariableList elementInterfaceMethodArguments;
    vector<MethodSignatureDeclaration*> elementInterfaceMethods;
    vector<ModelTypeSpecifier*> elementThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    MethodSignatureDeclaration* getElementSignature =
    new MethodSignatureDeclaration(intSpecifier,
                                   "getElement",
                                   elementInterfaceMethodArguments,
                                   elementThrownExceptions);
    elementInterfaceMethods.push_back(getElementSignature);
    vector<InterfaceTypeSpecifier*> elementParentInterfaces;
    mElementInterface = new Interface(elementInterfaceFullName,
                                      elementInterfaceStructType,
                                      elementParentInterfaces,
                                      elementInterfaceMethods);
    mContext.addInterface(mElementInterface);
    mElementInterface->buildMethods(mContext);
    
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    vector<MethodSignatureDeclaration*> complicatedElementInterfaceMethods;
    complicatedElementInterfaceMethods.push_back(getElementSignature);
    vector<InterfaceTypeSpecifier*> complicatedElementParentInterfaces;
    vector<string> package;
    InterfaceTypeSpecifier* elementInterfaceTypeSpecifier =
    new InterfaceTypeSpecifier(package, "IElement");
    complicatedElementParentInterfaces.push_back(elementInterfaceTypeSpecifier);
    mComplicatedElementInterface = new Interface(complicatedElementFullName,
                                                 complicatedElementIinterfaceStructType,
                                                 complicatedElementParentInterfaces,
                                                 complicatedElementInterfaceMethods);
    mContext.addInterface(mComplicatedElementInterface);
    mComplicatedElementInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<MethodSignatureDeclaration*> objectInterfaceMethods;
    vector<ModelTypeSpecifier*> objectThrownExceptions;
    MethodSignatureDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceMethodArguments,
                                     objectThrownExceptions);
    objectInterfaceMethods.push_back(methodBarSignature);
    vector<InterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = new Interface(objectFullName,
                                     objectInterfaceStructType,
                                     objectParentInterfaces,
                                     objectInterfaceMethods);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string complicatedNodeFullName = "systems.vos.wisey.compiler.tests.NComplicatedNode";
    mStructType = StructType::create(mLLVMContext, complicatedNodeFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    mLeftField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mLeft", 0, fieldArguments);
    mRightField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "mRight", 1, fieldArguments);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    IMethod* method = new Method("getElement",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 thrownExceptions,
                                 NULL);
    vector<IMethod*> methods;
    methods.push_back(method);
    IMethod* fooMethod = new Method("foo",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    mComplicatedNode = new Node(complicatedNodeFullName, mStructType);
    mComplicatedNode->setFields(fields);
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simpleNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<Field*> simpleNodeFields;
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         PrimitiveTypes::INT_TYPE,
                                         "mLeft",
                                         0,
                                         fieldArguments));
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         PrimitiveTypes::INT_TYPE,
                                         "mRight",
                                         1,
                                         fieldArguments));
    mAreaField = new Field(STATE_FIELD, PrimitiveTypes::INT_TYPE, "mArea", 2, fieldArguments);
    simpleNodeFields.push_back(mAreaField);
    mSimpleNode = new Node(simpleNodeFullName, simpleNodeStructType);
    mSimpleNode->setFields(simpleNodeFields);
    mContext.addNode(mSimpleNode);
    
    vector<Type*> simplerNodeTypes;
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    simplerNodeTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string simplerNodeFullName = "systems.vos.wisey.compiler.tests.NSimplerNode";
    StructType* simplerNodeStructType = StructType::create(mLLVMContext, simplerNodeFullName);
    simplerNodeStructType->setBody(simplerNodeTypes);
    vector<Field*> simplerNodeFields;
    simplerNodeFields.push_back(new Field(STATE_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          "mLeft",
                                          0,
                                          fieldArguments));
    simplerNodeFields.push_back(new Field(STATE_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          "mRight",
                                          1,
                                          fieldArguments));
    mSimplerNode = new Node(simplerNodeFullName, simplerNodeStructType);
    mSimplerNode->setFields(simplerNodeFields);
    mContext.addNode(mSimplerNode);
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<InterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<MethodSignatureDeclaration*> vehicleMethods;
    mVehicleInterface = new Interface(vehicleFullName,
                                      vehicleInterfaceStructType,
                                      vehicleParentInterfaces,
                                      vehicleMethods);
    
    IConcreteObjectType::generateNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mField1Expression, generateIR(_)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    Value* field2Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mField2Expression, generateIR(_)).WillByDefault(Return(field2Value));
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
  EXPECT_EQ(mComplicatedNode->getOwner()->getObject(), mComplicatedNode);
}

TEST_F(NodeOwnerTest, getNameTest) {
  EXPECT_STREQ(mComplicatedNode->getOwner()->getName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode*");
}

TEST_F(NodeOwnerTest, getLLVMTypeTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getLLVMType(mLLVMContext),
            mComplicatedNode->getLLVMType(mLLVMContext)->getPointerElementType());
}

TEST_F(NodeOwnerTest, getTypeKindTest) {
  EXPECT_EQ(mComplicatedNode->getOwner()->getTypeKind(), NODE_OWNER_TYPE);
}

TEST_F(NodeOwnerTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canCastTo(mElementInterface));
}

TEST_F(NodeOwnerTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mSimplerNode->getOwner()));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mVehicleInterface->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mComplicatedNode->getOwner()));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mElementInterface->getOwner()));
  
  EXPECT_FALSE(mComplicatedNode->getOwner()->canAutoCastTo(mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->getOwner()->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->getOwner()->canAutoCastTo(mElementInterface));
}

TEST_F(NodeOwnerTest, castToFirstInterfaceTest) {
  PointerType* type = mComplicatedNode->getOwner()->getLLVMType(mLLVMContext)->getPointerTo();
  ConstantPointerNull* pointer = ConstantPointerNull::get(type);
  mComplicatedNode->getOwner()->castTo(mContext, pointer, mComplicatedElementInterface->getOwner());
  EXPECT_EQ(mBasicBlock->size(), 1u);
  
  *mStringStream << *mBasicBlock->begin();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode** null to "
               "%systems.vos.wisey.compiler.tests.IComplicatedElement**");
  mStringBuffer.clear();
}

TEST_F(NodeOwnerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
    ConstantPointerNull::get(mComplicatedNode->getLLVMType(mLLVMContext));

  mComplicatedNode->getOwner()->castTo(mContext, pointer, mElementInterface->getOwner());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NComplicatedNode*, "
  "%systems.vos.wisey.compiler.tests.NComplicatedNode** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 8"
  "\n  %3 = alloca %systems.vos.wisey.compiler.tests.IElement*"
  "\n  %4 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.IElement*"
  "\n  store %systems.vos.wisey.compiler.tests.IElement* %4, "
  "%systems.vos.wisey.compiler.tests.IElement** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
