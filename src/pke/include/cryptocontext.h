// @file cryptocontext.h -- Control for encryption operations.
// @author TPOC: contact@palisade-crypto.org
//
// @copyright Copyright (c) 2019, New Jersey Institute of Technology (NJIT))
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

#ifndef SRC_PKE_CRYPTOCONTEXT_H_
#define SRC_PKE_CRYPTOCONTEXT_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

//#include "palisade.h"
#include "encoding/plaintextfactory.h"
#include "key/allkey.h"
#include "schemerns/allrns.h"

#include "cryptocontextfactory.h"

#include "utils/caller_info.h"
#include "utils/serial.h"

namespace lbcrypto {

template <typename Element>
class CryptoContextImpl;

template <typename Element>
using CryptoContext = shared_ptr<CryptoContextImpl<Element>>;

/**
 * @brief CryptoContextImpl
 *
 * A CryptoContextImpl is the object used to access the PALISADE library
 *
 * All PALISADE functionality is accessed by way of an instance of a
 * CryptoContextImpl; we say that various objects are "created in" a context,
 * and can only be used in the context in which they were created
 *
 * All PALISADE methods are accessed through CryptoContextImpl methods. Guards
 * are implemented to make certain that only valid objects that have been
 * created in the context are used
 *
 * Contexts are created using the CryptoContextFactory, and can be serialized
 * and recovered from a serialization
 */
template <typename Element>
class CryptoContextImpl : public Serializable {
  using IntType = typename Element::Integer;
  using ParmType = typename Element::Params;

  friend class CryptoContextFactory<Element>;

 protected:
  // crypto parameters used for this context
  shared_ptr<CryptoParametersBase<Element>> params;
  // algorithm used; accesses all crypto methods
  shared_ptr<SchemeBase<Element>> scheme;

  static std::map<string, std::vector<EvalKey<Element>>>& evalMultKeyMap() {
    // cached evalmult keys, by secret key UID
    static std::map<string, std::vector<EvalKey<Element>>> s_evalMultKeyMap;
    return s_evalMultKeyMap;
  }

  static std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>&
  evalSumKeyMap() {
    // cached evalsum keys, by secret key UID
    static std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>
        s_evalSumKeyMap;
    return s_evalSumKeyMap;
  }

  static std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>&
  evalAutomorphismKeyMap() {
    // cached evalautomorphism keys, by secret key UID
    static std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>
        s_evalAutomorphismKeyMap;
    return s_evalAutomorphismKeyMap;
  }

  string m_schemeId;

  uint32_t m_keyGenLevel;

