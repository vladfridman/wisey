//
//  IObjectWithVTable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectWithVTable_h
#define IObjectWithVTable_h

#include "IObjectType.hpp"

namespace wisey {
  
/**
 * Interface representing a object that has a vTable: controller or a model
 */
class IObjectWithVTable : public virtual IObjectType {
    
public:
  
  virtual ~IObjectWithVTable() { }

  /**
   * Returns the name of the vTable global varaible
   */
  virtual std::string getVTableName() const = 0;
  
  /**
   * Returns the vTabla size
   */
  virtual unsigned long getVTableSize() const = 0;
  
};
  
} /* namespace wisey */

#endif /* IObjectWithVTable_h */
