#ifndef BINTRAY_CLIENT_HPP
#define BINTRAY_CLIENT_HPP

#include <vector>
#include <WString.h>
#include <utility>

class BintrayClient
{
public:
    BintrayClient(const String& user, const String& repository, const String& package);
    String getUser() const;
    String getRepository() const;
    String getPackage() const;
    String getStorageHost() const;
    String getApiHost() const;
    const char* getUserCertificate(const String& url) const;
    String getLatestVersion() const;
    String getBinaryPath(const String& version) const;

private:
    String requestHTTPContent(const String& url) const;
    String getLatestVersionRequestUrl() const;
    String getBinaryPathRequestUrl(const String& version) const;
    String m_user;
    String m_repo;
    String m_package;
    const String m_storageHost;
    const String m_apiHost;
    std::vector<std::pair<String, const char*>> m_certificates;
};

#endif // BINTRAY_CLIENT_HPP