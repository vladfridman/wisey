//
//  TestController.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Controller}
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockReferenceVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/Argument.hpp"
#include "wisey/Constant.hpp"
#include "wisey/Controller.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullType.hpp"
#include "wisey/ObjectKindGlobal.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedField.hpp"
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

struct ControllerTest : public Test {
  Controller* mMultiplierController;
  Controller* mAdditorController;
  Controller* mDoublerController;
  Controller* mSimpleController;
  Controller* mThreadController;
  Interface* mCalculatorInterface;
  Interface* mScienceCalculatorInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  Node* mOwnerNode;
  Model* mReferenceModel;
  IMethod* mMethod;
  StructType* mStructType;
  ReceivedField* mLeftField;
  ReceivedField* mRightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock *mBasicBlock;
  wisey::Constant* mConstant;
  NiceMock<MockReferenceVariable>* mThreadVariable;
  NiceMock<MockReferenceVariable>* mCallstackVariable;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  LLVMFunction* mLLVMFunction;

  ControllerTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    string calculatorFullName = "systems.vos.wisey.compiler.tests.ICalculator";
    StructType* calculatorIinterfaceStructType = StructType::create(mLLVMContext,
                                                                    calculatorFullName);
    VariableList calculatorInterfaceArguments;
    vector<IObjectElementDefinition*> calculatorInterfaceElements;
    vector<IModelTypeSpecifier*> calculatorThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    IObjectElementDefinition* calculateSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "calculate",
                                     calculatorInterfaceArguments,
                                     calculatorThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    calculatorInterfaceElements.push_back(calculateSignature);
    vector<IInterfaceTypeSpecifier*> calculatorParentInterfaces;
    mCalculatorInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                   calculatorFullName,
                                                   calculatorIinterfaceStructType,
                                                   calculatorParentInterfaces,
                                                   calculatorInterfaceElements,
                                                   mContext.getImportProfile(),
                                                   0);
    mContext.addInterface(mCalculatorInterface, 0);
    mCalculatorInterface->buildMethods(mContext);
    
    string scienceCalculatorFullName = "systems.vos.wisey.compiler.tests.IScienceCalculator";
    StructType* scienceCalculatorIinterfaceStructType =
      StructType::create(mLLVMContext, scienceCalculatorFullName);
    vector<IObjectElementDefinition*> scienceCalculatorInterfaceElements;
    MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
    calculateSignature = new MethodSignatureDeclaration(intSpecifier,
                                                        "calculate",
                                                        calculatorInterfaceArguments,
                                                        calculatorThrownExceptions,
                                                        methodQualifiers,
                                                        0);
    scienceCalculatorInterfaceElements.push_back(calculateSignature);
    vector<IInterfaceTypeSpecifier*> scienceCalculatorParentInterfaces;
    PackageType* packageType = new PackageType(mPackage);
    FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
    InterfaceTypeSpecifier* calculatorSpecifier = new InterfaceTypeSpecifier(packageExpression,
                                                                             "ICalculator",
                                                                             0);
    scienceCalculatorParentInterfaces.push_back(calculatorSpecifier);
    mScienceCalculatorInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                          scienceCalculatorFullName,
                                                          scienceCalculatorIinterfaceStructType,
                                                          scienceCalculatorParentInterfaces,
                                                          scienceCalculatorInterfaceElements,
                                                          mContext.getImportProfile(),
                                                          0);
    mContext.addInterface(mScienceCalculatorInterface, 0);
    mScienceCalculatorInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceArguments;
    vector<IObjectElementDefinition*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDefinition* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceArguments,
                                     objectThrownExceptions,
                                     new MethodQualifiers(0),
                                     0);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<IInterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                               objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements,
                                               mContext.getImportProfile(),
                                               0);
    mContext.addInterface(mObjectInterface, 0);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string multiplierFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    mStructType = StructType::create(mLLVMContext, multiplierFullName);
    mStructType->setBody(types);
    mMultiplierController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                      multiplierFullName,
                                                      mStructType,
                                                      mContext.getImportProfile(),
                                                      5);
    vector<IField*> fields;
    mLeftField = new ReceivedField(PrimitiveTypes::INT, "left", 0);
    mRightField = new ReceivedField(PrimitiveTypes::INT, "right", 0);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<const wisey::Argument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method(mMultiplierController,
                         "calculate",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT,
                         methodArguments,
                         thrownExceptions,
                         new MethodQualifiers(0),
                         NULL,
                         0);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    IMethod* fooMethod = new Method(mMultiplierController,
                                    "foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    methodArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    NULL,
                                    0);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);
    
    IntConstant* intConstant = new IntConstant(5, 0);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT,
                                    "MYCONSTANT",
                                    intConstant,
                                    0);
    wisey::Constant* privateConstant = new wisey::Constant(PRIVATE_ACCESS,
                                                           PrimitiveTypes::INT,
                                                           "MYCONSTANT3",
                                                           intConstant,
                                                           0);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);
    constants.push_back(privateConstant);

    vector<const IType*> functionArgumentTypes;
    LLVMFunctionType* llvmFunctionType = LLVMFunctionType::create(LLVMPrimitiveTypes::I8,
                                                                  functionArgumentTypes);
    vector<const wisey::Argument*> llvmFunctionArguments;
    Block* functionBlock = new Block();
    CompoundStatement* functionCompoundStatement = new CompoundStatement(functionBlock, 0);
    mLLVMFunction = new LLVMFunction(mMultiplierController,
                                     "myfunction",
                                     PUBLIC_ACCESS,
                                     llvmFunctionType,
                                     LLVMPrimitiveTypes::I8,
                                     llvmFunctionArguments,
                                     functionCompoundStatement,
                                     0);
    vector<LLVMFunction*> functions;
    functions.push_back(mLLVMFunction);

    mMultiplierController->setFields(mContext, fields, interfaces.size());
    mMultiplierController->setMethods(methods);
    mMultiplierController->setInterfaces(interfaces);
    mMultiplierController->setConstants(constants);
    mMultiplierController->setLLVMFunctions(functions);
    
    vector<Type*> ownerTypes;
    ownerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                         ->getPointerTo()->getPointerTo());
    string ownerFullName = "systems.vos.wisey.compiler.tests.NOwner";
    StructType* ownerStructType = StructType::create(mLLVMContext, ownerFullName);
    ownerStructType->setBody(ownerTypes);
    mOwnerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                               ownerFullName,
                               ownerStructType,
                               mContext.getImportProfile(),
                               0);
    mContext.addNode(mOwnerNode, 0);
    
    vector<Type*> referenceTypes;
    referenceTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                             ->getPointerTo()->getPointerTo());
    string referenceFullName = "systems.vos.wisey.compiler.tests.MReference";
    StructType* referenceStructType = StructType::create(mLLVMContext, referenceFullName);
    referenceStructType->setBody(referenceTypes);
    mReferenceModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                      referenceFullName,
                                      referenceStructType,
                                      mContext.getImportProfile(),
                                      0);
    mContext.addModel(mReferenceModel, 0);
    
    vector<Type*> simpleControllerTypes;
    simpleControllerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                                    ->getPointerTo()->getPointerTo());
    string simpleControllerFullName = "systems.vos.wisey.compiler.tests.CSimpleController";
    StructType* simpleControllerStructType = StructType::create(mLLVMContext,
                                                                simpleControllerFullName);
    simpleControllerStructType->setBody(simpleControllerTypes);
    mSimpleController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                  simpleControllerFullName,
                                                  simpleControllerStructType,
                                                  mContext.getImportProfile(),
                                                  0);
    mContext.addController(mSimpleController, 0);

    vector<Type*> additorTypes;
    additorTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                             ->getPointerTo()->getPointerTo());
    additorTypes.push_back(mOwnerNode->getOwner()->getLLVMType(mContext));
    additorTypes.push_back(mReferenceModel->getLLVMType(mContext));
    string additorFullName = "systems.vos.wisey.compiler.tests.CAdditor";
    StructType* additorStructType = StructType::create(mLLVMContext, additorFullName);
    additorStructType->setBody(additorTypes);
    vector<IField*> additorFields;
    additorFields.push_back(new ReceivedField(mOwnerNode->getOwner(), "mOwner", 1));
    additorFields.push_back(new ReceivedField(mReferenceModel, "mReference", 3));
    mAdditorController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                   additorFullName,
                                                   additorStructType,
                                                   mContext.getImportProfile(),
                                                   0);
    mAdditorController->setFields(mContext, additorFields, 1u);
    mContext.addController(mMultiplierController, 0);

    vector<Type*> doublerTypes;
    doublerTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                           ->getPointerTo()->getPointerTo());
    doublerTypes.push_back(Type::getInt32Ty(mLLVMContext));
    doublerTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string doublerFullName = "systems.vos.wisey.compiler.tests.CDoubler";
    StructType* doublerStructType = StructType::create(mLLVMContext, doublerFullName);
    doublerStructType->setBody(doublerTypes);
    vector<IField*> doublerFields;
    InjectionArgumentList fieldArguments;
    doublerFields.push_back(new InjectedField(PrimitiveTypes::INT,
                                              PrimitiveTypes::INT,
                                              "left",
                                              fieldArguments,
                                              mContext.getImportProfile()->getSourceFileName(),
                                              3));
    mDoublerController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                   doublerFullName,
                                                   doublerStructType,
                                                   mContext.getImportProfile(),
                                                   0);
    mDoublerController->setFields(mContext, doublerFields, 1u);
    mContext.addController(mDoublerController, 0);

    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDefinition*> vehicleElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleElements,
                                                mContext.getImportProfile(),
                                                0);
    mContext.addInterface(mVehicleInterface, 0);
    mVehicleInterface->buildMethods(mContext);

    IConcreteObjectType::declareTypeNameGlobal(mContext, mOwnerNode);
    IConcreteObjectType::declareVTable(mContext, mOwnerNode);
    IConcreteObjectType::declareTypeNameGlobal(mContext, mAdditorController);
    IConcreteObjectType::declareVTable(mContext, mAdditorController);
    IConcreteObjectType::declareTypeNameGlobal(mContext, mSimpleController);
    IConcreteObjectType::declareVTable(mContext, mSimpleController);

    vector<Type*> threadTypes;
    threadTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                           ->getPointerTo()->getPointerTo());
    string threadFullName = "systems.vos.wisey.compiler.tests.CThread";
    StructType* threadStructType = StructType::create(mLLVMContext, threadFullName);
    threadStructType->setBody(threadTypes);
    vector<IField*> threadFields;
    mThreadController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                  threadFullName,
                                                  threadStructType, mContext.getImportProfile(),
                                                  0);
    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    vector<Interface*> threadInterfaces;
    threadInterfaces.push_back(threadInterface);
    mThreadController->setInterfaces(threadInterfaces);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockReferenceVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_, _)).WillByDefault(Return(threadObject));

    Controller* callstackController =
    mContext.getController(Names::getCallStackControllerFullName(), 0);
    Value* callstackObject = ConstantPointerNull::get(callstackController->getLLVMType(mContext));
    mCallstackVariable = new NiceMock<MockReferenceVariable>();
    ON_CALL(*mCallstackVariable, getName()).WillByDefault(Return(ThreadExpression::CALL_STACK));
    ON_CALL(*mCallstackVariable, getType()).WillByDefault(Return(callstackController));
    ON_CALL(*mCallstackVariable, generateIdentifierIR(_, _)).WillByDefault(Return(callstackObject));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ControllerTest() {
    delete mStringStream;
    delete mThreadVariable;
    delete mCallstackVariable;
  }
};

