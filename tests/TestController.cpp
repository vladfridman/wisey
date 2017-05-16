//
//  TestController.cpp
//  Yazyk
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
#include "yazyk/Controller.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

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
  Method* mMethod;
  StructType* mStructType;
  Field* mLeftField;
  Field* mRightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock *mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ControllerTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> calculatorInterfaceTypes;
    StructType* calculatorIinterfaceStructType = StructType::create(mLLVMContext, "ICalculator");
    calculatorIinterfaceStructType->setBody(calculatorInterfaceTypes);
    vector<MethodArgument*> calculatorInterfaceMethodArguments;
    vector<MethodSignature*> calculatorInterfaceMethods;
    vector<IType*> calculatorThrownExceptions;
    MethodSignature* calculateSignature = new MethodSignature("calculate",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              calculatorInterfaceMethodArguments,
                                                              calculatorThrownExceptions,
                                                              0);
    calculatorInterfaceMethods.push_back(calculateSignature);
    vector<Interface*> calculatorParentInterfaces;
    mCalculatorInterface = new Interface("ICalculator",
                                         calculatorIinterfaceStructType,
                                         calculatorParentInterfaces,
                                         calculatorInterfaceMethods);
    
    vector<Type*> scienceCalculatorInterfaceTypes;
    StructType* scienceCalculatorIinterfaceStructType =
      StructType::create(mLLVMContext, "IScienceCalculator");
    scienceCalculatorIinterfaceStructType->setBody(scienceCalculatorInterfaceTypes);
    vector<MethodSignature*> scienceCalculatorInterfaceMethods;
    scienceCalculatorInterfaceMethods.push_back(calculateSignature);
    vector<Interface*> scienceCalculatorParentInterfaces;
    scienceCalculatorParentInterfaces.push_back(mCalculatorInterface);
    mScienceCalculatorInterface = new Interface("IScienceCalculator",
                                    scienceCalculatorIinterfaceStructType,
                                    scienceCalculatorParentInterfaces,
                                    scienceCalculatorInterfaceMethods);

    vector<Type*> objectInterfaceTypes;
    StructType* objectInterfaceStructType = StructType::create(mLLVMContext, "IObject");
    objectInterfaceStructType->setBody(objectInterfaceTypes);
    vector<MethodArgument*> objectInterfaceMethodArguments;
    vector<MethodSignature*> objectInterfaceMethods;
    vector<IType*> objectThrownExceptions;
    MethodSignature* methodBarSignature = new MethodSignature("foo",
                                                              AccessLevel::PUBLIC_ACCESS,
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
                                                              objectThrownExceptions,
                                                              0);
    objectInterfaceMethods.push_back(methodBarSignature);
    vector<Interface*> objectParentInterfaces;
    mObjectInterface = new Interface("IObject",
                                     objectInterfaceStructType,
                                     objectParentInterfaces,
                                     objectInterfaceMethods);

    
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "CMultiplier");
    mStructType->setBody(types);
    vector<Field*> receivedFields;
    vector<Field*> injectedFields;
    vector<Field*> stateFields;
    ExpressionList fieldArguments;
    mLeftField = new Field(PrimitiveTypes::INT_TYPE, "left", 0, fieldArguments);
    mRightField = new Field(PrimitiveTypes::INT_TYPE, "right", 1, fieldArguments);
    receivedFields.push_back(mLeftField);
    receivedFields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    vector<IType*> thrownExceptions;
    mMethod = new Method("calculate",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         thrownExceptions,
                         NULL,
                         0);
    vector<Method*> methods;
    methods.push_back(mMethod);
    Method* fooMethod = new Method("foo",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   thrownExceptions,
                                   NULL,
                                   1);
    methods.push_back(fooMethod);
    
    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);
    
    mMultiplierController = new Controller("CMultiplier",
                                           mStructType,
                                           receivedFields,
                                           injectedFields,
                                           stateFields,
                                           methods,
                                           interfaces);
    
    vector<Type*> additorTypes;
    additorTypes.push_back(Type::getInt32Ty(mLLVMContext));
    additorTypes.push_back(Type::getInt32Ty(mLLVMContext));
    StructType *additorStructType = StructType::create(mLLVMContext, "CAdditor");
    additorStructType->setBody(additorTypes);
    vector<Field*> additorReceivedFields;
    vector<Field*> additorInjectedFields;
    vector<Field*> additorStateFields;
    additorReceivedFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "left", 0, fieldArguments));
    additorReceivedFields.push_back(new Field(PrimitiveTypes::INT_TYPE,
                                              "right",
                                              1,
                                              fieldArguments));
    vector<Method*> additorMethods;
    vector<Interface*> additorInterfaces;
    mAdditorController = new Controller("CAdditor",
                                        additorStructType,
                                        additorReceivedFields,
                                        additorInjectedFields,
                                        additorStateFields,
                                        additorMethods,
                                        additorInterfaces);
    mContext.addController(mMultiplierController);

    vector<Type*> doublerTypes;
    doublerTypes.push_back(Type::getInt32Ty(mLLVMContext));
    doublerTypes.push_back(Type::getInt32Ty(mLLVMContext));
    StructType *doublerStructType = StructType::create(mLLVMContext, "CDoubler");
    doublerStructType->setBody(doublerTypes);
    vector<Field*> doublerReceivedFields;
    vector<Field*> doublerInjectedFields;
    vector<Field*> doublerStateFields;
    doublerInjectedFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "left", 0, fieldArguments));
    vector<Method*> doublerMethods;
    vector<Interface*> doublerInterfaces;
    mDoublerController = new Controller("CAdditor",
                                        doublerStructType,
                                        doublerReceivedFields,
                                        doublerInjectedFields,
                                        doublerStateFields,
                                        doublerMethods,
                                        doublerInterfaces);
    mContext.addController(mDoublerController);

    vector<Type*> vehicleInterfaceTypes;
    StructType* vehicleInterfaceStructType = StructType::create(mLLVMContext, "IVehicle");
    vehicleInterfaceStructType->setBody(vehicleInterfaceTypes);
    vector<MethodArgument*> vehicleInterfaceMethodArguments;
    vector<MethodSignature*> vehicleInterfaceMethods;
    vector<IType*> vehicleThrownExceptions;
    vector<Interface*> vehicleParentInterfaces;
    mVehicleInterface = new Interface("IVehicle",
                                      vehicleInterfaceStructType,
                                      vehicleParentInterfaces,
                                      vehicleInterfaceMethods);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ControllerTest() {
    delete mStringStream;
  }
};

