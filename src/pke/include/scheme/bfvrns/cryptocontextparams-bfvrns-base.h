/**
 * @file cryptocontextparams-bfvrns-base.h
 *
 * @brief Base class fr BFV* scheme parameters
 *
 * @author TPOC: contact@palisade-crypto.org
 *
 * @contributor Dmitriy Suponitskiy
 *
 * @copyright Copyright (c) 2021, Duality Technologies (https://dualitytech.com/)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. THIS SOFTWARE IS
 * PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _CRYPTOCONTEXTPARAMS_BFVRNS_BASE_H_
#define _CRYPTOCONTEXTPARAMS_BFVRNS_BASE_H_

#include "scheme/cryptocontextparams-base.h"

namespace lbcrypto {

//====================================================================================================================
// ATTN: after the bfvrns* schemes are combined, we must remove class BFVRNSParams
class BFVRNSParams : public Params {
  usint evalAddCount;
  usint evalMultCount;
  usint keySwitchCount;

public:
    BFVRNSParams(SCHEME scheme) :
        Params(scheme), evalAddCount(0), evalMultCount(0), keySwitchCount(0) {}
    BFVRNSParams(const BFVRNSParams& obj) = default;
    BFVRNSParams(BFVRNSParams&& obj) = default;

    // getters
    usint GetEvalAddCount() const {
        return evalAddCount;
    }
    usint GetEvalMultCount() const {
        return evalMultCount;
    }
    usint GetKeySwitchCount() const {
        return keySwitchCount;
    }

    // setters
    void SetEvalAddCount(usint evalAddCount0) {
        evalAddCount = evalAddCount0;
    }
    void SetEvalMultCount(usint evalMultCount0) {
        evalMultCount = evalMultCount0;
    }
    void SetKeySwitchCount(usint keySwitchCount0) {
        keySwitchCount = keySwitchCount0;
    }
};
//====================================================================================================================

}  // namespace lbcrypto

#endif  // _CRYPTOCONTEXTPARAMS_BFVRNS_BASE_H_
