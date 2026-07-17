/// @file test.h
/// @brief The single "include all" header for testing units.
///
/// @author Luke Ian Turner
/// @date 2026-06-26
/// @copyright Copyright (c) 2026 Luke Ian Turner
/// @copyright
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// @copyright
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// @copyright
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#pragma once

#include <gtest/gtest.h>

#include "nmbs/nmbs.h"
#include "nmbs/nmbs_private.h"

#include "nmbs/nmbs_c.h"

#define NMBS_REQUIREMENT_SOURCE(SOURCE) RecordProperty("requirement_source", #SOURCE);
#define NMBS_REQUIREMENT_TEXT(REQUIREMENT) RecordProperty("requirement_text", #REQUIREMENT);

#define NMBS_REQUIREMENT_STANDARD_4778_2_A1 NMBS_REQUIREMENT_SOURCE(ADatP-4778.2 Ed.A V.1)
