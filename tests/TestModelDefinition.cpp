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

#include "MockStatement.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

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
    vector<ITypeSpecifier*> thrownExceptions;
    mMethodDeclaration = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                               *floatTypeSpecifier,
                                               "foo",
                                               methodArguments,
                                               thrownExceptions,
                                               *compoundStatement);
    mMethodDeclarations.push_back(mMethodDeclaration);
  }
};

TEST_F(ModelDefinitionTest, simpleDefinitionTest) {
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

TEST_F(ModelDefinitionTest, interfaceImplmenetationDefinitionTest) {
  StructType *structType = StructType::create(mLLVMContext, "interface.myinterface");
  vector<Type*> types;
  Type* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  types.push_back(vtableType);
  structType->setBody(types);
  vector<MethodSignature*> interfaceMethodSignatures;
  vector<MethodArgument*> methodArguments;
  vector<IType*> methodThrownExceptions;
  methodArguments.push_back(new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument"));
  MethodSignature* methodSignature = new MethodSignature("foo",
                                                         AccessLevel::PUBLIC_ACCESS,
                                                         PrimitiveTypes::FLOAT_TYPE,
                                                         methodArguments,
                                                         methodThrownExceptions,
                                                         0);
  interfaceMethodSignatures.push_back(methodSignature);
  vector<Interface*> parentInterfaces;
  Interface *interface = new Interface("myinterface",
                                       structType,
                                       parentInterfaces,
                                       interfaceMethodSignatures);
  Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, interface->getName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());

  mContext.addInterface(interface);
  vector<string> interfaces;
  interfaces.push_back("myinterface");
  
  ModelDefinition modelDefinition("mymodel", mFields, mMethodDeclarations, interfaces);
  modelDefinition.generateIR(mContext);
  
  GlobalVariable* vTablePointer = mContext.getModule()->getGlobalVariable("model.mymodel.vtable");
  
  EXPECT_NE(vTablePointer, nullptr);
  ASSERT_TRUE(vTablePointer->getType()->getPointerElementType()->isStructTy());
  EXPECT_EQ(vTablePointer->getType()->getPointerElementType()->getStructNumElements(), 1u);
  Constant* vTableInitializer = vTablePointer->getInitializer();
  ASSERT_TRUE(vTableInitializer->getType()->isStructTy());
  EXPECT_EQ(vTableInitializer->getType()->getStructNumElements(), 1u);

  GlobalVariable* vModelTypesPointer =
    mContext.getModule()->getGlobalVariable("model.mymodel.typetable");
  EXPECT_NE(vModelTypesPointer, nullptr);
  ASSERT_TRUE(vModelTypesPointer->getType()->getPointerElementType()->isArrayTy());
  EXPECT_EQ(vModelTypesPointer->getType()->getPointerElementType()->getArrayNumElements(), 3u);
}

TEST_F(ModelDefinitionTest, interfaceNotDefinedDeathTest) {
  vector<string> interfaces;
  interfaces.push_back("myinterface");
  
  ModelDefinition modelDefinition("mymodel", mFields, mMethodDeclarations, interfaces);
  
  EXPECT_EXIT(modelDefinition.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface myinterface is not defined");
}

TEST_F(TestFileSampleRunner, modelDefinitionRunTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, modelDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_model_method.yz", "0");
}
