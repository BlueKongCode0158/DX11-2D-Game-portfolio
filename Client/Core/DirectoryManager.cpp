#include "pch.h"
#include "DirectoryManager.h"

void DirectoryManager::RegisterPath(const std::string& path)
{
    std::filesystem::path registeredPath = _rootParentPath / path;
    if (!std::filesystem::exists(registeredPath))
        std::filesystem::create_directories(registeredPath);

    if (!std::filesystem::is_directory(registeredPath))
        return;

    _paths[path] = registeredPath;
}

//std::filesystem::exists : 존재하는지 여부
//std::filesystem::is_directory : 폴더 인지 여부


void DirectoryManager::Init()
{
    //경로
    _rootPath = std::filesystem::current_path();
    if (!std::filesystem::exists(_rootPath))            //사실 굳이 필요 없긴 하다
        return;

    if (!std::filesystem::is_directory(_rootPath))      //사실 굳이 필요 없긴 하다.
        return;

    //부모경로 --> 이전 경로
    _rootParentPath = _rootPath.parent_path();
    if (!std::filesystem::exists(_rootParentPath))      //사실 굳이 필요 없긴 하다.
        return;

    if (!std::filesystem::is_directory(_rootParentPath))//사실 굳이 필요 없긴 하다.
        return;

    RegisterPath("Logs");
    RegisterPath("HLSL");
    RegisterPath("Resources");
    RegisterPath("Resources\\Level");
}

void DirectoryManager::Destroy()
{
    _paths.clear();
}

std::optional<std::filesystem::path> DirectoryManager::GetCachePath(const std::string& pathName) const
{
    auto it = _paths.find(pathName);
    if (it == _paths.end())
        return std::nullopt; //없다.

    return it->second;
}

bool DirectoryManager::GetDirectoryFromRoot(const std::string& dirName, OUT std::filesystem::path& outValue)
{
    std::filesystem::path foundPath = _rootPath / dirName;
    if (false == std::filesystem::exists(foundPath))
    {
        return false;
    }
    if (false == std::filesystem::is_directory(foundPath))
    {
        return false;
    }

    outValue = foundPath;
    return true;
}

bool DirectoryManager::GetDirectoryFromRoot(const std::filesystem::path& dir, OUT std::filesystem::path& outValue)
{
    std::filesystem::path foundPath = _rootPath / dir;
    if (false == std::filesystem::exists(foundPath))
    {
        return false;
    }

    if (false == std::filesystem::is_directory(foundPath))
    {
        return false;
    }

    outValue = foundPath;
    return true;
}

bool DirectoryManager::GetDirectory(const std::filesystem::path& basePath, const std::filesystem::path& dirName, OUT std::filesystem::path& outValue)
{
    std::filesystem::path foundPath = basePath / dirName;
    if (false == std::filesystem::exists(foundPath))
    {
        return false;
    }

    if (false == std::filesystem::is_directory(foundPath))
    {
        return false;
    }
    outValue = foundPath;
    return true;
}

bool DirectoryManager::GetFile(const std::filesystem::path& basePath, const std::filesystem::path& dirName, OUT std::filesystem::path& outValue)
{
    std::filesystem::path foundPath = basePath / dirName;
    outValue = foundPath;

    if (false == std::filesystem::exists(foundPath))
    {
        return false;
    }

    if (false == std::filesystem::is_regular_file(foundPath))
    {
        return false;
    }
    return true;
}

bool DirectoryManager::IsFile(const std::filesystem::path& path)
{
    if (false == std::filesystem::exists(path))
    {
        return false;
    }

    if (false == std::filesystem::is_regular_file(path))
    {
        return false;
    }
    return true;
}

bool DirectoryManager::IsDirectory(const std::filesystem::path& path)
{
    if (false == std::filesystem::exists(path))
    {
        return false;
    }
    if (false == std::filesystem::is_directory(path))
    {
        return false;
    }
    return true;
}

bool DirectoryManager::IsExtension(const std::filesystem::path& path, const std::string& extension)
{
    std::filesystem::path fileExtension = path.extension();
    std::string strExtension = fileExtension.string();
    
    std::string compareExtension = extension;
    std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), toupper);
    std::transform(compareExtension.begin(), compareExtension.end(), compareExtension.begin(), toupper);
   
    if (compareExtension == strExtension)
    {
        return true;
    }

    //if (compareExtension.compare(strExtension))
    //{
    //    return false;
    //}
    return false;
}
