#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include <string.h>

std::string privateKey ="-----BEGIN PRIVATE KEY-----\n"\
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDlCpivShZByWzK\n"\
"7FNw/zRTXiEC9gxrtA9x3BDkHGOCBU3jtuSJHY+Z70AmcjQSojwa1+iUrughqOEY\n"\
"mizngH6O4Q2IwR5/7W1o6Bi2uiFLLjy4G0S+66DQJSSYdL3sG3FpY5btgeBIBt5C\n"\
"aZHppgmZTECPKJZXJiSHZNqc0tXg7Z/nlmmyxBcRCWJHLMa1jgjeR6rE+LjBOnjv\n"\
"z8yaTKjdp7Iq7iAW4JHmyBFWUiuItwN6Ko9uyJDgFG86osOv+CO6ps1tuWAMdote\n"\
"FYcx87Qi2SIR/QV74GYm+Ticc+paf/6DcRwLlyRMxdskimbLzq+J99JP3X0eJbSq\n"\
"eqCie1u1AgMBAAECggEACBTfDST7qDprorUC1F8bhnXbtioehz4CeIBCaVDAyGxC\n"\
"/WNX6NL4nOlxSu9R0TbVEOu9hWIu3HcuwBU6kI2QXvQSa+5H0vg6mXkXsXow+raI\n"\
"28R8i7xZKixRValy2m31IGSW7/jv7rgCp3cS+AVM2+ecnzXYU362oGoJgP87Ht/6\n"\
"d9HS7VUgEqVvWEor+8c1Qb4S0ayEb/6qytZOj4vxKSbEMHJVBUugE8KfaaJXIi4B\n"\
"p74/z1CXe76BqxBbZj/zACi+zluKmoHX0QtgSlBj5z39gnHJKCNPoxz179sFCdoY\n"\
"PPYGsXsqsceQWbBIWJu60k/18frtjMR9nCXds3x9sQKBgQD21T0ujiW/YpMHk1zj\n"\
"LMARz5W2NZcul7u1BOVMGPmCwAvzRSo+g4pvq7/k1/EQVIf2TUMgnT6JKsPUvGd/\n"\
"Ez6N8/YycgGf7w93gXc491AP1ijGjIl5JD9VDR8eOXtQ2rdafAM/TI1x1llpn84m\n"\
"bWwumKCK2BIZYkvrQsqMlJuHJQKBgQDtjDRNLh10GAUx0UkvEc+AekGZg2pF8BBQ\n"\
"FMCOMZQqhjv5cBKF/2zl5h3AbxedD4GEv3ZsVGokKNtbOcQPNkq4s4m98irx78wi\n"\
"qa5itQraCs/Op30YPOWlleNSteUsT3kks3AmzucjHy+Ra5JfbxoxOrCU7Ey1yrx9\n"\
"nsRdFORlUQKBgEIUekmhYnenIyePCvoCe/UIttLN7AhcNLNUeTJzKTEYyto4tQOB\n"\
"YalduITCTtYJW1DUBFmR3zEHkt+8BJUfiMql+k4s+URg0qoyrcFBbFyg/8+Pxin5\n"\
"Sz8wwRjtWJcfxqBhvNjQXLmSIePGFYFyH9kloS0qhUXoa9yyx9j02+JFAoGBALW9\n"\
"OpaB3Y7ggbjngbI9Uh/daSrl2/y9+f0Ko3cmf1E5edEQ6VsTXGFXeROs6sgDAPhq\n"\
"81NWEg6wI4Lb4yAwTWKaGZv1Ft4atHcgkoZ6NF81Pl/AzHgqvxe8YTr6ZudLtkdZ\n"\
"8Uf/q963EVGUl1vkSIm5eTe2wcmj3inNujXssgDBAoGAJ6cEX8POpBiWVyRCSNR5\n"\
"22+z6F+CuXTCwpJVKnFkCq25MKOdNVvWPIn2Q4tPiA7riusWDD145A49kIvTKmIm\n"\
"Hs5MfrIQARhZUvkP38fXwAQ7cGVrKpSEFdK+btCAoUzLB9qZZiIHZ8Z2mJ5pJklA\n"\
"LzWuBqvORaQ8WDJzrCUtiTI=\n"\
"-----END PRIVATE KEY-----\n0";

std::string publicKey ="-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA5QqYr0oWQclsyuxTcP80\n"\
"U14hAvYMa7QPcdwQ5BxjggVN47bkiR2Pme9AJnI0EqI8GtfolK7oIajhGJos54B+\n"\
"juENiMEef+1taOgYtrohSy48uBtEvuug0CUkmHS97BtxaWOW7YHgSAbeQmmR6aYJ\n"\
"mUxAjyiWVyYkh2TanNLV4O2f55ZpssQXEQliRyzGtY4I3keqxPi4wTp478/Mmkyo\n"\
"3aeyKu4gFuCR5sgRVlIriLcDeiqPbsiQ4BRvOqLDr/gjuqbNbblgDHaLXhWHMfO0\n"\
"ItkiEf0Fe+BmJvk4nHPqWn/+g3EcC5ckTMXbJIpmy86viffST919HiW0qnqgontb\n"\
"tQIDAQAB\n"\
"-----END PUBLIC KEY-----\n0";

