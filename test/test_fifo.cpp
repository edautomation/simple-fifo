#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>  // For memset

#include "byte_fifo.h"

static constexpr uint32_t kFifoSize = 10;

class ByteFifoTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        buffer_ = new uint8_t[kFifoSize];
        fifo_ = new byte_fifo_t{buffer_, kFifoSize, 0, 0, 0};
        byte_fifo_init(fifo_);
    }

    void TearDown() override
    {
        delete[] buffer_;
        delete fifo_;
    }

    uint8_t* buffer_;
    byte_fifo_t* fifo_;
};

TEST_F(ByteFifoTest, Init)
{
    fifo_->n_elements = 42;
    fifo_->read_index = 42;
    fifo_->write_index = 42;
    EXPECT_EQ(byte_fifo_init(fifo_), 0);
    EXPECT_EQ(fifo_->n_elements, 0);
    EXPECT_EQ(fifo_->write_index, 0);
    EXPECT_EQ(fifo_->read_index, 0);
}

TEST_F(ByteFifoTest, InitNull)
{
    EXPECT_EQ(byte_fifo_init(nullptr), BF_ERR_NULLPTR);
}

TEST_F(ByteFifoTest, InitNullData)
{
    byte_fifo_t null_data_fifo = {nullptr, kFifoSize, 0, 0, 0};
    EXPECT_EQ(byte_fifo_init(&null_data_fifo), BF_ERR_NULLPTR);
}

TEST_F(ByteFifoTest, InitZeroSize)
{
    byte_fifo_t zero_size_fifo = {buffer_, 0, 0, 0, 0};
    EXPECT_EQ(byte_fifo_init(&zero_size_fifo), BF_ERR_INVAL);
}

TEST_F(ByteFifoTest, InitReset)
{
    memset(buffer_, 42, kFifoSize);
    EXPECT_EQ(byte_fifo_init(fifo_), 0);
    for (auto i = 0; i < kFifoSize; i++)
    {
        EXPECT_EQ(buffer_[i], 0);
    }
}

TEST_F(ByteFifoTest, IsEmpty)
{
    EXPECT_EQ(byte_fifo_is_empty(nullptr), BF_ERR_NULLPTR);
    EXPECT_EQ(byte_fifo_is_empty(fifo_), 1);
    uint8_t data[] = {1};
    byte_fifo_write(fifo_, data, 1);
    EXPECT_EQ(byte_fifo_is_empty(fifo_), 0);
}

TEST_F(ByteFifoTest, IsFull)
{
    EXPECT_EQ(byte_fifo_is_full(nullptr), BF_ERR_NULLPTR);
    uint8_t data[kFifoSize];
    memset(data, 1, kFifoSize);
    EXPECT_EQ(byte_fifo_write(fifo_, data, kFifoSize), kFifoSize);
    EXPECT_EQ(byte_fifo_is_full(fifo_), 1);
}

TEST_F(ByteFifoTest, WriteRead)
{
    uint8_t data[] = {1, 2, 3};
    EXPECT_EQ(byte_fifo_write(fifo_, data, 3), 3);

    uint8_t read_data[3];
    EXPECT_EQ(byte_fifo_read(fifo_, read_data, 3), 3);
    EXPECT_EQ(memcmp(data, read_data, 3), 0);
}

TEST_F(ByteFifoTest, Overwrite)
{
    uint8_t data1[kFifoSize];
    memset(data1, 1, kFifoSize);
    uint8_t data2[] = {2, 3};

    EXPECT_EQ(byte_fifo_write(fifo_, data1, kFifoSize), kFifoSize);
    EXPECT_EQ(byte_fifo_overwrite(fifo_, data2, 2), 2);

    uint8_t read_data[2];
    uint8_t expected_read_data[] = {1, 1};
    EXPECT_EQ(byte_fifo_read(fifo_, read_data, 2), 2);
    EXPECT_EQ(memcmp(expected_read_data, read_data, 2), 0);
}

TEST_F(ByteFifoTest, NullPointer)
{
    uint8_t data[] = {1, 2};
    EXPECT_EQ(byte_fifo_write(nullptr, data, 2), BF_ERR_NULLPTR);
    EXPECT_EQ(byte_fifo_write(fifo_, nullptr, 2), BF_ERR_NULLPTR);
    EXPECT_EQ(byte_fifo_read(nullptr, data, 2), BF_ERR_NULLPTR);
    EXPECT_EQ(byte_fifo_read(fifo_, nullptr, 2), BF_ERR_NULLPTR);
}

TEST_F(ByteFifoTest, CheckErrorCode)
{
    EXPECT_EQ(BF_ERR_OK, byte_fifo_get_error(0));
    EXPECT_EQ(BF_ERR_OK, byte_fifo_get_error(1));
    EXPECT_EQ(BF_ERR_OK, byte_fifo_get_error(42));
    EXPECT_EQ(BF_ERR_OK, byte_fifo_get_error(INT32_MAX));
    EXPECT_EQ(BF_ERR_OK, byte_fifo_get_error(INT32_MAX - 1));
    EXPECT_EQ(-1, byte_fifo_get_error(-1));
    EXPECT_EQ(-42, byte_fifo_get_error(-42));
    EXPECT_EQ(-INT32_MIN + 1, byte_fifo_get_error(-INT32_MIN + 1));
    EXPECT_EQ(-INT32_MIN, byte_fifo_get_error(-INT32_MIN));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
