//
//  TestController.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Controller}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Constant.hpp"
#include "wisey/Controller.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/NullType.hpp"
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

struct ControllerTest : public Test {
  Controller* mMultiplierController;
  Controller* mAdditorController;
  Controller* mDoublerController;
  Interface* mCalculatorInterface;
  Interface* mScienceCalculatorInterface;
  Interface* mObjectInterface;
  Interface* mVehicleInterface;
  IMethod* mMethod;
  StructType* mStructType;
  Field* mLeftField;
  Field* mRightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock *mBasicBlock;
  wisey::Constant* mConstant;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  
  ControllerTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::run(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");

    string calculatorFullName = "systems.vos.wisey.compiler.tests.ICalculator";
    StructType* calculatorIinterfaceStructType = StructType::create(mLLVMContext,
                                                                    calculatorFullName);
    VariableList calculatorInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> calculatorInterfaceElements;
    vector<ModelTypeSpecifier*> calculatorThrownExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* calculateSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "calculate",
                                     calculatorInterfaceMethodArguments,
                                     calculatorThrownExceptions);
    calculatorInterfaceElements.push_back(calculateSignature);
    vector<InterfaceTypeSpecifier*> calculatorParentInterfaces;
    mCalculatorInterface = Interface::newInterface(calculatorFullName,
                                                   calculatorIinterfaceStructType,
                                                   calculatorParentInterfaces,
                                                   calculatorInterfaceElements);
    mContext.addInterface(mCalculatorInterface);
    mCalculatorInterface->buildMethods(mContext);
    
    string scienceCalculatorFullName = "systems.vos.wisey.compiler.tests.IScienceCalculator";
    StructType* scienceCalculatorIinterfaceStructType =
      StructType::create(mLLVMContext, scienceCalculatorFullName);
    vector<IObjectElementDeclaration*> scienceCalculatorInterfaceElements;
    scienceCalculatorInterfaceElements.push_back(calculateSignature);
    vector<InterfaceTypeSpecifier*> scienceCalculatorParentInterfaces;
    vector<string> package;
    InterfaceTypeSpecifier* calculatorSpecifier = new InterfaceTypeSpecifier(package,
                                                                             "ICalculator");
    scienceCalculatorParentInterfaces.push_back(calculatorSpecifier);
    mScienceCalculatorInterface = Interface::newInterface(scienceCalculatorFullName,
                                                          scienceCalculatorIinterfaceStructType,
                                                          scienceCalculatorParentInterfaces,
                                                          scienceCalculatorInterfaceElements);
    mContext.addInterface(mScienceCalculatorInterface);
    mScienceCalculatorInterface->buildMethods(mContext);

