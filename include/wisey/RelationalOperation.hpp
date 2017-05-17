//
//  RelationalOperation.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef RelationalOperation_h
#define RelationalOperation_h

namespace wisey {

/**
 * Types of relational operations
 */
typedef enum RelationalOperationEnum {
  RELATIONAL_OPERATION_LT,
  RELATIONAL_OPERATION_GT,
  RELATIONAL_OPERATION_LE,
  RELATIONAL_OPERATION_GE,
  RELATIONAL_OPERATION_EQ,
  RELATIONAL_OPERATION_NE,
} RelationalOperation;
  
} /* namespace wisey */

#endif /* RelationalOperation_h */
