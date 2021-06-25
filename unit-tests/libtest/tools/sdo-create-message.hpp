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

#ifndef LELY_UNIT_TEST_SDO_CREATE_MESSAGE_HPP_
#define LELY_UNIT_TEST_SDO_CREATE_MESSAGE_HPP_

#include <lely/can/msg.h>
#include <lely/co/type.h>

#include "sdo-defines.hpp"

namespace SdoCreateMsg {
can_msg Abort(const co_unsigned16_t idx, const co_unsigned8_t subidx,
              const uint_least32_t recipient_id, const co_unsigned32_t ac);
can_msg Default(const co_unsigned16_t idx, const co_unsigned8_t subidx,
                const uint_least32_t recipient_id);
// block download initiate request
can_msg BlkDnIniReq(const co_unsigned16_t idx, const co_unsigned8_t subidx,
                    const uint_least32_t recipient_id,
                    const co_unsigned8_t cs_flags = 0, const size_t size = 0);
// block download sub-object request
can_msg BlkDnSubReq(const co_unsigned16_t idx, const co_unsigned8_t subidx,
                    const uint_least32_t recipient_id,
                    const co_unsigned8_t seqno, const co_unsigned8_t last = 0);
// block download end
can_msg BlkDnEnd(const co_unsigned16_t idx, const co_unsigned8_t subidx,
                 const uint_least32_t recipient_id, const co_unsigned16_t crc,
                 const co_unsigned8_t cs_flags = 0);
// download initiate request
can_msg DnIniReq(co_unsigned16_t idx, co_unsigned8_t subidx,
                 uint_least32_t recipient_id, uint_least8_t buffer[],
                 uint_least8_t cs_flags = 0);
// download segment request
can_msg DnSeg(const co_unsigned16_t idx, const co_unsigned8_t subidx,
              const uint_least32_t recipient_id, const uint_least8_t buf[],
              const size_t size, const uint_least8_t cs_flags = 0);
// upload initiate request
can_msg UpIniReq(const co_unsigned16_t idx, const co_unsigned8_t subidx,
                 const uint_least32_t recipient_id);
// block upload initiate request
can_msg BlkUpIniReq(const co_unsigned16_t idx, const co_unsigned8_t subidx,
                    const uint_least32_t recipient_id,
                    const co_unsigned8_t blksize = CO_SDO_MAX_SEQNO);
}  // namespace SdoCreateMsg

#endif  // LELY_UNIT_TEST_SDO_CREATE_MESSAGE_HPP_