    string objectFullName = "systems.vos.wisey.compiler.tests.IObject";
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, objectFullName);
    VariableList objectInterfaceMethodArguments;
    vector<IObjectElementDeclaration*> objectInterfaceElements;
    vector<ModelTypeSpecifier*> objectThrownExceptions;
    IObjectElementDeclaration* methodBarSignature =
      new MethodSignatureDeclaration(intSpecifier,
                                     "foo",
                                     objectInterfaceMethodArguments,
                                     objectThrownExceptions);
    objectInterfaceElements.push_back(methodBarSignature);
    vector<InterfaceTypeSpecifier*> objectParentInterfaces;
    mObjectInterface = Interface::newInterface(objectFullName,
                                               objectInterfaceStructType,
                                               objectParentInterfaces,
                                               objectInterfaceElements);
    mContext.addInterface(mObjectInterface);
    mObjectInterface->buildMethods(mContext);

    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string multiplierFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    mStructType = StructType::create(mLLVMContext, multiplierFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    mLeftField = new Field(RECEIVED_FIELD, PrimitiveTypes::INT_TYPE, "left", fieldArguments);
    mRightField = new Field(RECEIVED_FIELD, PrimitiveTypes::INT_TYPE, "right", fieldArguments);
    fields.push_back(mLeftField);
    fields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<const Model*> thrownExceptions;
    mMethod = new Method("calculate",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL);
    vector<IMethod*> methods;
    methods.push_back(mMethod);
    IMethod* fooMethod = new Method("foo",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    thrownExceptions,
                                    NULL);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);
    
    IntConstant* intConstant = new IntConstant(5);
    mConstant = new wisey::Constant(PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    "MYCONSTANT",
                                    intConstant);
    wisey::Constant* privateConstant = new wisey::Constant(PRIVATE_ACCESS,
                                                           PrimitiveTypes::INT_TYPE,
                                                           "MYCONSTANT3",
                                                           intConstant);
    vector<wisey::Constant*> constants;
    constants.push_back(mConstant);
    constants.push_back(privateConstant);

    mMultiplierController = Controller::newController(multiplierFullName, mStructType);
    mMultiplierController->setFields(fields, interfaces.size() + 1);
    mMultiplierController->setMethods(methods);
    mMultiplierController->setInterfaces(interfaces);
    mMultiplierController->setConstants(constants);
    
    vector<Type*> additorTypes;
    additorTypes.push_back(Type::getInt64Ty(mLLVMContext));
    additorTypes.push_back(Type::getInt32Ty(mLLVMContext));
    additorTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string additorFullName = "systems.vos.wisey.compiler.tests.CAdditor";
    StructType *additorStructType = StructType::create(mLLVMContext, additorFullName);
    additorStructType->setBody(additorTypes);
    vector<Field*> additorFields;
    additorFields.push_back(new Field(RECEIVED_FIELD,
                                      PrimitiveTypes::INT_TYPE,
                                      "left",
                                      fieldArguments));
    additorFields.push_back(new Field(RECEIVED_FIELD,
                                      PrimitiveTypes::INT_TYPE,
                                      "right",
                                      fieldArguments));
    mAdditorController = Controller::newController(additorFullName, additorStructType);
    mAdditorController->setFields(additorFields, 1u);
    mContext.addController(mMultiplierController);

    vector<Type*> doublerTypes;
    doublerTypes.push_back(Type::getInt64Ty(mLLVMContext));
    doublerTypes.push_back(Type::getInt32Ty(mLLVMContext));
    doublerTypes.push_back(Type::getInt32Ty(mLLVMContext));
    string doublerFullName = "systems.vos.wisey.compiler.tests.CDoubler";
    StructType* doublerStructType = StructType::create(mLLVMContext, doublerFullName);
    doublerStructType->setBody(doublerTypes);
    vector<Field*> doublerFields;
    doublerFields.push_back(new Field(INJECTED_FIELD,
                                      PrimitiveTypes::INT_TYPE,
                                      "left",
                                      fieldArguments));
    mDoublerController = Controller::newController(doublerFullName, doublerStructType);
    mDoublerController->setFields(doublerFields, 1u);
    mContext.addController(mDoublerController);

    string vehicleFullName = "systems.vos.wisey.compiler.tests.IVehicle";
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, vehicleFullName);
    vector<InterfaceTypeSpecifier*> vehicleParentInterfaces;
    vector<IObjectElementDeclaration*> vehicleElements;
    mVehicleInterface = Interface::newInterface(vehicleFullName,
                                                vehicleInterfaceStructType,
                                                vehicleParentInterfaces,
                                                vehicleElements);
    mContext.addInterface(mVehicleInterface);
    mVehicleInterface->buildMethods(mContext);

    IConcreteObjectType::generateNameGlobal(mContext, mAdditorController);
    IConcreteObjectType::generateVTable(mContext, mAdditorController);
    
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
  
  ~ControllerTest() {
    delete mStringStream;
  }
};

TEST_F(ControllerTest, getNameTest) {
  EXPECT_STREQ(mMultiplierController->getName().c_str(),
               "systems.vos.wisey.compiler.tests.CMultiplier");
}

TEST_F(ControllerTest, getShortNameTest) {
  EXPECT_STREQ(mMultiplierController->getShortName().c_str(),
               "CMultiplier");
}

TEST_F(ControllerTest, getVTableNameTest) {
  EXPECT_STREQ(mMultiplierController->getVTableName().c_str(),
               "systems.vos.wisey.compiler.tests.CMultiplier.vtable");
}

