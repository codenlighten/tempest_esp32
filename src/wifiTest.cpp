#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/sha256.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

// WiFi credentials
const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";

// HTTP endpoint
const char* httpEndpoint = "https://yourhttpsendpoint.com/api";

// Function prototypes
void generateKeyPair(mbedtls_ecdsa_context &ecdsa);
String signMessage(mbedtls_ecdsa_context &ecdsa, const String &message);
String sha256(const String &data);
void sendSignedMessage(const String &message, const String &signature);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect
    }

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");

    // Initialize ECDSA context for key pair generation
    mbedtls_ecdsa_context ecdsa;
    mbedtls_ecdsa_init(&ecdsa);
    generateKeyPair(ecdsa);

    // Sign and send a message every 10 seconds
    while(true) {
        String message = "Current Time: " + String(millis());
        String hash = sha256(message);
        String signature = signMessage(ecdsa, hash);
        sendSignedMessage(hash, signature);
        delay(10000); // 10 seconds
    }

    // Free ECDSA context
    mbedtls_ecdsa_free(&ecdsa);
}

void loop() {
    // Not used in this example
}

void generateKeyPair(mbedtls_ecdsa_context &ecdsa) {
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "ecdsa";

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen(pers));
    mbedtls_ecdsa_genkey(&ecdsa, MBEDTLS_ECP_DP_SECP256R1, mbedtls_ctr_drbg_random, &ctr_drbg);

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

String signMessage(mbedtls_ecdsa_context &ecdsa, const String &message) {
    unsigned char hash[32];
    unsigned char sig[64];
    size_t sig_len;

    mbedtls_sha256((unsigned char*)message.c_str(), message.length(), hash, 0);
    mbedtls_ecdsa_write_signature(&ecdsa, MBEDTLS_MD_SHA256, hash, sizeof(hash), sig, &sig_len, mbedtls_ctr_drbg_random, &ctr_drbg);

    char buf[3];
    String sigHex = "";
    for (size_t i = 0; i < sig_len; i++) {
        sprintf(buf, "%02x", sig[i]);
        sigHex += buf;
    }

    return sigHex;
}

String sha256(const String &data) {
    unsigned char hash[32];
    mbedtls_sha256((unsigned char*)data.c_str(), data.length(), hash, 0);

    char buf[3];
    String hashHex = "";
    for (size_t i = 0; i < sizeof(hash); i++) {
        sprintf(buf, "%02x", hash[i]);
        hashHex += buf;
    }

    return hashHex;
}

void sendSignedMessage(const String &message, const String &signature) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(httpEndpoint);
        http.addHeader("Content-Type", "application/json");

        String payload = "{\"message\":\"" + message + "\", \"signature\":\"" + signature + "\"}";
        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi not connected");
    }
}
