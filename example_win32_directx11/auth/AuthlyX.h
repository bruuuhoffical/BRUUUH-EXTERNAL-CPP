#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <winhttp.h>
#include <sddl.h>
#include <iphlpapi.h>
#include <wincrypt.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "shell32.lib")

class AuthlyLogger {
public:
    static bool Enabled;
    static std::string AppName;

    static void Log(const std::string& content) {
        if (!Enabled) return;

        try {
            char commonAppData[MAX_PATH];
            if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, commonAppData))) {
                std::string baseDir = std::string(commonAppData) + "\\AuthlyX\\logs\\" + AppName;

                CreateDirectoryA(baseDir.c_str(), NULL);

                std::string logFile = baseDir + "\\" + GetCurrentDate() + "_log.txt";
                std::string redacted = Redact(content);

                std::ofstream file(logFile, std::ios::app);
                if (file) {
                    file << "[" << GetCurrentTime() << "] " << redacted << std::endl;
                }
            }
        }
        catch (...) {}
    }

private:
    static std::string GetCurrentDate() {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buffer[32];
        sprintf_s(buffer, sizeof(buffer), "%04d_%02d_%02d", st.wYear, st.wMonth, st.wDay);
        return buffer;
    }

    static std::string GetCurrentTime() {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buffer[32];
        sprintf_s(buffer, sizeof(buffer), "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
        return buffer;
    }

    static std::string Redact(const std::string& text) {
        if (text.empty()) return text;

        std::string result = text;
        std::vector<std::string> fields = { "session_id", "owner_id", "secret", "password", "key", "license_key", "hash" };

        for (const auto& field : fields) {
            std::string pattern = "\"" + field + "\":\"[^\"]*\"";
            size_t pos = 0;
            while ((pos = result.find(pattern, pos)) != std::string::npos) {
                result.replace(pos, pattern.length(), "\"" + field + "\":\"REDACTED\"");
                pos += field.length() + 13;
            }
        }

        return result;
    }
};

bool AuthlyLogger::Enabled = true;
std::string AuthlyLogger::AppName = "AuthlyX";

class AuthlyX {
private:
    std::string baseUrl = "https://authly.cc/api/v1";
    std::string sessionId;
    std::string ownerId;
    std::string appName;
    std::string version;
    std::string secret;
    std::string applicationHash;
    bool initialized = false;

public:
    struct Response {
        bool success = false;
        std::string message;
        std::string raw;
    };

    struct UserData {
        std::string username;
        std::string email;
        std::string licenseKey;
        std::string subscription;
        std::string expiryDate;
        std::string lastLogin;
        std::string hwid;
        std::string ipAddress;
        std::string registeredAt;
    };

    struct VariableData {
        std::string varKey;
        std::string varValue;
        std::string updatedAt;
    };

    struct UpdateData {
        bool available = false;
        std::string latestVersion;
        std::string downloadUrl;
        bool forceUpdate = false;
        std::string changelog;
    };

    struct ChatMessage {
        int id = 0;
        std::string username;
        std::string message;
        std::string createdAt;
    };

    struct ChatMessages {
        std::string channelName;
        std::vector<ChatMessage> messages;
        int count = 0;
    };

    Response response;
    UserData userData;
    VariableData variableData;
    UpdateData updateData;
    ChatMessages chatMessages;

    AuthlyX(const std::string& ownerId, const std::string& appName,
        const std::string& version, const std::string& secret)
        : ownerId(ownerId), appName(appName), version(version), secret(secret) {

        if (ownerId.empty() || appName.empty() || version.empty() || secret.empty()) {
            Error("Invalid application credentials provided.");
            TerminateProcess(GetCurrentProcess(), 1);
            return;
        }

        AuthlyLogger::AppName = appName;
        CalculateApplicationHash();
    }

