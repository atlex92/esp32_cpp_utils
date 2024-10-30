
#include "unity.h"
#include "nvs.hpp"

/**
    Nvs class unit tests
 */
static constexpr const char* const kTagNvs{"nvs"};
static constexpr const char* const kNvsStorage{"test_nvs"};

TEST_CASE("check non-existing key", kTagNvs)
{
    Nvs nvs{kNvsStorage};
    TEST_ASSERT_FALSE(nvs.readI8("non_existing_i8"));
    TEST_ASSERT_FALSE(nvs.readI16("non_existing_i16"));
    TEST_ASSERT_FALSE(nvs.readI32("non_existing_i32"));
    TEST_ASSERT_FALSE(nvs.readI64("non_existing_i64"));

    TEST_ASSERT_FALSE(nvs.readU8("non_existing_ui8"));
    TEST_ASSERT_FALSE(nvs.readU16("non_existing_ui16"));
    TEST_ASSERT_FALSE(nvs.readU32("non_existing_ui32"));
    TEST_ASSERT_FALSE(nvs.readU64("non_existing_ui64"));

    TEST_ASSERT_FALSE(nvs.readString("non_existing_str"));
}

TEST_CASE("check key creation", kTagNvs)
{
    Nvs nvs{kNvsStorage};
    const int64_t test_value{1};
    TEST_ASSERT_TRUE(nvs.write("new_i8", static_cast<int8_t>(test_value)));
    TEST_ASSERT_TRUE(nvs.write("new_i16", static_cast<int16_t>(test_value)));
    TEST_ASSERT_TRUE(nvs.write("new_i32", static_cast<int32_t>(test_value)));
    TEST_ASSERT_TRUE(nvs.write("new_i64", static_cast<int64_t>(test_value)));

    TEST_ASSERT_TRUE(nvs.write("new_ui8", static_cast<uint8_t>(test_value)));
    TEST_ASSERT_TRUE(nvs.write("new_ui16", static_cast<uint16_t>(test_value)));
    TEST_ASSERT_TRUE(nvs.write("new_ui32", static_cast<uint32_t>(test_value)));
    TEST_ASSERT_TRUE(nvs.write("new_ui64", static_cast<uint64_t>(test_value)));

    TEST_ASSERT_TRUE(nvs.write("new_str", "test_value"));

    {
        auto read_res{nvs.readI8("new_i8")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {
        auto read_res{nvs.readI16("new_i16")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {
        auto read_res{nvs.readI32("new_i32")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {
        auto read_res{nvs.readI64("new_i64")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {
        auto read_res{nvs.readU8("new_ui8")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {
        auto read_res{nvs.readU16("new_ui16")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {
        auto read_res{nvs.readU32("new_ui32")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value()); 
    }

    {        
        auto read_res{nvs.readU64("new_ui64")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
    }

    {        
        auto read_res{nvs.readString("new_str")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL_STRING("test_value", read_res.value().c_str());
    }
}

TEST_CASE("check key erasing", kTagNvs)
{
    Nvs nvs{kNvsStorage};
    const int64_t test_value{1};

    {
        auto read_res{nvs.readI8("new_i8")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_i8"));
        read_res = nvs.readI8("new_i8");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readI16("new_i16")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_i16"));
        read_res = nvs.readI16("new_i16");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readI32("new_i32")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_i32"));
        read_res = nvs.readI32("new_i32");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readI64("new_i64")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_i64"));
        read_res = nvs.readI64("new_i64");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readU8("new_ui8")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_ui8"));
        read_res = nvs.readU8("new_ui8");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readU16("new_ui16")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_ui16"));
        read_res = nvs.readU16("new_ui16");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readU32("new_ui32")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_ui32"));
        read_res = nvs.readU32("new_ui32");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readU64("new_ui64")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL(test_value, read_res.value());
        TEST_ASSERT_TRUE(nvs.erase("new_ui64"));
        read_res = nvs.readU64("new_ui64");
        TEST_ASSERT_FALSE(read_res.has_value());
    }

    {
        auto read_res{nvs.readString("new_str")};
        TEST_ASSERT_TRUE(read_res.has_value());
        TEST_ASSERT_EQUAL_STRING("test_value", read_res.value().c_str());
        TEST_ASSERT_TRUE(nvs.erase("new_str"));
        read_res = nvs.readString("new_str");
        TEST_ASSERT_FALSE(read_res.has_value());
    }
}
