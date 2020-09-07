/**@file
 * This is the public header file of the CANopen library.
 *
 * @copyright 2016-2020 Lely Industries N.V.
 *
 * @author J. S. Seldenthuis <jseldenthuis@lely.com>
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

#ifndef LELY_CO_CO_H_
#define LELY_CO_CO_H_

#include <lely/util/util.h>

struct __co_dev;
/// An opaque CANopen device type.
typedef struct __co_dev co_dev_t;

struct __co_obj;
/// An opaque CANopen object type.
typedef struct __co_obj co_obj_t;

struct __co_sub;
/// An opaque CANopen sub-object type.
typedef struct __co_sub co_sub_t;

struct __co_ssdo;
/// An opaque CANopen Server-SDO service type.
typedef struct __co_ssdo co_ssdo_t;

struct __co_csdo;
/// An opaque CANopen Client-SDO service type.
typedef struct __co_csdo co_csdo_t;

struct __co_rpdo;
/// An opaque CANopen Receive-PDO service type.
typedef struct __co_rpdo co_rpdo_t;

struct __co_tpdo;
/// An opaque CANopen Transmit-PDO service type.
typedef struct __co_tpdo co_tpdo_t;

struct __co_sync;
/// An opaque CANopen SYNC producer/consumer service type.
typedef struct __co_sync co_sync_t;

struct __co_time;
/// An opaque CANopen TIME producer/consumer service type.
typedef struct __co_time co_time_t;

struct __co_emcy;
/// An opaque CANopen EMCY producer/consumer service type.
typedef struct __co_emcy co_emcy_t;

struct __co_nmt;
/// An opaque CANopen NMT master/slave service type.
typedef struct __co_nmt co_nmt_t;

struct __co_lss;
/// An opaque CANopen LSS master/slave service type.
typedef struct __co_lss co_lss_t;

#endif // !LELY_CO_CO_H_
