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

#include "yazyk/Controller.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct ControllerTest : public Test {
  Controller* mController;
  Controller* mAdditorController;
  Interface* mCalculatorInterface;
  Interface* mScienceCalculatorInterface;
  Interface* mObjectInterface;
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
    MethodSignature* calculateSignature = new MethodSignature("calculate",
                                                              PrimitiveTypes::INT_TYPE,
                                                              calculatorInterfaceMethodArguments,
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
    vector<MethodArgument*> scienceCalculatorInterfaceMethodArguments;
    vector<MethodSignature*> scienceCalculatorInterfaceMethods;
    calculateSignature = new MethodSignature("calculate",
                                             PrimitiveTypes::INT_TYPE,
                                             scienceCalculatorInterfaceMethodArguments,
                                             0);
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
    MethodSignature* methodBarSignature = new MethodSignature("bar",
                                                              PrimitiveTypes::INT_TYPE,
                                                              objectInterfaceMethodArguments,
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
    mLeftField = new Field(PrimitiveTypes::INT_TYPE, "left", 0);
    mRightField = new Field(PrimitiveTypes::INT_TYPE, "right", 1);
    receivedFields.push_back(mLeftField);
    receivedFields.push_back(mRightField);
    vector<MethodArgument*> methodArguments;
    mMethod = new Method("calculate", PrimitiveTypes::INT_TYPE, methodArguments, 0, NULL);
    vector<Method*> methods;
    methods.push_back(mMethod);
    methods.push_back(new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments, 1, NULL));
    
    vector<Interface*> interfaces;
    interfaces.push_back(mScienceCalculatorInterface);
    interfaces.push_back(mObjectInterface);
    
    mController = new Controller("CMultiplier",
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
    stateFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "left", 0));
    stateFields.push_back(new Field(PrimitiveTypes::INT_TYPE, "right", 1));
    vector<Method*> additorMethods;
    vector<Interface*> additorInterfaces;
    mAdditorController = new Controller("CAdditor",
                                        additorStructType,
                                        additorReceivedFields,
                                        additorInjectedFields,
                                        additorStateFields,
                                        additorMethods,
                                        additorInterfaces);
    mContext.addController(mController);

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
  EXPECT_STREQ(mController->getName().c_str(), "CMultiplier");
  EXPECT_STREQ(mController->getVTableName().c_str(), "controller.CMultiplier.vtable");
  EXPECT_EQ(mController->getTypeKind(), CONTROLLER_TYPE);
  EXPECT_EQ(mController->getLLVMType(mLLVMContext), mStructType->getPointerTo());
  EXPECT_EQ(mController->getInterfaces().size(), 2u);
}

TEST_F(ControllerTest, findFeildTest) {
  EXPECT_EQ(mController->findField("left"), mLeftField);
  EXPECT_EQ(mController->findField("right"), mRightField);
  EXPECT_EQ(mController->findField("depth"), nullptr);
}

TEST_F(ControllerTest, findMethodTest) {
  EXPECT_EQ(mController->findMethod("calculate"), mMethod);
  EXPECT_EQ(mController->findMethod("bar"), nullptr);
}

TEST_F(ControllerTest, methodIndexesTest) {
  EXPECT_EQ(mController->findMethod("calculate")->getIndex(), 0u);
  EXPECT_EQ(mController->findMethod("foo")->getIndex(), 1u);
}

TEST_F(ControllerTest, getObjectNameGlobalVariableNameTest) {
  ASSERT_STREQ(mController->getObjectNameGlobalVariableName().c_str(),
               "controller.CMultiplier.name");
}

TEST_F(ControllerTest, getTypeTableName) {
  ASSERT_STREQ(mController->getTypeTableName().c_str(), "controller.CMultiplier.typetable");
}

TEST_F(ControllerTest, getFlattenedInterfaceHierarchyTest) {
  vector<Interface*> allInterfaces = mController->getFlattenedInterfaceHierarchy();
  
  EXPECT_EQ(allInterfaces.size(), 3u);
  EXPECT_EQ(allInterfaces.at(0), mScienceCalculatorInterface);
  EXPECT_EQ(allInterfaces.at(1), mCalculatorInterface);
  EXPECT_EQ(allInterfaces.at(2), mObjectInterface);
}

TEST_F(ControllerTest, testInject) {
  Value* result = mAdditorController->inject(mContext);
  
  EXPECT_NE(result, nullptr);
  EXPECT_TRUE(BitCastInst::classof(result));
  
  EXPECT_EQ(2ul, mBasicBlock->size());
  
  BasicBlock::iterator iterator = mBasicBlock->begin();
  *mStringStream << *iterator;
  string expected = "  %malloccall = tail call i8* @malloc(i32 trunc (i64 mul nuw (i64 ptrtoint"
    " (i32* getelementptr (i32, i32* null, i32 1) to i64), i64 2) to i32))";
  EXPECT_STREQ(mStringStream->str().c_str(), expected.c_str());
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %injectvar = bitcast i8* %malloccall to %CAdditor*");
  mStringBuffer.clear();
}
