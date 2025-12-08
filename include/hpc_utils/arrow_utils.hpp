#pragma once

#include <arrow/io/file.h>
#include <cstdint>
#include <fmt/format.h>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <memory>
#include <parquet/arrow/reader.h>

namespace hpc_utils {

template <typename Type>
Type _arrow_check(arrow::Result<Type>&& maybe_result, const char* file, int line) {
    if (!maybe_result.ok()) [[unlikely]] {
        throw std::runtime_error(
            fmt::format("Arrow operation failed {}:{}: {}", file, line, maybe_result.status().ToString()));
    }

    return std::move(maybe_result).ValueOrDie();
}

static inline void _arrow_check(arrow::Status&& status, const char* file, int line) {
    if (!status.ok()) [[unlikely]] {
        throw std::runtime_error(fmt::format("Arrow operation failed {}:{}: {}", file, line, status.ToString()));
    }
}

#define arrow_check(maybe_result_or_status) _arrow_check(maybe_result_or_status, __FILE__, __LINE__)

template <typename Type>
arrow::Type::type arrow_type_id() = delete;

template <>
inline arrow::Type::type arrow_type_id<std::int8_t>() {
    return arrow::Type::INT8;
};
template <>
inline arrow::Type::type arrow_type_id<std::int16_t>() {
    return arrow::Type::INT16;
};
template <>
inline arrow::Type::type arrow_type_id<std::int32_t>() {
    return arrow::Type::INT32;
};
template <>
inline arrow::Type::type arrow_type_id<std::int64_t>() {
    return arrow::Type::INT64;
};

template <>
inline arrow::Type::type arrow_type_id<std::uint8_t>() {
    return arrow::Type::UINT8;
};
template <>
inline arrow::Type::type arrow_type_id<std::uint16_t>() {
    return arrow::Type::UINT16;
};
template <>
inline arrow::Type::type arrow_type_id<std::uint32_t>() {
    return arrow::Type::UINT32;
};
template <>
inline arrow::Type::type arrow_type_id<std::uint64_t>() {
    return arrow::Type::UINT64;
};

template <>
inline arrow::Type::type arrow_type_id<float>() {
    return arrow::Type::FLOAT;
};
template <>
inline arrow::Type::type arrow_type_id<double>() {
    return arrow::Type::DOUBLE;
};

template <typename Type>
struct arrow_array_type {};

template <>
struct arrow_array_type<std::int8_t> {
    using type = arrow::Int8Array;
};

template <>
struct arrow_array_type<std::int16_t> {
    using type = arrow::Int16Array;
};

template <>
struct arrow_array_type<std::int32_t> {
    using type = arrow::Int32Array;
};

template <>
struct arrow_array_type<std::int64_t> {
    using type = arrow::Int64Array;
};

template <>
struct arrow_array_type<std::uint8_t> {
    using type = arrow::UInt8Array;
};

template <>
struct arrow_array_type<std::uint16_t> {
    using type = arrow::UInt16Array;
};

template <>
struct arrow_array_type<std::uint32_t> {
    using type = arrow::UInt32Array;
};

template <>
struct arrow_array_type<std::uint64_t> {
    using type = arrow::UInt64Array;
};

template <>
struct arrow_array_type<float> {
    using type = arrow::FloatArray;
};

template <>
struct arrow_array_type<double> {
    using type = arrow::DoubleArray;
};

template <typename Type>
using arrow_array_type_v = arrow_array_type<Type>::type;

static inline std::shared_ptr<arrow::io::ReadableFile> arrow_open_file(const std::string& file_name) {
    return arrow_check(arrow::io::ReadableFile::Open(file_name));
}

static inline std::unique_ptr<parquet::arrow::FileReader>
make_parquet_reader(const std::shared_ptr<arrow::io::ReadableFile>& file) {
    return arrow_check(parquet::arrow::OpenFile(file, arrow::default_memory_pool()));
}

static inline std::shared_ptr<arrow::Schema>
get_parquet_schema(const std::unique_ptr<parquet::arrow::FileReader>& reader) {
    std::shared_ptr<arrow::Schema> schema;
    arrow_check(reader->GetSchema(&schema));
    return schema;
}

template <typename Type>
std::vector<Type> parquet_read_col(const std::unique_ptr<parquet::arrow::FileReader>& reader,
                                   const std::shared_ptr<arrow::Schema>& schema, const std::string_view& col_name) {
    std::vector<Type> data;

    auto col_index = schema->GetFieldIndex(col_name);
    if (col_index < 0) [[unlikely]] {
        // panic("'{}' column not found", col_name);
        throw std::runtime_error(fmt::format("Column {} not found; schema=\n{}.", col_name, schema->ToString()));
    }
    auto field = schema->field(col_index);
    if (field->type()->id() != arrow_type_id<Type>()) [[unlikely]] {
        throw std::runtime_error(
            fmt::format("Column {} not of type {}; schema=\n{}.", col_name, typeid(Type).name(), schema->ToString()));
    }

    for (int rg = 0; rg < reader->num_row_groups(); rg++) {
        auto row_group = reader->RowGroup(rg);
        std::shared_ptr<arrow::ChunkedArray> column;
        arrow_check(row_group->Column(col_index)->Read(&column));
        for (int c = 0; c < column->num_chunks(); c++) {
            auto chunk = std::dynamic_pointer_cast<arrow_array_type_v<Type>>(column->chunk(c));
            for (int i = 0; i < chunk->length(); i++) {
                auto o = chunk->Value(i);
                data.push_back(o);
            }
        }
    }

    return data;
}

} // namespace hpc_utils
