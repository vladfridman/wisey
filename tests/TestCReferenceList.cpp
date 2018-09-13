//
//  TestCReferenceList.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link wisey.data.CReferenceList}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"

TEST_F(TestFileRunner, cReferenceListRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_creference_list.yz",
                                        "bmw is not null\n"
                                        "List size: 3\n"
                                        "Lookup last: BMW\n"
                                        "Lookup first: Mercedes\n"
                                        "List size: 1\n"
                                        "Lookup last: Toyota\n"
                                        "Lookup first: Toyota\n"
                                        "List size: 0\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "freeing systems.vos.wisey.compiler.tests.CProgram.mList\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
