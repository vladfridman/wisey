//
//  TestModelDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/Interface.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/ModelDefinition.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

class MockStatement : public IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct ModelDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  MethodDeclaration *mMethodDeclaration;
  vector<ModelFieldDeclaration *> mFields;
  vector<MethodDeclaration *> mMethodDeclarations;
  Block mBlock;
  NiceMock<MockStatement> mMockStatement;
 
  
  ModelDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    mBlock.getStatements().push_back(&mMockStatement);
    CompoundStatement* compoundStatement = new CompoundStatement(mBlock);
    PrimitiveTypeSpecifier* intTypeSpecifier =
      new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
      new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
      new VariableDeclaration(*intTypeSpecifier, *intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    mMethodDeclaration = new MethodDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                               *floatTypeSpecifier,
                                               "foo",
                                               methodArguments,
                                               *compoundStatement);
    mMethodDeclarations.push_back(mMethodDeclaration);
  }
};

TEST_F(ModelDefinitionTest, TestSimpleDefinition) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  ModelFieldDeclaration* field1 = new ModelFieldDeclaration(*longType, "field1");
  ModelFieldDeclaration* field2 = new ModelFieldDeclaration(*floatType, "field2");
  mFields.push_back(field1);
  mFields.push_back(field2);
  
  vector<string> interfaces;
  ModelDefinition modelDefinition("mymodel", mFields, mMethodDeclarations, interfaces);

  EXPECT_CALL(mMockStatement, generateIR(_));
  
  modelDefinition.generateIR(mContext);
  Model* model = mContext.getModel("mymodel");
  StructType* structType = (StructType*) model->getLLVMType(mLLVMContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_TRUE(structType->getNumElements() == 2);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(model->getName().c_str(), "mymodel");
}

TEST_F(ModelDefinitionTest, TestInterfaceImplmenetationDefinition) {
  StructType *structType = StructType::create(mLLVMContext, "interface.myinterface");
  vector<Type*> types;
  Type* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  types.push_back(vtableType);
  structType->setBody(types);
  map<string, Method*>* interfaceMethods = new map<string, Method*>();
  (*interfaceMethods)["foo"] = mMethodDeclaration->getMethod(mContext);
  Interface *interface = new Interface("myinterface", structType, interfaceMethods);
  mContext.addInterface(interface);
  vector<string> interfaces;
  interfaces.push_back("myinterface");
  
  ModelDefinition modelDefinition("mymodel", mFields, mMethodDeclarations, interfaces);
  modelDefinition.generateIR(mContext);
  
  GlobalVariable* vTablePointer = mContext.getModule()->getGlobalVariable("model.mymodel.vtable");
  
  EXPECT_NE(vTablePointer, nullptr);
  ASSERT_TRUE(vTablePointer->getType()->getPointerElementType()->isArrayTy());
  EXPECT_EQ(vTablePointer->getType()->getPointerElementType()->getArrayNumElements(), 1u);
  Constant* vTableInitializer = vTablePointer->getInitializer();
  ASSERT_TRUE(vTableInitializer->getType()->isArrayTy());
  EXPECT_EQ(vTableInitializer->getType()->getArrayNumElements(), 1u);
  ArrayRef<Constant*> arrayRef = (ArrayRef<Constant*>) vTablePointer;
  ASSERT_EQ(arrayRef.size(), 1u);
}

TEST_F(ModelDefinitionTest, InterfaceNotDefinedDeathTest) {
  vector<string> interfaces;
  interfaces.push_back("myinterface");
  
  ModelDefinition modelDefinition("mymodel", mFields, mMethodDeclarations, interfaces);
  
  EXPECT_EXIT(modelDefinition.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface myinterface is not defined");
}

TEST_F(TestFileSampleRunner, ModelDefinitionTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, ModelDefinitionWithMethodTest) {
  runFile("tests/samples/test_model_method.yz", "0");
}

TEST_F(TestFileSampleRunner, ModelImplmenetingInterfaceDefinitionTest) {
  runFile("tests/samples/test_interface_implementation.yz", "25");
}
