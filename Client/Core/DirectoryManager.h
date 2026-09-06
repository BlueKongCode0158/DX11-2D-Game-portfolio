#pragma once
#include "Common/Singleton.h"
#include <fstream>
#include <filesystem> //C++에서 제공하는 파일 경로, 파일에 대한 헤더
#include <unordered_map>

class DirectoryManager : public Singleton<DirectoryManager>
{
    DECLARE_SINGLETON(DirectoryManager);
private:
    //std::filesystem::path  경로
    
    //기본 경로
    std::filesystem::path _rootPath;
    
    //루트 패스의 부모 경로
    std::filesystem::path _rootParentPath;

    //자주 사용하는 경로들은 미리 캐싱해놓는다.
    std::unordered_map<std::string, std::filesystem::path> _paths;

private:
    void RegisterPath(const std::string& path);

public:
    void Init();
    virtual void Destroy() override;

    const std::filesystem::path& GetRootPath() const { return _rootPath; }

    //std::optional<T> : 있을수도 있고, 없을수도 있다.(값이)
    std::optional<std::filesystem::path> GetCachePath(const std::string& pathName) const;

public:
    bool GetDirectoryFromRoot(const std::string& dirName, OUT std::filesystem::path& outValue);
    bool GetDirectoryFromRoot(const std::filesystem::path& dir, OUT std::filesystem::path& outValue);
    bool GetDirectory(const std::filesystem::path& basePath, const std::filesystem::path& dirName, OUT std::filesystem::path& outValue);
    bool GetFile(const std::filesystem::path& basePath, const std::filesystem::path& dirName, OUT std::filesystem::path& outValue);
    bool IsFile(const std::filesystem::path& path);
    bool IsDirectory(const std::filesystem::path& path);
    bool IsExtension(const std::filesystem::path& path, const std::string& extension);

};

