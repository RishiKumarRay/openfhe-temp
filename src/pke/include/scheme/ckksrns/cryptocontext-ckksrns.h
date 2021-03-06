/**
 * @file cryptocontext-ckksrns.h
 *
 * @brief API to generate CKKSRNS crypto context
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
#ifndef _CRYPTOCONTEXT_CKKSRNS_H_
#define _CRYPTOCONTEXT_CKKSRNS_H_

#include "lattice/lat-hal.h"
#include "scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h"
#include "scheme/ckksrns/cryptocontextparams-ckksrns.h"
#include "scheme/ckksrns/ckksrns-cryptoparameters.h"
#include "scheme/ckksrns/ckksrns-scheme.h"
#include "cryptocontextfactory.h"

namespace lbcrypto {

class CryptoContextCKKSRNS {
    using Element = DCRTPoly;

public:
    using ContextType               = CryptoContext<Element>; // required by GenCryptoContext() in gen-cryptocontext.h
    using Factory                   = CryptoContextFactory<Element>;
    using PublicKeyEncryptionScheme = SchemeCKKSRNS;
    using CryptoParams              = CryptoParametersCKKSRNS;

    static CryptoContext<Element> genCryptoContext(const CCParams<CryptoContextCKKSRNS>& parameters) {
        return genCryptoContextCKKSRNSInternal<CryptoContextCKKSRNS, Element>(parameters);
    }
};

}  // namespace lbcrypto

#endif // _CRYPTOCONTEXT_CKKSRNS_H_

