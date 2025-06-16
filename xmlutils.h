/*
 * Copyright (c) 2020-2025
 * All rights reserved.
 *
 * This code is licensed under the BSD 3-Clause License.
 * See the LICENSE file for details.
 */

#pragma once

#include <cassert>
#include <map>
#include <string>

// parse xml header info
void parse_xml_header(const std::string& dicxml, std::map<std::string, std::string>& tagm);
