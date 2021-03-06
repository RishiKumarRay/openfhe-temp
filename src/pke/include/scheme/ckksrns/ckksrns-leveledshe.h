// @file pre-base.h -- Public key type for lattice crypto operations.
// @author TPOC: contact@palisade-crypto.org
//
// @copyright Copyright (c) 2019, New Jersey Institute of Technology (NJIT)
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution. THIS SOFTWARE IS
// PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LBCRYPTO_CRYPTO_CKKSRNS_LEVELEDSHE_H
#define LBCRYPTO_CRYPTO_CKKSRNS_LEVELEDSHE_H

#include "schemerns/rns-leveledshe.h"

/**
 * @namespace lbcrypto
 * The namespace of lbcrypto
 */
namespace lbcrypto {

class LeveledSHECKKSRNS : public LeveledSHERNS {
public:

  virtual ~LeveledSHECKKSRNS() {}

  /////////////////////////////////////////
  // SHE ADDITION
  /////////////////////////////////////////

  using LeveledSHERNS::EvalAdd;
  using LeveledSHERNS::EvalAddInPlace;

  /////////////////////////////////////////
  // SHE ADDITION PLAINTEXT
  /////////////////////////////////////////

  /////////////////////////////////////////
  // SHE ADDITION CONSTANT
  /////////////////////////////////////////

  virtual Ciphertext<DCRTPoly> EvalAdd(ConstCiphertext<DCRTPoly> ciphertext,
                                      double constant) const override;

  virtual void EvalAddInPlace(Ciphertext<DCRTPoly> &ciphertext,
                              double constant) const override;

  /////////////////////////////////////////
  // SHE SUBTRACTION
  /////////////////////////////////////////

  /////////////////////////////////////////
  // SHE SUBTRACTION PLAINTEXT
  /////////////////////////////////////////

  /////////////////////////////////////////
  // SHE SUBTRACTION CONSTANT
  /////////////////////////////////////////

  using LeveledSHERNS::EvalSub;
  using LeveledSHERNS::EvalSubInPlace;

  virtual Ciphertext<DCRTPoly> EvalSub(ConstCiphertext<DCRTPoly> ciphertext,
                                      double constant) const override;

  virtual void EvalSubInPlace(Ciphertext<DCRTPoly> &ciphertext,
                              double constant) const override;

  /////////////////////////////////////////
  // SHE MULTIPLICATION
  /////////////////////////////////////////

  /////////////////////////////////////////
  // SHE MULTIPLICATION PLAINTEXT
  /////////////////////////////////////////

  /////////////////////////////////////////
  // SHE MULTIPLICATION CONSTANT
  /////////////////////////////////////////

  using LeveledSHEBase<DCRTPoly>::EvalMult;
  using LeveledSHEBase<DCRTPoly>::EvalMultInPlace;

  virtual Ciphertext<DCRTPoly> EvalMult(ConstCiphertext<DCRTPoly> ciphertext,
                                      double constant) const override;

  virtual void EvalMultInPlace(Ciphertext<DCRTPoly> &ciphertext,
                              double constant) const override;

  /////////////////////////////////////
  // Mod Reduce
  /////////////////////////////////////

  /**
   * Method for rescaling in-place.
   *
   * @param cipherText is the ciphertext to perform modreduce on.
   * @param levels the number of towers to drop.
   * @details \p cipherText will have modulus reduction performed in-place.
   */
  virtual void ModReduceInternalInPlace(Ciphertext<DCRTPoly> &ciphertext,
                                        size_t levels = 1) const override;

  /////////////////////////////////////
  // Level Reduce
  /////////////////////////////////////

  /**
   * Method for in-place Level Reduction in the CKKS scheme. It just drops
   * "levels" number of the towers of the ciphertext without changing the
   * underlying plaintext.
   *
   * @param cipherText1 is the ciphertext to be level reduced in-place
   * @param linearKeySwitchHint not used in the CKKS scheme.
   * @param levels the number of towers to drop.
   */
  virtual void LevelReduceInternalInPlace(Ciphertext<DCRTPoly> &ciphertext,
                                          const EvalKey<DCRTPoly> evalKey,
                                          size_t levels) const override;

  /////////////////////////////////////
  // Compress
  /////////////////////////////////////

  virtual Ciphertext<DCRTPoly> Compress(ConstCiphertext<DCRTPoly> ciphertext,
                                        size_t towersLeft = 1) const override;

  /////////////////////////////////////
  // CKKS Core
  /////////////////////////////////////

  void EvalMultCoreInPlace(Ciphertext<DCRTPoly> &ciphertext, double constant) const;

  virtual void AdjustLevelsAndDepthInPlace(
      Ciphertext<DCRTPoly> &ciphertext1, Ciphertext<DCRTPoly> &ciphertext2) const override;

  virtual void AdjustLevelsAndDepthToOneInPlace(
      Ciphertext<DCRTPoly> &ciphertext1, Ciphertext<DCRTPoly> &ciphertext2) const override;

  virtual void AdjustLevelsAndDepthInPlace(
      Ciphertext<DCRTPoly> &ciphertext, DCRTPoly &plaintext, usint ptDepth) const override;

  virtual void AdjustLevelsAndDepthToOneInPlace(
      Ciphertext<DCRTPoly> &ciphertext, DCRTPoly &plaintext, usint ptDepth) const override;

  vector<DCRTPoly::Integer> GetElementForEvalAddOrSub(
      ConstCiphertext<DCRTPoly> ciphertext, double constant) const;

  vector<DCRTPoly::Integer> GetElementForEvalMult(
        ConstCiphertext<DCRTPoly> ciphertext, double constant) const;

  /////////////////////////////////////
  // SERIALIZATION
  /////////////////////////////////////

  template <class Archive>
  void save(Archive &ar) const {
    ar(cereal::base_class<LeveledSHERNS>(this));
  }

  template <class Archive>
  void load(Archive &ar) {
    ar(cereal::base_class<LeveledSHERNS>(this));
  }

  std::string SerializedObjectName() const { return "LeveledSHECKKSRNS"; }
};

}  // namespace lbcrypto

#endif
