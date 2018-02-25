//
//  TestNode.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Node}
//

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
#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/Constant.hpp"
#include "wisey/Field.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/Node.hpp"
#include "wisey/NodeTypeSpecifier.hpp"
#include "wisey/NullType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
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

struct NodeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Node* mComplicatedNode;
  Node* mSimpleNode;
  Node* mSimplerNode;
  Node* mOwnerNode;
  Model* mReferenceModel;
  Interface* mComplicatedElementInterface;
  Interface* mElementInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  IMethod* mMethod;
  StructType* mStructType;
  Field* mLeftField;
  Field* mRightField;
  BasicBlock* mBasicBlock;
  NiceMock<MockExpression>* mField1Expression;
  NiceMock<MockExpression>* mField2Expression;
  wisey::Constant* mConstant;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  NodeTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mField1Expression(new NiceMock<MockExpression>()),
  mField2Expression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    string elementInterfaceFullName = "systems.vos.wisey.compiler.tests.IElement";
    StructType* elementInterfaceStructType = StructType::create(mLLVMContext,
                                                                elementInterfaceFullName);
    VariableList elementInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> elementInterfaceElements;
    vector<IModelTypeSpecifier*> elementThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* getElementSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "getElement",
                                     elementInterfaceMethodArguments,
                                     elementThrownExceptions);
    elementInterfaceElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> elementParentInterfaces;
    mElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                elementInterfaceFullName,
                                                elementInterfaceStructType,
                                                elementParentInterfaces,
                                                elementInterfaceElements);
    mContext.addInterface(mElementInterface);
    mElementInterface->buildMethods(mContext);
    
    string complicatedElementFullName = "systems.vos.wisey.compiler.tests.IComplicatedElement";
    StructType* complicatedElementIinterfaceStructType =
    StructType::create(mLLVMContext, complicatedElementFullName);
    vector<IObjectElementDeclaration*> complicatedElementInterfaceElements;
    complicatedElementInterfaceElements.push_back(getElementSignature);
    vector<IInterfaceTypeSpecifier*> complicatedElementParentInterfaces;
    InterfaceTypeSpecifier* elementInterfaceSpecifier = new InterfaceTypeSpecifier(NULL,
                                                                                   "IElement");
    complicatedElementParentInterfaces.push_back(elementInterfaceSpecifier);
    mComplicatedElementInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                           complicatedElementFullName,
                                                           complicatedElementIinterfaceStructType,
                                                           complicatedElementParentInterfaces,
                                                           complicatedElementInterfaceElements);
    mContext.addInterface(mComplicatedElementInterface);
    mComplicatedElementInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceMethodArguments,
                                     objectThrownExceptions);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements);
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
    mComplicatedNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                     complicatedNodeFullName,
                                     mStructType);
   InjectionArgumentList arguments;
    mLeftField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE,  NULL, "mLeft", arguments);
    mRightField = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE,  NULL, "mRight", arguments);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method(mComplicatedNode,
                         "getElement",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL,
                         0);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    Method* fooMethod = new Method(mComplicatedNode,
                                   "foo",
                                   AccessLevel::PRIVATE_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL,
                                   0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mComplicatedElementInterface);
    interfaces.push_back(mObjectInterface);
    
    IntConstant* intConstant = new IntConstant(5);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    "MYCONSTANT",
                                    intConstant);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);
    
    mComplicatedNode->setFields(fields, interfaces.size() + 1);
    mComplicatedNode->setMethods(methods);
    mComplicatedNode->setInterfaces(interfaces);
    mComplicatedNode->setConstants(constants);
    
    vector<Type*> ownerTypes;
    ownerTypes.push_back(Type::getInt64Ty(mLLVMContext));
    string ownerFullName = "systems.vos.wisey.compiler.tests.NOwner";
    StructType* ownerStructType = StructType::create(mLLVMContext, ownerFullName);
    ownerStructType->setBody(ownerTypes);
    mOwnerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS, ownerFullName, ownerStructType);
    mContext.addNode(mOwnerNode);
    
    vector<Type*> referenceTypes;
    referenceTypes.push_back(Type::getInt64Ty(mLLVMContext));
    string referenceFullName = "systems.vos.wisey.compiler.tests.MReference";
    StructType* referenceStructType = StructType::create(mLLVMContext, referenceFullName);
    referenceStructType->setBody(referenceTypes);
    mReferenceModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                      referenceFullName,
                                      referenceStructType);
    mContext.addModel(mReferenceModel);
    
    vector<Type*> simpleNodeTypes;
    simpleNodeTypes.push_back(Type::getInt64Ty(mLLVMContext));
    simpleNodeTypes.push_back(mOwnerNode->getLLVMType(mContext));
    simpleNodeTypes.push_back(mReferenceModel->getLLVMType(mContext));
    string simpleNodeFullName = "systems.vos.wisey.compiler.tests.NSimpleNode";
    StructType* simpleNodeStructType = StructType::create(mLLVMContext, simpleNodeFullName);
    simpleNodeStructType->setBody(simpleNodeTypes);
    vector<Field*> simpleNodeFields;
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         mOwnerNode->getOwner(),
                                         NULL,
                                         "mOwner",
                                        arguments));
    simpleNodeFields.push_back(new Field(FIXED_FIELD,
                                         mReferenceModel,
                                         NULL,
                                         "mReference",
                                         arguments));
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
                                          arguments));
    simplerNodeFields.push_back(new Field(FIXED_FIELD,
                                          PrimitiveTypes::INT_TYPE,
                                          NULL,
                                          "mRight",
                                          arguments));
    mSimplerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                                 simplerNodeFullName,
                                 simplerNodeStructType);
    mSimplerNode->setFields(simplerNodeFields, 1u);
    mContext.addNode(mSimplerNode);
    
    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDeclaration*> vehicleElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleElements);
    mContext.addInterface(mVehicleInterface);
    mVehicleInterface->buildMethods(mContext);

    IConcreteObjectType::generateNameGlobal(mContext, mOwnerNode);
    IConcreteObjectType::generateShortNameGlobal(mContext, mOwnerNode);
    IConcreteObjectType::generateVTable(mContext, mOwnerNode);

    IConcreteObjectType::generateShortNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateNameGlobal(mContext, mSimpleNode);
    IConcreteObjectType::generateVTable(mContext, mSimpleNode);
    
    Value* field1Value = ConstantPointerNull::get(mOwnerNode->getOwner()->getLLVMType(mContext));
    ON_CALL(*mField1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
    ON_CALL(*mField1Expression, getType(_)).WillByDefault(Return(mOwnerNode->getOwner()));
    Value* field2Value = ConstantPointerNull::get(mReferenceModel->getLLVMType(mContext));
    ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
    ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(mReferenceModel));

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
  
  ~NodeTest() {
    delete mStringStream;
    delete mField1Expression;
    delete mField2Expression;
    delete mThreadVariable;
  }
};

