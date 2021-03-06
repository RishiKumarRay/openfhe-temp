/**
 * @file cryptocontextparams-defaults.h
 *
 * @brief Collection of parameter default values for different schemes
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
#ifndef _CRYPTOCONTEXTPARAMS_DEFAULTS_H_
#define _CRYPTOCONTEXTPARAMS_DEFAULTS_H_

//TODO: move enums to separate files (dsuponit)
#include "lattice/stdlatticeparms.h" // SecurityLevel
#include "scheme/scheme-id.h" // SCHEME
#include "utils/inttypes.h"
#include "constants.h"

namespace lbcrypto {

// TODO: all default values below should be reviewed for every scheme (dsuponit)
namespace CKKSRNS_SCHEME_DEFAULTS {
    constexpr SCHEME               scheme              = CKKSRNS_SCHEME;
    constexpr PlaintextModulus     ptModulus           = 0;
    constexpr usint                relinWindow         = 0;
    constexpr float                standardDeviation   = 3.19;
    constexpr float                rootHermiteFactor   = 1.006;
    constexpr float                assuranceMeasure    = 9.0;
    constexpr MODE                 mode                = OPTIMIZED;
    constexpr int                  depth               = 1;
    constexpr int                  maxDepth            = 2;
    constexpr KeySwitchTechnique   ksTech              = HYBRID;
// Backend-specific settings for CKKS
#if NATIVEINT == 128
    constexpr RescalingTechnique   rsTech              = FIXEDAUTO;
    constexpr usint                firstModSize        = 105;
#else
    constexpr RescalingTechnique   rsTech              = FLEXIBLEAUTO;
    constexpr usint                firstModSize        = 60;
#endif
    constexpr usint                cyclOrder           = 0;
    constexpr usint                numPrimes           = 0;
    constexpr usint                scaleExp            = 0;
    constexpr usint                batchSize           = 0;
    constexpr uint32_t             numLargeDigits      = 0;
    constexpr usint                multiplicativeDepth = 0;
    constexpr usint                scalingFactorBits   = 0;
    constexpr SecurityLevel        securityLevel       = HEStd_128_classic;
    constexpr usint                ringDim             = 0;
    constexpr EncryptionTechnique  encryptionTechnique = STANDARD;
    constexpr MultiplicationTechnique multiplicationTechnique = HPS;
    constexpr usint                multiHopQModulusLowerBound = 0;
}; // namespace CKKSRNS_SCHEME_DEFAULTS

namespace BFVRNS_SCHEME_DEFAULTS {
    constexpr SCHEME               scheme              = BFVRNS_SCHEME;
    constexpr PlaintextModulus     ptModulus           = 0;
    constexpr usint                relinWindow         = 0;
    constexpr float                standardDeviation   = 0.0;
    constexpr float                rootHermiteFactor   = 0.0;
    constexpr float                assuranceMeasure    = 36.0;
    constexpr MODE                 mode                = OPTIMIZED;
    constexpr int                  depth               = 1;
    constexpr int                  maxDepth            = 2;
    constexpr KeySwitchTechnique   ksTech              = BV;          // set to avoid compiler errors
    constexpr RescalingTechnique   rsTech              = NORESCALE; // set to avoid compiler errors
    constexpr usint                firstModSize        = 60;
    constexpr usint                cyclOrder           = 0;
    constexpr usint                numPrimes           = 0;
    constexpr usint                scaleExp            = 0;
    constexpr usint                batchSize           = 0;
    constexpr uint32_t             numLargeDigits      = 0;
    constexpr usint                multiplicativeDepth = 0;
    constexpr usint                scalingFactorBits   = 0;
    constexpr SecurityLevel        securityLevel       = HEStd_128_classic; // set to avoid compiler errors
    constexpr usint                ringDim             = 0;
    constexpr EncryptionTechnique  encryptionTechnique = STANDARD;
    constexpr MultiplicationTechnique multiplicationTechnique = HPS;
    constexpr usint                multiHopQModulusLowerBound = 0;
}; // namespace BFVRNS_SCHEME_DEFAULTS

namespace BGVRNS_SCHEME_DEFAULTS {
    constexpr SCHEME               scheme              = BGVRNS_SCHEME;
    constexpr PlaintextModulus     ptModulus           = 0;
    constexpr usint                relinWindow         = 0;
    constexpr float                standardDeviation   = 3.19;
    constexpr float                rootHermiteFactor   = 1.006;
    constexpr float                assuranceMeasure    = 9.0;
    constexpr MODE                 mode                = OPTIMIZED;
    constexpr int                  depth               = 1;
    constexpr int                  maxDepth            = 2;
    constexpr KeySwitchTechnique   ksTech              = HYBRID;
    constexpr RescalingTechnique   rsTech              = FLEXIBLEAUTO; // set to avoid compiler errors
    constexpr usint                firstModSize        = 0;
    constexpr usint                cyclOrder           = 0;
    constexpr usint                numPrimes           = 0;
    constexpr usint                scaleExp            = 0;
    constexpr usint                batchSize           = 0;
    constexpr uint32_t             numLargeDigits      = 0;
    constexpr usint                multiplicativeDepth = 0;
    constexpr usint                scalingFactorBits   = 0;
    constexpr SecurityLevel        securityLevel       = HEStd_128_classic;
    constexpr usint                ringDim             = 0;
    constexpr EncryptionTechnique  encryptionTechnique = STANDARD;
    constexpr MultiplicationTechnique multiplicationTechnique = HPS;
    constexpr usint                multiHopQModulusLowerBound = 0;
}; // namespace BGVRNS_SCHEME_DEFAULTS

//====================================================================================================================

}  // namespace lbcrypto


#endif // _CRYPTOCONTEXTPARAMS_DEFAULTS_H_

