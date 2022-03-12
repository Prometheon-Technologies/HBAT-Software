#include <Arduino.h>
#include <ArduinoJSON.h>
#include <HTTPClient.h>

#include "BintrayClient.hpp"
#include "BintrayCertificates.hpp"

BintrayClient::BintrayClient(const String &user, const String &repository, const String &package)
    : m_user(user), m_repo(repository), m_package(package),
      m_storageHost("dl.bintray.com"),
      m_apiHost("api.bintray.com")
{
    m_certificates.emplace_back("cloudfront.net", CLOUDFRONT_API_ROOT_CA);
    m_certificates.emplace_back("akamai.bintray.com", BINTRAY_AKAMAI_ROOT_CA);
    m_certificates.emplace_back("bintray.com", BINTRAY_API_ROOT_CA);
}

String BintrayClient::getUser() const
{
    return m_user;
}

String BintrayClient::getRepository() const
{
    return m_repo;
}

String BintrayClient::getPackage() const
{
    return m_package;
}

String BintrayClient::getStorageHost() const
{
    return m_storageHost;
}

String BintrayClient::getApiHost() const
{
    return m_apiHost;
}

String BintrayClient::getLatestVersionRequestUrl() const
{
    return String("https://") + getApiHost() + "/packages/" + getUser() + "/" + getRepository() + "/" + getPackage() + "/versions/_latest";
}

String BintrayClient::getBinaryPathRequestUrl(const String &version) const
{
    return String("https://") + getApiHost() + "/packages/" + getUser() + "/" + getRepository() + "/" + getPackage() + "/versions/" + version + "/files";
}

const char *BintrayClient::getUserCertificate(const String &url) const
{
    for (auto &cert : m_certificates)
    {
        if (url.indexOf(cert.first) >= 0)
        {
            return cert.second;
        }
    }
    // Return the default certificate for *.bintray.com.
    return m_certificates.rbegin()->second;
}

String BintrayClient::requestHTTPContent(const String &url) const
{
    String payload;
    HTTPClient http;
    http.begin(url, getUserCertificate(url));
    int httpCode = http.GET();
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            payload = http.getString();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    return payload;
}

String BintrayClient::getLatestVersion() const
{
    String version;
    const String url = getLatestVersionRequestUrl();
    String jsonResult = requestHTTPContent(url);
    const size_t bufferSize = 1024;
    if (jsonResult.length() > bufferSize)
    {
        Serial.println("Error: Could parse JSON. Input data is too big.");
        return version;
    }
    StaticJsonDocument<bufferSize> doc;
    DeserializationError error = deserializeJson(doc, jsonResult.c_str());
    // Check for parsing errors.
    if (error)
    {
        Serial.printf("Error: Could not parse JSON: %s\n", error.c_str());
        return version;
    }
    return doc["name"].as<String>();
}

String BintrayClient::getBinaryPath(const String &version) const
{
    String binaryPath;
    const String url = getBinaryPathRequestUrl(version);
    String jsonResult = requestHTTPContent(url);
    const size_t bufferSize = 1024;
    if (jsonResult.length() > bufferSize)
    {
        Serial.println("Error: Could parse JSON. Input data is too big.");
        return binaryPath;
    }
    StaticJsonDocument<bufferSize> doc;
    DeserializationError error = deserializeJson(doc, jsonResult.c_str());
    // Check for parsing errors.
    JsonObject firstItem = doc[0];

    if (error)
    {
        Serial.printf("Error: Could not parse JSON: %s\n", error.c_str());
        return binaryPath;
    }
    return "/" + getUser() + "/" + getRepository() + "/" + firstItem["path"].as<String>();
}