TEST_F(NodeTest, getAccessLevelTest) {
  EXPECT_EQ(mComplicatedNode->getAccessLevel(), AccessLevel::PUBLIC_ACCESS);
}

TEST_F(NodeTest, getOwnerTest) {
  ASSERT_NE(mComplicatedNode->getOwner(), nullptr);
  EXPECT_EQ(mComplicatedNode->getOwner()->getObjectType(), mComplicatedNode);
}

TEST_F(NodeTest, getObjectTypeTest) {
  EXPECT_EQ(mComplicatedNode, mComplicatedNode->getObjectType());
}

TEST_F(NodeTest, getNameTest) {
  EXPECT_STREQ(mComplicatedNode->getTypeName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode");
}

TEST_F(NodeTest, getShortNameTest) {
  EXPECT_STREQ(mComplicatedNode->getShortName().c_str(), "NComplicatedNode");
}

TEST_F(NodeTest, getVTableNameTest) {
  EXPECT_STREQ(mComplicatedNode->getVTableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.vtable");
}

TEST_F(NodeTest, getTypeKindTest) {
  EXPECT_EQ(mComplicatedNode->getTypeKind(), NODE_TYPE);
}

TEST_F(NodeTest, getLLVMTypeTest) {
  EXPECT_EQ(mComplicatedNode->getLLVMType(mContext), mStructType->getPointerTo());
}

TEST_F(NodeTest, getInterfacesTest) {
  EXPECT_EQ(mComplicatedNode->getInterfaces().size(), 2u);
}

TEST_F(NodeTest, getVTableSizeTest) {
  EXPECT_EQ(mComplicatedNode->getVTableSize(), 3u);
}

TEST_F(NodeTest, getFieldsTest) {
  EXPECT_EQ(mComplicatedNode->getFields().size(), 2u);
}

TEST_F(NodeTest, findFeildTest) {
  EXPECT_EQ(mComplicatedNode->findField("mLeft"), mLeftField);
  EXPECT_EQ(mComplicatedNode->findField("mRight"), mRightField);
  EXPECT_EQ(mComplicatedNode->findField("mDepth"), nullptr);
}

TEST_F(NodeTest, findConstantTest) {
  EXPECT_EQ(mComplicatedNode->findConstant("MYCONSTANT"), mConstant);
}

TEST_F(NodeTest, findConstantDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mComplicatedNode->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Node systems.vos.wisey.compiler.tests.NComplicatedNode "
              "does not have constant named MYCONSTANT2");
}

TEST_F(NodeTest, getFieldIndexTest) {
  EXPECT_EQ(mComplicatedNode->getFieldIndex(mLeftField), 3u);
  EXPECT_EQ(mComplicatedNode->getFieldIndex(mRightField), 4u);
}

TEST_F(NodeTest, findMethodTest) {
  EXPECT_EQ(mComplicatedNode->findMethod("getElement"), mMethod);
  EXPECT_EQ(mComplicatedNode->findMethod("bar"), nullptr);
}

TEST_F(NodeTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.name");
}

TEST_F(NodeTest, getObjectShortNameGlobalVariableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getObjectShortNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.shortname");
}

TEST_F(NodeTest, getTypeTableNameTest) {
  ASSERT_STREQ(mComplicatedNode->getTypeTableName().c_str(),
               "systems.vos.wisey.compiler.tests.NComplicatedNode.typetable");
}

TEST_F(NodeTest, innerObjectsTest) {
  NiceMock<MockObjectType> innerObject1;
  ON_CALL(innerObject1, getShortName()).WillByDefault(Return("MObject1"));
  NiceMock<MockObjectType> innerObject2;
  ON_CALL(innerObject2, getShortName()).WillByDefault(Return("MObject2"));
  
  mComplicatedNode->addInnerObject(&innerObject1);
  mComplicatedNode->addInnerObject(&innerObject2);
  
  EXPECT_EQ(mComplicatedNode->getInnerObject("MObject1"), &innerObject1);
  EXPECT_EQ(mComplicatedNode->getInnerObject("MObject2"), &innerObject2);
  EXPECT_EQ(mComplicatedNode->getInnerObjects().size(), 2u);
}

TEST_F(NodeTest, markAsInnerTest) {
  EXPECT_FALSE(mComplicatedNode->isInner());
  mComplicatedNode->markAsInner();
  EXPECT_TRUE(mComplicatedNode->isInner());
}

TEST_F(NodeTest, getReferenceAdjustmentFunctionTest) {
  Function* result = mComplicatedNode->getReferenceAdjustmentFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(result, AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(mContext));
}

TEST_F(NodeTest, canCastToTest) {
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, mElementInterface));
  EXPECT_FALSE(mComplicatedNode->canCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mComplicatedNode->canCastTo(mContext, mComplicatedNode));
}

