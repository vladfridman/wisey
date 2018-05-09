//
//  TestIConcreteObjectDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IConcreteObjectDefinition}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, modelMethodOverloadRunDeathTest) {
  expectFailCompile("tests/samples/test_model_method_overload.yz",
                    1,
                    "tests/samples/test_model_method_overload.yz\\(12\\): Error: "
                    "Method named 'getYear' was already defined on line 8, method overloading is not allowed");
}

TEST_F(TestFileRunner, modelFunctionOverloadRunDeathTest) {
  expectFailCompile("tests/samples/test_model_function_overload.yz",
                    1,
                    "tests/samples/test_model_function_overload.yz\\(12\\): Error: "
                    "Function or method named 'getYear' was already defined on line 8, method overloading is not allowed");
}

TEST_F(TestFileRunner, modelRedefineFieldRunDeathTest) {
  expectFailCompile("tests/samples/test_model_redefine_field.yz",
                    1,
                    "tests/samples/test_model_redefine_field.yz\\(7\\): Error: "
                    "Field named 'mField' was already defined on line 6");
}

TEST_F(TestFileRunner, modelRedefineConstantRunDeathTest) {
  expectFailCompile("tests/samples/test_model_redefine_constant.yz",
                    1,
                    "tests/samples/test_model_redefine_constant.yz\\(7\\): Error: "
                    "Constant named 'CONSTANT' was already defined on line 6");
}
