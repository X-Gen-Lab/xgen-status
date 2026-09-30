/**
 * \file            test_status.cpp
 * \brief           Preserve status values and diagnostic string behavior
 */

#include <gtest/gtest.h>
#include <xgen/status/status.h>
#include <xgen/status/version.h>

#include <climits>
#include <ostream>

#if XGS_TEST_STRINGS
extern "C" const char* xgs_test_status_string_from_int(int status);
#endif

struct StatusCase {
    xgs_status_t status;
    int value;
    const char* text;
};

void PrintTo(const StatusCase& value, std::ostream* output) {
    *output << value.value;
}

class StatusValue : public ::testing::TestWithParam<StatusCase> {};

TEST_P(StatusValue, PreservesNumericContract) {
    EXPECT_EQ(GetParam().value, static_cast<int>(GetParam().status));
}

#if XGS_TEST_STRINGS
TEST_P(StatusValue, DescribesKnownStatus) {
    const char* text = xgs_status_string(GetParam().status);
    ASSERT_NE(nullptr, text);
    EXPECT_STREQ(GetParam().text, text);
}

TEST(StatusStrings, DescribesUnrecognizedCValues) {
    const int unknown_values[] = {INT_MIN, -100, -8, 1, 7, 100, INT_MAX};
    for (int value : unknown_values) {
        EXPECT_STREQ("Unknown status", xgs_test_status_string_from_int(value));
    }
}

TEST(StatusStrings, PreviouslyReturnedTextSurvivesLaterCalls) {
    const char* first = xgs_status_string(XGS_NO_MEMORY);
    EXPECT_STREQ("Busy", xgs_status_string(XGS_BUSY));
    EXPECT_STREQ("No memory", first);
}
#endif

INSTANTIATE_TEST_SUITE_P(
    DefinedStatuses, StatusValue,
    ::testing::Values(StatusCase{XGS_OK, 0, "OK"},
                      StatusCase{XGS_INVALID_ARGUMENT, -1, "Invalid argument"},
                      StatusCase{XGS_NO_MEMORY, -2, "No memory"},
                      StatusCase{XGS_CAPACITY, -3, "Capacity exhausted"},
                      StatusCase{XGS_NOT_FOUND, -4, "Not found"},
                      StatusCase{XGS_BUSY, -5, "Busy"},
                      StatusCase{XGS_UNSUPPORTED, -6, "Unsupported"},
                      StatusCase{XGS_ALREADY_EXISTS, -7, "Already exists"}));

TEST(StatusVersion, PublishesIndependentPackageIdentity) {
    EXPECT_EQ(0, XGS_VERSION_MAJOR);
    EXPECT_EQ(1, XGS_VERSION_MINOR);
    EXPECT_EQ(0, XGS_VERSION_PATCH);
    EXPECT_EQ(1, XGS_ABI_VERSION);
    EXPECT_STREQ("0.1.0", XGS_VERSION_STRING);
}
