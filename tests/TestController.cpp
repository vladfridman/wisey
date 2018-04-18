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
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/Constant.hpp"
#include "wisey/Controller.hpp"
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
  NiceMock<MockVariable>* mThreadVariable;
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
    VariableList calculatorInterfaceMethodArguments;
    vector<IObjectElementDefinition*> calculatorInterfaceElements;
    vector<IModelTypeSpecifier*> calculatorThrownExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    IObjectElementDefinition* calculateSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "calculate",
                                     calculatorInterfaceMethodArguments,
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
                                                   0);
    mContext.addInterface(mCalculatorInterface);
    mCalculatorInterface->buildMethods(mContext);
    
    string scienceCalculatorFullName = "systems.vos.wisey.compiler.tests.IScienceCalculator";
    StructType* scienceCalculatorIinterfaceStructType =
      StructType::create(mLLVMContext, scienceCalculatorFullName);
    vector<IObjectElementDefinition*> scienceCalculatorInterfaceElements;
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
                                                          0);
    mContext.addInterface(mScienceCalculatorInterface);
    mScienceCalculatorInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDefinition*> objectInterfaceElements;
    vector<IModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDefinition* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceMethodArguments,
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
                                               0);
    mContext.addInterface(mObjectInterface);
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
                                                      5);
    vector<IField*> fields;
    mLeftField = new ReceivedField(PrimitiveTypes::INT_TYPE, "left", 0);
    mRightField = new ReceivedField(PrimitiveTypes::INT_TYPE, "right", 0);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method(mMultiplierController,
                         "calculate",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
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
                                    PrimitiveTypes::INT_TYPE,
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

    mMultiplierController->setFields(fields, interfaces.size());
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
    mOwnerNode = Node::newNode(AccessLevel::PUBLIC_ACCESS, ownerFullName, ownerStructType, 0);
    mContext.addNode(mOwnerNode);
    
    vector<Type*> referenceTypes;
    referenceTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                             ->getPointerTo()->getPointerTo());
    string referenceFullName = "systems.vos.wisey.compiler.tests.MReference";
    StructType* referenceStructType = StructType::create(mLLVMContext, referenceFullName);
    referenceStructType->setBody(referenceTypes);
    mReferenceModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                      referenceFullName,
                                      referenceStructType,
                                      0);
    mContext.addModel(mReferenceModel);
    
    vector<Type*> additorTypes;
    additorTypes.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                             ->getPointerTo()->getPointerTo());
    additorTypes.push_back(mOwnerNode->getOwner()->getLLVMType(mContext));
    additorTypes.push_back(mReferenceModel->getLLVMType(mContext));
    string additorFullName = "systems.vos.wisey.compiler.tests.CAdditor";
    StructType* additorStructType = StructType::create(mLLVMContext, additorFullName);
    additorStructType->setBody(additorTypes);
    vector<IField*> additorFields;
    additorFields.push_back(new ReceivedField(mOwnerNode->getOwner(), "mOwner", 0));
    additorFields.push_back(new ReceivedField(mReferenceModel, "mReference", 0));
    mAdditorController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                   additorFullName,
                                                   additorStructType,
                                                   0);
    mAdditorController->setFields(additorFields, 1u);
    mContext.addController(mMultiplierController);

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
    doublerFields.push_back(new InjectedField(PrimitiveTypes::INT_TYPE,
                                              PrimitiveTypes::INT_TYPE,
                                              "left",
                                              fieldArguments,
                                              mContext.getImportProfile()->getSourceFileName(),
                                              3));
    mDoublerController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                   doublerFullName,
                                                   doublerStructType,
                                                   0);
    mDoublerController->setFields(doublerFields, 1u);
    mContext.addController(mDoublerController);

    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<IInterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDefinition*> vehicleElements;
    mVehicleInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleElements,
                                                0);
    mContext.addInterface(mVehicleInterface);
    mVehicleInterface->buildMethods(mContext);

    IConcreteObjectType::generateNameGlobal(mContext, mOwnerNode);
    IConcreteObjectType::generateShortNameGlobal(mContext, mOwnerNode);
    IConcreteObjectType::generateVTable(mContext, mOwnerNode);
    IConcreteObjectType::generateNameGlobal(mContext, mAdditorController);
    IConcreteObjectType::generateShortNameGlobal(mContext, mAdditorController);
    IConcreteObjectType::generateVTable(mContext, mAdditorController);

    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ControllerTest() {
    delete mStringStream;
    delete mThreadVariable;
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

TEST_F(ControllerTest, getVTableSizeTest) {
  EXPECT_EQ(3u, mMultiplierController->getVTableSize());
}

TEST_F(ControllerTest, getFieldsTest) {
  EXPECT_EQ(2u, mMultiplierController->getFields().size());
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
  EXPECT_EQ(mConstant, mMultiplierController->findConstant("MYCONSTANT"));
}

TEST_F(ControllerTest, findConstantDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  
  EXPECT_EXIT(mMultiplierController->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Controller systems.vos.wisey.compiler.tests.CMultiplier "
              "does not have constant named MYCONSTANT2");
}

TEST_F(ControllerTest, findLLVMFunctionTest) {
  EXPECT_EQ(mLLVMFunction, mMultiplierController->findLLVMFunction("myfunction"));
}

TEST_F(ControllerTest, findLLVMFunctionDeathTest) {
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mMultiplierController->findLLVMFunction("nonexistingfunction"),
              ::testing::ExitedWithCode(1),
              "LLVM function nonexistingfunction not found in object systems.vos.wisey.compiler.tests.CMultiplier");
}