TEST_F(ControllerTest, isPublicTest) {
  EXPECT_TRUE(mMultiplierController->isPublic());
}

TEST_F(ControllerTest, getLineTest) {
  EXPECT_EQ(5, mMultiplierController->getLine());
}

TEST_F(ControllerTest, getNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier",
               mMultiplierController->getTypeName().c_str());
}

TEST_F(ControllerTest, getShortNameTest) {
  EXPECT_STREQ("CMultiplier",
               mMultiplierController->getShortName().c_str());
}

TEST_F(ControllerTest, getVTableNameTest) {
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier.vtable",
               mMultiplierController->getVTableName().c_str());
}

TEST_F(ControllerTest, getLLVMTypeTest) {
  EXPECT_EQ(mStructType->getPointerTo(), mMultiplierController->getLLVMType(mContext));
}

TEST_F(ControllerTest, getInterfacesTest) {
  EXPECT_EQ(2u, mMultiplierController->getInterfaces().size());
}

TEST_F(ControllerTest, getFieldsTest) {
  EXPECT_EQ(2u, mMultiplierController->getFields().size());
}

TEST_F(ControllerTest, getReceivedFieldsTest) {
  std::vector<IField*> fields = mMultiplierController->getReceivedFields();
  EXPECT_EQ(2u, fields.size());
  EXPECT_EQ(mLeftField, fields.front());
  EXPECT_EQ(mRightField, fields.back());
}

