#pragma once

namespace PubSub {

struct IDataReader
{
    IDataReader() = default;
    virtual ~IDataReader() = default;
    IDataReader(const IDataReader&) = delete;
    IDataReader(IDataReader&&) = default;
    IDataReader& operator= (const IDataReader&) = delete;
    IDataReader& operator= (IDataReader&&) noexcept = default;

    virtual void startReadingData() = 0;
};

}