TEST_F(NodeTest, canAutoCastToTest) {
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, mSimplerNode));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, mComplicatedNode));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, mElementInterface));
  EXPECT_FALSE(mComplicatedNode->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mComplicatedNode->canAutoCastTo(mContext, mComplicatedNode));
}

TEST_F(NodeTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->castTo(mContext, pointer, mComplicatedElementInterface, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IComplicatedElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->castTo(mContext, pointer, mElementInterface, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 16"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IElement*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, isPrimitiveTest) {
  EXPECT_FALSE(mSimpleNode->isPrimitive());
}

TEST_F(NodeTest, isOwnerTest) {
  EXPECT_FALSE(mSimpleNode->isOwner());
}

TEST_F(NodeTest, isReferenceTest) {
  EXPECT_TRUE(mSimpleNode->isReference());
}

TEST_F(NodeTest, isArrayTest) {
  EXPECT_FALSE(mSimpleNode->isArray());
}

TEST_F(NodeTest, isFunctionTest) {
  EXPECT_FALSE(mSimpleNode->isFunction());
}

TEST_F(NodeTest, isPackageTest) {
  EXPECT_FALSE(mSimpleNode->isPackage());
}

TEST_F(NodeTest, isObjectTest) {
  EXPECT_FALSE(mSimpleNode->isController());
  EXPECT_FALSE(mSimpleNode->isInterface());
  EXPECT_FALSE(mSimpleNode->isModel());
  EXPECT_TRUE(mSimpleNode->isNode());
}

TEST_F(NodeTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i64*"
  "\n  %refCounter = load i64, i64* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, getFlattenedInterfaceHierarchyTest) {
  vector<Interface*> allInterfaces = mComplicatedNode->getFlattenedInterfaceHierarchy();
  
  EXPECT_EQ(allInterfaces.size(), 3u);
  EXPECT_EQ(allInterfaces.at(0), mComplicatedElementInterface);
  EXPECT_EQ(allInterfaces.at(1), mElementInterface);
  EXPECT_EQ(allInterfaces.at(2), mObjectInterface);
}

TEST_F(NodeTest, getMissingFieldsTest) {
  set<string> givenFields;
  givenFields.insert("mLeft");
  
  vector<string> missingFields = mComplicatedNode->getMissingFields(givenFields);
  
  ASSERT_EQ(missingFields.size(), 1u);
  EXPECT_EQ(missingFields.at(0), "mRight");
}

TEST_F(NodeTest, buildTest) {
  string argumentSpecifier1("withOwner");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withReference");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  Value* result = mSimpleNode->build(mContext, argumentList, 0);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint ("
  "%systems.vos.wisey.compiler.tests.NSimpleNode* getelementptr ("
  "%systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.NSimpleNode*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NSimpleNode* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint ("
  "%systems.vos.wisey.compiler.tests.NSimpleNode* getelementptr ("
  "%systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* %buildervar, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NOwner* null, "
  "%systems.vos.wisey.compiler.tests.NOwner** %1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.NSimpleNode, "
  "%systems.vos.wisey.compiler.tests.NSimpleNode* %buildervar, i32 0, i32 2"
  "\n  store %systems.vos.wisey.compiler.tests.MReference* null, "
  "%systems.vos.wisey.compiler.tests.MReference** %2"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MReference* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %3, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, buildInvalidObjectBuilderArgumentsDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  string argumentSpecifier1("owner");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withReference");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  const char *expected =
  "Error: Object builder argument should start with 'with'. e.g. .withField\\(value\\)."
  "\nError: Some arguments for the node systems.vos.wisey.compiler.tests.NSimpleNode "
  "builder are not well formed";
  
  EXPECT_EXIT(mSimpleNode->build(mContext, argumentList, 0),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(NodeTest, buildIncorrectArgumentTypeDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  Value* fieldValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.0f);
  ON_CALL(*mField2Expression, generateIR(_, _)).WillByDefault(Return(fieldValue));
  ON_CALL(*mField2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));

  string argumentSpecifier1("withOwner");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  string argumentSpecifier2("withReference");
  ObjectBuilderArgument *argument2 = new ObjectBuilderArgument(argumentSpecifier2,
                                                               mField2Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  argumentList.push_back(argument2);
  
  EXPECT_EXIT(mSimpleNode->build(mContext, argumentList, 0),
              ::testing::ExitedWithCode(1),
              "Error: Node builder argument value for field mReference does not match its type");
}

TEST_F(NodeTest, buildNotAllFieldsAreSetDeathTest) {
  Mock::AllowLeak(mField1Expression);
  Mock::AllowLeak(mField2Expression);
  Mock::AllowLeak(mThreadVariable);

  string argumentSpecifier1("withReference");
  ObjectBuilderArgument *argument1 = new ObjectBuilderArgument(argumentSpecifier1,
                                                               mField1Expression);
  ObjectBuilderArgumentList argumentList;
  argumentList.push_back(argument1);
  
  const char *expected =
  "Error: Field mOwner of the node systems.vos.wisey.compiler.tests.NSimpleNode is not initialized."
  "\nError: Some fields of the node systems.vos.wisey.compiler.tests.NSimpleNode "
  "are not initialized.";
  
  EXPECT_EXIT(mSimpleNode->build(mContext, argumentList, 0),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(NodeTest, printToStreamTest) {
  stringstream stringStream;
  Model* innerPublicModel = Model::newModel(PUBLIC_ACCESS, "MInnerPublicModel", NULL);
  vector<Field*> fields;
  InjectionArgumentList arguments;
  Field* field1 = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mField1", arguments);
  Field* field2 = new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "mField2", arguments);
  fields.push_back(field1);
  fields.push_back(field2);
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
  
  mComplicatedNode->addInnerObject(innerPublicModel);
  mComplicatedNode->addInnerObject(innerPrivateModel);
  mComplicatedNode->printToStream(mContext, stringStream);

  EXPECT_STREQ("external node systems.vos.wisey.compiler.tests.NComplicatedNode\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IComplicatedElement,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  fixed int mLeft;\n"
               "  fixed int mRight;\n"
               "\n"
               "  int getElement();\n"
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

TEST_F(NodeTest, createLocalVariableTest) {
  mComplicatedNode->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %referenceDeclaration = alloca %systems.vos.wisey.compiler.tests.NComplicatedNode*"
  "\n  store %systems.vos.wisey.compiler.tests.NComplicatedNode* null, %systems.vos.wisey.compiler.tests.NComplicatedNode** %referenceDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NodeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  InjectionArgumentList injectionArgumentList;
  Field* field = new Field(FIXED_FIELD, mComplicatedNode, NULL, "mField", injectionArgumentList);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mComplicatedNode->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(NodeTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mComplicatedNode->getLLVMType(mContext));
  mComplicatedNode->createParameterVariable(mContext, "var", value);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NComplicatedNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, linkListRunTest) {
  runFile("tests/samples/test_linklist.yz", "9");
}

TEST_F(TestFileSampleRunner, linkListReverseRunTest) {
  runFile("tests/samples/test_linklist_reverse.yz", "7");
}

TEST_F(TestFileSampleRunner, nodeBuilderObjectArgumentAutocast) {
  runFile("tests/samples/test_node_builder_object_argument_autocast.yz", "2018");
}

TEST_F(TestFileSampleRunner, nodeBuilderPrimitiveArgumentAutocast) {
  runFile("tests/samples/test_node_builder_primitive_argument_autocast.yz", "0");
}

TEST_F(TestFileSampleRunner, nodeBuilderSetStateFieldsRunTest) {
  runFile("tests/samples/test_node_builder_set_state_fields.yz", "2018");
}

TEST_F(TestFileSampleRunner, nodeWithModelFieldCompileTest) {
  compileFile("tests/samples/test_node_with_model_field.yz");
}

TEST_F(TestFileSampleRunner, nodeWithControllerFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_controller_field.yz",
                    1,
                    "Error: Nodes can only have fields of primitive or model or node type");
}