TEST_F(ControllerTest, getOwnerTest) {
  ASSERT_NE(nullptr, mMultiplierController->getOwner());
  EXPECT_EQ(mMultiplierController, mMultiplierController->getOwner()->getReference());
}

TEST_F(ControllerTest, getFieldIndexTest) {
  EXPECT_EQ(2u, mMultiplierController->getFieldIndex(mLeftField));
  EXPECT_EQ(3u, mMultiplierController->getFieldIndex(mRightField));
}

TEST_F(ControllerTest, findFeildTest) {
  EXPECT_EQ(mLeftField, mMultiplierController->findField("left"));
  EXPECT_EQ(mRightField, mMultiplierController->findField("right"));
  EXPECT_EQ(nullptr, mMultiplierController->findField("depth"));
}

TEST_F(ControllerTest, findMethodTest) {
  EXPECT_EQ(mMethod, mMultiplierController->findMethod("calculate"));
  EXPECT_EQ(nullptr, mMultiplierController->findMethod("bar"));
}

TEST_F(ControllerTest, findConstantTest) {
  EXPECT_EQ(mConstant, mMultiplierController->findConstant(mContext, "MYCONSTANT", 5));
}

TEST_F(ControllerTest, findConstantDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mCallstackVariable);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mMultiplierController->findConstant(mContext, "MYCONSTANT2", 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Controller systems.vos.wisey.compiler.tests.CMultiplier does not have constant named MYCONSTANT2\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, findLLVMFunctionTest) {
  EXPECT_EQ(mLLVMFunction, mMultiplierController->findLLVMFunction("myfunction"));
}

TEST_F(ControllerTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier.typename",
               mMultiplierController->getObjectNameGlobalVariableName().c_str());
}

TEST_F(ControllerTest, getTypeTableNameTest) {
  ASSERT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier.typetable",
               mMultiplierController->getTypeTableName().c_str());
}

TEST_F(ControllerTest, innerObjectsTest) {
  NiceMock<MockObjectType> innerObject1;
  ON_CALL(innerObject1, getShortName()).WillByDefault(Return("MObject1"));
  NiceMock<MockObjectType> innerObject2;
  ON_CALL(innerObject2, getShortName()).WillByDefault(Return("MObject2"));
  
  mMultiplierController->addInnerObject(&innerObject1);
  mMultiplierController->addInnerObject(&innerObject2);
  
  EXPECT_EQ(&innerObject1, mMultiplierController->getInnerObject("MObject1"));
  EXPECT_EQ(&innerObject2, mMultiplierController->getInnerObject("MObject2"));
  EXPECT_EQ(2u, mMultiplierController->getInnerObjects().size());
}

TEST_F(ControllerTest, markAsInnerTest) {
  EXPECT_FALSE(mMultiplierController->isInner());
  mMultiplierController->markAsInner();
  EXPECT_TRUE(mMultiplierController->isInner());
}