    bool Init() {
        std::map<std::string, std::string> payload = {
            {"owner_id", ownerId},
            {"app_name", appName},
            {"version", version},
            {"secret", secret},
            {"hash", applicationHash}
        };

        std::string responseStr = PostJson("init", BuildJson(payload));

        if (responseStr.empty()) {
            response.message = "Connection failed - check internet connection";
            Error("Initialization failed: " + response.message);
            return false;
        }

       std::string errorCode = ExtractJsonValue(responseStr, "code");
        if (errorCode == "INVALID_VERSION") {
            bool autoUpdateEnabled = false;
            size_t autoUpdatePos = responseStr.find("\"auto_update_enabled\":");
            std::string downloadUrl = ExtractJsonValue(responseStr, "auto_update_download_url");
            
            if (autoUpdatePos != std::string::npos) {
                size_t valueStart = responseStr.find(':', autoUpdatePos) + 1;
                std::string valueStr = responseStr.substr(valueStart, 10);
                if (valueStr.find("true") != std::string::npos) {
                    autoUpdateEnabled = true;
                }
            } else {
                size_t updatePos = responseStr.find("\"update\":");
                if (updatePos != std::string::npos) {
                    size_t downloadUrlPos = responseStr.find("\"download_url\":", updatePos);
                    if (downloadUrlPos != std::string::npos) {
                        std::string updateDownloadUrl = ExtractJsonValue(responseStr.substr(updatePos), "download_url");
                        if (!updateDownloadUrl.empty()) {
                            autoUpdateEnabled = true;
                            downloadUrl = updateDownloadUrl;
                            AuthlyLogger::Log("[AUTHLY_DEBUG] auto_update_enabled not found, but update object with download_url exists - assuming enabled");
                        }
                    }
                }
            }
            
            std::string errorMessage = ExtractJsonValue(responseStr, "message");
            if (errorMessage.empty()) {
                errorMessage = "Outdated Version";
            }
            
            AuthlyLogger::Log("[AUTHLY] Version is outdated: " + errorMessage);
            AuthlyLogger::Log("[AUTHLY] auto_update_enabled: " + std::string(autoUpdateEnabled ? "true" : "false") + ", download_url: " + downloadUrl);
            
            if (!autoUpdateEnabled) {
                AuthlyLogger::Log("[AUTHLY] Auto-update is disabled. Application will close in 5 seconds...");
                Sleep(5000);
                ExitProcess(1);
                return false;
            } else {
                std::cout << "\n1. Download Latest" << std::endl;
                std::cout << "2. Exit" << std::endl;
                std::cout << "\nSelect an option (1 or 2): ";
                
                std::string choice;
                std::getline(std::cin, choice);
                
                if (choice == "1" && !downloadUrl.empty()) {
                    std::wstring wideUrl(downloadUrl.begin(), downloadUrl.end());
                    ShellExecuteW(NULL, L"open", wideUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    AuthlyLogger::Log("[AUTHLY] Opening download URL in browser...");
                }
                
                ExitProcess(1);
                return false;
            }
        }

        ParseResponse(responseStr);

        if (response.success) {
            sessionId = ExtractJsonValue(responseStr, "session_id");
            initialized = true;
            AuthlyLogger::Log("[INIT] Successfully initialized AuthlyX session");
            
            LoadUpdateData(responseStr);
            
            if (updateData.available && updateData.forceUpdate) {
                AuthlyLogger::Log("[UPDATE] Force update required. Opening download URL: " + updateData.downloadUrl);
                if (!updateData.downloadUrl.empty()) {
                    OpenDownloadUrl();
                }
                Error("Update required. Please install version " + updateData.latestVersion + " and restart the application.");
                TerminateProcess(GetCurrentProcess(), 1);
                return false;
            }
        }
        else {
            size_t updatePos = responseStr.find("\"update\":");
            if (updatePos != std::string::npos) {
                LoadUpdateData(responseStr);
                if (updateData.available && updateData.forceUpdate) {
                    AuthlyLogger::Log("[UPDATE] Force update required. Opening download URL: " + updateData.downloadUrl);
                    if (!updateData.downloadUrl.empty()) {
                        OpenDownloadUrl();
                    }
                }
            }
            
            Error("Initialization failed: " + response.message);
        }

        return response.success;
    }

    void CheckInit() {
        if (!initialized) {
            Error("You must Initialize AuthlyX first");
            TerminateProcess(GetCurrentProcess(), 1);
        }
    }

    bool Login(const std::string& username, const std::string& password) {
        CheckInit();

        std::map<std::string, std::string> payload = {
            {"session_id", sessionId},
            {"username", username},
            {"password", password},
            {"hwid", GetSystemSid()},
            {"ip", GetPublicIp()}
        };

        std::string responseStr = PostJson("login", BuildJson(payload));
        ParseResponse(responseStr);

        return response.success;
    }

    bool Register(const std::string& username, const std::string& password,
        const std::string& key, const std::string& email = "") {
        CheckInit();

        std::map<std::string, std::string> payload = {
            {"session_id", sessionId},
            {"username", username},
            {"password", password},
            {"key", key},
            {"email", email},
            {"hwid", GetSystemSid()}
        };

        std::string responseStr = PostJson("register", BuildJson(payload));
        ParseResponse(responseStr);

        return response.success;
    }

    bool LicenseLogin(const std::string& licenseKey) {
        CheckInit();

        std::map<std::string, std::string> payload = {
            {"session_id", sessionId},
            {"license_key", licenseKey},
            {"hwid", GetSystemSid()},
            {"ip", GetPublicIp()}
        };

        std::string responseStr = PostJson("licenses", BuildJson(payload));
        ParseResponse(responseStr);

        return response.success;
    }

    std::string GetVariable(const std::string& varKey) {
        CheckInit();

        std::map<std::string, std::string> payload = {
            {"session_id", sessionId},
            {"var_key", varKey}
        };

        std::string responseStr = PostJson("variables", BuildJson(payload));
        ParseResponse(responseStr);

        return variableData.varValue;
    }

    bool SetVariable(const std::string& varKey, const std::string& varValue) {
        CheckInit();

        std::map<std::string, std::string> payload = {
            {"session_id", sessionId},
            {"var_key", varKey},
            {"var_value", varValue}
        };

        std::string responseStr = PostJson("variables/set", BuildJson(payload));
        ParseResponse(responseStr);

        return response.success;
    }

    bool Log(const std::string& message) {
        CheckInit();

        std::map<std::string, std::string> payload = {
            {"session_id", sessionId},
            {"message", message}
        };

        std::string responseStr = PostJson("logs", BuildJson(payload));
        ParseResponse(responseStr);

        return response.success;
    }

    bool ValidateSession() {
        if (!initialized || sessionId.empty()) {
            AuthlyLogger::Log("[VALIDATE_SESSION] Not initialized or session ID is empty");
            return false;
        }

        try {
            AuthlyLogger::Log("[VALIDATE_SESSION] Validating session: " + sessionId);

            std::map<std::string, std::string> payload = {
                {"session_id", sessionId}
            };

            std::string responseStr = PostJson("validate-session", BuildJson(payload));
            ParseResponse(responseStr);

            bool isValid = response.success && (response.message.find("valid") != std::string::npos || response.message.find("Valid") != std::string::npos);
            AuthlyLogger::Log("[VALIDATE_SESSION] Result: " + std::string(isValid ? "true" : "false") + ", Success: " + std::string(response.success ? "true" : "false") + ", Message: " + response.message);

            return isValid;
        }
        catch (...) {
            AuthlyLogger::Log("[VALIDATE_SESSION] Exception occurred");
            return false;
        }
    }

    std::string GetSessionId() const { return sessionId; }
    std::string GetCurrentApplicationHash() const { return applicationHash; }
    bool IsInitialized() const { return initialized; }
    std::string GetAppName() const { return appName; }

    bool IsUpdateAvailable() const { return updateData.available; }
    UpdateData GetUpdateInfo() const { return updateData; }

    std::string GetChats(const std::string& channelName)
    {
        return "No chats in channel: " + channelName;
    }

    void SendChat(const std::string& message)
    {
        SendChat(message, "global");
    }

    void SendChat(const std::string& message, const std::string& channelName)
    {
        std::cout << "[" << channelName << "] " << message << std::endl;
    }


private:
    std::string BuildJson(const std::map<std::string, std::string>& data) {
        std::string json = "{";
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it != data.begin()) json += ",";
            json += "\"" + it->first + "\":\"" + EscapeJsonString(it->second) + "\"";
        }
        json += "}";
        return json;
    }

    std::string EscapeJsonString(const std::string& input) {
        std::string output;
        for (char c : input) {
            switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c; break;
            }
        }
        return output;
    }

    std::string PostJson(const std::string& endpoint, const std::string& jsonPayload) {
        HINTERNET hSession = WinHttpOpen(L"AuthlyX", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) {
            response.success = false;
            response.message = "Failed to create HTTP session";
            return "";
        }

        HINTERNET hConnect = WinHttpConnect(hSession, L"authly.cc", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            response.success = false;
            response.message = "Failed to connect to server";
            return "";
        }

        std::wstring wideEndpoint = std::wstring(endpoint.begin(), endpoint.end());
        wideEndpoint = L"/api/v1/" + wideEndpoint;

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wideEndpoint.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            response.success = false;
            response.message = "Failed to create HTTP request";
            return "";
        }

        std::wstring headers = L"Content-Type: application/json\r\n";
        BOOL bResults = WinHttpSendRequest(hRequest, headers.c_str(), (DWORD)headers.length(),
            (LPVOID)jsonPayload.c_str(), (DWORD)jsonPayload.length(),
            (DWORD)jsonPayload.length(), 0);

        if (!bResults) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            response.success = false;
            response.message = "Failed to send HTTP request";
            return "";
        }

        bResults = WinHttpReceiveResponse(hRequest, NULL);
        if (!bResults) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            response.success = false;
            response.message = "Failed to receive HTTP response";
            return "";
        }

        std::string responseStr;
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;

        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize) || !dwSize) break;

            std::vector<char> buffer(dwSize + 1);
            if (WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                responseStr.append(buffer.data(), dwDownloaded);
            }
        } while (dwSize > 0);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        return responseStr;
    }

    void CalculateApplicationHash() {
        try {
            char modulePath[MAX_PATH];
            if (GetModuleFileNameA(NULL, modulePath, MAX_PATH) == 0) {
                applicationHash = "UNKNOWN_HASH";
                return;
            }

            HCRYPTPROV hProv = 0;
            HCRYPTHASH hHash = 0;

            if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
                applicationHash = "UNKNOWN_HASH";
                return;
            }

            if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
                CryptReleaseContext(hProv, 0);
                applicationHash = "UNKNOWN_HASH";
                return;
            }

            std::ifstream file(modulePath, std::ios::binary);
            if (!file) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                applicationHash = "UNKNOWN_HASH";
                return;
            }

            const size_t bufferSize = 8192;
            char buffer[bufferSize];

            while (file.read(buffer, bufferSize) || file.gcount() > 0) {
                if (!CryptHashData(hHash, (BYTE*)buffer, (DWORD)file.gcount(), 0)) {
                    CryptDestroyHash(hHash);
                    CryptReleaseContext(hProv, 0);
                    applicationHash = "UNKNOWN_HASH";
                    return;
                }
            }

            DWORD hashSize = 0;
            DWORD dwCount = sizeof(DWORD);
            if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&hashSize, &dwCount, 0)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                applicationHash = "UNKNOWN_HASH";
                return;
            }

            std::vector<BYTE> hashBytes(hashSize);
            if (!CryptGetHashParam(hHash, HP_HASHVAL, hashBytes.data(), &hashSize, 0)) {
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                applicationHash = "UNKNOWN_HASH";
                return;
            }

            std::stringstream ss;
            for (DWORD i = 0; i < hashSize; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)hashBytes[i];
            }
            applicationHash = ss.str();

            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);

            AuthlyLogger::Log("[HASH] Calculated application hash: " + applicationHash.substr(0, 16) + "...");
        }
        catch (...) {
            applicationHash = "UNKNOWN_HASH";
            AuthlyLogger::Log("[HASH_ERROR] Failed to calculate hash");
        }
    }

    std::string GetSystemSid() {
        HANDLE hToken = NULL;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            return "UNKNOWN_SID";
        }

        DWORD dwSize = 0;
        GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
        if (dwSize == 0) {
            CloseHandle(hToken);
            return "UNKNOWN_SID";
        }

        PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwSize);
        if (!pTokenUser) {
            CloseHandle(hToken);
            return "UNKNOWN_SID";
        }

        std::string sid = "UNKNOWN_SID";
        if (GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize)) {
            LPSTR sidStr = NULL;
            if (ConvertSidToStringSidA(pTokenUser->User.Sid, &sidStr)) {
                sid = sidStr;
                LocalFree(sidStr);
            }
        }

        free(pTokenUser);
        CloseHandle(hToken);
        return sid;
    }

    std::string GetPublicIp() {
        HINTERNET hSession = WinHttpOpen(L"AuthlyX/IPCheck", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) {
            return "UNKNOWN_IP";
        }

        HINTERNET hConnect = WinHttpConnect(hSession, L"api.ipify.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "UNKNOWN_IP";
        }

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "UNKNOWN_IP";
        }

        BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        if (!bResults) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "UNKNOWN_IP";
        }

        bResults = WinHttpReceiveResponse(hRequest, NULL);
        if (!bResults) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "UNKNOWN_IP";
        }

        std::string publicIp;
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;

        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize) || !dwSize) break;

            std::vector<char> buffer(dwSize + 1);
            if (WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                publicIp.append(buffer.data(), dwDownloaded);
            }
        } while (dwSize > 0);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        publicIp.erase(std::remove(publicIp.begin(), publicIp.end(), '\r'), publicIp.end());
        publicIp.erase(std::remove(publicIp.begin(), publicIp.end(), '\n'), publicIp.end());
        publicIp.erase(std::remove(publicIp.begin(), publicIp.end(), ' '), publicIp.end());

        if (publicIp.empty() || publicIp.find('.') == std::string::npos) {
            return "UNKNOWN_IP";
        }

        return publicIp;
    }

    std::string ExtractJsonValue(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\":\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) {
            searchKey = "\"" + key + "\":";
            pos = json.find(searchKey);
            if (pos == std::string::npos) return "";
        }

        size_t start = pos + searchKey.length();
        size_t end = json.find('"', start);
        if (end == std::string::npos) return "";

        return json.substr(start, end - start);
    }

    void ParseResponse(const std::string& jsonResponse) {
        response.raw = jsonResponse;

        if (jsonResponse.empty()) {
            response.success = false;
            response.message = "Empty response from server";
            return;
        }

        if (jsonResponse.find("<!DOCTYPE html>") != std::string::npos ||
            jsonResponse.find("<html>") != std::string::npos) {
            response.success = false;
            response.message = "Server error - please try again later";
            return;
        }

        size_t successPos = jsonResponse.find("\"success\":");
        if (successPos != std::string::npos) {
            size_t valueStart = jsonResponse.find_first_of("tf", successPos);
            if (valueStart != std::string::npos) {
                std::string successStr = jsonResponse.substr(valueStart, 4);
                response.success = (successStr.find("true") != std::string::npos);
            }
        }

        size_t msgPos = jsonResponse.find("\"message\":");
        if (msgPos != std::string::npos) {
            size_t msgStart = jsonResponse.find('"', msgPos + 10) + 1;
            size_t msgEnd = jsonResponse.find('"', msgStart);
            if (msgStart != std::string::npos && msgEnd != std::string::npos) {
                response.message = jsonResponse.substr(msgStart, msgEnd - msgStart);
            }
        }

        if (response.message.empty()) {
            response.message = response.success ? "Success" : "Unknown error";
        }

        LoadUserData(jsonResponse);
        LoadVariableData(jsonResponse);
        LoadUpdateData(jsonResponse);
        LoadChatData(jsonResponse);
    }

    void LoadUserData(const std::string& jsonResponse) {
        userData.username = ExtractJsonValue(jsonResponse, "username");
        userData.email = ExtractJsonValue(jsonResponse, "email");
        userData.licenseKey = ExtractJsonValue(jsonResponse, "license_key");
        userData.subscription = ExtractJsonValue(jsonResponse, "subscription");
        userData.expiryDate = ExtractJsonValue(jsonResponse, "expiry_date");
        userData.lastLogin = ExtractJsonValue(jsonResponse, "last_login");
        userData.registeredAt = ExtractJsonValue(jsonResponse, "created_at");
        userData.hwid = GetSystemSid();
        userData.ipAddress = GetPublicIp();
    }

    void LoadVariableData(const std::string& jsonResponse) {
        variableData.varKey = ExtractJsonValue(jsonResponse, "var_key");
        variableData.varValue = ExtractJsonValue(jsonResponse, "var_value");
        variableData.updatedAt = ExtractJsonValue(jsonResponse, "updated_at");
    }

    void LoadUpdateData(const std::string& jsonResponse) {
        try {
            size_t updateStart = jsonResponse.find("\"update\":{");
            if (updateStart == std::string::npos) {
                updateData.available = false;
                updateData.latestVersion = "";
                updateData.downloadUrl = "";
                updateData.forceUpdate = false;
                updateData.changelog = "";
                return;
            }

            size_t braceCount = 0;
            size_t updateObjStart = jsonResponse.find('{', updateStart);
            size_t updateObjEnd = updateObjStart;
            
            for (size_t i = updateObjStart; i < jsonResponse.length(); i++) {
                if (jsonResponse[i] == '{') braceCount++;
                if (jsonResponse[i] == '}') braceCount--;
                if (braceCount == 0) {
                    updateObjEnd = i + 1;
                    break;
                }
            }

            std::string updateObj = jsonResponse.substr(updateObjStart, updateObjEnd - updateObjStart);
            
            updateData.available = (ExtractJsonValue(updateObj, "available") == "true");
            updateData.latestVersion = ExtractJsonValue(updateObj, "latest_version");
            updateData.downloadUrl = ExtractJsonValue(updateObj, "download_url");
            updateData.forceUpdate = (ExtractJsonValue(updateObj, "force_update") == "true");
            updateData.changelog = ExtractJsonValue(updateObj, "changelog");
            
            if (updateData.available) {
                AuthlyLogger::Log("[UPDATE] Update available: " + updateData.latestVersion + ", Force: " + (updateData.forceUpdate ? "true" : "false"));
            }
        }
        catch (...) {
            AuthlyLogger::Log("[UPDATE_ERROR] Failed to load update data");
        }
    }

    void LoadChatData(const std::string& jsonResponse) {
        try {
            size_t dataStart = jsonResponse.find("\"data\":{");
            if (dataStart == std::string::npos) {
                chatMessages.channelName = "";
                chatMessages.messages.clear();
                chatMessages.count = 0;
                return;
            }

            chatMessages.channelName = ExtractJsonValue(jsonResponse, "channel_name");
            
            size_t messagesStart = jsonResponse.find("\"messages\":[", dataStart);
            if (messagesStart == std::string::npos) {
                chatMessages.messages.clear();
                chatMessages.count = 0;
                return;
            }

            chatMessages.messages.clear();
            
            size_t bracketCount = 0;
            size_t arrayStart = jsonResponse.find('[', messagesStart);
            size_t pos = arrayStart + 1;
            
            while (pos < jsonResponse.length()) {
                if (jsonResponse[pos] == '[') bracketCount++;
                if (jsonResponse[pos] == ']') {
                    if (bracketCount == 0) break;
                    bracketCount--;
                }
                if (jsonResponse[pos] == '{' && bracketCount == 0) {
                    size_t objEnd = pos;
                    int objBraceCount = 0;
                    for (size_t i = pos; i < jsonResponse.length(); i++) {
                        if (jsonResponse[i] == '{') objBraceCount++;
                        if (jsonResponse[i] == '}') {
                            objBraceCount--;
                            if (objBraceCount == 0) {
                                objEnd = i + 1;
                                break;
                            }
                        }
                    }
                    
                    std::string msgObj = jsonResponse.substr(pos, objEnd - pos);
                    ChatMessage msg;
                    msg.id = std::stoi(ExtractJsonValue(msgObj, "id"));
                    msg.username = ExtractJsonValue(msgObj, "username");
                    msg.message = ExtractJsonValue(msgObj, "message");
                    msg.createdAt = ExtractJsonValue(msgObj, "created_at");
                    chatMessages.messages.push_back(msg);
                    
                    pos = objEnd;
                } else {
                    pos++;
                }
            }
            
            chatMessages.count = chatMessages.messages.size();
        }
        catch (...) {
            AuthlyLogger::Log("[CHAT_DATA_ERROR] Failed to load chat data");
            chatMessages.messages.clear();
            chatMessages.count = 0;
        }
    }

    void OpenDownloadUrl() {
        if (!updateData.downloadUrl.empty()) {
            std::wstring wideUrl(updateData.downloadUrl.begin(), updateData.downloadUrl.end());
            ShellExecuteW(NULL, L"open", wideUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
    }

    void Error(const std::string& message) {
        AuthlyLogger::Log("[ERROR] " + message);

        std::string cmd = "cmd.exe /c start cmd /C \"color 4 && title AuthlyX Error && echo " +
            message + " && timeout /t 5\"";

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

        if (pi.hProcess) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
};