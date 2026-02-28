#pragma once

#include "IDataReader.hpp"
#include <memory>

namespace PubSub {

class Publisher;

class CsvDataReader : public IDataReader {
public:

    explicit CsvDataReader(const Publisher& publisher);
    ~CsvDataReader() override;
    CsvDataReader(const CsvDataReader&) = delete;
    CsvDataReader(CsvDataReader&&) = default;
    CsvDataReader& operator= (const CsvDataReader&) = delete;
    CsvDataReader& operator= (CsvDataReader&&) noexcept = default;

    void stop();
    bool isRunning() const;
    void startReadingData() override;

private:
    void runner();

private:
    struct PimplData;
    std::unique_ptr<PimplData> m_pimpl;
};

}
