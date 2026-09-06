#pragma once
// 에셋 바이너리 캐시용 저수준 read/write 헬퍼.
// JSON 파싱 비용을 없애기 위해 "이미 디코드된 중간 표현"을 .bin 으로 직렬화한다.
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdint>

namespace BinaryIO
{
    // POD 값 하나를 그대로 기록/복원한다.
    template <typename T>
    inline void WritePod(std::ofstream& os, const T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "WritePod requires trivially copyable type");
        os.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template <typename T>
    inline bool ReadPod(std::ifstream& is, T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "ReadPod requires trivially copyable type");
        is.read(reinterpret_cast<char*>(&value), sizeof(T));
        return static_cast<bool>(is);
    }

    // 길이(uint32) + 바이트 형태로 문자열을 기록/복원한다.
    inline void WriteStr(std::ofstream& os, const std::string& str)
    {
        const uint32_t len = static_cast<uint32_t>(str.size());
        WritePod(os, len);
        if (len > 0)
        {
            os.write(str.data(), static_cast<std::streamsize>(len));
        }
    }

    inline bool ReadStr(std::ifstream& is, std::string& out)
    {
        uint32_t len = 0;
        if (!ReadPod(is, len))
        {
            return false;
        }
        out.resize(len);
        if (len > 0)
        {
            is.read(out.data(), static_cast<std::streamsize>(len));
        }
        return static_cast<bool>(is);
    }

    // 캐시 무효화용 원본 파일 메타데이터(수정 시각 + 크기).
    struct SourceMeta
    {
        int64_t  writeTime = 0;
        uint64_t fileSize = 0;
    };

    // 원본 JSON 의 현재 메타데이터를 읽어온다. 실패 시 false.
    inline bool QuerySourceMeta(const std::filesystem::path& srcPath, SourceMeta& out)
    {
        std::error_code ec;
        const auto ftime = std::filesystem::last_write_time(srcPath, ec);
        if (ec)
        {
            return false;
        }
        const auto fsize = std::filesystem::file_size(srcPath, ec);
        if (ec)
        {
            return false;
        }
        out.writeTime = ftime.time_since_epoch().count();
        out.fileSize = static_cast<uint64_t>(fsize);
        return true;
    }
}