TEST_F(ControllerTest, getTypeKindTest) {
  EXPECT_EQ(mMultiplierController->getTypeKind(), CONTROLLER_TYPE);
}

TEST_F(ControllerTest, getLLVMTypeTest) {
  EXPECT_EQ(mMultiplierController->getLLVMType(mLLVMContext),
            mStructType->getPointerTo()->getPointerTo());
}

TEST_F(ControllerTest, getInterfacesTest) {
  EXPECT_EQ(mMultiplierController->getInterfaces().size(), 2u);
}

TEST_F(ControllerTest, getVTableSizeTest) {
  EXPECT_EQ(mMultiplierController->getVTableSize(), 3u);
}

TEST_F(ControllerTest, getFieldsTest) {
  EXPECT_EQ(mMultiplierController->getFields().size(), 2u);
}

TEST_F(ControllerTest, getOwnerTest) {
  ASSERT_NE(mMultiplierController->getOwner(), nullptr);
  EXPECT_EQ(mMultiplierController->getOwner()->getObject(), mMultiplierController);
}

TEST_F(ControllerTest, getFieldIndexTest) {
  EXPECT_EQ(mMultiplierController->getFieldIndex(mLeftField), 3u);
  EXPECT_EQ(mMultiplierController->getFieldIndex(mRightField), 4u);
}

TEST_F(ControllerTest, findFeildTest) {
  EXPECT_EQ(mMultiplierController->findField("left"), mLeftField);
  EXPECT_EQ(mMultiplierController->findField("right"), mRightField);
  EXPECT_EQ(mMultiplierController->findField("depth"), nullptr);
}

TEST_F(ControllerTest, findMethodTest) {
  EXPECT_EQ(mMultiplierController->findMethod("calculate"), mMethod);
  EXPECT_EQ(mMultiplierController->findMethod("bar"), nullptr);
}

TEST_F(ControllerTest, findConstantTest) {
  EXPECT_EQ(mMultiplierController->findConstant("MYCONSTANT"), mConstant);
}

TEST_F(ControllerTest, findConstantDeathTest) {
  EXPECT_EXIT(mMultiplierController->findConstant("MYCONSTANT2"),
              ::testing::ExitedWithCode(1),
              "Error: Controller systems.vos.wisey.compiler.tests.CMultiplier "
              "does not have constant named MYCONSTANT2");
}

TEST_F(ControllerTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mMultiplierController->getObjectNameGlobalVariableName().c_str(),
               "systems.vos.wisey.compiler.tests.CMultiplier.name");
}

TEST_F(ControllerTest, getTypeTableNameTest) {
  ASSERT_STREQ(mMultiplierController->getTypeTableName().c_str(),
               "systems.vos.wisey.compiler.tests.CMultiplier.typetable");
}

TEST_F(ControllerTest, canCastToTest) {
  EXPECT_FALSE(mMultiplierController->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mMultiplierController->canCastTo(mAdditorController));
  EXPECT_FALSE(mMultiplierController->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mMultiplierController->canCastTo(mMultiplierController));
  EXPECT_TRUE(mMultiplierController->canCastTo(mCalculatorInterface));
  EXPECT_FALSE(mMultiplierController->canCastTo(NullType::NULL_TYPE));
}

TEST_F(ControllerTest, canAutoCastToTest) {
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mAdditorController));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mVehicleInterface));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mMultiplierController));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mCalculatorInterface));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(NullType::NULL_TYPE));
}

TEST_F(ControllerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get((PointerType*) mMultiplierController->getLLVMType(mLLVMContext));
  mMultiplierController->castTo(mContext, pointer, mScienceCalculatorInterface);
  EXPECT_EQ(mBasicBlock->size(), 1u);
  
  *mStringStream << *mBasicBlock->begin();
  EXPECT_STREQ(mStringStream->str().c_str(),
              "  %0 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier** null to "
               "%systems.vos.wisey.compiler.tests.IScienceCalculator**");
  mStringBuffer.clear();
}

