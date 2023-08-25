/**
   dtl -- Diff Template Library

   In short, Diff Template Library is distributed under so called "BSD license",

   Copyright (c) 2015 Tatsuhiko Kubo <cubicdaiya@gmail.com>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   * Neither the name of the authors nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* If you use this library, you must include dtl.hpp only. */

#ifndef DTL_VARIABLES_H
#define DTL_VARIABLES_H

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace dtl {

using edit_t = int;

constexpr edit_t SES_DELETE = -1;
constexpr edit_t SES_COMMON = 0;
constexpr edit_t SES_ADD = 1;

/**
 * mark of SES
 */
constexpr char SES_MARK_DELETE = '-';
constexpr char SES_MARK_COMMON = ' ';
constexpr char SES_MARK_ADD    = '+';

/**
 * info for Unified Format
 */
typedef struct elem_info {
  long long beforeIdx;  // index of prev sequence
  long long afterIdx;   // index of after sequence
  edit_t type;          // type of edit(Add, Delete, Common)
  bool operator==(const elem_info& other) const {
    return (this->beforeIdx == other.beforeIdx && this->afterIdx == other.afterIdx &&
            this->type == other.type);
  }
} elemInfo;

const long long DTL_SEPARATE_SIZE = 3;
const long long DTL_CONTEXT_SIZE = 3;

/**
 * coordinate for registering route
 */
typedef struct Point {
  long long x;  // x cordinate
  long long y;  // y cordinate
  long long k;  // vertex
} P;

/**
 * limit of coordinate size
 */
const unsigned long long MAX_COORDINATES_SIZE = 2000000;

typedef std::vector<long long> editPath;
typedef std::vector<P> editPathCoordinates;

/**
 * Structure of Unified Format Hunk
 */
template <typename sesElem>
struct uniHunk {
  long long a, b, c, d;            // @@ -a,b +c,d @@
  std::vector<sesElem> common[2];  // anteroposterior commons on changes
  std::vector<sesElem> change;     // changes
  long long inc_dec_count;         // count of increase and decrease
};

#define dtl_typedefs(elem, sequence)                             \
  typedef std::pair<elem, elemInfo> sesElem;                          \
  typedef std::vector<sesElem> sesElemVec;                            \
  typedef std::vector<uniHunk<sesElem> > uniHunkVec;                  \
  typedef std::list<elem> elemList;                                   \
  typedef std::vector<elem> elemVec;                                  \
  typedef typename uniHunkVec::iterator uniHunkVec_iter;         \
  typedef typename sesElemVec::iterator sesElemVec_iter;         \
  typedef typename elemList::iterator elemList_iter;             \
  typedef typename sequence::iterator sequence_iter;             \
  typedef typename sequence::const_iterator sequence_const_iter; \
  typedef typename elemVec::iterator elemVec_iter;
}  // namespace dtl

#endif  // DTL_VARIABLES_H