TEST_F(ControllerTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier.name",
               mMultiplierController->getObjectNameGlobalVariableName().c_str());
}

TEST_F(ControllerTest, getObjectShortNameGlobalVariableNameTest) {
  ASSERT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier.shortname",
               mMultiplierController->getObjectShortNameGlobalVariableName().c_str());
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

TEST_F(ControllerTest, getReferenceAdjustmentFunctionTest) {
  Function* result = mMultiplierController->getReferenceAdjustmentFunction(mContext);
  
  ASSERT_NE(nullptr, result);
  EXPECT_EQ(result, AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(mContext));
}

TEST_F(ControllerTest, canCastToTest) {
  EXPECT_FALSE(mMultiplierController->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
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
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
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
}

TEST_F(ControllerTest, isObjectTest) {
  EXPECT_TRUE(mMultiplierController->isController());
  EXPECT_FALSE(mMultiplierController->isInterface());
  EXPECT_FALSE(mMultiplierController->isModel());
  EXPECT_FALSE(mMultiplierController->isNode());
  EXPECT_FALSE(mMultiplierController->isThread());
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

TEST_F(ControllerTest, injectTest) {
  InjectionArgumentList injectionArguments;

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
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CAdditor.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CAdditor.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor.refCounter, %systems.vos.wisey.compiler.tests.CAdditor.refCounter* %injectvar, i32 0, i32 1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, %systems.vos.wisey.compiler.tests.CAdditor* %1, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.NOwner* null, %systems.vos.wisey.compiler.tests.NOwner** %2"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, %systems.vos.wisey.compiler.tests.CAdditor* %1, i32 0, i32 2"
  "\n  store %systems.vos.wisey.compiler.tests.MReference* null, %systems.vos.wisey.compiler.tests.MReference** %3"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MReference* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 1)"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.CAdditor* %1 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CAdditor.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
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
  ON_CALL(*injectArgument1Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  Value* field2Value = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*injectArgument2Expression, generateIR(_, _)).WillByDefault(Return(field2Value));
  ON_CALL(*injectArgument2Expression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  InjectionArgument* injectionArgument1 = new InjectionArgument("withOwner",
                                                                injectArgument1Expression);
  InjectionArgument* injectionArgument2 = new InjectionArgument("withReference",
                                                                injectArgument2Expression);

  injectionArguments.push_back(injectionArgument1);
  injectionArguments.push_back(injectionArgument2);
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Injector argumet value for field 'mOwner' "
              "does not match its type");
}

TEST_F(ControllerTest, injectNonInjectableTypeDeathTest) {
  InjectionArgumentList injectionArguments;
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(mDoublerController->inject(mContext, injectionArguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type int is not injectable");
}

TEST_F(ControllerTest, notWellFormedInjectionArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(mThreadVariable);
  InjectionArgument* injectionArgument = new InjectionArgument("owner",
                                                               injectArgument1Expression);
  injectionArguments.push_back(injectionArgument);

  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Injection argument should start with 'with'. e.g. .withField\\(value\\).\n"
              "Error: Some injection arguments for injected object "
              "systems.vos.wisey.compiler.tests.CAdditor are not well formed");
}

TEST_F(ControllerTest, injectTooFewArgumentsDeathTest) {
  InjectionArgumentList injectionArguments;
  NiceMock<MockExpression>* injectArgument1Expression = new NiceMock<MockExpression>();
  Mock::AllowLeak(injectArgument1Expression);
  Mock::AllowLeak(mThreadVariable);
  InjectionArgument* injectionArgument = new InjectionArgument("withOwner",
                                                               injectArgument1Expression);
  injectionArguments.push_back(injectionArgument);
  
  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Received field mReference is not initialized");
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
  InjectionArgument* injectionArgument1 = new InjectionArgument("withFoo",
                                                                injectArgument1Expression);
  InjectionArgument* injectionArgument2 = new InjectionArgument("withOwner",
                                                                injectArgument2Expression);
  InjectionArgument* injectionArgument3 = new InjectionArgument("withReference",
                                                                injectArgument3Expression);
  injectionArguments.push_back(injectionArgument1);
  injectionArguments.push_back(injectionArgument2);
  injectionArguments.push_back(injectionArgument3);
  
  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments, 0),
              ::testing::ExitedWithCode(1),
              "Error: Injector could not find field mFoo in object "
              "systems.vos.wisey.compiler.tests.CAdditor");
}

