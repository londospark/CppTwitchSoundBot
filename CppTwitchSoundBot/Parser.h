#pragma once
#include <string>
#include <map>

using user_attributes = std::map<std::string, std::string>;

user_attributes attributes_for(const std::string& attrList);