/**
 * @file gen-cryptocontext-ckks-internal.h
 *
 * @brief API to generate CKKS crypto context. MUST NOT (!) be used without a wrapper function.
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
#ifndef _GEN_CRYPTOCONTEXT_CKKSRNS_INTERNAL_H_
#define _GEN_CRYPTOCONTEXT_CKKSRNS_INTERNAL_H_

#include "encoding/encodingparams.h"
#include "constants.h"
#include "utils/exception.h"

namespace lbcrypto {

// forward declarations (don't include headers as compilation fails when you do)
template <typename T>
class CCParams;

template<typename ContextGeneratorType, typename Element>
typename ContextGeneratorType::ContextType genCryptoContextCKKSRNSInternal(const CCParams<ContextGeneratorType>& parameters) {
#if NATIVEINT == 128
    if (parameters.GetRescalingTechnique() == FLEXIBLEAUTO) {
        PALISADE_THROW(config_error, "128-bit CKKS is not supported for the FLEXIBLEAUTO method.");
    }
#endif
    using ParmType = typename Element::Params;
    using IntType = typename Element::Integer;

    auto ep = std::make_shared<ParmType>(0, IntType(0), IntType(0));
    // In CKKS, the plaintext modulus is equal to the scaling factor.
    EncodingParams encodingParams(
        std::make_shared<EncodingParamsImpl>(parameters.GetScalingFactorBits(), parameters.GetBatchSize()));

    auto params = std::make_shared<typename ContextGeneratorType::CryptoParams>(
        ep,
        encodingParams,
        parameters.GetStandardDeviation(),
        parameters.GetAssuranceMeasure(),
        parameters.GetRootHermiteFactor(),
        parameters.GetRelinWindow(),
        parameters.GetMode(),
        parameters.GetDepth(),
        parameters.GetMaxDepth());

    params->SetStdLevel(parameters.GetSecurityLevel()); // TODO (dsuponit): do we need this as we already have root hermit factor?
    // for CKKS scheme noise scale is always set to 1
    params->SetNoiseScale(1);

    uint32_t numLargeDigits = parameters.GetNumLargeDigits();
    if (!numLargeDigits) {  // Choose one of the default values
        if (parameters.GetMultiplicativeDepth() > 3)        // If more than 4 towers, use 3 digits
            numLargeDigits = 3;
        else if (parameters.GetMultiplicativeDepth() == 0)  // if there is only 1 tower, use one digit
            numLargeDigits = 1;
        else                                // If 2, 3 or 4 towers, use 2 digits (1 <= multiplicativeDepth <=3 )
            numLargeDigits = 2;
    }

    auto scheme = std::make_shared<typename ContextGeneratorType::PublicKeyEncryptionScheme>();
    scheme->SetKeySwitchingTechnique(parameters.GetKeySwitchTechnique());
    scheme->ParamsGenCKKSRNS(
        params,
        2 * parameters.GetRingDim(),
        parameters.GetMultiplicativeDepth() + 1,
        parameters.GetScalingFactorBits(),
        parameters.GetRelinWindow(),
        parameters.GetMode(),
        parameters.GetFirstModSize(),
        numLargeDigits,
        parameters.GetKeySwitchTechnique(),
        parameters.GetRescalingTechnique());

    auto cc = ContextGeneratorType::Factory::GetContext(params, scheme);
    cc->setSchemeId("CKKSRNS"); // TODO (dsuponit): do we need this? if we do then it should SCHEME::CKKSRNS_SCHEME from pke/include/scheme/scheme-id.h, not a string
    return cc;
}
}  // namespace lbcrypto

#endif // _GEN_CRYPTOCONTEXT_CKKSRNS_INTERNAL_H_