TEST_F(ControllerTest, modelInstantiationTest) {
  EXPECT_STREQ(mMultiplierController->getName().c_str(), "CMultiplier");
  EXPECT_STREQ(mMultiplierController->getVTableName().c_str(), "controller.CMultiplier.vtable");
  EXPECT_EQ(mMultiplierController->getTypeKind(), CONTROLLER_TYPE);
  EXPECT_EQ(mMultiplierController->getLLVMType(mLLVMContext), mStructType->getPointerTo());
  EXPECT_EQ(mMultiplierController->getInterfaces().size(), 2u);
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

TEST_F(ControllerTest, methodIndexesTest) {
  EXPECT_EQ(mMultiplierController->findMethod("calculate")->getIndex(), 0u);
  EXPECT_EQ(mMultiplierController->findMethod("foo")->getIndex(), 1u);
}

TEST_F(ControllerTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mMultiplierController->getObjectNameGlobalVariableName().c_str(),
               "controller.CMultiplier.name");
}

TEST_F(ControllerTest, getTypeTableNameTest) {
  ASSERT_STREQ(mMultiplierController->getTypeTableName().c_str(),
               "controller.CMultiplier.typetable");
}

TEST_F(ControllerTest, canCastToTest) {
  EXPECT_FALSE(mMultiplierController->canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mMultiplierController->canCastTo(mAdditorController));
  EXPECT_FALSE(mMultiplierController->canCastTo(mVehicleInterface));
  EXPECT_TRUE(mMultiplierController->canCastTo(mMultiplierController));
  EXPECT_TRUE(mMultiplierController->canCastTo(mCalculatorInterface));
}

TEST_F(ControllerTest, canAutoCastToTest) {
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mAdditorController));
  EXPECT_FALSE(mMultiplierController->canAutoCastTo(mVehicleInterface));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mMultiplierController));
  EXPECT_TRUE(mMultiplierController->canAutoCastTo(mCalculatorInterface));
}

TEST_F(ControllerTest, castToFirstInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get((PointerType*) mMultiplierController->getLLVMType(mLLVMContext));
  mMultiplierController->castTo(mContext, pointer, mScienceCalculatorInterface);
  ASSERT_EQ(mBasicBlock->size(), 1u);
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %0 = bitcast %CMultiplier* null to %IScienceCalculator*");
  mStringBuffer.clear();
}

TEST_F(ControllerTest, castToSecondInterfaceTest) {
  ConstantPointerNull* pointer =
  ConstantPointerNull::get((PointerType*) mMultiplierController->getLLVMType(mLLVMContext));
  mMultiplierController->castTo(mContext, pointer, mCalculatorInterface);
  ASSERT_EQ(mBasicBlock->size(), 3u);
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %0 = bitcast %CMultiplier* null to i8*");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %1 = getelementptr i8, i8* %0, i64 8");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %2 = bitcast i8* %1 to %ICalculator*");
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
  
  EXPECT_EQ(6ul, mBasicBlock->size());
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %malloccall = tail call i8* @malloc(i64 mul nuw (i64 ptrtoint "
  "(i32* getelementptr (i32, i32* null, i32 1) to i64), i64 2))"
  "\n  %injectvar = bitcast i8* %malloccall to %CAdditor*"
  "\n  %0 = getelementptr %CAdditor, %CAdditor* %injectvar, i32 0, i32 0\n  store i32 3, i32* %0"
  "\n  %1 = getelementptr %CAdditor, %CAdditor* %injectvar, i32 0, i32 1\n  store i32 5, i32* %1\n";

  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
}

TEST_F(ControllerTest, testInjectWrongTypeOfArgumentDeathTest) {
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

TEST_F(ControllerTest, testInjectNonInjectableTypeDeathTest) {
  ExpressionList injectionArguments;
  
  EXPECT_EXIT(mDoublerController->inject(mContext, injectionArguments),
              ::testing::ExitedWithCode(1),
              "Error: Attempt to inject a variable that is not a Controller or an Interface");
}

TEST_F(ControllerTest, testInjectTooFewArgumentsDeathTest) {
  ExpressionList injectionArguments;
  NiceMock<MockExpression> injectArgument1;
  Mock::AllowLeak(&injectArgument1);
  injectionArguments.push_back(&injectArgument1);
  
  EXPECT_EXIT(mAdditorController->inject(mContext, injectionArguments),
              ::testing::ExitedWithCode(1),
              "Error: Not all received fields of controller 'CAdditor' are initialized.");
}

TEST_F(ControllerTest, testInjectTooManyArgumentsDeathTest) {
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
              "Error: Too many arguments provided when injecting controller 'CAdditor'");
}
