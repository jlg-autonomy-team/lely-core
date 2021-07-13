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

#include <memory>

#include <CppUTest/TestHarness.h>

#include <lely/co/csdo.h>
#include <lely/co/type.h>
#include <lely/can/net.h>

#include <libtest/allocators/default.hpp>
#include <libtest/tools/lely-unit-test.hpp>

#include "holder/dev.hpp"
#include "obj-init/sdo-client-par.hpp"

TEST_GROUP(CO_CsdoDnInd) {
  using Sub00HighestSubidxSupported =
      Obj1280SdoClientPar::Sub00HighestSubidxSupported;
  using Sub01CobIdReq = Obj1280SdoClientPar::Sub01CobIdReq;
  using Sub02CobIdRes = Obj1280SdoClientPar::Sub02CobIdRes;

  static const co_unsigned8_t DEV_ID = 0x01u;
  static const co_unsigned8_t CSDO_NUM = 0x01u;
  const co_unsigned16_t IDX = 0x1280u;
  const co_unsigned32_t DEFAULT_COBID_REQ = 0x600u + DEV_ID;
  const co_unsigned32_t DEFAULT_COBID_RES = 0x580u + DEV_ID;

  co_csdo_t* csdo = nullptr;
  co_dev_t* dev = nullptr;
  can_net_t* net = nullptr;
  Allocators::Default defaultAllocator;
  std::unique_ptr<CoDevTHolder> dev_holder;
  std::unique_ptr<CoObjTHolder> obj1280;

  void StartCSDO() { CHECK_EQUAL(0, co_csdo_start(csdo)); }

  void CreateObj1280Defaults() const {
    obj1280->EmplaceSub<Sub00HighestSubidxSupported>(0x02u);
    obj1280->EmplaceSub<Sub01CobIdReq>(DEFAULT_COBID_REQ);
    obj1280->EmplaceSub<Sub02CobIdRes>(DEFAULT_COBID_RES);
  }

  TEST_SETUP() {
    LelyUnitTest::DisableDiagnosticMessages();
    net = can_net_create(defaultAllocator.ToAllocT(), 0);
    CHECK(net != nullptr);

    can_net_set_send_func(net, CanSend::Func, nullptr);

    dev_holder.reset(new CoDevTHolder(DEV_ID));
    dev = dev_holder->Get();
    CHECK(dev != nullptr);

    dev_holder->CreateObj<Obj1280SdoClientPar>(obj1280);
    CreateObj1280Defaults();
    csdo = co_csdo_create(net, dev, CSDO_NUM);
    CHECK(csdo != nullptr);
  }

  TEST_TEARDOWN() {
    co_csdo_destroy(csdo);

    dev_holder.reset();
    can_net_destroy(net);

    CoCsdoDnCon::Clear();
  }
};

