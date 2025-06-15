/*
 * Copyright (c) 2020-2025
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#ifndef MDICT_XMLUTILS_H_
#define MDICT_XMLUTILS_H_

#include <cassert>
#include <map>
#include <string>

// parse xml header info
std::map<std::string, std::string> parseXMLHeader(std::string dicxml);

#endif
