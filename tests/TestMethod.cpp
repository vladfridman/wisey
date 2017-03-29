//
//  TestMethod.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Method}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct MethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Method* mMethod;
  Model* mModel;
  Block mBlock;
  CompoundStatement mCompoundStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  MethodTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mCompoundStatement(CompoundStatement(mBlock)) {

    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    mMethod = new Method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    StructType* structType = StructType::create(mLLVMContext, "Object");
    structType->setBody(types);
    map<string, Field*> fields;
    fields["foo"] = new Field(PrimitiveTypes::INT_TYPE, "foo", 0);
    fields["bar"] = new Field(PrimitiveTypes::INT_TYPE, "bar", 1);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("Object", structType, fields, methods, interfaces);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~MethodTest() {
    delete mStringStream;
  }
};

TEST_F(MethodTest, basicMethodsTest) {
  ASSERT_STREQ(mMethod->getName().c_str(), "mymethod");
  ASSERT_EQ(mMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  ASSERT_EQ(mMethod->getArguments().size(), 2u);
}

TEST_F(MethodTest, methodFooIRTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  Method method("foo", PrimitiveTypes::FLOAT_TYPE, arguments, 0, &mCompoundStatement);
  Value* function = method.generateIR(mContext, mModel);
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal float @object.Object.foo(%Object* %this, i32 %intargument) {"
  "\nentry:"
  "\n  %this.param = alloca %Object*"
  "\n  store %Object* %this, %Object** %this.param"
  "\n  %intargument.param = alloca i32"
  "\n  store i32 %intargument, i32* %intargument.param"
  "\n  ret void"
  "\n}"
  "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}