TEST_F(ControllerTest, injectFieldTest) {
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
                                                          0);
  childController->setFields(childFields, 1u);
  mContext.addController(childController);
  IConcreteObjectType::generateNameGlobal(mContext, childController);
  IConcreteObjectType::generateShortNameGlobal(mContext, childController);
  IConcreteObjectType::generateVTable(mContext, childController);

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
                                                           0);
  parentController->setFields(parentFields, 1u);
  mContext.addController(parentController);
  IConcreteObjectType::generateNameGlobal(mContext, parentController);
  IConcreteObjectType::generateShortNameGlobal(mContext, parentController);
  IConcreteObjectType::generateVTable(mContext, parentController);

  InjectionArgumentList injectionArguments;
  Value* result = parentController->inject(mContext, injectionArguments, 0);
  
  EXPECT_NE(result, nullptr);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CParent.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CParent.refCounter, %systems.vos.wisey.compiler.tests.CParent.refCounter* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CParent.refCounter*"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.CParent.refCounter* %injectvar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CParent.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CParent.refCounter, %systems.vos.wisey.compiler.tests.CParent.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CParent.refCounter, %systems.vos.wisey.compiler.tests.CParent.refCounter* %injectvar, i32 0, i32 1"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CChild.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CChild.refCounter, %systems.vos.wisey.compiler.tests.CChild.refCounter* null, i32 1) to i64))"
  "\n  %injectvar2 = bitcast i8* %malloccall1 to %systems.vos.wisey.compiler.tests.CChild.refCounter*"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.CChild.refCounter* %injectvar2 to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 ptrtoint (%systems.vos.wisey.compiler.tests.CChild.refCounter* getelementptr (%systems.vos.wisey.compiler.tests.CChild.refCounter, %systems.vos.wisey.compiler.tests.CChild.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.CChild.refCounter, %systems.vos.wisey.compiler.tests.CChild.refCounter* %injectvar2, i32 0, i32 1"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.CChild* %3 to i8*"
  "\n  %5 = getelementptr i8, i8* %4, i64 0"
  "\n  %6 = bitcast i8* %5 to i32 (...)***"
  "\n  %7 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CChild.vtable, i32 0, i32 0, i32 0"
  "\n  %8 = bitcast i8** %7 to i32 (...)**"
  "\n  store i32 (...)** %8, i32 (...)*** %6"
  "\n  %9 = getelementptr %systems.vos.wisey.compiler.tests.CParent, %systems.vos.wisey.compiler.tests.CParent* %1, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.CChild* %3, %systems.vos.wisey.compiler.tests.CChild** %9"
  "\n  %10 = bitcast %systems.vos.wisey.compiler.tests.CParent* %1 to i8*"
  "\n  %11 = getelementptr i8, i8* %10, i64 0"
  "\n  %12 = bitcast i8* %11 to i32 (...)***"
  "\n  %13 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @systems.vos.wisey.compiler.tests.CParent.vtable, i32 0, i32 0, i32 0"
  "\n  %14 = bitcast i8** %13 to i32 (...)**"
  "\n  store i32 (...)** %14, i32 (...)*** %12"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, printToStreamTest) {
  stringstream stringStream;
  Model* innerPublicModel = Model::newModel(PUBLIC_ACCESS, "MInnerPublicModel", NULL, 0);
  vector<IField*> fields;
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField1", 0));
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "mField2", 0));
  innerPublicModel->setFields(fields, 0);
  
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
  
  mMultiplierController->addInnerObject(innerPublicModel);
  mMultiplierController->addInnerObject(innerPrivateModel);
  mMultiplierController->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external controller systems.vos.wisey.compiler.tests.CMultiplier\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IScienceCalculator,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  receive int left;\n"
               "  receive int right;\n"
               "\n"
               "  int calculate();\n"
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

