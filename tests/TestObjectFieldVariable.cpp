//
//  TestObjectFieldVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectFieldVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/ObjectFieldVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct ObjectFieldVariableTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  BasicBlock* mBasicBlock;
  Value* mObjectFieldValue;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ObjectFieldVariable* mObjectFieldVariable;
  
  ObjectFieldVariableTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType* structType = StructType::create(llvmContext, "Object");
    structType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    ModelField* widthField = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    ModelField* heightField = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    (*fields)["foo"] = widthField;
    (*fields)["bar"] = heightField;
    map<string, Method*>* methods = new map<string, Method*>();
    mModel = new Model("Object", structType, fields, methods);
 
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    AllocaInst *alloc = new AllocaInst(mModel->getLLVMType(llvmContext),
                                       "this.param",
                                       mContext.getBasicBlock());
    IVariable* variable = new LocalStackVariable("this", mModel, alloc);
    mContext.getScopes().setVariable(variable);
   
    mObjectFieldValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    mObjectFieldVariable = new ObjectFieldVariable("foo", mObjectFieldValue, mModel);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ObjectFieldVariableTest() {
    delete mStringStream;
    delete mModel;
  }
};

TEST_F(ObjectFieldVariableTest, BasicFieldsTest) {
  EXPECT_STREQ(mObjectFieldVariable->getName().c_str(), "foo");
  EXPECT_EQ(mObjectFieldVariable->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(mObjectFieldVariable->getValue(), mObjectFieldValue);
}

TEST_F(ObjectFieldVariableTest, ObjectFieldVariableGenerateIdentifierIRTest) {
  mObjectFieldVariable->generateIdentifierIR(mContext, "test");

  *mStringStream << *mBasicBlock;
  string expected = string() +
    "\nentry:" +
    "\n  %this.param = alloca %Object*"
    "\n  %this = load %Object*, %Object** %this.param"
    "\n  %0 = getelementptr %Object, %Object* %this, i32 0, i32 0"
    "\n  %1 = load i32, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