  /**
   * TypeCheck makes sure that an operation between two ciphertexts is permitted
   * @param a
   * @param b
   */
  void TypeCheck(ConstCiphertext<Element> a, ConstCiphertext<Element> b,
                 CALLER_INFO_ARGS_HDR) const {
    if (a == nullptr || b == nullptr) {
      std::string errorMsg(std::string("Null Ciphertext") + CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (a->GetCryptoContext().get() != this) {
      std::string errorMsg(
          std::string("Ciphertext was not created in this CryptoContext") +
          CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (a->GetCryptoContext() != b->GetCryptoContext()) {
      std::string errorMsg(
          std::string(
              "Ciphertexts were not created in the same CryptoContext") +
          CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (a->GetKeyTag() != b->GetKeyTag()) {
      std::string errorMsg(
          std::string("Ciphertexts were not encrypted with same keys") +
          CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (a->GetEncodingType() != b->GetEncodingType()) {
      std::stringstream ss;
      ss << "Ciphertext encoding types " << a->GetEncodingType();
      ss << " and " << b->GetEncodingType();
      ss << " do not match";
      ss << CALLER_INFO;
      PALISADE_THROW(type_error, ss.str());
    }
  }

  /**
   * TypeCheck makes sure that an operation between two ciphertexts is permitted
   * This is intended for mutable methods, hence inputs are Ciphretext instead
   * of ConstCiphertext.
   *
   * @param a
   * @param b
   */
  /*
 void TypeCheck(Ciphertext<Element> a,
                Ciphertext<Element> b,
                CALLER_INFO_ARGS_HDR) const {
   if (a == nullptr || b == nullptr) {
     std::string errorMsg(std::string("Null Ciphertext") + CALLER_INFO);
     PALISADE_THROW(type_error, errorMsg);
   }
   if (a->GetCryptoContext().get() != this) {
     std::string errorMsg(
       std::string("Ciphertext was not created in this CryptoContext") +
       CALLER_INFO);
     PALISADE_THROW(type_error, errorMsg);
   }
   if (a->GetCryptoContext() != b->GetCryptoContext()) {
     std::string errorMsg(
       std::string("Ciphertexts were not created in the same CryptoContext") +
       CALLER_INFO);
     PALISADE_THROW(type_error, errorMsg);
   }
   if (a->GetKeyTag() != b->GetKeyTag()) {
     std::string errorMsg(
       std::string("Ciphertexts were not encrypted with same keys") +
       CALLER_INFO);
     PALISADE_THROW(type_error, errorMsg);
   }
   if (a->GetEncodingType() != b->GetEncodingType()) {
     std::stringstream ss;
     ss << "Ciphertext encoding types " << a->GetEncodingType();
     ss << " and " << b->GetEncodingType();
     ss << " do not match";
     ss << CALLER_INFO;
     PALISADE_THROW(type_error, ss.str());
   }
 }
 */

  /**
   * TypeCheck makes sure that an operation between a ciphertext and a plaintext
   * is permitted
   * @param a
   * @param b
   */
  void TypeCheck(ConstCiphertext<Element> a, ConstPlaintext b,
                 CALLER_INFO_ARGS_HDR) const {
    if (a == nullptr) {
      std::string errorMsg(std::string("Null Ciphertext") + CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (b == nullptr) {
      std::string errorMsg(std::string("Null Plaintext") + CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (a->GetCryptoContext().get() != this) {
      std::string errorMsg(
          std::string("Ciphertext was not created in this CryptoContext") +
          CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }
    if (a->GetEncodingType() != b->GetEncodingType()) {
      std::stringstream ss;
      ss << "Ciphertext encoding type " << a->GetEncodingType();
      ss << " and Plaintext encoding type " << b->GetEncodingType();
      ss << " do not match";
      ss << CALLER_INFO;
      PALISADE_THROW(type_error, ss.str());
    }
  }

  bool Mismatched(const CryptoContext<Element> a) const {
    if (a.get() != this) {
      return true;
    }
    return false;
  }

  //TODO Check error occur, fix later
//  void CheckKey(const Key<Element> key) const {
//    if (key == nullptr || Mismatched(key->GetCryptoContext()))
//      PALISADE_THROW(config_error,
//                     "Information was not generated with this crypto context");
//  }

  void CheckConstCiphertext(ConstCiphertext<Element> ciphertext) const {
    if (ciphertext == nullptr || Mismatched(ciphertext->GetCryptoContext()))
      PALISADE_THROW(config_error,
                     "Information was not generated with this crypto context");
  }

  void CheckCiphertext(const Ciphertext<Element>& ciphertext) const {
    if (ciphertext == nullptr || Mismatched(ciphertext->GetCryptoContext()))
      PALISADE_THROW(config_error,
                     "Information was not generated with this crypto context");
  }

 public:
  PrivateKey<Element> privateKey;

  /**
   * This stores the private key in the crypto context.
   * This is only intended for debugging and should not be
   * used in production systems. Please define DEBUG_KEY in
   * palisade.h to enable this.
   *
   * If used, one can create a key pair and store the secret
   * key in th crypto context like this:
   *
   * auto keys = cc->KeyGen();
   * cc->SetPrivateKey(keys.secretKey);
   *
   * After that, anyone in the code, one can access the
   * secret key by getting the crypto context and doing the
   * following:
   *
   * auto sk = cc->GetPrivateKey();
   *
   * This key can be used for decrypting any intermediate
   * ciphertexts for debugging purposes.
   *
   * @param sk the secret key
   *
   */
  void SetPrivateKey(const PrivateKey<Element> privateKey) {
#ifdef DEBUG_KEY
    cerr << "Warning - SetPrivateKey is only intended to be used for debugging "
            "purposes - not for production systems."
         << endl;
    this->privateKey = privateKey;
#else
    PALISADE_THROW(
        not_available_error,
        "SetPrivateKey is only allowed if DEBUG_KEY is set in palisade.h");
#endif
  }

  /**
   * This gets the private key from the crypto context.
   * This is only intended for debugging and should not be
   * used in production systems. Please define DEBUG_KEY in
   * palisade.h to enable this.
   *
   * If used, one can create a key pair and store the secret
   * key in th crypto context like this:
   *
   * auto keys = cc->KeyGen();
   * cc->SetPrivateKey(keys.secretKey);
   *
   * After that, anyone in the code, one can access the
   * secret key by getting the crypto context and doing the
   * following:
   *
   * auto sk = cc->GetPrivateKey();
   *
   * This key can be used for decrypting any intermediate
   * ciphertexts for debugging purposes.
   *
   * @return the secret key
   *
   */
  const PrivateKey<Element> GetPrivateKey() {
#ifdef DEBUG_KEY
    return this->privateKey;
#else
    PALISADE_THROW(
        not_available_error,
        "GetPrivateKey is only allowed if DEBUG_KEY is set in palisade.h");
#endif
  }

  void setSchemeId(string schemeTag) { this->m_schemeId = schemeTag; }

  string getSchemeId() const { return this->m_schemeId; }

  /**
   * CryptoContextImpl constructor from pointers to parameters and scheme
   * @param params - pointer to CryptoParameters
   * @param scheme - pointer to Crypto Scheme
   */
  CryptoContextImpl(CryptoParametersBase<Element>* params = nullptr,
                    SchemeBase<Element>* scheme = nullptr,
                    const string& schemeId = "Not") {
    this->params.reset(params);
    this->scheme.reset(scheme);
    this->m_keyGenLevel = 0;
    this->m_schemeId = schemeId;
  }

  /**
   * CryptoContextImpl constructor from shared pointers to parameters and scheme
   * @param params - shared pointer to CryptoParameters
   * @param scheme - sharedpointer to Crypto Scheme
   */
  CryptoContextImpl(shared_ptr<CryptoParametersBase<Element>> params,
                    shared_ptr<SchemeBase<Element>> scheme,
                    const string& schemeId = "Not") {
    this->params = params;
    this->scheme = scheme;
    this->m_keyGenLevel = 0;
    this->m_schemeId = schemeId;
  }

  /**
   * Copy constructor
   * @param c - source
   */
  CryptoContextImpl(const CryptoContextImpl<Element>& c) {
    params = c.params;
    scheme = c.scheme;
    this->m_keyGenLevel = 0;
    this->m_schemeId = c.m_schemeId;
  }

  /**
   * Assignment
   * @param rhs - assigning from
   * @return this
   */
  CryptoContextImpl<Element>& operator=(const CryptoContextImpl<Element>& rhs) {
    params = rhs.params;
    scheme = rhs.scheme;
    m_keyGenLevel = rhs.m_keyGenLevel;
    m_schemeId = rhs.m_schemeId;
    return *this;
  }

  /**
   * A CryptoContextImpl is only valid if the shared pointers are both valid
   */
  operator bool() const { return params && scheme; }

  /**
   * Private methods to compare two contexts; this is only used internally and
   * is not generally available
   * @param a - operand 1
   * @param b - operand 2
   * @return true if the implementations have identical parms and scheme
   */
  friend bool operator==(const CryptoContextImpl<Element>& a,
                         const CryptoContextImpl<Element>& b) {
    // Identical if the parameters and the schemes are identical... the exact
    // same object, OR the same type and the same values
    if (a.params.get() == b.params.get()) {
      return true;
    } else {
      if (typeid(*a.params.get()) != typeid(*b.params.get())) {
        return false;
      }
      if (*a.params.get() != *b.params.get()) return false;
    }

    if (a.scheme.get() == b.scheme.get()) {
      return true;
    } else {
      if (typeid(*a.scheme.get()) != typeid(*b.scheme.get())) {
        return false;
      }
      if (*a.scheme.get() != *b.scheme.get()) return false;
    }

    return true;
  }

  friend bool operator!=(const CryptoContextImpl<Element>& a,
                         const CryptoContextImpl<Element>& b) {
    return !(a == b);
  }

  /**
   * SerializeEvalMultKey for a single EvalMult key or all EvalMult keys
   *
   * @param ser - stream to serialize to
   * @param sertype - type of serialization
   * @param id for key to serialize - if empty string, serialize them all
   * @return true on success
   */
  template <typename ST>
  static bool SerializeEvalMultKey(std::ostream& ser, const ST& sertype, string id = "");

  /**
   * SerializeEvalMultKey for all EvalMultKeys made in a given context
   *
   * @param cc whose keys should be serialized
   * @param ser - stream to serialize to
   * @param sertype - type of serialization
   * @return true on success (false on failure or no keys found)
   */
  template <typename ST>
  static bool SerializeEvalMultKey(std::ostream& ser, const ST& sertype, const CryptoContext<Element> cc) {
    std::map<string, std::vector<EvalKey<Element>>> omap;
    for (const auto& k : GetAllEvalMultKeys()) {
      if (k.second[0]->GetCryptoContext() == cc) {
        omap[k.first] = k.second;
      }
    }

    if (omap.size() == 0) return false;

    Serial::Serialize(omap, ser, sertype);
    return true;
  }

  /**
   * DeserializeEvalMultKey deserialize all keys in the serialization
   * deserialized keys silently replace any existing matching keys
   * deserialization will create CryptoContextImpl if necessary
   *
   * @param serObj - stream with a serialization
   * @return true on success
   */
  template <typename ST>
  static bool DeserializeEvalMultKey(std::istream& ser, const ST& sertype) {
    std::map<string, std::vector<EvalKey<Element>>> evalMultKeyMap;

    Serial::Deserialize(GetAllEvalMultKeys(), ser, sertype);

    // The deserialize call created any contexts that needed to be created....
    // so all we need to do is put the keys into the maps for their context

    for (auto k : GetAllEvalMultKeys()) {
      GetAllEvalMultKeys()[k.first] = k.second;
    }

    return true;
  }

  /**
   * ClearEvalMultKeys - flush EvalMultKey cache
   */
  static void ClearEvalMultKeys();

  /**
   * ClearEvalMultKeys - flush EvalMultKey cache for a given id
   * @param id
   */
  static void ClearEvalMultKeys(const string& id);

  /**
   * ClearEvalMultKeys - flush EvalMultKey cache for a given context
   * @param cc
   */
  static void ClearEvalMultKeys(const CryptoContext<Element> cc);

  /**
   * InsertEvalMultKey - add the given vector of keys to the map, replacing the
   * existing vector if there
   * @param vectorToInsert
   */
  static void InsertEvalMultKey(
      const std::vector<EvalKey<Element>>& evalKeyVec);

  /**
   * SerializeEvalSumKey for a single EvalSum key or all of the EvalSum keys
   *
   * @param ser - stream to serialize to
   * @param sertype - type of serialization
   * @param id - key to serialize; empty string means all keys
   * @return true on success
   */
  template <typename ST>
  static bool SerializeEvalSumKey(std::ostream& ser, const ST& sertype,
                                  string id = "") {
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>* smap;
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>> omap;

    if (id.length() == 0) {
      smap = &GetAllEvalSumKeys();
    } else {
      auto k = GetAllEvalSumKeys().find(id);

      if (k == GetAllEvalSumKeys().end()) return false;  // no such id

      smap = &omap;
      omap[k->first] = k->second;
    }
    Serial::Serialize(*smap, ser, sertype);
    return true;
  }

  /**
   * SerializeEvalSumKey for all of the EvalSum keys for a context
   *
   * @param ser - stream to serialize to
   * @param sertype - type of serialization
   * @param cc - context
   * @return true on success
   */
  template <typename ST>
  static bool SerializeEvalSumKey(std::ostream& ser, const ST& sertype,
                                  const CryptoContext<Element> cc) {
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>> omap;
    for (const auto& k : GetAllEvalSumKeys()) {
      if (k.second->begin()->second->GetCryptoContext() == cc) {
        omap[k.first] = k.second;
      }
    }

    if (omap.size() == 0) return false;

    Serial::Serialize(omap, ser, sertype);

    return true;
  }

  /**
   * DeserializeEvalSumKey deserialize all keys in the serialization
   * deserialized keys silently replace any existing matching keys
   * deserialization will create CryptoContextImpl if necessary
   *
   * @param ser - stream to serialize from
   * @param sertype - type of serialization
   * @return true on success
   */
  template <typename ST>
  static bool DeserializeEvalSumKey(std::istream& ser, const ST& sertype) {
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>
        evalSumKeyMap;

    Serial::Deserialize(evalSumKeyMap, ser, sertype);

    // The deserialize call created any contexts that needed to be created....
    // so all we need to do is put the keys into the maps for their context

    for (auto k : evalSumKeyMap) {
      GetAllEvalSumKeys()[k.first] = k.second;
    }

    return true;
  }

  /**
   * ClearEvalSumKeys - flush EvalSumKey cache
   */
  static void ClearEvalSumKeys();

  /**
   * ClearEvalSumKeys - flush EvalSumKey cache for a given id
   * @param id
   */
  static void ClearEvalSumKeys(const string& id);

  /**
   * ClearEvalSumKeys - flush EvalSumKey cache for a given context
   * @param cc
   */
  static void ClearEvalSumKeys(const CryptoContext<Element> cc);

  /**
   * InsertEvalSumKey - add the given map of keys to the map, replacing the
   * existing map if there
   * @param mapToInsert
   */
  static void InsertEvalSumKey(
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap);

  /**
   * SerializeEvalAutomorphismKey for a single EvalAuto key or all of the
   * EvalAuto keys
   *
   * @param ser - stream to serialize to
   * @param sertype - type of serialization
   * @param id - key to serialize; empty string means all keys
   * @return true on success
   */
  template <typename ST>
  static bool SerializeEvalAutomorphismKey(std::ostream& ser, const ST& sertype,
                                           string id = "") {
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>* smap;
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>> omap;
    if (id.length() == 0) {
      smap = &GetAllEvalAutomorphismKeys();
    } else {
      auto k = GetAllEvalAutomorphismKeys().find(id);

      if (k == GetAllEvalAutomorphismKeys().end()) return false;  // no such id

      smap = &omap;
      omap[k->first] = k->second;
    }
    Serial::Serialize(*smap, ser, sertype);
    return true;
  }

  /**
   * SerializeEvalAutomorphismKey for all of the EvalAuto keys for a context
   *
   * @param ser - stream to serialize to
   * @param sertype - type of serialization
   * @param cc - context
   * @return true on success
   */
  template <typename ST>
  static bool SerializeEvalAutomorphismKey(std::ostream& ser, const ST& sertype,
                                           const CryptoContext<Element> cc) {
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>> omap;
    for (const auto& k : GetAllEvalAutomorphismKeys()) {
      if (k.second->begin()->second->GetCryptoContext() == cc) {
        omap[k.first] = k.second;
      }
    }

    if (omap.size() == 0) return false;

    Serial::Serialize(omap, ser, sertype);
    return true;
  }

  /**
   * DeserializeEvalAutomorphismKey deserialize all keys in the serialization
   * deserialized keys silently replace any existing matching keys
   * deserialization will create CryptoContextImpl if necessary
   *
   * @param ser - stream to serialize from
   * @param sertype - type of serialization
   * @return true on success
   */
  template <typename ST>
  static bool DeserializeEvalAutomorphismKey(std::istream& ser,
                                             const ST& sertype) {
    std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>
        evalSumKeyMap;

    Serial::Deserialize(evalSumKeyMap, ser, sertype);

    // The deserialize call created any contexts that needed to be created....
    // so all we need to do is put the keys into the maps for their context

    for (auto k : evalSumKeyMap) {
      GetAllEvalAutomorphismKeys()[k.first] = k.second;
    }

    return true;
  }

  /**
   * ClearEvalAutomorphismKeys - flush EvalAutomorphismKey cache
   */
  static void ClearEvalAutomorphismKeys();

  /**
   * ClearEvalAutomorphismKeys - flush EvalAutomorphismKey cache for a given id
   * @param id
   */
  static void ClearEvalAutomorphismKeys(const string& id);

  /**
   * ClearEvalAutomorphismKeys - flush EvalAutomorphismKey cache for a given
   * context
   * @param cc
   */
  static void ClearEvalAutomorphismKeys(const CryptoContext<Element> cc);

  /**
   * InsertEvalAutomorphismKey - add the given map of keys to the map, replacing
   * the existing map if there
   * @param mapToInsert
   */
  static void InsertEvalAutomorphismKey(
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap);

  // TURN FEATURES ON
  /**
   * Enable a particular feature for use with this CryptoContextImpl
   * @param feature - the feature that should be enabled
   */
  void Enable(PKESchemeFeature feature) { scheme->Enable(feature); }

  /**
   * Enable several features at once
   * @param featureMask - bitwise or of several PKESchemeFeatures
   */
  void Enable(usint featureMask) { scheme->Enable(featureMask); }

  // GETTERS
  /**
   * Getter for Scheme
   * @return scheme
   */
  const shared_ptr<SchemeBase<Element>> GetScheme() const { return scheme; }

  /**
   * Getter for CryptoParams
   * @return params
   */
  const shared_ptr<CryptoParametersBase<Element>> GetCryptoParameters() const {
    return params;
  }

  size_t GetKeyGenLevel() const { return m_keyGenLevel; }

  void SetKeyGenLevel(size_t level) { m_keyGenLevel = level; }

  /**
   * Getter for element params
   * @return
   */
  const shared_ptr<ParmType> GetElementParams() const {
    return params->GetElementParams();
  }

  /**
   * Getter for encoding params
   * @return
   */
  const EncodingParams GetEncodingParams() const {
    return params->GetEncodingParams();
  }

  /**
   * Get the cyclotomic order used for this context
   *
   * @return
   */
  usint GetCyclotomicOrder() const {
    return params->GetElementParams()->GetCyclotomicOrder();
  }

  /**
   * Get the ring dimension used for this context
   *
   * @return
   */
  usint GetRingDimension() const {
    return params->GetElementParams()->GetRingDimension();
  }

  /**
   * Get the ciphertext modulus used for this context
   *
   * @return
   */
  const IntType& GetModulus() const {
    return params->GetElementParams()->GetModulus();
  }

  /**
   * Get the ciphertext modulus used for this context
   *
   * @return
   */
  const IntType& GetRootOfUnity() const {
    return params->GetElementParams()->GetRootOfUnity();
  }

  // KEYS GETTERS

  static std::map<string, std::vector<EvalKey<Element>>>& GetAllEvalMultKeys();

  static const vector<EvalKey<Element>>& GetEvalMultKeyVector(
      const string& keyID);

  static std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>&
  GetAllEvalAutomorphismKeys();

  static const std::map<usint, EvalKey<Element>>& GetEvalAutomorphismKeyMap(
      const string& id);

  static std::map<string, shared_ptr<std::map<usint, EvalKey<Element>>>>&
  GetAllEvalSumKeys();

  /**
   * GetEvalSumKey  returns the map
   *
   * @return the EvalSum key map
   */
  static const std::map<usint, EvalKey<Element>>& GetEvalSumKeyMap(
      const string& id);

  // PLAINTEXT FACTORY METHODS

  // TODO to be deprecated in 2.0
  /**
   * MakeStringPlaintext constructs a StringEncoding in this context
   * @param str
   * @return plaintext
   */
  Plaintext MakeStringPlaintext(const string& str) const {
    return PlaintextFactory::MakePlaintext(String, this->GetElementParams(),
                                           this->GetEncodingParams(), str);
  }

  /**
   * MakeCoefPackedPlaintext constructs a CoefPackedEncoding in this context
   * @param value
   * @return plaintext
   */
  Plaintext MakeCoefPackedPlaintext(const vector<int64_t>& value) const {
    return PlaintextFactory::MakePlaintext(CoefPacked, this->GetElementParams(),
                                           this->GetEncodingParams(), value);
  }

  /**
   * MakePackedPlaintext constructs a PackedEncoding in this context
   * @param value
   * @return plaintext
   */
  Plaintext MakePackedPlaintext(const vector<int64_t>& value) const {
    return PlaintextFactory::MakePlaintext(Packed, this->GetElementParams(),
                                           this->GetEncodingParams(), value);
  }

  /**
   * MakePlaintext static that takes a cc and calls the Plaintext Factory
   * @param encoding
   * @param cc
   * @param value
   * @return
   */
  template <typename Value1>
  static Plaintext MakePlaintext(PlaintextEncodings encoding,
                                 CryptoContext<Element> cc,
                                 const Value1& value) {
    return PlaintextFactory::MakePlaintext(encoding, cc->GetElementParams(),
                                           cc->GetEncodingParams(), value);
  }

  template <typename Value1, typename Value2>
  static Plaintext MakePlaintext(PlaintextEncodings encoding,
                                 CryptoContext<Element> cc, const Value1& value,
                                 const Value2& value2) {
    return PlaintextFactory::MakePlaintext(encoding, cc->GetElementParams(),
                                           cc->GetEncodingParams(), value,
                                           value2);
  }

  /**
   * COMPLEX ARITHMETIC IS NOT AVAILABLE STARTING WITH 1.10.6,
   * AND THIS METHOD BE DEPRECATED. USE THE REAL-NUMBER METHOD INSTEAD.
   * MakeCKKSPackedPlaintext constructs a CKKSPackedEncoding in this context
   * from a vector of complex numbers
   * @param value - input vector
   * @paran depth - depth used to encode the vector
   * @param level - level at each the vector will get encrypted
   * @param params - parameters to be usef for the ciphertext
   * @return plaintext
   */
  virtual Plaintext MakeCKKSPackedPlaintext(
      const std::vector<std::complex<double>>& value, size_t depth = 1,
      uint32_t level = 0, const shared_ptr<ParmType> params = nullptr) const {
    Plaintext p;
    const auto cryptoParams =
        std::dynamic_pointer_cast<CryptoParametersRNS>(
            GetCryptoParameters());

    double scFact = cryptoParams->GetScalingFactorReal(level);

    if (params == nullptr) {
      shared_ptr<ILDCRTParams<DCRTPoly::Integer>> elemParamsPtr;
      if (level != 0) {
        ILDCRTParams<DCRTPoly::Integer> elemParams =
            *(cryptoParams->GetElementParams());
        for (uint32_t i = 0; i < level; i++) {
          elemParams.PopLastParam();
        }
        elemParamsPtr =
            std::make_shared<ILDCRTParams<DCRTPoly::Integer>>(elemParams);
      } else {
        elemParamsPtr = cryptoParams->GetElementParams();
      }

      p = Plaintext(std::make_shared<CKKSPackedEncoding>(
          elemParamsPtr, this->GetEncodingParams(), value, depth, level,
          scFact));
    } else {
      p = Plaintext(std::make_shared<CKKSPackedEncoding>(
          params, this->GetEncodingParams(), value, depth, level, scFact));
    }

    p->Encode();
    return p;
  }

  /**
   * MakeCKKSPackedPlaintext constructs a CKKSPackedEncoding in this context
   * from a vector of real numbers
   * @param value - input vector
   * @paran depth - depth used to encode the vector
   * @param level - level at each the vector will get encrypted
   * @param params - parameters to be usef for the ciphertext
   * @return plaintext
   */
  virtual Plaintext MakeCKKSPackedPlaintext(
      const std::vector<double>& value, size_t depth = 1, uint32_t level = 0,
      const shared_ptr<ParmType> params = nullptr) const {
    std::vector<std::complex<double>> complexValue(value.size());
    std::transform(value.begin(), value.end(), complexValue.begin(),
                   [](double da) { return std::complex<double>(da); });

    return MakeCKKSPackedPlaintext(complexValue, depth, level, params);
  }

  /**
   * GetPlaintextForDecrypt returns a new Plaintext to be used in decryption.
   *
   * @param pte Type of plaintext we want to return
   * @param evp Element parameters
   * @param ep Encoding parameters
   * @return plaintext
   */
  static Plaintext GetPlaintextForDecrypt(PlaintextEncodings pte, shared_ptr<ParmType> evp, EncodingParams ep);

  /////////////////////////////////////////
  // PKE Wrapper
  /////////////////////////////////////////

  /**
   * KeyGen generates a key pair using this algorithm's KeyGen method
   * @return a public/secret key pair
   */
  KeyPair<Element> KeyGen() {
    return GetScheme()->KeyGen(CryptoContextFactory<Element>::GetContextForPointer(this), false);
  }

  /**
   * SparseKeyGen generates a key pair with special structure, and without full
   * entropy, for use in special cases like Ring Reduction
   * @return a public/secret key pair
   */
  KeyPair<Element> SparseKeyGen() {
    return GetScheme()->KeyGen(CryptoContextFactory<Element>::GetContextForPointer(this), true);
  }

  /**
   * Encrypt a plaintext using a given public key
   * @param publicKey
   * @param plaintext
   * @return ciphertext (or null on failure)
   */
  Ciphertext<Element> Encrypt(Plaintext plaintext, const PublicKey<Element> publicKey) const {
    if (plaintext == nullptr)
      PALISADE_THROW(type_error, "Input plaintext is nullptr");
//    CheckKey(publicKey);

    Ciphertext<Element> ciphertext =
        GetScheme()->Encrypt(plaintext->GetElement<Element>(), publicKey);

    if (ciphertext) {
      ciphertext->SetEncodingType(plaintext->GetEncodingType());
      ciphertext->SetScalingFactor(plaintext->GetScalingFactor());
      ciphertext->SetDepth(plaintext->GetDepth());
      ciphertext->SetLevel(plaintext->GetLevel());
    }

    return ciphertext;
  }

  Ciphertext<Element> Encrypt(const PublicKey<Element> publicKey, Plaintext plaintext) const {
    return Encrypt(plaintext, publicKey);
  }

  /**
   * Encrypt a plaintext using a given private key
   * @param privateKey
   * @param plaintext
   * @return ciphertext (or null on failure)
   */
  Ciphertext<Element> Encrypt(Plaintext plaintext, const PrivateKey<Element> privateKey) const {
//    if (plaintext == nullptr)
//      PALISADE_THROW(type_error, "Input plaintext is nullptr");
//    CheckKey(privateKey);

    Ciphertext<Element> ciphertext =
        GetScheme()->Encrypt(plaintext->GetElement<Element>(), privateKey);

    if (ciphertext) {
      ciphertext->SetEncodingType(plaintext->GetEncodingType());
      ciphertext->SetScalingFactor(plaintext->GetScalingFactor());
      ciphertext->SetDepth(plaintext->GetDepth());
      ciphertext->SetLevel(plaintext->GetLevel());
    }

    return ciphertext;
  }

  Ciphertext<Element> Encrypt(const PrivateKey<Element> privateKey, Plaintext plaintext) const {
    return Encrypt(plaintext, privateKey);
  }

  /**
   * Decrypt a single ciphertext into the appropriate plaintext
   *
   * @param privateKey - decryption key
   * @param ciphertext - ciphertext to decrypt
   * @param plaintext - resulting plaintext object pointer is here
   * @return
   */
  DecryptResult Decrypt(ConstCiphertext<Element> ciphertext,
                        const PrivateKey<Element> privateKey,
                        Plaintext* plaintext);

  DecryptResult Decrypt(const PrivateKey<Element> privateKey,
                        ConstCiphertext<Element> ciphertext,
                        Plaintext* plaintext) {
    return Decrypt(ciphertext, privateKey, plaintext);
  }

  /////////////////////////////////////////
  // KeySwitch Wrapper
  /////////////////////////////////////////

  /**
   * KeySwitchGen creates a key that can be used with the PALISADE KeySwitch
   * operation
   * @param key1
   * @param key2
   * @return new evaluation key
   */
  EvalKey<Element> KeySwitchGen(const PrivateKey<Element> oldPrivateKey, const PrivateKey<Element> newPrivateKey) const {
//    CheckKey(oldPrivateKey);
//    CheckKey(newPrivateKey);

    return GetScheme()->KeySwitchGen(oldPrivateKey, newPrivateKey);
  }

  /**
   * KeySwitch - PALISADE KeySwitch method
   * @param keySwitchHint - reference to KeySwitchHint
   * @param ciphertext - ciphertext
   * @return new CiphertextImpl after applying key switch
   */
  Ciphertext<Element> KeySwitch(ConstCiphertext<Element> ciphertext, const EvalKey<Element> evalKey) const {
    CheckConstCiphertext(ciphertext);
//    CheckKey(evalKey);

    return GetScheme()->KeySwitch(ciphertext, evalKey);
  }

  /**
   * KeySwitch - PALISADE KeySwitchInPlace method
   * @param keySwitchHint - reference to KeySwitchHint
   * @param ciphertext - ciphertext on which to perform in-place key switching
   */
  void KeySwitchInPlace(Ciphertext<Element>& ciphertext, const EvalKey<Element> evalKey) const {
    CheckCiphertext(ciphertext);
//    CheckKey(evalKey);

    GetScheme()->KeySwitchInPlace(ciphertext, evalKey);
  }

  /////////////////////////////////////////
  // SHE NEGATION Wrapper
  /////////////////////////////////////////

  /**
   * EvalSub - PALISADE Negate method for a ciphertext
   * @param ct
   * @return new ciphertext -ct
   */
  Ciphertext<Element> EvalNegate(ConstCiphertext<Element> ciphertext) const {
    CheckConstCiphertext(ciphertext);

    return GetScheme()->EvalNegate(ciphertext);
  }

  void EvalNegateInPlace(Ciphertext<Element>& ciphertext) const {
    CheckCiphertext(ciphertext);

    GetScheme()->EvalNegateInPlace(ciphertext);
  }

  /////////////////////////////////////////
  // SHE ADDITION Wrapper
  /////////////////////////////////////////

  Ciphertext<Element> EvalAdd(ConstCiphertext<Element> ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    return GetScheme()->EvalAdd(ciphertext1, ciphertext2);
  }

  /**
   * EvalAdd - PALISADE EvalAddInPlace method for a pair of ciphertexts
   * @param ct1 Input/output ciphertext
   * @param ct2 Input cipherext
   * @return \p ct1 contains \p ct1 + \p ct2
   */
  void EvalAddInPlace(Ciphertext<Element>& ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    GetScheme()->EvalAddInPlace(ciphertext1, ciphertext2);
  }

  /**
   * EvalAdd - PALISADE EvalAddMutable method for a pair of ciphertexts.
   * This is a mutable version - input ciphertexts may get automatically
   * rescaled, or level-reduced.
   *
   * @param ct1
   * @param ct2
   * @return new ciphertext for ct1 + ct2
   */
  Ciphertext<Element> EvalAddMutable(Ciphertext<Element>& ciphertext1, Ciphertext<Element>& ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    return GetScheme()->EvalAddMutable(ciphertext1, ciphertext2);
  }

  /**
   * EvalAdd - PALISADE EvalAdd method for a ciphertext and plaintext
   * @param ciphertext
   * @param plaintext
   * @return new ciphertext for ciphertext + plaintext
   */
  Ciphertext<Element> EvalAdd(ConstCiphertext<Element> ciphertext, ConstPlaintext plaintext) const {
    TypeCheck(ciphertext, plaintext);
    plaintext->SetFormat(EVALUATION);
    return GetScheme()->EvalAdd(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalAdd(ConstPlaintext plaintext, ConstCiphertext<Element> ciphertext) const {
    return EvalAdd(ciphertext, plaintext);
  }

  void EvalAddInPlace(Ciphertext<Element>& ciphertext, ConstPlaintext plaintext) const {
    TypeCheck(ciphertext, plaintext);
    plaintext->SetFormat(EVALUATION);
    GetScheme()->EvalAddInPlace(ciphertext, plaintext);
  }

  void EvalAddInPlace(ConstPlaintext plaintext, Ciphertext<Element>& ciphertext) const {
    EvalAddInPlace(ciphertext, plaintext);
  }
  /**
   * EvalAdd - PALISADE EvalAddMutable method for a ciphertext and plaintext
   * This is a mutable version - input ciphertexts may get automatically
   * rescaled, or level-reduced.
   *
   * @param ciphertext
   * @param plaintext
   * @return new ciphertext for ciphertext + plaintext
   */
  Ciphertext<Element> EvalAddMutable(Ciphertext<Element>& ciphertext, Plaintext plaintext) const {
    TypeCheck((ConstCiphertext<Element>)ciphertext, (ConstPlaintext)plaintext);
    plaintext->SetFormat(EVALUATION);
    return GetScheme()->EvalAddMutable(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalAddMutable(Plaintext plaintext, Ciphertext<Element>& ciphertext) const {
    return EvalAddMutable(ciphertext, plaintext);
  }

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //Ciphertext<Element> EvalAdd(ConstCiphertext<Element> ciphertext, const NativeInteger& constant) const {
  //  return GetScheme()->EvalAdd(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //Ciphertext<Element> EvalAdd(const NativeInteger& constant, ConstCiphertext<Element> ciphertext) const {
  //  return EvalAdd(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //void EvalAddInPlace(Ciphertext<Element>& ciphertext, const NativeInteger& constant) const {
  //  GetScheme()->EvalAddInPlace(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //void EvalAddInPlace(const NativeInteger& constant, Ciphertext<Element>& ciphertext) const {
  //  EvalAddInPlace(ciphertext, constant);
  //}

  /**
   * EvalAdd - PALISADE EvalAdd method for a ciphertext and constant
   * @param ciphertext
   * @param constant
   * @return new ciphertext for ciphertext + constant
   */
  Ciphertext<Element> EvalAdd(ConstCiphertext<Element> ciphertext, double constant) const {
    Ciphertext<Element> result =
        constant >= 0 ? GetScheme()->EvalAdd(ciphertext, constant)
                      : GetScheme()->EvalSub(ciphertext, -constant);
    return result;
  }

  Ciphertext<Element> EvalAdd(double constant, ConstCiphertext<Element> ciphertext) const {
    return EvalAdd(ciphertext, constant);
  }

  void EvalAddInPlace(Ciphertext<Element>& ciphertext, double constant) const {
    if (constant >= 0) {
      GetScheme()->EvalAddInPlace(ciphertext, constant);
    } else {
      GetScheme()->EvalSubInPlace(ciphertext, -constant);
    }
  }

  void EvalAddInPlace(double constant, Ciphertext<Element>& ciphertext) const {
    EvalAddInPlace(ciphertext, constant);
  }

  /////////////////////////////////////////
  // SHE SUBTRACTION Wrapper
  /////////////////////////////////////////

  /**
   * EvalSub - PALISADE EvalSub method for a pair of ciphertexts
   * @param ct1
   * @param ct2
   * @return new ciphertext for ct1 - ct2
   */
  Ciphertext<Element> EvalSub(ConstCiphertext<Element> ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    return GetScheme()->EvalSub(ciphertext1, ciphertext2);
  }

  void EvalSubInPlace(Ciphertext<Element>& ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    GetScheme()->EvalSubInPlace(ciphertext1, ciphertext2);
  }

  /**
   * EvalSub - PALISADE EvalSubMutable method for a pair of ciphertexts
   * This is a mutable version - input ciphertexts may get automatically
   * rescaled, or level-reduced.
   *
   * @param ct1
   * @param ct2
   * @return new ciphertext for ct1 - ct2
   */
  Ciphertext<Element> EvalSubMutable(Ciphertext<Element>& ciphertext1, Ciphertext<Element>& ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    return GetScheme()->EvalSubMutable(ciphertext1, ciphertext2);
  }

  /**
   * EvalSubPlain - PALISADE EvalSub method for a ciphertext and plaintext
   * @param ciphertext
   * @param plaintext
   * @return new ciphertext for ciphertext - plaintext
   */
  Ciphertext<Element> EvalSub(ConstCiphertext<Element> ciphertext, ConstPlaintext plaintext) const {
    TypeCheck(ciphertext, plaintext);
    return GetScheme()->EvalSub(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalSub(ConstPlaintext plaintext, ConstCiphertext<Element> ciphertext) const {
    return EvalAdd(EvalNegate(ciphertext), plaintext);
  }

  /**
   * EvalSubPlain - PALISADE EvalSubMutable method for a ciphertext and
   * plaintext This is a mutable version - input ciphertexts may get
   * automatically rescaled, or level-reduced.
   *
   * @param ciphertext
   * @param plaintext
   * @return new ciphertext for ciphertext - plaintext
   */
  Ciphertext<Element> EvalSubMutable(Ciphertext<Element>& ciphertext, Plaintext plaintext) const {
    TypeCheck((ConstCiphertext<Element>)ciphertext, (ConstPlaintext)plaintext);
    return GetScheme()->EvalSubMutable(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalSubMutable(Plaintext plaintext, Ciphertext<Element>& ciphertext) const {
    Ciphertext<Element> negated = EvalNegate(ciphertext);
    Ciphertext<Element> result = EvalAddMutable(negated, plaintext);
    ciphertext = EvalNegate(negated);
    return result;
  }

  Ciphertext<Element> EvalSub(ConstCiphertext<Element> ciphertext, double constant) const {
    Ciphertext<Element> result =
        constant >= 0 ? GetScheme()->EvalSub(ciphertext, constant)
                      : GetScheme()->EvalAdd(ciphertext, -constant);
    return result;
  }

  Ciphertext<Element> EvalSub(double constant, ConstCiphertext<Element> ciphertext) const {
    return EvalAdd(EvalNegate(ciphertext), constant);
  }

  void EvalSubInPlace(Ciphertext<Element>& ciphertext, double constant) const {
    if (constant >= 0) {
      GetScheme()->EvalSubInPlace(ciphertext, constant);
    } else {
      GetScheme()->EvalAddInPlace(ciphertext, -constant);
    }
  }

  void EvalSubInPlace(double constant, Ciphertext<Element>& ciphertext) const {
    EvalNegateInPlace(ciphertext);
    EvalAddInPlace(ciphertext, constant);
  }

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //Ciphertext<Element> EvalSub(ConstCiphertext<Element> ciphertext, const NativeInteger& constant) const {
  //  return GetScheme()->EvalSub(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //Ciphertext<Element> EvalSub(const NativeInteger& constant, ConstCiphertext<Element> ciphertext) const {
  //  return EvalAdd(EvalNegate(ciphertext), constant);
  //}

  void EvalSubInPlace(Ciphertext<Element>& ciphertext, const NativeInteger& constant) const {
    GetScheme()->EvalSubInPlace(ciphertext, constant);
  }

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //void EvalSubInPlace(const NativeInteger& constant, Ciphertext<Element>& ciphertext) const {
  //  EvalNegateInPlace(ciphertext);
  //  EvalAddInPlace(ciphertext, constant);
  //}

  /////////////////////////////////////////
  // SHE MULTIPLICATION Wrapper
  /////////////////////////////////////////

  /**
   * EvalMultKeyGen creates a key that can be used with the PALISADE EvalMult
   * operator
   * the new evaluation key is stored in cryptocontext
   * @param key
   */
  void EvalMultKeyGen(const PrivateKey<Element> privateKey);

  /**
   * EvalMultsKeyGen creates a vector evalmult keys that can be used with the
   * PALISADE EvalMult operator 1st key (for s^2) is used for multiplication of
   * ciphertexts of depth 1 2nd key (for s^3) is used for multiplication of
   * ciphertexts of depth 2, etc.
   * a vector of new evaluation keys is stored in crytpocontext
   *
   * @param key
   */
  void EvalMultKeysGen(const PrivateKey<Element> privateKey);

  /**
   * EvalMult - PALISADE EvalMult method for a pair of ciphertexts - with key
   * switching
   * @param ct1
   * @param ct2
   * @return new ciphertext for ct1 * ct2
   */
  Ciphertext<Element> EvalMult(ConstCiphertext<Element> ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);

    const auto evalKeyVec = GetEvalMultKeyVector(ciphertext1->GetKeyTag());
    if (!evalKeyVec.size()) {
      PALISADE_THROW(type_error, "Evaluation key has not been generated for EvalMult");
    }

    return GetScheme()->EvalMult(ciphertext1, ciphertext2, evalKeyVec[0]);
  }

  /**
   * EvalMult - PALISADE EvalMult method for a pair of ciphertexts - with key
   * switching This is a mutable version - input ciphertexts may get
   * automatically rescaled, or level-reduced.
   *
   * @param ct1
   * @param ct2
   * @return new ciphertext for ct1 * ct2
   */
  Ciphertext<Element> EvalMultMutable(Ciphertext<Element>& ciphertext1, Ciphertext<Element>& ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);

    const auto evalKeyVec = GetEvalMultKeyVector(ciphertext1->GetKeyTag());
    if (!evalKeyVec.size()) {
      PALISADE_THROW(type_error, "Evaluation key has not been generated for EvalMultMutable");
    }

    return GetScheme()->EvalMultMutable(ciphertext1, ciphertext2,
                                        evalKeyVec[0]);
  }

  /**
   * EvalMult - PALISADE EvalMult method for a pair of ciphertexts - no key
   * switching (relinearization)
   * @param ct1
   * @param ct2
   * @return new ciphertext for ct1 * ct2
   */
  Ciphertext<Element> EvalMultNoRelin(
      ConstCiphertext<Element> ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    TypeCheck(ciphertext1, ciphertext2);
    return GetScheme()->EvalMult(ciphertext1, ciphertext2);
  }

  /**
   * Function for evaluating multiplication on ciphertext followed by
   * relinearization operation. Currently it assumes that the input arguments
   * have total depth smaller than the supported depth. Otherwise, it throws an
   * error.
   *
   * @param ct1 first input ciphertext.
   * @param ct2 second input ciphertext.
   *
   * @return new ciphertext
   */
  Ciphertext<Element> EvalMultAndRelinearize(
      ConstCiphertext<Element> ciphertext1, ConstCiphertext<Element> ciphertext2) const {
    // input parameter check
    if (!ciphertext1 || !ciphertext2)
      PALISADE_THROW(type_error, "Input ciphertext is nullptr");

    const auto evalKeyVec = GetEvalMultKeyVector(ciphertext1->GetKeyTag());

    if (evalKeyVec.size() < (ciphertext1->GetElements().size() +
                             ciphertext2->GetElements().size() - 3)) {
      PALISADE_THROW(type_error,
                     "Insufficient value was used for maxDepth to generate "
                     "keys for EvalMult");
    }

    return GetScheme()->EvalMultAndRelinearize(ciphertext1, ciphertext2,
                                               evalKeyVec);
  }

  /**
   * Function for relinearization of a ciphertext.
   *
   * @param ct input ciphertext.
   *
   * @return relinearized ciphertext
   */
  Ciphertext<Element> Relinearize(ConstCiphertext<Element> ciphertext) const {
    // input parameter check
    if (!ciphertext) PALISADE_THROW(type_error, "Input ciphertext is nullptr");

    const auto evalKeyVec = GetEvalMultKeyVector(ciphertext->GetKeyTag());

    if (evalKeyVec.size() < (ciphertext->GetElements().size() - 2)) {
      PALISADE_THROW(type_error,
                     "Insufficient value was used for maxDepth to generate "
                     "keys for EvalMult");
    }

    return GetScheme()->Relinearize(ciphertext, evalKeyVec);
  }

  /**
   * Function for relinearization of a ciphertext.
   *
   * @param ct input ciphertext.
   *
   * @return relinearized ciphertext
   */
  void RelinearizeInPlace(Ciphertext<Element>& ciphertext) const {
    // input parameter check
    if (!ciphertext) PALISADE_THROW(type_error, "Input ciphertext is nullptr");

    const auto evalKeyVec = GetEvalMultKeyVector(ciphertext->GetKeyTag());
    if (evalKeyVec.size() < (ciphertext->GetElements().size() - 2)) {
      PALISADE_THROW(type_error,
                     "Insufficient value was used for maxDepth to generate "
                     "keys for EvalMult");
    }

    GetScheme()->RelinearizeInPlace(ciphertext, evalKeyVec);
  }

  Ciphertext<Element> EvalMult(ConstCiphertext<Element> ciphertext, ConstPlaintext plaintext) const {
    TypeCheck(ciphertext, plaintext);
    return GetScheme()->EvalMult(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalMult(ConstPlaintext plaintext, ConstCiphertext<Element> ciphertext) const {
    return EvalMult(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalMultMutable(Ciphertext<Element>& ciphertext, Plaintext plaintext) const {
    TypeCheck(ciphertext, plaintext);
    return GetScheme()->EvalMultMutable(ciphertext, plaintext);
  }

  Ciphertext<Element> EvalMultMutable(Plaintext plaintext, Ciphertext<Element>& ciphertext) const {
    return EvalMultMutable(ciphertext, plaintext);
  }

  //template <typename T = const NativeInteger,
  //    typename std::enable_if <!std::is_same<ConstCiphertext<Element>, T>::value, bool>::type = true>

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //Ciphertext<Element> EvalMult(ConstCiphertext<Element> ciphertext, const NativeInteger& constant) const {
  //  if (!ciphertext) {
  //    PALISADE_THROW(type_error, "Input ciphertext is nullptr");
  //  }
  //  return GetScheme()->EvalMult(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //Ciphertext<Element> EvalMult(const NativeInteger& constant, ConstCiphertext<Element> ciphertext) const {
  //  return EvalMult(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //void EvalMultInPlace(Ciphertext<Element>& ciphertext, const NativeInteger& constant) const {
  //  if (!ciphertext) {
  //    PALISADE_THROW(type_error, "Input ciphertext is nullptr");
  //  }

  //  GetScheme()->EvalMultInPlace(ciphertext, constant);
  //}

  // TODO (dsuponit): commented the code below to avoid compiler errors
  //void EvalMultInPlace(const NativeInteger& constant, Ciphertext<Element>& ciphertext) const {
  //  EvalMultInPlace(ciphertext, constant);
  //}

  Ciphertext<Element> EvalMult(ConstCiphertext<Element> ciphertext, double constant) const {
    if (!ciphertext) {
      PALISADE_THROW(type_error, "Input ciphertext is nullptr");
    }
    return GetScheme()->EvalMult(ciphertext, constant);
  }

  Ciphertext<Element> EvalMult(double constant, ConstCiphertext<Element> ciphertext) const {
    return EvalMult(ciphertext, constant);
  }

  void EvalMultInPlace(Ciphertext<Element>& ciphertext, double constant) const {
    if (!ciphertext) {
      PALISADE_THROW(type_error, "Input ciphertext is nullptr");
    }

    GetScheme()->EvalMultInPlace(ciphertext, constant);
  }

  void EvalMultInPlace(double constant, Ciphertext<Element>& ciphertext) const {
    EvalMultInPlace(ciphertext, constant);
  }

  /////////////////////////////////////////
  // SHE AUTOMORPHISM Wrapper
  /////////////////////////////////////////

  /**
   * Generate automophism keys for a given private key; Uses the private key for
   * encryption
   *
   * @param privateKey private key.
   * @param indexList list of automorphism indices to be computed
   * @return returns the evaluation keys
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> EvalAutomorphismKeyGen(
      const PrivateKey<Element> privateKey,
      const std::vector<usint>& indexList) const {
//    CheckKey(privateKey);
    if (!indexList.size())
      PALISADE_THROW(config_error, "Input index vector is empty");

    return GetScheme()->EvalAutomorphismKeyGen(privateKey, indexList);
  }

  /**
   * Generate automophism keys for a given private key
   *
   * @param publicKey original public key.
   * @param origPrivateKey original private key.
   * @param indexList list of automorphism indices to be computed
   * @return returns the evaluation keys; index 0 of the vector corresponds to
   * plaintext index 2, index 1 to plaintex index 3, etc.
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> EvalAutomorphismKeyGen(
      const PublicKey<Element> publicKey, const PrivateKey<Element> privateKey,
      const std::vector<usint>& indexList) const {
//    CheckKey(publicKey);
//    CheckKey(privateKey);
    if (!indexList.size())
      PALISADE_THROW(config_error, "Input index vector is empty");

    return GetScheme()->EvalAutomorphismKeyGen(publicKey, privateKey,
                                               indexList);
  }

  /**
   * Function for evaluating automorphism of ciphertext at index i
   *
   * @param ciphertext the input ciphertext.
   * @param i automorphism index
   * @param &evalKeys - reference to the vector of evaluation keys generated by
   * EvalAutomorphismKeyGen.
   * @return resulting ciphertext
   */
  Ciphertext<Element> EvalAutomorphism(
      ConstCiphertext<Element> ciphertext, usint i,
      const std::map<usint, EvalKey<Element>>& evalKeyMap,
      CALLER_INFO_ARGS_HDR) const {
    CheckConstCiphertext(ciphertext);

    if (evalKeyMap.empty()) {
      std::string errorMsg(std::string("Empty input key map") + CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }

    auto key = evalKeyMap.find(i);

    if (key == evalKeyMap.end()) {
      std::string errorMsg(std::string("Could not find an EvalKey for index ") +
                           std::to_string(i) + CALLER_INFO);
      PALISADE_THROW(type_error, errorMsg);
    }

    auto evalKey = key->second;

//    CheckKey(evalKey);

    return GetScheme()->EvalAutomorphism(ciphertext, i, evalKeyMap);
  }

  /**
   * EvalFastRotationPrecompute implements the precomputation step of
   * hoisted automorphisms.
   *
   * Please refer to Section 5 of Halevi and Shoup, "Faster Homomorphic
   * linear transformations in HELib." for more details, link:
   * https://eprint.iacr.org/2018/244.
   *
   * Generally, automorphisms are performed with three steps: (1) the
   * automorphism is applied on the ciphertext, (2) the automorphed values are
   * decomposed into digits, and (3) key switching is applied to make it
   * possible to further compute on the ciphertext.
   *
   * Hoisted automorphisms is a technique that performs the digit decomposition
   * for the original ciphertext first, and then performs the automorphism and
   * the key switching on the decomposed digits. The benefit of this is that the
   * digit decomposition is independent of the automorphism rotation index, so
   * it can be reused for multiple different indices. This can greatly improve
   * performance when we have to compute many automorphisms on the same
   * ciphertext. This routinely happens when we do permutations (EvalPermute).
   *
   * EvalFastRotationPrecompute implements the digit decomposition step of
   * hoisted automorphisms.
   *
   * @param ct the input ciphertext on which to do the precomputation (digit
   * decomposition)
   */
  shared_ptr<vector<Element>> EvalFastRotationPrecompute(ConstCiphertext<Element> ciphertext) const {
    return GetScheme()->EvalFastRotationPrecompute(ciphertext);
  }

  /**
   * EvalFastRotation implements the automorphism and key switching step of
   * hoisted automorphisms.
   *
   * Please refer to Section 5 of Halevi and Shoup, "Faster Homomorphic
   * linear transformations in HELib." for more details, link:
   * https://eprint.iacr.org/2018/244.
   *
   * Generally, automorphisms are performed with three steps: (1) the
   * automorphism is applied on the ciphertext, (2) the automorphed values are
   * decomposed into digits, and (3) key switching is applied to make it
   * possible to further compute on the ciphertext.
   *
   * Hoisted automorphisms is a technique that performs the digit decomposition
   * for the original ciphertext first, and then performs the automorphism and
   * the key switching on the decomposed digits. The benefit of this is that the
   * digit decomposition is independent of the automorphism rotation index, so
   * it can be reused for multiple different indices. This can greatly improve
   * performance when we have to compute many automorphisms on the same
   * ciphertext. This routinely happens when we do permutations (EvalPermute).
   *
   * EvalFastRotation implements the automorphism and key swithcing step of
   * hoisted automorphisms.
   *
   * This method assumes that all required rotation keys exist. This may not be
   * true if we are using baby-step/giant-step key switching. Please refer to
   * Section 5.1 of the above reference and EvalPermuteBGStepHoisted to see how
   * to deal with this issue.
   *
   * @param ct the input ciphertext to perform the automorphism on
   * @param index the index of the rotation. Positive indices correspond to left
   * rotations and negative indices correspond to right rotations.
   * @param m is the cyclotomic order
   * @param digits the digit decomposition created by EvalFastRotationPrecompute
   * at the precomputation step.
   */
  Ciphertext<Element> EvalFastRotation(
      ConstCiphertext<Element> ciphertext, const usint index, const usint m,
      const shared_ptr<vector<Element>> digits) const {
    return GetScheme()->EvalFastRotation(ciphertext, index, m, digits);
  }

  /**
   * EvalAtIndexKeyGen generates evaluation keys for a list of indices
   *
   * @param privateKey private key.
   * @param indexList list of indices.
   * @param publicKey public key (used in NTRU schemes).
   */
  void EvalAtIndexKeyGen(const PrivateKey<Element> privateKey,
                         const std::vector<int32_t>& indexList,
                         const PublicKey<Element> publicKey = nullptr);

  /**
   * Moves i-th slot to slot 0
   *
   * @param ciphertext.
   * @param i the index.
   * @return resulting ciphertext
   */
  Ciphertext<Element> EvalAtIndex(ConstCiphertext<Element> ciphertext,
                                  int32_t index) const;

  /////////////////////////////////////////
  // Leveled SHE Wrapper
  /////////////////////////////////////////

  /**
   * ComposedEvalMult - PALISADE composed evalmult
   * @param ciphertext1 - vector for first cipher text
   * @param ciphertext2 - vector for second cipher text
   * @param quadKeySwitchHint - is the quadratic key switch hint from original
   * private key to the quadratic key return vector of resulting ciphertext
   */
  Ciphertext<Element> ComposedEvalMult(
      ConstCiphertext<Element> ciphertext1,
      ConstCiphertext<Element> ciphertext2) const {
    CheckConstCiphertext(ciphertext1);
    CheckConstCiphertext(ciphertext2);

    auto evalKeyVec = GetEvalMultKeyVector(ciphertext1->GetKeyTag());
    if (!evalKeyVec.size()) {
      PALISADE_THROW(type_error,
                     "Evaluation key has not been generated for EvalMult");
    }

    return GetScheme()->ComposedEvalMult(ciphertext1, ciphertext2,
                                         evalKeyVec[0]);
  }

  /**
   * Rescale - An alias for PALISADE ModReduce method.
   * This is because ModReduce is called Rescale in CKKS.
   *
   * @param ciphertext - ciphertext
   * @return mod reduced ciphertext
   */
  Ciphertext<Element> Rescale(ConstCiphertext<Element> ciphertext) const {
    CheckConstCiphertext(ciphertext);

    return GetScheme()->ModReduce(ciphertext);
  }

  /**
   * Rescale - An alias for PALISADE ModReduceInPlace method.
   * This is because ModReduceInPlace is called RescaleInPlace in CKKS.
   *
   * @param ciphertext - ciphertext to be mod-reduced in-place
   */
  void RescaleInPlace(Ciphertext<Element>& ciphertext) const {
    CheckCiphertext(ciphertext);

    GetScheme()->ModReduceInPlace(ciphertext);
  }

  /**
   * ModReduce - PALISADE ModReduce method used only for BGVrns
   * @param ciphertext - ciphertext
   * @return mod reduced ciphertext
   */
  Ciphertext<Element> ModReduce(ConstCiphertext<Element> ciphertext) const {
    CheckConstCiphertext(ciphertext);

    return GetScheme()->ModReduce(ciphertext);
  }

  /**
   * ModReduce - PALISADE ModReduceInPlace method used only for BGVrns
   * @param ciphertext - ciphertext to be mod-reduced in-place
   */
  void ModReduceInPlace(Ciphertext<Element>& ciphertext) const {
    CheckCiphertext(ciphertext);

    GetScheme()->ModReduceInPlace(ciphertext);
  }

  /**
   * LevelReduce - PALISADE LevelReduce method
   * @param cipherText1
   * @param linearKeySwitchHint
   * @return vector of level reduced ciphertext
   */
  Ciphertext<Element> LevelReduce(ConstCiphertext<Element> ciphertext,
                                  const EvalKey<Element> evalKey,
                                  size_t levels = 1) const {
    CheckConstCiphertext(ciphertext);

    return GetScheme()->LevelReduce(ciphertext, evalKey, levels);
  }

  void LevelReduceInPlace(Ciphertext<Element> &ciphertext,
                                  const EvalKey<Element> evalKey,
                                  size_t levels = 1) const {
    CheckCiphertext(ciphertext);

    GetScheme()->LevelReduceInPlace(ciphertext, evalKey, levels);
  }
  /**
   * Compress - Reduces the size of ciphertext modulus to minimize the
   * communication cost before sending the encrypted result for decryption
   * @param ciphertext1 - input ciphertext
   * @param numTowers - number of CRT limbs after compressing (default is 1)
   * @return compressed ciphertext
   */
  Ciphertext<Element> Compress(ConstCiphertext<Element> ciphertext, uint32_t towersLeft = 1) const {
    if (ciphertext == nullptr)
      PALISADE_THROW(config_error, "input ciphertext is invalid (has no data)");

    return GetScheme()->Compress(ciphertext, towersLeft);
  }

  /////////////////////////////////////////
  // Advanced SHE Wrapper
  /////////////////////////////////////////

  /**
   * EvalAddMany - Evaluate addition on a vector of ciphertexts.
   * It computes the addition in a binary tree manner.
   *
   * @param ctList is the list of ciphertexts.
   *
   * @return new ciphertext.
   */
  Ciphertext<Element> EvalAddMany(const vector<Ciphertext<Element>>& ciphertextVec) const {
    // input parameter check
    if (!ciphertextVec.size())
      PALISADE_THROW(type_error, "Empty input ciphertext vector");

    return GetScheme()->EvalAddMany(ciphertextVec);
  }

  /**
   * EvalAddManyInPlace - Evaluate addition on a vector of ciphertexts.
   * Addition is computed in a binary tree manner. Difference with EvalAddMany
   * is that EvalAddManyInPlace uses the input ciphertext vector to store
   * intermediate results, to avoid the overhead of using extra tepmorary
   * space.
   *
   * @param ctList is the list of ciphertexts.
   *
   * @return new ciphertext.
   */
  Ciphertext<Element> EvalAddManyInPlace(vector<Ciphertext<Element>>& ciphertextVec) const {
    // input parameter check
    if (!ciphertextVec.size())
      PALISADE_THROW(type_error, "Empty input ciphertext vector");

    return GetScheme()->EvalAddManyInPlace(ciphertextVec);
  }

  /**
   * EvalLinearWSum - PALISADE EvalLinearWSum method to compute a linear
   * weighted sum
   *
   * @param ciphertexts a list of ciphertexts
   * @param constants a list of weights
   * @return new ciphertext containing the weighted sum
   */
  Ciphertext<Element> EvalLinearWSum(vector<ConstCiphertext<Element>>& ciphertextVec,
                                     const vector<double> &constantVec) const {
    return GetScheme()->EvalLinearWSum(ciphertextVec, constantVec);
  }

  Ciphertext<Element> EvalLinearWSum(
      const vector<double> &constantsVec,
      vector<ConstCiphertext<Element>>& ciphertextVec) const {
    return EvalLinearWSum(ciphertextVec, constantsVec);
  }

  /**
   * EvalLinearWSum - method to compute a linear weighted sum.
   * This is a mutable version, meaning the level/depth of input
   * ciphertexts may change in the process.
   *
   * @param ciphertexts a list of ciphertexts
   * @param constants a list of weights
   * @return new ciphertext containing the weighted sum
   */
  Ciphertext<Element> EvalLinearWSumMutable(
      vector<Ciphertext<Element>>& ciphertextVec,
      const vector<double> &constantsVec) const {
    return GetScheme()->EvalLinearWSumMutable(ciphertextVec, constantsVec);
  }

  Ciphertext<Element> EvalLinearWSumMutable(
      const vector<double> &constantsVec,
      vector<Ciphertext<Element>>& ciphertextVec) const {
    return EvalLinearWSumMutable(ciphertextVec, constantsVec);
  }

  /**
   * EvalMultMany - PALISADE function for evaluating multiplication on
   * ciphertext followed by relinearization operation (at the end). It computes
   * the multiplication in a binary tree manner. Also, it reduces the number of
   * elements in the ciphertext to two after each multiplication.
   * Currently it assumes that the consecutive two input arguments have
   * total depth smaller than the supported depth. Otherwise, it throws an
   * error.
   *
   * @param cipherTextList  is the ciphertext list.
   *
   * @return new ciphertext.
   */
  Ciphertext<Element> EvalMultMany(const vector<Ciphertext<Element>>& ciphertextVec) const {
    // input parameter check
    if (!ciphertextVec.size()) {
      PALISADE_THROW(type_error, "Empty input ciphertext vector");
    }

    const auto evalKeyVec = GetEvalMultKeyVector(ciphertextVec[0]->GetKeyTag());
    if (evalKeyVec.size() < (ciphertextVec[0]->GetElements().size() - 2)) {
      PALISADE_THROW(type_error,
                     "Insufficient value was used for maxDepth to generate "
                     "keys for EvalMult");
    }

    return GetScheme()->EvalMultMany(ciphertextVec, evalKeyVec);
  }

  /**
   * Method for polynomial evaluation for polynomials represented as power
   * series.
   *
   * @param &cipherText input ciphertext
   * @param &coefficients is the vector of coefficients in the polynomial; the
   * size of the vector is the degree of the polynomial + 1
   * @return the result of polynomial evaluation.
   */
  virtual Ciphertext<Element> EvalPoly(
      ConstCiphertext<Element> ciphertext,
      const std::vector<double>& coefficients) const {
    CheckConstCiphertext(ciphertext);

    return GetScheme()->EvalPoly(ciphertext, coefficients);
  }

  /**
   * EvalSumKeyGen Generates the key map to be used by evalsum
   *
   * @param privateKey private key.
   * @param publicKey public key (used in NTRU schemes).
   */
  void EvalSumKeyGen(const PrivateKey<Element> privateKey,
                     const PublicKey<Element> publicKey = nullptr);

  shared_ptr<std::map<usint, EvalKey<Element>>> EvalSumRowsKeyGen(
      const PrivateKey<Element> privateKey,
      const PublicKey<Element> publicKey = nullptr, usint rowSize = 0,
      usint subringDim = 0);

  shared_ptr<std::map<usint, EvalKey<Element>>> EvalSumColsKeyGen(
      const PrivateKey<Element> privateKey,
      const PublicKey<Element> publicKey = nullptr);

  /**
   * Function for evaluating a sum of all components
   *
   * @param ciphertext the input ciphertext.
   * @param batchSize size of the batch
   * @return resulting ciphertext
   */
  Ciphertext<Element> EvalSum(ConstCiphertext<Element> ciphertext, usint batchSize) const;

  Ciphertext<Element> EvalSumRows(
      ConstCiphertext<Element> ciphertext, usint rowSize,
      const std::map<usint, EvalKey<Element>>& evalSumKeyMap,
      usint subringDim = 0) const;

  Ciphertext<Element> EvalSumCols(
      ConstCiphertext<Element> ciphertext, usint rowSize,
      const std::map<usint, EvalKey<Element>>& evalSumKeyMap) const;

  /**
   * Evaluates inner product in batched encoding
   *
   * @param ciphertext1 first vector.
   * @param ciphertext2 second vector.
   * @param batchSize size of the batch to be summed up
   * @return resulting ciphertext
   */
  Ciphertext<Element> EvalInnerProduct(ConstCiphertext<Element> ciphertext1,
                                       ConstCiphertext<Element> ciphertext2,
                                       usint batchSize) const;

  /**
   * Evaluates inner product in batched encoding
   *
   * @param ciphertext1 first vector - ciphertext.
   * @param plaintext second vector - plaintext.
   * @param batchSize size of the batch to be summed up
   * @return resulting ciphertext
   */
  Ciphertext<Element> EvalInnerProduct(ConstCiphertext<Element> ciphertext,
                                       ConstPlaintext plaintext,
                                       usint batchSize) const;

  /**
   * Merges multiple ciphertexts with encrypted results in slot 0 into a single
   * ciphertext The slot assignment is done based on the order of ciphertexts in
   * the vector
   *
   * @param ciphertextVector vector of ciphertexts to be merged.
   * @param &evalKeys - reference to the map of evaluation keys generated by
   * EvalAutomorphismKeyGen.
   * @return resulting ciphertext
   */
  Ciphertext<Element> EvalMerge(const vector<Ciphertext<Element>>& ciphertextVec) const;

  /////////////////////////////////////////
  // PRE Wrapper
  /////////////////////////////////////////

  /**
   * ReKeyGen produces an Eval Key that PALISADE can use for Proxy Re Encryption
   * @param newKey (public)
   * @param oldKey (private)
   * @return new evaluation key
   */
  EvalKey<Element> ReKeyGen(const PrivateKey<Element> oldPrivateKey,
                            const PublicKey<Element> newPublicKey) const {
//    CheckKey(oldPrivateKey);
//    CheckKey(newPublicKey);

    return GetScheme()->ReKeyGen(oldPrivateKey, newPublicKey);
  }

  /**
   * ReKeyGen produces an Eval Key that PALISADE can use for Proxy Re Encryption
   * NOTE this functionality has been completely removed from PALISADE
   * @param newKey (private)
   * @param oldKey (private)
   * @return new evaluation key
   */
  EvalKey<Element> ReKeyGen(const PrivateKey<Element> originalPrivateKey,
                            const PrivateKey<Element> newPrivateKey) const
      __attribute__((deprecated("functionality removed from PALISADE")));

  /**
   * ReEncrypt - Proxy Re Encryption mechanism for PALISADE
   * @param evalKey - evaluation key from the PRE keygen method
   * @param ciphertext - vector of shared pointers to encrypted Ciphertext
   * @param publicKey the public key of the recipient of the re-encrypted
   * ciphertext.
   * @return vector of shared pointers to re-encrypted ciphertexts
   */
  Ciphertext<Element> ReEncrypt(ConstCiphertext<Element> ciphertext,
                                EvalKey<Element> evalKey,
                                const PublicKey<Element> publicKey = nullptr,
                                usint noiseflooding = 0) const {
    CheckConstCiphertext(ciphertext);
//    CheckKey(evalKey);

    return GetScheme()->ReEncrypt(ciphertext, evalKey, publicKey, noiseflooding);
  }

  /////////////////////////////////////////
  // Multiparty Wrapper
  /////////////////////////////////////////

  /**
   * Threshold FHE: Generates a public key from a vector of secret shares.
   * ONLY FOR DEBUGGIN PURPOSES. SHOULD NOT BE USED IN PRODUCTION.
   *
   * @param secretkeys secrete key shares.
   * @return key pair including the private for the current party and joined
   * public key
   */
  KeyPair<Element> MultipartyKeyGen(
      const vector<PrivateKey<Element>>& privateKeyVec) {
    if (!privateKeyVec.size())
      PALISADE_THROW(config_error, "Input private key vector is empty");
    return GetScheme()->MultipartyKeyGen(
        CryptoContextFactory<Element>::GetContextForPointer(this),
        privateKeyVec, false);
  }

  /**
   * Threshold FHE: Generation of a public key derived
   * from a previous joined public key (for prior secret shares) and the secret
   * key share of the current party.
   *
   * @param pk joined public key from prior parties.
   * @param makeSparse set to true if ring reduce by a factor of 2 is to be
   * used. NOT SUPPORTED BY ANY SCHEME ANYMORE.
   * @param fresh set to true if proxy re-encryption is used in the multi-party
   * protocol or star topology is used
   * @return key pair including the secret share for the current party and
   * joined public key
   */
  KeyPair<Element> MultipartyKeyGen(const PublicKey<Element> publicKey,
                                    bool makeSparse = false,
                                    bool fresh = false) {
    if (!publicKey) PALISADE_THROW(config_error, "Input public key is empty");
    return GetScheme()->MultipartyKeyGen(
        CryptoContextFactory<Element>::GetContextForPointer(this), publicKey,
        makeSparse, fresh);
  }

  /**
   * Threshold FHE: Method for decryption operation run by the lead decryption
   * client
   *
   * @param privateKey secret key share used for decryption.
   * @param ciphertext ciphertext id decrypted.
   */
  vector<Ciphertext<Element>> MultipartyDecryptLead(
      const vector<Ciphertext<Element>>& ciphertextVec,
      const PrivateKey<Element> privateKey) const {
//    CheckKey(privateKey);

    vector<Ciphertext<Element>> newCiphertextVec;

    for (size_t i = 0; i < ciphertextVec.size(); i++) {
      CheckCiphertext(ciphertextVec[i]);
      newCiphertextVec.push_back(
          GetScheme()->MultipartyDecryptLead(ciphertextVec[i], privateKey));
    }

    return newCiphertextVec;
  }

  /**
   * Threshold FHE: "Partial" decryption computed by all parties except for the
   * lead one
   *
   * @param privateKey secret key share used for decryption.
   * @param ciphertext ciphertext that is being decrypted.
   */
  vector<Ciphertext<Element>> MultipartyDecryptMain(
      const vector<Ciphertext<Element>>& ciphertextVec,
      const PrivateKey<Element> privateKey) const {
//    CheckKey(privateKey);

    vector<Ciphertext<Element>> newCiphertextVec;
    for (size_t i = 0; i < ciphertextVec.size(); i++) {
      CheckCiphertext(ciphertextVec[i]);
      newCiphertextVec.push_back(
          GetScheme()->MultipartyDecryptMain(ciphertextVec[i], privateKey));
    }

    return newCiphertextVec;
  }

  /**
   * Threshold FHE: Method for combining the partially decrypted ciphertexts
   * and getting the final decryption in the clear.
   *
   * @param &partialCiphertextVec vector of "partial" decryptions.
   * @param *plaintext the plaintext output.
   * @return the decoding result.
   */
  DecryptResult MultipartyDecryptFusion(
      const vector<Ciphertext<Element>>& partialCiphertextVec,
      Plaintext* plaintext) const;

  /**
   * Threshold FHE: Generates a joined evaluation key
   * from the current secret share and a prior joined
   * evaluation key
   *
   * @param originalPrivateKey secret key transformed from.
   * @param newPrivateKey secret key transformed to.
   * @param ek the prior joined evaluation key.
   * @return the new joined evaluation key.
   */
  EvalKey<Element> MultiKeySwitchGen(
      const PrivateKey<Element> originalPrivateKey,
      const PrivateKey<Element> newPrivateKey,
      const EvalKey<Element> evalKey) const {
    if (!originalPrivateKey)
      PALISADE_THROW(config_error, "Input first private key is nullptr");
    if (!newPrivateKey)
      PALISADE_THROW(config_error, "Input second private key is nullptr");
    if (!evalKey)
      PALISADE_THROW(config_error, "Input evaluation key is nullptr");

    return GetScheme()->MultiKeySwitchGen(originalPrivateKey, newPrivateKey,
                                          evalKey);
  }

  /**
   * Threshold FHE: Generates joined automorphism keys
   * from the current secret share and prior joined
   * automorphism keys
   *
   * @param privateKey secret key share.
   * @param eAuto a dictionary with prior joined automorphism keys.
   * @param &indexList a vector of automorphism indices.
   * @param keyId - new key identifier used for the resulting evaluation key
   * @return a dictionary with new joined automorphism keys.
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> MultiEvalAutomorphismKeyGen(
      const PrivateKey<Element> privateKey,
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap,
      const std::vector<usint>& indexList, const std::string& keyId = "") {
    if (!privateKey)
      PALISADE_THROW(config_error, "Input private key is nullptr");
    if (!evalKeyMap)
      PALISADE_THROW(config_error, "Input evaluation key map is nullptr");
    if (!indexList.size())
      PALISADE_THROW(config_error, "Input index vector is empty");

    return GetScheme()->MultiEvalAutomorphismKeyGen(privateKey, evalKeyMap,
                                                    indexList, keyId);
  }

  /**
   * Threshold FHE: Generates joined rotation keys
   * from the current secret share and prior joined
   * rotation keys
   *
   * @param privateKey secret key share.
   * @param eAuto a dictionary with prior joined rotation keys.
   * @param &indexList a vector of rotation indices.
   * @param keyId - new key identifier used for the resulting evaluation key
   * @return a dictionary with new joined rotation keys.
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> MultiEvalAtIndexKeyGen(
      const PrivateKey<Element> privateKey,
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap,
      const std::vector<int32_t>& indexList, const std::string& keyId = "") {
    if (!privateKey)
      PALISADE_THROW(config_error, "Input private key is nullptr");
    if (!evalKeyMap)
      PALISADE_THROW(config_error, "Input evaluation key map is nullptr");
    if (!indexList.size())
      PALISADE_THROW(config_error, "Input index vector is empty");

    return GetScheme()->MultiEvalAtIndexKeyGen(privateKey, evalKeyMap,
                                               indexList, keyId);
  }

  /**
   * Threshold FHE: Generates joined summation evaluation keys
   * from the current secret share and prior joined
   * summation keys
   *
   * @param privateKey secret key share.
   * @param eSum a dictionary with prior joined summation keys.
   * @param keyId - new key identifier used for the resulting evaluation key
   * @return new joined summation keys.
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> MultiEvalSumKeyGen(
      const PrivateKey<Element> privateKey,
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap,
      const std::string& keyId = "") {
    if (!privateKey)
      PALISADE_THROW(config_error, "Input private key is nullptr");
    if (!evalKeyMap)
      PALISADE_THROW(config_error, "Input evaluation key map is nullptr");
    return GetScheme()->MultiEvalSumKeyGen(privateKey, evalKeyMap, keyId);
  }

  /**
   * Threshold FHE: Adds two prior evaluation keys
   *
   * @param a first evaluation key.
   * @param b second evaluation key.
   * @param keyId - new key identifier used for the resulting evaluation key
   * @return the new joined key.
   */
  EvalKey<Element> MultiAddEvalKeys(EvalKey<Element> evalKey1,
                                    EvalKey<Element> evalKey2,
                                    const std::string& keyId = "") {
    if (!evalKey1)
      PALISADE_THROW(config_error, "Input first evaluation key is nullptr");
    if (!evalKey2)
      PALISADE_THROW(config_error, "Input second evaluation key is nullptr");

    return GetScheme()->MultiAddEvalKeys(evalKey1, evalKey2, keyId);
  }

  /**
   * Threshold FHE: Generates a partial evaluation key for homomorphic
   * multiplication based on the current secret share and an existing partial
   * evaluation key
   *
   * @param evalKey prior evaluation key.
   * @param sk current secret share.
   * @param keyId - new key identifier used for the resulting evaluation key
   * @return the new joined key.
   */
  EvalKey<Element> MultiMultEvalKey(PrivateKey<Element> privateKey,
                                    EvalKey<Element> evalKey,
                                    const std::string& keyId = "") {
    if (!privateKey)
      PALISADE_THROW(config_error, "Input private key is nullptr");
    if (!evalKey)
      PALISADE_THROW(config_error, "Input evaluation key is nullptr");

    return GetScheme()->MultiMultEvalKey(privateKey, evalKey, keyId);
  }

  /**
   * Threshold FHE: Adds two prior evaluation key sets for summation
   *
   * @param es1 first summation key set.
   * @param es2 second summation key set.
   * @param keyId - new key identifier used for the resulting evaluation key
   * @return the new joined key set for summation.
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> MultiAddEvalSumKeys(
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap1,
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap2,
      const std::string& keyId = "") {
    if (!evalKeyMap1)
      PALISADE_THROW(config_error, "Input first evaluation key map is nullptr");
    if (!evalKeyMap2)
      PALISADE_THROW(config_error,
                     "Input second evaluation key map is nullptr");

    return GetScheme()->MultiAddEvalSumKeys(evalKeyMap1, evalKeyMap2, keyId);
  }

  /**
   * Threshold FHE: Adds two prior evaluation key sets for automorphisms
   *
   * @param es1 first automorphism key set.
   * @param es2 second automorphism key set.
   * @param keyId - new key identifier used for the resulting evaluation key.
   * @return the new joined key set for summation.
   */
  shared_ptr<std::map<usint, EvalKey<Element>>> MultiAddEvalAutomorphismKeys(
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap1,
      const shared_ptr<std::map<usint, EvalKey<Element>>> evalKeyMap2,
      const std::string& keyId = "") {
    if (!evalKeyMap1)
      PALISADE_THROW(config_error, "Input first evaluation key map is nullptr");
    if (!evalKeyMap2)
      PALISADE_THROW(config_error,
                     "Input second evaluation key map is nullptr");

    return GetScheme()->MultiAddEvalAutomorphismKeys(evalKeyMap1, evalKeyMap2,
                                                     keyId);
  }

  /**
   * Threshold FHE: Adds two  partial public keys
   *
   * @param pubKey1 first public key.
   * @param pubKey2 second public key.
   * @param keyId - new key identifier used for the resulting evaluation key.
   * @return the new joined key.
   */
  PublicKey<Element> MultiAddPubKeys(PublicKey<Element> publicKey1,
                                     PublicKey<Element> publicKey2,
                                     const std::string& keyId = "") {
    if (!publicKey1)
      PALISADE_THROW(config_error, "Input first public key is nullptr");
    if (!publicKey2)
      PALISADE_THROW(config_error, "Input second public key is nullptr");

    return GetScheme()->MultiAddPubKeys(publicKey1, publicKey2, keyId);
  }

  /**
   * Threshold FHE: Adds two  partial evaluation keys for multiplication
   *
   * @param evalKey1 first evaluation key.
   * @param evalKey2 second evaluation key.
   * @param keyId - new key identifier used for the resulting evaluation key.
   * @return the new joined key.
   */
  EvalKey<Element> MultiAddEvalMultKeys(EvalKey<Element> evalKey1,
                                        EvalKey<Element> evalKey2,
                                        const std::string& keyId = "") {
    if (!evalKey1)
      PALISADE_THROW(config_error, "Input first evaluation key is nullptr");
    if (!evalKey2)
      PALISADE_THROW(config_error, "Input second evaluation key is nullptr");

    return GetScheme()->MultiAddEvalMultKeys(evalKey1, evalKey2, keyId);
  }

  template <class Archive>
  void save(Archive& ar, std::uint32_t const version) const {
    ar(cereal::make_nvp("cc", params));
    ar(cereal::make_nvp("kt", scheme));
    ar(cereal::make_nvp("si", m_schemeId));
  }

  template <class Archive>
  void load(Archive& ar, std::uint32_t const version) {
    if (version > SerializedVersion()) {
      PALISADE_THROW(deserialize_error,
                     "serialized object version " + std::to_string(version) +
                         " is from a later version of the library");
    }
    ar(cereal::make_nvp("cc", params));
    ar(cereal::make_nvp("kt", scheme));
    ar(cereal::make_nvp("si", m_schemeId));

    // NOTE: a pointer to this object will be wrapped in a shared_ptr, and is a
    // "CryptoContext". PALISADE relies on the notion that identical
    // CryptoContextImpls are not duplicated in memory Once we deserialize this
    // object, we must check to see if there is a matching object for this
    // object that's already existing in memory if it DOES exist, use it. If it
    // does NOT exist, add this to the cache of all contexts
  }

  virtual std::string SerializedObjectName() const { return "CryptoContext"; }
  static uint32_t SerializedVersion() { return 1; }
};

}  // namespace lbcrypto

#endif /* SRC_PKE_CRYPTOCONTEXT_H_ */
