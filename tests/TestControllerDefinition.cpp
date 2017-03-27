//
//  TestControllerDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerDefinition}
//

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, controllerDefinitionSyntaxRunTest) {
  runFile("tests/samples/test_controller_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, controllerDefinitionWithModelStateSyntaxRunTest) {
  runFile("tests/samples/test_controller_definition_with_model_state.yz", "0");
}