RSA* createPrivateRSA(std::string key) {
  RSA *rsa = NULL;
  const char* c_string = key.c_str();
  BIO * keybio = BIO_new_mem_buf((const void*)c_string, -1);
  if (keybio==NULL) {
      return 0;
  }
  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
  return rsa;
}

RSA* createPublicRSA(std::string key) {
  RSA *rsa = NULL;
  BIO *keybio;
  const char* c_string = key.c_str();
  keybio = BIO_new_mem_buf((const void*)c_string, -1);
  if (keybio==NULL) {
      return 0;
  }
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
  return rsa;
}

bool RSASign( RSA* rsa,
              const unsigned char* Msg,
              size_t MsgLen,
              unsigned char** EncMsg,
              size_t* MsgLenEnc) {
  EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();
  EVP_PKEY* priKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(priKey, rsa);
  if (EVP_DigestSignInit(m_RSASignCtx,NULL, EVP_sha512(), NULL,priKey)<=0) {
      return false;
  }
  if (EVP_DigestSignUpdate(m_RSASignCtx, Msg, MsgLen) <= 0) {
      return false;
  }
  if (EVP_DigestSignFinal(m_RSASignCtx, NULL, MsgLenEnc) <=0) {
      return false;
  }
  *EncMsg = (unsigned char*)malloc(*MsgLenEnc);
  if (EVP_DigestSignFinal(m_RSASignCtx, *EncMsg, MsgLenEnc) <= 0) {
      return false;
  }
  EVP_MD_CTX_free(m_RSASignCtx);
  return true;
}

bool RSAVerifySignature( RSA* rsa,
                         unsigned char* MsgHash,
                         size_t MsgHashLen,
                         const char* Msg,
                         size_t MsgLen,
                         bool* Authentic) {
  *Authentic = false;
  EVP_PKEY* pubKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(pubKey, rsa);
  EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();

  if (EVP_DigestVerifyInit(m_RSAVerifyCtx,NULL, EVP_sha512(),NULL,pubKey)<=0) {
    return false;
  }
  if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, Msg, MsgLen) <= 0) {
    return false;
  }
  int AuthStatus = EVP_DigestVerifyFinal(m_RSAVerifyCtx, MsgHash, MsgHashLen);
  if (AuthStatus==1) {
    *Authentic = true;
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return true;
  } else if(AuthStatus==0){
    *Authentic = false;
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return true;
  } else{
    *Authentic = false;
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return false;
  }
}

void Base64Encode( const unsigned char* buffer,
                   size_t length,
                   char** base64Text) {
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_write(bio, buffer, length);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);
  BIO_free_all(bio);

  *base64Text=(*bufferPtr).data;
}

size_t calcDecodeLength(const char* b64input) {
  size_t len = strlen(b64input);
  size_t padding = 0;

  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;
  return (len*3)/4 - padding;
}

void Base64Decode(const char* b64message, unsigned char** buffer, size_t* length) {
  BIO *bio, *b64;

  int decodeLen = calcDecodeLength(b64message);
  *buffer = (unsigned char*)malloc(decodeLen + 1);
  (*buffer)[decodeLen] = '\0';

  bio = BIO_new_mem_buf(b64message, -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);

  *length = BIO_read(bio, *buffer, strlen(b64message));
  BIO_free_all(bio);
}

char* signMessage(std::string privateKey, std::string plainText) {
  RSA* privateRSA = createPrivateRSA(privateKey); 
  unsigned char* encMessage;
  char* base64Text;
  size_t encMessageLength;
  RSASign(privateRSA, (unsigned char*) plainText.c_str(), plainText.length(), &encMessage, &encMessageLength);
  Base64Encode(encMessage, encMessageLength, &base64Text);
  free(encMessage);
  return base64Text;
}

bool verifySignature(std::string publicKey, std::string plainText, char* signatureBase64) {
  RSA* publicRSA = createPublicRSA(publicKey);
  unsigned char* encMessage;
  size_t encMessageLength;
  bool authentic;
  Base64Decode(signatureBase64, &encMessage, &encMessageLength);
  bool result = RSAVerifySignature(publicRSA, encMessage, encMessageLength, plainText.c_str(), plainText.length(), &authentic);
  return result & authentic;
}

int main() {
  std::string plainText = "My secret message.\n";
  char* signature = signMessage(privateKey, plainText);
  fprintf(stderr, "%s\n", signature);
  bool authentic = verifySignature(publicKey, "My secret message.\n", signature);
  if ( authentic ) {
    std::cout << "Authentic" << std::endl;
  } else {
    std::cout << "Not Authentic" << std::endl;
  }
}