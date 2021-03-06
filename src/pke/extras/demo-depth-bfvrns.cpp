// @file demo-depth-bfvrns.cpp.
// @author TPOC: contact@palisade-crypto.org
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

/*
 * @section DESCRIPTION
 * HPS demo for a homomorphic multiplication of depth 6 and three different
 * approaches for depth-3 multiplications
 *
 */

#define PROFILE

#include <iostream>

#include "palisade.h"
#include "scheme/bfvrns/cryptocontext-bfvrns.h"
#include "gen-cryptocontext.h"
#include "utils/debug.h"

using namespace std;
using namespace lbcrypto;

int main(int argc, char *argv[]) {
  ////////////////////////////////////////////////////////////
  // Set-up of parameters
  TimeVar t;
  double processingTime(0.0);

  int numkeys = 1 << 4;
  int numruns = 1 << 4;

  CCParams<CryptoContextBFVRNS> parameters;
  parameters.SetPlaintextModulus(65537);
  parameters.SetStandardDeviation(3.2);
  parameters.SetEvalMultCount(3);
  parameters.SetRelinWindow(1);
  parameters.SetScalingFactorBits(60);

  CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
  // enable features that you wish to use
  cryptoContext->Enable(PKE);
  cryptoContext->Enable(KEYSWITCH);
  cryptoContext->Enable(LEVELEDSHE);

  std::cout << "\np = "
            << cryptoContext->GetCryptoParameters()->GetPlaintextModulus()
            << std::endl;
  std::cout << "n = "
            << cryptoContext->GetCryptoParameters()
                       ->GetElementParams()
                       ->GetCyclotomicOrder() /
                   2
            << std::endl;
  std::cout << "log2 q = "
            << log2(cryptoContext->GetCryptoParameters()
                        ->GetElementParams()
                        ->GetModulus()
                        .ConvertToDouble())
            << std::endl;

  // Initialize Public Key Containers
  KeyPair<DCRTPoly> keyPair;

  cout << "\nMeasuring Multiplicative Depth:\n";
  TIC(t);

  std::vector<int64_t> vectorOfInts1 = { 1 };
  Plaintext plaintext = cryptoContext->MakeCoefPackedPlaintext(vectorOfInts1);
  Plaintext dec(plaintext);
  Ciphertext<DCRTPoly> ciphertext;
  std::vector<int> depth(numruns, 0);
  std::vector<int> min(numkeys, 0);

  for (int i = 0; i < numkeys; i++) {
    keyPair = cryptoContext->KeyGen();
    cryptoContext->EvalMultKeysGen(keyPair.secretKey);

    cout << "Key " << i << ": ";

    for (int j = 0; j < numruns; j++) {
      ciphertext = cryptoContext->Encrypt(keyPair.publicKey, plaintext);

      dec = plaintext;

      depth[j] = 0;
      while (dec == plaintext) {
        ciphertext = cryptoContext->EvalMult(ciphertext, ciphertext);
        cryptoContext->Decrypt(keyPair.secretKey, ciphertext, &dec);

        depth[j]++;
      }
      depth[j]--;
      cerr << depth[j] << " ";
    }
    min[i] = depth[0];
    for (int j = 1; j < numruns; j++)
      if (min[i] > depth[j]) min[i] = depth[j];
    cout << "--> " << min[i] << "\n";
  }
  cout << "\n";

  int MIN = min[0];
  for (int i = 1; i < numkeys; i++)
    if (MIN > min[i]) MIN = min[i];
  cout << "Smallest depth = " << MIN;

  processingTime = TOC(t);
  std::cout << " in " << processingTime / 1000. << "s\n" << std::endl;

  return 0;
}