TEST_F(ControllerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get(mMultiplierController->getLLVMType(mLLVMContext));
  mMultiplierController->castTo(mContext, pointer, mCalculatorInterface);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.CMultiplier*, "
  "%systems.vos.wisey.compiler.tests.CMultiplier** null"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.CMultiplier* %0 to i8*"
  "\n  %2 = getelementptr i8, i8* %1, i64 8"
  "\n  %3 = alloca %systems.vos.wisey.compiler.tests.ICalculator*"
  "\n  %4 = bitcast i8* %2 to %systems.vos.wisey.compiler.tests.ICalculator*"
  "\n  store %systems.vos.wisey.compiler.tests.ICalculator* %4, "
  "%systems.vos.wisey.compiler.tests.ICalculator** %3\n";
  
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
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
  ExpressionList injectionArguments;
  NiceMock<MockExpression> injectArgument1;
  NiceMock<MockExpression> injectArgument2;
  Value* field1Value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
  ON_CALL(injectArgument1, generateIR(_)).WillByDefault(Return(field1Value));
  ON_CALL(injectArgument1, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  Value* field2Value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  ON_CALL(injectArgument2, generateIR(_)).WillByDefault(Return(field2Value));
  ON_CALL(injectArgument2, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  injectionArguments.push_back(&injectArgument1);
  injectionArguments.push_back(&injectArgument2);

  Value* result = mAdditorController->inject(mContext, injectionArguments);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  EXPECT_EQ(8ul, mBasicBlock->size());
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%systems.vos.wisey.compiler.tests.CAdditor* "
  "getelementptr (%systems.vos.wisey.compiler.tests.CAdditor, "
  "%systems.vos.wisey.compiler.tests.CAdditor* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CAdditor*"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, "
  "%systems.vos.wisey.compiler.tests.CAdditor* %injectvar, i32 0, i32 0"
  "\n  store i64 0, i64* %0"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, "
  "%systems.vos.wisey.compiler.tests.CAdditor* %injectvar, i32 0, i32 1"
  "\n  store i32 3, i32* %1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.CAdditor, "
  "%systems.vos.wisey.compiler.tests.CAdditor* %injectvar, i32 0, i32 2"
  "\n  store i32 5, i32* %2\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, injectWrongTypeOfArgumentDeathTest) {
  ExpressionList injectionArguments;
  NiceMock<MockExpression> injectArgument1;
  NiceMock<MockExpression> injectArgument2;
  Mock::AllowLeak(&injectArgument1);
  Mock::AllowLeak(&injectArgument2);
  Value* field1Value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(injectArgument1, generateIR(_)).WillByDefault(Return(field1Value));
  ON_CALL(injectArgument1, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  Value* field2Value = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(injectArgument2, generateIR(_)).WillByDefault(Return(field2Value));
  ON_CALL(injectArgument2, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  injectionArguments.push_back(&injectArgument1);
  injectionArguments.push_back(&injectArgument2);
  
  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments),
              ::testing::ExitedWithCode(1),
              "Error: Controller injector argumet value for field 'right' does not match its type");
}

TEST_F(ControllerTest, injectNonInjectableTypeDeathTest) {
  ExpressionList injectionArguments;
  
  EXPECT_EXIT(mDoublerController->inject(mContext, injectionArguments),
              ::testing::ExitedWithCode(1),
              "Error: Attempt to inject a variable that is not a Controller or an Interface");
}

TEST_F(ControllerTest, injectTooFewArgumentsDeathTest) {
  ExpressionList injectionArguments;
  NiceMock<MockExpression> injectArgument1;
  Mock::AllowLeak(&injectArgument1);
  injectionArguments.push_back(&injectArgument1);
  
  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments),
              ::testing::ExitedWithCode(1),
              "Error: Not all received fields of controller "
              "systems.vos.wisey.compiler.tests.CAdditor are initialized.");
}

