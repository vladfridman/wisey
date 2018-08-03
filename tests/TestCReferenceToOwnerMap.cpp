//
//  TestCReferenceToOwnerMap.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.CReferenceToOwnerMap}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cReferenceToOwnerMapRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_creference_to_owner_map.yz",
                                        "Hashmap size: 3\n"
                                        "bmw is null\n"
                                        "Lookup Toyota: Toyota\n"
                                        "Lookup BMW: BMW\n"
                                        "Lookup Mercedes: Mercedes\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "BMW was erased\n"
                                        "Take result: Toyota\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "Toyota was erased\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "Mercedes was erased\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mMap\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
