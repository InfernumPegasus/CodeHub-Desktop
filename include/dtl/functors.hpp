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

#ifndef DTL_FUNCTORS_H
#define DTL_FUNCTORS_H

#include <fmt/color.h>
#include <fmt/ostream.h>

#include <fstream>
#include <iostream>

#include "variables.hpp"

namespace dtl {

template <typename OStream>
void PrintOstreamFormatted(const std::string& formatted, OStream& oStream,
                           fmt::text_style style = fmt::emphasis()) {
  if (typeid(oStream) == typeid(std::cout)) {
    fmt::print(style, formatted);
  } else {
    oStream << formatted;
  }
}

template <typename SesElemType>
void PrintElemWithStatus(const SesElemType& se, const char status, std::ostream& ostream,
                         fmt::color color) {
  const std::string str = fmt::format("{}{}\n", status, se.first);
  PrintOstreamFormatted(str, ostream, fg(color));
}

template <typename SesElemType, typename OStream = std::ostream>
class Printer {
 public:
  Printer() : out_(std::cout) {}

  explicit Printer(OStream& out) : out_(out) {}

  virtual ~Printer() = default;

  virtual void operator()(const SesElemType& se) const = 0;

 protected:
  OStream& out_;
};

template <typename SesElemType, typename OStream = std::ostream>
class CommonPrinter : public Printer<SesElemType, OStream> {
 public:
  CommonPrinter() : Printer<SesElemType, OStream>() {}

  explicit CommonPrinter(OStream& out) : Printer<SesElemType, OStream>(out) {}

  ~CommonPrinter() = default;

  void operator()(const SesElemType& se) const {
    fmt::print("{}{}\n", SES_MARK_COMMON, se.first);
  }
};

template <typename SesElemType, typename OStream = std::ostream>
class ChangePrinter : public Printer<SesElemType, OStream> {
 public:
  ChangePrinter() : Printer<SesElemType, OStream>() {}

  explicit ChangePrinter(OStream& out) : Printer<SesElemType, OStream>(out) {}

  ~ChangePrinter() = default;

  void operator()(const SesElemType& se) const {
    switch (se.second.type) {
      case SES_ADD:
        PrintElemWithStatus(se, SES_MARK_ADD, this->out_, fmt::color::green);
        break;
      case SES_DELETE:
        PrintElemWithStatus(se, SES_MARK_DELETE, this->out_, fmt::color::red);
        break;
      case SES_COMMON:
        this->out_ << SES_MARK_COMMON << se.first << std::endl;
        break;
    }
  }
};

template <typename sesElem, typename stream = std::ostream>
class UniHunkPrinter {
 public:
  UniHunkPrinter() : out_(std::cout) {}

  explicit UniHunkPrinter(stream& out) : out_(out) {}

  ~UniHunkPrinter() = default;

  void operator()(const uniHunk<sesElem>& hunk) const {
    const auto str =
        fmt::format("@@ -{},{}, +{},{} @@\n", hunk.a, hunk.b, hunk.c, hunk.d);
    constexpr fmt::text_style style = fg(fmt::color::violet) /*| fmt::emphasis::bold*/;
    PrintOstreamFormatted(str, this->out_, style);

    for_each(hunk.common[0].begin(), hunk.common[0].end(),
             CommonPrinter<sesElem, stream>(out_));
    for_each(hunk.change.begin(), hunk.change.end(),
             ChangePrinter<sesElem, stream>(out_));
    for_each(hunk.common[1].begin(), hunk.common[1].end(),
             CommonPrinter<sesElem, stream>(out_));
  }

 private:
  stream& out_;
};

template <typename sesElem, typename storedData>
class Storage {
 public:
  explicit Storage(storedData& sd) : storedData_(sd) {}

  virtual ~Storage() = default;

  virtual void operator()(const sesElem& se) const = 0;

 protected:
  storedData& storedData_;
};

template <typename elem>
class Compare {
 public:
  Compare() = default;

  virtual ~Compare() = default;

  virtual inline bool impl(const elem& e1, const elem& e2) const { return e1 == e2; }
};
}  // namespace dtl

#endif  // DTL_FUNCTORS_H