TEST_F(ControllerTest, canCastToTest) {
  EXPECT_FALSE(mMultiplierController->canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mMultiplierController->canCastTo(mContext, mAdditorController));
  EXPECT_FALSE(mMultiplierController->canCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mMultiplierController->canCastTo(mContext, mMultiplierController));
  EXPECT_TRUE(mMultiplierController->canCastTo(mContext, mCalculatorInterface));
  EXPECT_FALSE(mMultiplierController->canCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mMultiplierController->canCastTo(mContext, mMultiplierController));
  EXPECT_FALSE(mMultiplierController->canCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mMultiplierController->
               canCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mMultiplierController->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mMultiplierController->
               canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ControllerTest, canAutoCastToTest) {
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mContext, mAdditorController));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mContext, mVehicleInterface));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mContext, mMultiplierController));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mContext, mCalculatorInterface));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mContext, NullType::NULL_TYPE));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mContext, mMultiplierController));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mContext, WiseyModelType::WISEY_MODEL_TYPE));
  EXPECT_FALSE(mMultiplierController->
               canAutoCastTo(mContext, WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mMultiplierController->
               canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(ControllerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get((llvm::PointerType*) mMultiplierController->getLLVMType(mContext));
  mMultiplierController->castTo(mContext, pointer, mScienceCalculatorInterface, 0);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IScienceCalculator*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->castTo(mContext, pointer, mCalculatorInterface, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.ICalculator*\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, isTypeKindTest) {
  EXPECT_FALSE(mMultiplierController->isPrimitive());
  EXPECT_FALSE(mMultiplierController->isOwner());
  EXPECT_TRUE(mMultiplierController->isReference());
  EXPECT_FALSE(mMultiplierController->isArray());
  EXPECT_FALSE(mMultiplierController->isFunction());
  EXPECT_FALSE(mMultiplierController->isPackage());
  EXPECT_FALSE(mMultiplierController->isNative());
  EXPECT_FALSE(mMultiplierController->isPointer());
  EXPECT_FALSE(mMultiplierController->isImmutable());
}

TEST_F(ControllerTest, isObjectTest) {
  EXPECT_TRUE(mMultiplierController->isController());
  EXPECT_FALSE(mMultiplierController->isInterface());
  EXPECT_FALSE(mMultiplierController->isModel());
  EXPECT_FALSE(mMultiplierController->isNode());
}

TEST_F(ControllerTest, isScopeInjectedTest) {
  EXPECT_FALSE(mAdditorController->isScopeInjected(mContext));
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  mAdditorController->setScopeType(threadInterface);
  EXPECT_TRUE(mAdditorController->isScopeInjected(mContext));
}

TEST_F(ControllerTest, incrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, decrementReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->decrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 -1)\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, incrementReferenceCountForThreadTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mThreadController->getLLVMType(mContext));
  mThreadController->incrementReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CThread* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, decrementReferenceCountForThreadTest) {
  ConstantPointerNull* pointer = ConstantPointerNull::get(mThreadController->getLLVMType(mContext));
  mThreadController->decrementReferenceCount(mContext, pointer);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CThread* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %0, i64 -1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, getReferenceCountTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->getReferenceCount(mContext, pointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i64*"
  "\n  %1 = getelementptr i64, i64* %0, i64 -1"
  "\n  %refCounter = load i64, i64* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, getFlattenedInterfaceHierarchyTest) {
  vector<Interface*> allInterfaces = mMultiplierController->getFlattenedInterfaceHierarchy();
  
  EXPECT_EQ(allInterfaces.size(), 3u);
  EXPECT_EQ(allInterfaces.at(0), mScienceCalculatorInterface);
  EXPECT_EQ(allInterfaces.at(1), mCalculatorInterface);
  EXPECT_EQ(allInterfaces.at(2), mObjectInterface);
}

TEST_F(ControllerTest, createInjectFunctionTest) {
  mAdditorController->createInjectFunction(mContext, 0);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->getFunction(mAdditorController->getTypeName() +
                                                         ".inject");
  EXPECT_NE(nullptr, function);
  
  *mStringStream << *function;
  string expected =
  "\ndefine %systems.vos.wisey.compiler.tests.CAdditor* @systems.vos.wisey.compiler.tests.CAdditor.inject(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, %systems.vos.wisey.compiler.tests.NOwner* %mOwner, %systems.vos.wisey.compiler.tests.MReference* %mReference) {"
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CAdditor.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CAdditor.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* %injectvar, i32 0, i32 1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, %systems.vos.wisey.compiler.tests.CAdditor* %1, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NOwner* %mOwner, %systems.vos.wisey.compiler.tests.NOwner** %2"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, %systems.vos.wisey.compiler.tests.CAdditor* %1, i32 0, i32 2"
  "\n  store %systems.vos.wisey.compiler.tests.MReference* %mReference, %systems.vos.wisey.compiler.tests.MReference** %3"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MReference* %mReference to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 1)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.CAdditor* %1 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CAdditor.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
  "\n  ret %systems.vos.wisey.compiler.tests.CAdditor* %1"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, createContextInjectFunctionDeathTest) {
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  mAdditorController->setScopeType(threadInterface);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mAdditorController->createInjectFunction(mContext, 0));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Scope injected controllers can not have received fields\n",
               buffer.str().c_str());
  
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, createContextInjectFunctionTest) {
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  mSimpleController->setScopeType(threadInterface);
  Function* function = mSimpleController->createInjectFunction(mContext, 0);
  mContext.runComposingCallbacks();
  
  EXPECT_NE(nullptr, function);
  
  *mStringStream << *function;
  string expected =
  "\ndefine %systems.vos.wisey.compiler.tests.CSimpleController* @systems.vos.wisey.compiler.tests.CSimpleController.inject(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* %callstack, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 0)"
  "\n  %0 = bitcast %wisey.threads.IThread* %thread to i8*"
  "\n  invoke void @__checkForNullAndThrow(i8* %0)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %invoke.continue, %entry"
  "\n  %1 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %2 = alloca { i8*, i32 }"
  "\n  store { i8*, i32 } %1, { i8*, i32 }* %2"
  "\n  %3 = getelementptr { i8*, i32 }, { i8*, i32 }* %2, i32 0, i32 0"
  "\n  %4 = load i8*, i8** %3"
  "\n  %5 = call i8* @__cxa_get_exception_ptr(i8* %4)"
  "\n  %6 = getelementptr i8, i8* %5, i64 8"
  "\n  resume { i8*, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %7 = bitcast %wisey.threads.IThread* %thread to %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)***"
  "\n  %vtable = load %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)**, %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)*** %7"
  "\n  %8 = getelementptr %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)*, %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)** %vtable, i64 3"
  "\n  %9 = load %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)*, %wisey.threads.CContextManager* (%wisey.threads.IThread*, %wisey.threads.IThread*, %wisey.threads.CCallStack*)** %8"
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* %callstack, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 0)"
  "\n  %10 = invoke %wisey.threads.CContextManager* %9(%wisey.threads.IThread* %thread, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack)"
  "\n          to label %invoke.continue1 unwind label %cleanup"
  "\n"
  "\ninvoke.continue1:                                 ; preds = %invoke.continue"
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* %callstack, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 0)"
  "\n  %11 = bitcast %wisey.threads.CContextManager* %10 to i8*"
  "\n  invoke void @__checkForNullAndThrow(i8* %11)"
  "\n          to label %invoke.continue3 unwind label %cleanup2"
  "\n"
  "\ncleanup2:                                         ; preds = %invoke.continue6, %if.null, %invoke.continue4, %invoke.continue3, %invoke.continue1"
  "\n  %12 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %13 = alloca { i8*, i32 }"
  "\n  store { i8*, i32 } %12, { i8*, i32 }* %13"
  "\n  %14 = getelementptr { i8*, i32 }, { i8*, i32 }* %13, i32 0, i32 0"
  "\n  %15 = load i8*, i8** %14"
  "\n  %16 = call i8* @__cxa_get_exception_ptr(i8* %15)"
  "\n  %17 = getelementptr i8, i8* %16, i64 8"
  "\n  resume { i8*, i32 } %12"
  "\n"
  "\ninvoke.continue3:                                 ; preds = %invoke.continue1"
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* %callstack, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 0)"
  "\n  %18 = invoke i8* @wisey.threads.CContextManager.getInstance(%wisey.threads.CContextManager* %10, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @wisey.threads.IThread.typename, i32 0, i32 0), i8* getelementptr inbounds ([51 x i8], [51 x i8]* @systems.vos.wisey.compiler.tests.CSimpleController.typename, i32 0, i32 0))"
  "\n          to label %invoke.continue4 unwind label %cleanup2"
  "\n"
  "\ninvoke.continue4:                                 ; preds = %invoke.continue3"
  "\n  %19 = invoke i8* @__castObject(i8* %18, i8* getelementptr inbounds ([51 x i8], [51 x i8]* @systems.vos.wisey.compiler.tests.CSimpleController.typename, i32 0, i32 0))"
  "\n          to label %invoke.continue5 unwind label %cleanup2"
  "\n"
  "\ninvoke.continue5:                                 ; preds = %invoke.continue4"
  "\n  %20 = bitcast i8* %19 to %systems.vos.wisey.compiler.tests.CSimpleController*"
  "\n  %21 = icmp eq %systems.vos.wisey.compiler.tests.CSimpleController* %20, null"
  "\n  br i1 %21, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %invoke.continue5"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CSimpleController.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CSimpleController.refCounter, %systems.vos.wisey.compiler.tests.CSimpleController.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CSimpleController.refCounter*"
  "\n  %22 = bitcast %systems.vos.wisey.compiler.tests.CSimpleController.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %22, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CSimpleController.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CSimpleController.refCounter, %systems.vos.wisey.compiler.tests.CSimpleController.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %23 = getelementptr %systems.vos.wisey.compiler.tests.CSimpleController.refCounter, %systems.vos.wisey.compiler.tests.CSimpleController.refCounter* %injectvar, i32 0, i32 1"
  "\n  %24 = bitcast %systems.vos.wisey.compiler.tests.CSimpleController* %23 to i8*"
  "\n  %25 = getelementptr i8, i8* %24, i64 0"
  "\n  %26 = bitcast i8* %25 to i32 (...)***"
  "\n  %27 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CSimpleController.vtable, i32 0, i32 0, i32 0"
  "\n  %28 = bitcast i8** %27 to i32 (...)**"
  "\n  store i32 (...)** %28, i32 (...)*** %26"
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* %callstack, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 0)"
  "\n  %29 = bitcast %wisey.threads.CContextManager* %10 to i8*"
  "\n  invoke void @__checkForNullAndThrow(i8* %29)"
  "\n          to label %invoke.continue6 unwind label %cleanup2"
  "\n"
  "\nif.not.null:                                      ; preds = %invoke.continue5"
  "\n  ret %systems.vos.wisey.compiler.tests.CSimpleController* %20"
  "\n"
  "\ninvoke.continue6:                                 ; preds = %if.null"
  "\n  call void @wisey.threads.CCallStack.setLine(%wisey.threads.CCallStack* %callstack, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 0)"
  "\n  %30 = bitcast %systems.vos.wisey.compiler.tests.CSimpleController* %23 to i8*"
  "\n  invoke void @wisey.threads.CContextManager.setInstance(%wisey.threads.CContextManager* %10, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @wisey.threads.IThread.typename, i32 0, i32 0), i8* getelementptr inbounds ([51 x i8], [51 x i8]* @systems.vos.wisey.compiler.tests.CSimpleController.typename, i32 0, i32 0), i8* %30)"
  "\n          to label %invoke.continue7 unwind label %cleanup2"
  "\n"
  "\ninvoke.continue7:                                 ; preds = %invoke.continue6"
  "\n  ret %systems.vos.wisey.compiler.tests.CSimpleController* %23"
  "\n}"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, defineFieldInjectorFunctionsTest) {
  vector<Type*> childTypes;
  string childFullName = "systems.vos.wisey.compiler.tests.CChild";
  StructType* childStructType = StructType::create(mLLVMContext, childFullName);
  childTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                       ->getPointerTo()->getPointerTo());
  childStructType->setBody(childTypes);
  vector<IField*> childFields;
  Controller* childController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                          childFullName,
                                                          childStructType,
                                                          mContext.getImportProfile(),
                                                          0);
  childController->setFields(mContext, childFields, 1u);
  mContext.addController(childController, 0);
  IConcreteObjectType::declareTypeNameGlobal(mContext, childController);
  IConcreteObjectType::declareVTable(mContext, childController);
  
  vector<Type*> parentTypes;
  parentTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
  parentTypes.push_back(childController->getLLVMType(mContext));
  string parentFullName = "systems.vos.wisey.compiler.tests.CParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  parentStructType->setBody(parentTypes);
  vector<IField*> parentFields;
  InjectionArgumentList fieldArguments;
  parentFields.push_back(new InjectedField(childController->getOwner(),
                                           childController->getOwner(),
                                           "mChild",
                                           fieldArguments,
                                           mContext.getImportProfile()->getSourceFileName(),
                                           3));
  Controller* parentController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                           parentFullName,
                                                           parentStructType,
                                                           mContext.getImportProfile(),
                                                           0);
  parentController->setFields(mContext, parentFields, 1u);
  mContext.addController(parentController, 0);
  IConcreteObjectType::declareTypeNameGlobal(mContext, parentController);
  IConcreteObjectType::declareVTable(mContext, parentController);
  
  childController->declareInjectFunction(mContext, 0);
  parentController->defineFieldInjectorFunctions(mContext, 0);
  mContext.runComposingCallbacks();
  
  Function* function = mContext.getModule()->getFunction(parentController->getTypeName() +
                                                         ".mChild.inject");
  EXPECT_NE(nullptr, function);
  
  *mStringStream << *function;
  string expected =
  "\ndefine %systems.vos.wisey.compiler.tests.CChild* @systems.vos.wisey.compiler.tests.CParent.mChild.inject(%systems.vos.wisey.compiler.tests.CParent* %this, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, %systems.vos.wisey.compiler.tests.CChild** %fieldPointer) {"
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.CChild*, %systems.vos.wisey.compiler.tests.CChild** %fieldPointer"
  "\n  %isNull = icmp eq %systems.vos.wisey.compiler.tests.CChild* %0, null"
  "\n  br i1 %isNull, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  %1 = call %systems.vos.wisey.compiler.tests.CChild* @systems.vos.wisey.compiler.tests.CChild.inject(%wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack)"
  "\n  store %systems.vos.wisey.compiler.tests.CChild* %1, %systems.vos.wisey.compiler.tests.CChild** %fieldPointer"
  "\n  ret %systems.vos.wisey.compiler.tests.CChild* %1"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  ret %systems.vos.wisey.compiler.tests.CChild* %0"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, declareFieldInjectionFunctionsTest) {
  vector<Type*> childTypes;
  string childFullName = "systems.vos.wisey.compiler.tests.CChild";
  StructType* childStructType = StructType::create(mLLVMContext, childFullName);
  childTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                       ->getPointerTo()->getPointerTo());
  childStructType->setBody(childTypes);
  vector<IField*> childFields;
  Controller* childController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                          childFullName,
                                                          childStructType,
                                                          mContext.getImportProfile(),
                                                          0);
  childController->setFields(mContext, childFields, 1u);
  mContext.addController(childController, 0);
  IConcreteObjectType::declareTypeNameGlobal(mContext, childController);
  IConcreteObjectType::declareVTable(mContext, childController);
  
  vector<Type*> parentTypes;
  parentTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                        ->getPointerTo()->getPointerTo());
  parentTypes.push_back(childController->getLLVMType(mContext));
  string parentFullName = "systems.vos.wisey.compiler.tests.CParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  parentStructType->setBody(parentTypes);
  vector<IField*> parentFields;
  InjectionArgumentList fieldArguments;
  parentFields.push_back(new InjectedField(childController->getOwner(),
                                           childController->getOwner(),
                                           "mChild",
                                           fieldArguments,
                                           mContext.getImportProfile()->getSourceFileName(),
                                           3));
  Controller* parentController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                           parentFullName,
                                                           parentStructType,
                                                           mContext.getImportProfile(),
                                                           0);
  parentController->setFields(mContext, parentFields, 1u);
  mContext.addController(parentController, 0);
  IConcreteObjectType::declareTypeNameGlobal(mContext, parentController);
  IConcreteObjectType::declareVTable(mContext, parentController);
  
  parentController->declareFieldInjectionFunctions(mContext, 0);
  
  Function* function = mContext.getModule()->getFunction(parentController->getTypeName() +
                                                         ".mChild.inject");
  EXPECT_NE(nullptr, function);
}