TEST_F(ControllerTest, createLocalVariableTest) {
  mMultiplierController->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %referenceDeclaration = alloca %systems.vos.wisey.compiler.tests.CMultiplier*"
  "\n  store %systems.vos.wisey.compiler.tests.CMultiplier* null, %systems.vos.wisey.compiler.tests.CMultiplier** %referenceDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mMultiplierController, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mMultiplierController->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");

  EXPECT_NE(variable, nullptr);
}

TEST_F(ControllerTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mMultiplierController->getLLVMType(mContext));
  mMultiplierController->createParameterVariable(mContext, "var", value);
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

TEST_F(TestFileRunner, controllerInjectionChainRunTest) {
  runFile("tests/samples/test_controller_injection_chain.yz", "2");
}

TEST_F(TestFileRunner, controllerWithUninitializedStateFieldRunTest) {
  runFile("tests/samples/test_controller_with_unitilized_state_field.yz", "0");
}

TEST_F(TestFileRunner, controllerInjectionWithRecievedOwnerRunTest) {
  runFile("tests/samples/test_controller_injection_with_recieved_owner.yz", "5");
}

TEST_F(TestFileRunner, controllerInjectionWithRecievedReferenceRunTest) {
  runFile("tests/samples/test_controller_injection_with_recieved_reference.yz", "7");
}

TEST_F(TestFileRunner, injectArrayFieldRunTest) {
  runFile("tests/samples/test_inject_array_field.yz", "2018");
}

TEST_F(TestFileRunner, injectNonOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_non_owner_run_death_test.yz",
                    1,
                    "tests/samples/test_inject_non_owner_run_death_test.yz\\(9\\): Error: Injected fields must have owner type denoted by '\\*'");
}

