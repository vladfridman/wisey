//
//  TestInterfaceDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceDefinition}
//

#include "TestFileSampleRunner.hpp"

TEST_F(TestFileSampleRunner, InterfaceDefinitionTest) {
  runFile("tests/samples/test_interface_definition.yz", "0");
}