/// @name CSDO service: object 0x1280 modification using SDO
///@{

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "highest sub-index supported"
///       sub-object, a correct type of the entry, a pointer to a value,
///       a null memory buffer pointer, a pointer to download confirmation
///       function and a null user-specified data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       CO_SDO_AC_NO_WRITE is set as the abort code, the requested entry
///       is not changed
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
TEST(CO_CsdoDnInd, Co1280DnInd_NoWriteToSub00) {
  StartCSDO();

  const co_unsigned8_t val = 0x04u;
  const auto ret = co_dev_dn_val_req(dev, IDX, 0x00u, CO_DEFTYPE_UNSIGNED8,
                                     &val, nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x00u, CO_SDO_AC_NO_WRITE, nullptr);

  CHECK_EQUAL(0x02u, obj1280->GetSub<Sub00HighestSubidxSupported>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index, a type of the entry (shorter than
///       the type of the requested entry), a pointer to a value, a null memory
///       buffer pointer, a pointer to download confirmation function and
///       a null user-specified data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       CO_SDO_AC_TYPE_LEN_LO is set as the abort code, the requested entry
///       is not changed
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
TEST(CO_CsdoDnInd, Co1280DnInd_Sub_DnTooShort) {
  StartCSDO();

  const co_unsigned8_t val = 0u;
  const auto ret = co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED8,
                                     &val, nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, CO_SDO_AC_TYPE_LEN_LO, nullptr);

  CHECK_EQUAL(DEFAULT_COBID_REQ, obj1280->GetSub<Sub01CobIdReq>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object with
///        a client parameter "COB-ID client -> server (tx)" entry
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "COB-ID client -> server (tx)"
///       sub-object, a correct type of the entry, a pointer to a COB-ID value
///       equal to the old COB-ID, a null memory buffer pointer, a pointer to
///       download confirmation function and a null user-specified data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       0 is set as the abort code, the requested entry is not changed
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
///       \Calls co_sub_get_val_u32()
TEST(CO_CsdoDnInd, Co1280DnInd_CobidReq_SameAsOld) {
  StartCSDO();

  const co_unsigned32_t new_cobid_req = DEFAULT_COBID_REQ;
  const auto ret =
      co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED32, &new_cobid_req,
                        nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, 0u, nullptr);

  CHECK_EQUAL(DEFAULT_COBID_REQ, obj1280->GetSub<Sub01CobIdReq>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object with
///        a valid client parameter "COB-ID client -> server (tx)" entry
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "COB-ID client -> server (tx)"
///       sub-object, a correct type of the entry, a pointer to a new, valid
///       COB-ID value with a new CAN ID, a null memory buffer pointer,
///       a pointer to download confirmation function and a null
///       user-specified data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       CO_SDO_AC_PARAM_VAL is set as the abort code, the requested entry
///       is not changed
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
///       \Calls co_sub_get_val_u32()
TEST(CO_CsdoDnInd, Co1280DnInd_CobidReq_OldValid_NewValid_NewCanId) {
  StartCSDO();

  const co_unsigned32_t new_cobid_req = DEFAULT_COBID_REQ + 1u;
  const auto ret =
      co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED32, &new_cobid_req,
                        nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, CO_SDO_AC_PARAM_VAL, nullptr);

  CHECK_EQUAL(DEFAULT_COBID_REQ, obj1280->GetSub<Sub01CobIdReq>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object with a valid
///        client parameter "COB-ID client -> server (tx)" entry
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "COB-ID client -> server (tx)"
///       sub-object, a correct type of the entry, a pointer to a new, valid
///       COB-ID value with the same CAN ID, a null memory buffer pointer,
///       a pointer to download confirmation function and a null user-specified
///       data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       0 is set as the abort code, the requested entry is changed to
///       the requested value
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
///       \Calls co_sub_get_val_u32()
TEST(CO_CsdoDnInd, Co1280DnInd_CobidReq_OldValid_NewValid_OldId) {
  StartCSDO();

  const co_unsigned32_t new_cobid_req = DEFAULT_COBID_REQ | CO_SDO_COBID_FRAME;
  const auto ret =
      co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED32, &new_cobid_req,
                        nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, 0u, nullptr);

  CHECK_EQUAL(new_cobid_req, obj1280->GetSub<Sub01CobIdReq>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object with
///        a valid client parameter "COB-ID client -> server (tx)" entry
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "COB-ID client -> server (tx)"
///       sub-object, a correct type of the entry, a pointer to a new, invalid
///       COB-ID value with a new extended CAN ID, a null memory buffer pointer,
///       a pointer to download confirmation function and a null user-specified
///       data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       CO_SDO_AC_PARAM_VAL is set as the abort code, the requested entry is
///       not changed
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
///       \Calls co_sub_get_val_u32()
TEST(CO_CsdoDnInd, Co1280DnInd_CobidReq_NewExtId_FrameBitNotSet) {
  const co_unsigned32_t DEFAULT_COBID_REQ_EXT = DEFAULT_COBID_REQ | (1 << 28u);
  StartCSDO();

  const co_unsigned32_t new_cobid_req =
      DEFAULT_COBID_REQ_EXT | CO_SDO_COBID_VALID;
  const auto ret =
      co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED32, &new_cobid_req,
                        nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, CO_SDO_AC_PARAM_VAL, nullptr);

  CHECK_EQUAL(DEFAULT_COBID_REQ, obj1280->GetSub<Sub01CobIdReq>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object with
///        an invalid client parameter "COB-ID client -> server (tx)" entry
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "COB-ID client -> server (tx)"
///       sub-object, a correct type of the entry, a pointer to a new, valid
///       COB-ID, a null memory buffer pointer, a pointer to download
///       confirmation function and a null user-specified data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       0 is set as the abort code, the requested entry is changed to
///       the requested value
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
///       \Calls co_sub_get_val_u32()
TEST(CO_CsdoDnInd, Co1280DnInd_CobidReq_OldInvalid_NewValid) {
  obj1280->SetSub<Sub01CobIdReq>(obj1280->GetSub<Sub01CobIdReq>() |
                                 CO_SDO_COBID_VALID);
  StartCSDO();

  const co_unsigned32_t new_cobid_req = DEFAULT_COBID_REQ;
  const auto ret =
      co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED32, &new_cobid_req,
                        nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, 0u, nullptr);

  CHECK_EQUAL(new_cobid_req, obj1280->GetSub<Sub01CobIdReq>());
}

/// \Given a pointer to the device (co_dev_t) with the CSDO service started
///        and the object dictionary containing the 0x1280 object with
///        a valid client parameter "COB-ID client -> server (tx)" entry
///
/// \When co_dev_dn_val_req() is called with an index of the SDO client
///       parameter object, a sub-index of the "COB-ID client -> server (tx)"
///       sub-object, a correct type of the entry, a pointer to a new, invalid
///       COB-ID, a null memory buffer pointer, a pointer to download
///       confirmation function and a null user-specified data pointer
///
/// \Then 0 is returned, download confirmation function is called once,
///       0 is set as the abort code, the requested entry is changed to
///       the requested value
///       \Calls co_sub_get_type()
///       \Calls co_sdo_req_dn_val()
///       \Calls co_sub_get_subidx()
///       \Calls co_sub_get_val_u32()
TEST(CO_CsdoDnInd, Co1280DnInd_CobidReq_OldValid_NewInvalid) {
  StartCSDO();

  const co_unsigned32_t new_cobid_req = DEFAULT_COBID_REQ | CO_SDO_COBID_VALID;
  const auto ret =
      co_dev_dn_val_req(dev, IDX, 0x01u, CO_DEFTYPE_UNSIGNED32, &new_cobid_req,
                        nullptr, CoCsdoDnCon::Func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::GetNumCalled());
  CoCsdoDnCon::Check(nullptr, IDX, 0x01u, 0u, nullptr);

  CHECK_EQUAL(new_cobid_req, obj1280->GetSub<Sub01CobIdReq>());
}

///@}
