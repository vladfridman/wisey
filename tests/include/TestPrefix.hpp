//
//  TestPrefix.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TestPrefix_h
#define TestPrefix_h

#include "wisey/IRGenerationContext.hpp"

/**
 * Includes some object definitions that are neccessary for wisey code to run
 */
class TestPrefix {
  
public:
  
  static void run(wisey::IRGenerationContext& context);
  
private:
  
  static void defineNPEModel(wisey::IRGenerationContext& context);

  static void defineThreadController(wisey::IRGenerationContext& context);

};

#endif /* TestPrefix_h */
