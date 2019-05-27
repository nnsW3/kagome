/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libp2p/muxer/yamux/yamux_frame.hpp"

#include <gtest/gtest.h>
#include "testutil/literals.hpp"

using namespace libp2p::muxer;
using namespace kagome::common;

class YamuxFrameTest : public ::testing::Test {
 public:
  static constexpr size_t data_length = 6;
  static constexpr Yamux::StreamId default_stream_id = 1;
  static constexpr uint32_t default_ping_value = 337;

  Buffer data{"1234456789AB"_unhex};

  /**
   * Check that all frame's fields are as expected
   */
  void checkFrame(std::optional<YamuxFrame> frame_opt, uint8_t version,
                  YamuxFrame::FrameType type, YamuxFrame::Flag flag,
                  Yamux::StreamId stream_id, uint32_t length,
                  const Buffer &frame_data) {
    ASSERT_TRUE(frame_opt);
    auto frame = *frame_opt;
    ASSERT_EQ(frame.version_, version);
    ASSERT_EQ(frame.type_, type);
    ASSERT_EQ(frame.flag_, flag);
    ASSERT_EQ(frame.stream_id_, stream_id);
    ASSERT_EQ(frame.length_, length);
    ASSERT_EQ(frame.data_, frame_data);
  }
};

/**
 * @given data message frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, ParseFrameSuccess) {
  Buffer data_frame_bytes = dataMsg(default_stream_id, data);
  auto frame_opt = parseFrame(data_frame_bytes.toVector());

  SCOPED_TRACE("ParseFrameSuccess");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::DATA, YamuxFrame::Flag::SYN,
             default_stream_id, data_length, data);
}

/**
 * @given invalid frame
 * @when parsed by YamuxFrame
 * @then the frame is not parsed
 */
TEST_F(YamuxFrameTest, ParseFrameFailure) {
  auto frame_opt = parseFrame(data.toVector());

  ASSERT_FALSE(frame_opt);
}

/**
 * @given new stream frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, NewStreamMsg) {
  auto frame_bytes = newStreamMsg(default_stream_id);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("NewStreamMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::DATA, YamuxFrame::Flag::SYN,
             default_stream_id, 0, Buffer{});
}

/**
 * @given ack stream frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, AckStreamMsg) {
  auto frame_bytes = ackStreamMsg(default_stream_id);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("AckStreamMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::DATA, YamuxFrame::Flag::ACK,
             default_stream_id, 0, Buffer{});
}

/**
 * @given close stream frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, CloseStreamMsg) {
  auto frame_bytes = closeStreamMsg(default_stream_id);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("CloseStreamMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::DATA, YamuxFrame::Flag::FIN,
             default_stream_id, 0, Buffer{});
}

/**
 * @given reset frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, ResetStreamMsg) {
  auto frame_bytes = resetStreamMsg(default_stream_id);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("ResetStreamMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::DATA, YamuxFrame::Flag::RST,
             default_stream_id, 0, Buffer{});
}

/**
 * @given ping out frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, PingOutMsg) {
  auto frame_bytes = pingOutMsg(default_ping_value);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("PingOutMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::PING, YamuxFrame::Flag::SYN, 0,
             default_ping_value, Buffer{});
}

/**
 * @given ping response frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, PingResponseMsg) {
  auto frame_bytes = pingResponseMsg(default_ping_value);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("PingResponseMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::PING, YamuxFrame::Flag::ACK, 0,
             default_ping_value, Buffer{});
}

/**
 * @given go away frame
 * @when parsed by YamuxFrame
 * @then the frame is parsed successfully
 */
TEST_F(YamuxFrameTest, GoAwayMsg) {
  auto frame_bytes = goAwayMsg(YamuxFrame::GoAwayError::PROTOCOL_ERROR);
  auto frame_opt = parseFrame(frame_bytes.toVector());

  SCOPED_TRACE("GoAwayMsg");
  checkFrame(frame_opt, YamuxFrame::kDefaultVersion,
             YamuxFrame::FrameType::GO_AWAY, YamuxFrame::Flag::SYN, 0,
             static_cast<uint32_t>(YamuxFrame::GoAwayError::PROTOCOL_ERROR),
             Buffer{});
}