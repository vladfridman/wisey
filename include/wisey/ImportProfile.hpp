//
//  ImportProfile.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ImportProfile_h
#define ImportProfile_h

#include <map>

namespace wisey {

class IObjectType;
  
/**
 * Contains information about file imports that is stored together with every object defined in file
 */
class ImportProfile {
  std::string mPackage;
  std::map<std::string, std::string> mImports;

public:
  
  ImportProfile(std::string package);
  
  ~ImportProfile();
  
  /**
   * Add an object to the list of imports
   */
  void addImport(std::string shortName, std::string longName);
  
  /**
   * Search list of imports and get the full name of a given object
   */
  std::string getFullName(std::string shortName) const;
  
};
  
} /* namespace wisey */

#endif /* ImportProfile_h */
