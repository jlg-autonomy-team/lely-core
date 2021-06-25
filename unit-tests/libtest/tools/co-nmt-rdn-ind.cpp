/**@file
 * This file is part of the CANopen Library Unit Test Suite.
 *
 * @copyright 2021 N7 Space Sp. z o.o.
 *
 * Unit Test Suite was developed under a programme of,
 * and funded by, the European Space Agency.
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <CppUTest/TestHarness.h>

#include "co-nmt-rdn-ind.hpp"

unsigned CoNmtRdnInd::num_called;
co_nmt_t* CoNmtRdnInd::nmt_;
co_unsigned8_t CoNmtRdnInd::bus_id_;
int CoNmtRdnInd::reason_;
void* CoNmtRdnInd::data_;

void
CoNmtRdnInd::Func(co_nmt_t* const nmt, const co_unsigned8_t bus_id,
                  const int reason, void* const data) {
  num_called++;

  nmt_ = nmt;
  bus_id_ = bus_id;
  reason_ = reason;
  data_ = data;
}

void
CoNmtRdnInd::Clear() {
  num_called = 0;

  nmt_ = nullptr;
  bus_id_ = 0;
  reason_ = 0;
  data_ = nullptr;
}

void
CoNmtRdnInd::Check(const co_nmt_t* const nmt, const co_unsigned8_t bus_id,
                   const int reason, const void* const data) {
  POINTERS_EQUAL(nmt, nmt_);
  CHECK_EQUAL(bus_id, bus_id_);
  CHECK_EQUAL(reason, reason_);
  POINTERS_EQUAL(data, data_);
}