TEST_F(ControllerTest, injectTooManyArgumentsDeathTest) {
  ExpressionList injectionArguments;
  NiceMock<MockExpression> injectArgument1;
  NiceMock<MockExpression> injectArgument2;
  NiceMock<MockExpression> injectArgument3;
  Mock::AllowLeak(&injectArgument1);
  Mock::AllowLeak(&injectArgument2);
  Mock::AllowLeak(&injectArgument3);
  injectionArguments.push_back(&injectArgument1);
  injectionArguments.push_back(&injectArgument2);
  injectionArguments.push_back(&injectArgument3);
  
  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments),
              ::testing::ExitedWithCode(1),
              "Error: Too many arguments provided when injecting controller "
              "systems.vos.wisey.compiler.tests.CAdditor");
}

TEST_F(ControllerTest, injectFieldTest) {
  ExpressionList fieldArguments;

  vector<Type*> childTypes;
  string childFullName = "systems.vos.wisey.compiler.tests.CChild";
  StructType* childStructType = StructType::create(mLLVMContext, childFullName);
  childTypes.push_back(Type::getInt64Ty(mLLVMContext));
  childStructType->setBody(childTypes);
  vector<Field*> childFields;
  Controller* childController = Controller::newController(childFullName, childStructType);
  childController->setFields(childFields, 1u);
  mContext.addController(childController);

  vector<Type*> parentTypes;
  parentTypes.push_back(Type::getInt64Ty(mLLVMContext));
  parentTypes.push_back(childController->getLLVMType(mLLVMContext)->getPointerElementType());
  string parentFullName = "systems.vos.wisey.compiler.tests.CParent";
  StructType* parentStructType = StructType::create(mLLVMContext, parentFullName);
  parentStructType->setBody(parentTypes);
  vector<Field*> parentFields;
  parentFields.push_back(new Field(INJECTED_FIELD,
                                   childController->getOwner(),
                                   "mChild",
                                   fieldArguments));
  Controller* parentController = Controller::newController(parentFullName, parentStructType);
  parentController->setFields(parentFields, 1u);
  mContext.addController(parentController);

  ExpressionList injectionArguments;
  Value* result = parentController->inject(mContext, injectionArguments);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
  "(%systems.vos.wisey.compiler.tests.CParent* getelementptr "
  "(%systems.vos.wisey.compiler.tests.CParent, "
  "%systems.vos.wisey.compiler.tests.CParent* null, i32 1) to i64))"
  "\n  %injectvar = bitcast i8* %malloccall to %systems.vos.wisey.compiler.tests.CParent*"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CParent, "
  "%systems.vos.wisey.compiler.tests.CParent* %injectvar, i32 0, i32 0"
  "\n  store i64 0, i64* %0"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (i64* getelementptr "
  "(i64, i64* null, i32 1) to i64))"
  "\n  %injectvar2 = bitcast i8* %malloccall1 to %systems.vos.wisey.compiler.tests.CChild*"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.CChild, "
  "%systems.vos.wisey.compiler.tests.CChild* %injectvar2, i32 0, i32 0"
  "\n  store i64 0, i64* %1"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.CParent, "
    "%systems.vos.wisey.compiler.tests.CParent* %injectvar, i32 0, i32 1"
  "\n  store %systems.vos.wisey.compiler.tests.CChild* "
    "%injectvar2, %systems.vos.wisey.compiler.tests.CChild** %2\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, printToStreamTest) {
  stringstream stringStream;
  mMultiplierController->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external controller systems.vos.wisey.compiler.tests.CMultiplier\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IScienceCalculator,\n"
               "    systems.vos.wisey.compiler.tests.IObject {\n"
               "\n"
               "  receive int left;\n"
               "  receive int right;\n"
               "\n"
               "  public constant int MYCONSTANT = 5;\n"
               "\n"
               "  int calculate();\n"
               "  int foo();\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, controllerInjectionChainRunTest) {
  runFile("tests/samples/test_controller_injection_chain.yz", "2");
}

TEST_F(TestFileSampleRunner, controllerWithUninitializedStateFieldRunTest) {
  runFile("tests/samples/test_controller_with_unitilized_state_field.yz", "0");
}

TEST_F(TestFileSampleRunner, injectNonOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_non_owner_run_death_test.yz",
                    1,
                    "Error: Injected fields must have owner type denoted by '\\*'");
}
