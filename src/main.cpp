#include <Arduino.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "esp_random.h" // Include for ESP32 hardware RNG
// include hashing libraries
// #include "crypto/sha2.h"
#include "ripemd160.h"
#include "crypto/bip39.h"
#include "crypto/ecdsa.h"
#include "crypto/hmac.h"


//generate mnemonic


void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect
    }

    Serial.println("Starting Bitcoin private key generation...");
    std::vector<uint8_t> private_key = generate_private_key();
    if (private_key.empty()) {
        Serial.println("Bitcoin private key generation failed");
        return;
    }

    // Output the private key in hex format
    Serial.print("Bitcoin Private Key: ");
    for (auto byte : private_key) {
        Serial.print(String(byte, HEX));
    }
    Serial.println();

    // Generate the public key
    std::vector<uint8_t> public_key = generate_public_key(private_key);
    if (public_key.empty()) {
        Serial.println("Bitcoin public key generation failed");
        return;
    }

    // Output the public key in hex format
    Serial.print("Bitcoin Public Key: ");
    for (auto byte : public_key) {
        Serial.print(String(byte, HEX));
    }
    Serial.println();

    // Generate the hash160
    std::vector<uint8_t> hash160 = generate_hash160(public_key);
    if (hash160.empty()) {
        Serial.println("Hash160 generation failed");
        return;
    }


    // Serial.println((std::string("User Data: ") + user_data).c_str());
    // Serial.println((std::string("Data Hash: ") + data_hash).c_str());
    // Serial.print("Signature: ");
    // for (auto byte : signature) {
    //     Serial.print(String(byte, HEX));
    // }
    // Serial.println();
}


void loop() {
    // Your loop code here
}