TEST_F(ControllerTest, injectTest) {
  mContext.getScopes().setVariable(mContext, mThreadVariable);
  mContext.getScopes().setVariable(mContext, mCallstackVariable);

  InjectionArgumentList injectionArguments;
  mAdditorController->declareInjectFunction(mContext, 0);

  NiceMock<MockExpression> ownerExpression;
  Value* ownerValue = ConstantPointerNull::get(mOwnerNode->getOwner()->getLLVMType(mContext));
  ON_CALL(ownerExpression, generateIR(_, _)).WillByDefault(Return(ownerValue));
  ON_CALL(ownerExpression, getType(_)).WillByDefault(Return(mOwnerNode->getOwner()));
  NiceMock<MockExpression> referenceExpression;
  Value* referenceValue = ConstantPointerNull::get(mReferenceModel->getLLVMType(mContext));
  ON_CALL(referenceExpression, generateIR(_, _)).WillByDefault(Return(referenceValue));
  ON_CALL(referenceExpression, getType(_)).WillByDefault(Return(mReferenceModel));

  InjectionArgument* ownerArgument = new InjectionArgument("withOwner", &ownerExpression);
  InjectionArgument* referenceArgument = new InjectionArgument("withReference", 
                                                               &referenceExpression);

  injectionArguments.push_back(ownerArgument);
  injectionArguments.push_back(referenceArgument);
  
  Value* result = mAdditorController->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = call %systems.vos.wisey.compiler.tests.CAdditor* @systems.vos.wisey.compiler.tests.CAdditor.inject(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null, %systems.vos.wisey.compiler.tests.NOwner* null, %systems.vos.wisey.compiler.tests.MReference* null)"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, injectChangeArgumentOrderTest) {
  mContext.getScopes().setVariable(mContext, mThreadVariable);
  mContext.getScopes().setVariable(mContext, mCallstackVariable);

  InjectionArgumentList injectionArguments;
  mAdditorController->declareInjectFunction(mContext, 0);
  
  NiceMock<MockExpression> ownerExpression;
  Value* ownerValue = ConstantPointerNull::get(mOwnerNode->getOwner()->getLLVMType(mContext));
  ON_CALL(ownerExpression, generateIR(_, _)).WillByDefault(Return(ownerValue));
  ON_CALL(ownerExpression, getType(_)).WillByDefault(Return(mOwnerNode->getOwner()));
  NiceMock<MockExpression> referenceExpression;
  Value* referenceValue = ConstantPointerNull::get(mReferenceModel->getLLVMType(mContext));
  ON_CALL(referenceExpression, generateIR(_, _)).WillByDefault(Return(referenceValue));
  ON_CALL(referenceExpression, getType(_)).WillByDefault(Return(mReferenceModel));
  
  InjectionArgument* ownerArgument = new InjectionArgument("withOwner", &ownerExpression);
  InjectionArgument* referenceArgument = new InjectionArgument("withReference",
                                                               &referenceExpression);
  
  injectionArguments.push_back(referenceArgument);
  injectionArguments.push_back(ownerArgument);

  Value* result = mAdditorController->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = call %systems.vos.wisey.compiler.tests.CAdditor* @systems.vos.wisey.compiler.tests.CAdditor.inject(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null, %systems.vos.wisey.compiler.tests.NOwner* null, %systems.vos.wisey.compiler.tests.MReference* null)"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, injectWrongTypeOfArgumentDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  NiceMock<MockExpression>* injectArgument2Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(injectArgument2Expression);
  Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(*injectArgument1Expression, generateIR(_, _)).WillByDefault(Return(field1Value));
  ON_CALL(*injectArgument1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  Value* field2Value = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*injectArgument2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
  ON_CALL(*injectArgument2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  InjectionArgument* injectionArgument1 = new InjectionArgument("withOwner",
                                                                injectArgument1Expression);
  InjectionArgument* injectionArgument2 = new InjectionArgument("withReference",
                                                                injectArgument2Expression);

  injectionArguments.push_back(injectionArgument1);
  injectionArguments.push_back(injectionArgument2);
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mCallstackVariable);
  mAdditorController->declareInjectFunction(mContext, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mAdditorController->inject(mContext, injectionArguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Injector argumet value for field 'mOwner' does not match its type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, injectNonInjectableTypeDeathTest) {
  InjectionArgumentList injectionArguments;
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mCallstackVariable);

  IConcreteObjectType::declareTypeNameGlobal(mContext, mDoublerController);
  IConcreteObjectType::declareVTable(mContext, mDoublerController);

  mDoublerController->createInjectFunction(mContext, 0);
  mDoublerController->defineFieldInjectorFunctions(mContext, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mContext.runComposingCallbacks());
  EXPECT_STREQ("/tmp/source.yz(3): Error: type int is not injectable\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, notWellFormedInjectionArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mCallstackVariable);
  InjectionArgument* injectionArgument = new InjectionArgument("owner",
                                                               injectArgument1Expression);
  injectionArguments.push_back(injectionArgument);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mAdditorController->inject(mContext, injectionArguments, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Injection argument should start with 'with'. e.g. .withField(value).\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, injectTooFewArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mCallstackVariable);
  InjectionArgument* injectionArgument = new InjectionArgument("withOwner",
                                                               injectArgument1Expression);
  injectionArguments.push_back(injectionArgument);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mAdditorController->inject(mContext, injectionArguments, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Received field mReference is not initialized\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, injectTooManyArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  NiceMock<MockExpression>* injectArgument2Expression = new NiceMock<MockExpression>();
  NiceMock<MockExpression>* injectArgument3Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(injectArgument2Expression);
  Mock::AllowLeak(injectArgument3Expression);
  Mock::AllowLeak(mThreadVariable);
  Mock::AllowLeak(mCallstackVariable);
  InjectionArgument* injectionArgument1 = new InjectionArgument("withFoo",
                                                                injectArgument1Expression);
  InjectionArgument* injectionArgument2 = new InjectionArgument("withOwner",
                                                                injectArgument2Expression);
  InjectionArgument* injectionArgument3 = new InjectionArgument("withReference",
                                                                injectArgument3Expression);
  injectionArguments.push_back(injectionArgument1);
  injectionArguments.push_back(injectionArgument2);
  injectionArguments.push_back(injectionArgument3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mAdditorController->inject(mContext, injectionArguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Injector could not find field mFoo in object systems.vos.wisey.compiler.tests.CAdditor\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ControllerTest, printToStreamTest) {
  stringstream stringStream;
  mMultiplierController->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external controller systems.vos.wisey.compiler.tests.CMultiplier\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IScienceCalculator,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  constant int MYCONSTANT;\n"
               "\n"
               "  receive int left;\n"
               "  receive int right;\n"
               "\n"
               "  int calculate();\n"
               "  int foo();\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(ControllerTest, createLocalVariableTest) {
  mMultiplierController->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca %systems.vos.wisey.compiler.tests.CMultiplier*"
  "\n  store %systems.vos.wisey.compiler.tests.CMultiplier* null, %systems.vos.wisey.compiler.tests.CMultiplier** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new ReceivedField(mMultiplierController, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mMultiplierController->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");

  EXPECT_NE(variable, nullptr);
}

TEST_F(ControllerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);

  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %0, i64 1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, getObjectTypeNameGlobalTest) {
  EXPECT_EQ(ObjectKindGlobal::getController(mContext),
            mMultiplierController->getObjectTypeNameGlobal(mContext));
}

TEST_F(TestFileRunner, controllerInjectionChainRunTest) {
  runFile("tests/samples/test_controller_injection_chain.yz", 2);
}

TEST_F(TestFileRunner, controllerWithUninitializedStateFieldRunTest) {
  runFile("tests/samples/test_controller_with_unitilized_state_field.yz", 0);
}

TEST_F(TestFileRunner, controllerInjectionWithRecievedOwnerRunTest) {
  runFile("tests/samples/test_controller_injection_with_recieved_owner.yz", 5);
}

TEST_F(TestFileRunner, controllerInjectionWithRecievedReferenceRunTest) {
  runFile("tests/samples/test_controller_injection_with_recieved_reference.yz", 7);
}

TEST_F(TestFileRunner, injectArrayFieldRunTest) {
  runFile("tests/samples/test_inject_array_field.yz", 2018);
}

TEST_F(TestFileRunner, contextScopedInjectionOnIThreadRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_context_scoped_injection_on_ithread.yz",
                                        "Scoped services are the same\n"
                                        "Unscoped services are different\n"
                                        "destructor systems.vos.wisey.compiler.tests.CService\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "destructor systems.vos.wisey.compiler.tests.CService\n"
                                        "destructor systems.vos.wisey.compiler.tests.CScopedService\n",
                                        "");
}

TEST_F(TestFileRunner, injectNonOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_non_owner_run_death_test.yz",
                    1,
                    "tests/samples/test_inject_non_owner_run_death_test.yz\\(9\\): Error: "
                    "Injected fields must have owner type denoted by '\\*'");
}

TEST_F(TestFileRunner, initControllerStateFieldInlineRunDeathTest) {
  expectFailCompile("tests/samples/test_init_controller_state_field_inline.yz",
                    1,
                    "tests/samples/test_init_controller_state_field_inline.yz\\(16\\): Error: "
                    "Use receive type for fileds that are initialized at injection time");
}

TEST_F(TestFileRunner, initControllerStateFieldInInjectFieldRunDeathTest) {
  expectFailCompile("tests/samples/test_init_controller_state_field_in_inject_field.yz",
                    1,
                    "tests/samples/test_init_controller_state_field_in_inject_field.yz\\(14\\): Error: "
                    "Use receive type for fileds that are initialized at injection time");
}

TEST_F(TestFileRunner, controllerScopeInjectedWithReceivedFieldsRunDeathTest) {
  expectFailCompile("tests/samples/test_controller_scope_injected_with_received_fields.yz",
                    1,
                    "tests/samples/test_controller_scope_injected_with_received_fields.yz\\(4\\): Error: "
                    "Scope injected controllers can not have received fields");
}
