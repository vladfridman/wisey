//
//  TestPrefix.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef TestPrefix_h
#define TestPrefix_h

#include "wisey/ControllerDefinition.hpp"
#include "wisey/InterfaceDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ThreadDefinition.hpp"

namespace wisey {
  class IObjectElementDefinition;
}

/**
 * Includes some object definitions that are neccessary for wisey code to run
 */
class TestPrefix {
  
public:
  
  /**
   * Generate IR that defines system exceptions and thread controller.
   *
   * This is needed for running some tests
   */
  static void generateIR(wisey::IRGenerationContext& context);
  
private:
  
  static void defineFileStruct(wisey::IRGenerationContext& context);
  
  static void defineModel(wisey::IRGenerationContext& context,
                          std::string modelName,
                          std::vector<wisey::IObjectElementDefinition*> modelElements);
  
  static wisey::InterfaceDefinition* defineThreadInterface(wisey::IRGenerationContext& context);
  
  static wisey::ControllerDefinition* defineCallStackController(wisey::IRGenerationContext&
                                                                context);
  
  static wisey::ThreadDefinition* defineMainThread(wisey::IRGenerationContext& context);

};

#endif /* TestPrefix_h */
