/**
 * @file cryptocontextparams-bgvrns.h
 *
 * @brief Parameter class to generate BGVRNS crypto context
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
#ifndef _CRYPTOCONTEXTPARAMS_BGVRNS_H_
#define _CRYPTOCONTEXTPARAMS_BGVRNS_H_

#include "scheme/cryptocontextparams-base.h"

namespace lbcrypto {

class CryptoContextBGVRNS;

// every CCParams class should include the following forward declaration as there is 
// no general CCParams class template. This way we may create scheme specific classes
// derived from Params or have them completely independent.
template <typename T>
class CCParams;
//====================================================================================================================
template <>
class CCParams<CryptoContextBGVRNS> : public Params {
public:
    CCParams() : Params(BGVRNS_SCHEME) {}
    CCParams(const CCParams& obj) = default;
    CCParams(CCParams&& obj) = default;
};
//====================================================================================================================

}  // namespace lbcrypto


#endif // _CRYPTOCONTEXTPARAMS_BGVRNS_H_

