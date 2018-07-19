//
//  IBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IBuilder_h
#define IBuilder_h

#include "wisey/IExpression.hpp"
#include "wisey/IBuildableObjectType.hpp"
#include "wisey/BuilderArgument.hpp"

namespace wisey {
  
  /**
   * Represents object builder expression such as heap builder or pool builder
   */
  class IBuilder : public IExpression {
  
  public:

    /**
     * Checks that all creation arguments are there and that they have correct names
     */
    static void checkArguments(IRGenerationContext& context,
                               const IBuildableObjectType* buildable,
                               const BuilderArgumentList& builderArgumentList,
                               int line);


  private:
    
    static void checkArgumentsAreWellFormed(IRGenerationContext& context,
                                            const IBuildableObjectType* buildable,
                                            const BuilderArgumentList& builderArguments,
                                            int line);
    
    static void checkAllFieldsAreSet(IRGenerationContext& context,
                                     const IBuildableObjectType* buildable,
                                     const BuilderArgumentList& builderArguments,
                                     int line);
    
    static std::vector<std::string> getMissingFields(const IBuildableObjectType* buildable,
                                                     std::set<std::string> givenFields);

  };
  
} /* namespace wisey */


#endif /* IBuilder_h */
