#pragma once

#include <cstddef>
#include <vector>

namespace ros1 {

const char* const SHRUNK_IMAGE_MSGDEF = R"(Header header
uint32 height
uint32 width
string encoding
uint8 is_bigendian
uint32 step
uint8[] data

================================================================================
MSG: std_msgs/Header
uint32 seq
time stamp
string frame_id)";

struct Time {
  int32_t sec;
  uint32_t nsec;
};

struct Header {
  uint32_t seq;
  Time stamp;
  std::string frame_id;

  size_t sizeInBytes() const { return 4 + 8 + 4 + frame_id.size(); }
};

struct sensor_msgs__Image {
  Header header;
  uint32_t height;
  uint32_t width;
  std::string encoding;
  uint8_t is_bigendian;
  uint32_t step;
  std::vector<uint8_t> data;
};

struct foxglove_msgs__ShrunkImage {
  Header header;
  uint32_t height;
  uint32_t width;
  std::string encoding;
  uint8_t is_bigendian;
  uint32_t step;
  std::string format;
  std::vector<uint8_t> data;

  size_t sizeInBytes() const {
    return header.sizeInBytes() + 4 +  // height
           4 +                         // width
           4 + encoding.size() +       // encoding
           1 +                         // is_bigendian
           4 +                         // step
           4 + format.size() +         // format
           4 + data.size();            // data
  }
};

// Reading /////////////////////////////////////////////////////////////////////

int8_t ReadInt8(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const int8_t*>(data + offset);
  offset += sizeof(int8_t);
  return value;
}

uint8_t ReadUint8(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const uint8_t*>(data + offset);
  offset += sizeof(uint8_t);
  return value;
}

int16_t ReadInt16(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const int16_t*>(data + offset);
  offset += sizeof(int16_t);
  return value;
}

uint16_t ReadUint16(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const uint16_t*>(data + offset);
  offset += sizeof(uint16_t);
  return value;
}

int32_t ReadInt32(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const int32_t*>(data + offset);
  offset += sizeof(int32_t);
  return value;
}

uint32_t ReadUint32(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const uint32_t*>(data + offset);
  offset += sizeof(uint32_t);
  return value;
}

int64_t ReadInt64(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const int64_t*>(data + offset);
  offset += sizeof(int64_t);
  return value;
}

uint64_t ReadUint64(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const uint64_t*>(data + offset);
  offset += sizeof(uint64_t);
  return value;
}

float ReadFloat(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const float*>(data + offset);
  offset += sizeof(float);
  return value;
}

double ReadDouble(const std::byte* data, size_t& offset) {
  auto value = *reinterpret_cast<const double*>(data + offset);
  offset += sizeof(double);
  return value;
}

std::string ReadString(const std::byte* data, size_t& offset) {
  const auto length = ReadUint32(data, offset);
  const auto stringData = reinterpret_cast<const char*>(data + offset);
  offset += length;
  return std::string(stringData, length);
}

void ReadUint8Array(const std::byte* data, size_t& offset, std::vector<uint8_t>& output) {
  const uint32_t dataLength = ReadUint32(data, offset);
  output.resize(dataLength);
  std::memcpy(output.data(), data + offset, dataLength);
  offset += dataLength;
}

Header ReadHeader(const std::byte* data, size_t& offset) {
  Header header;
  header.seq = ReadUint32(data, offset);
  header.stamp.sec = ReadInt32(data, offset);
  header.stamp.nsec = ReadUint32(data, offset);
  header.frame_id = ReadString(data, offset);
  return header;
}

sensor_msgs__Image ReadImage(const std::byte* data, size_t& offset) {
  sensor_msgs__Image image;
  image.header = ReadHeader(data, offset);
  image.height = ReadUint32(data, offset);
  image.width = ReadUint32(data, offset);
  image.encoding = ReadString(data, offset);
  image.is_bigendian = ReadUint8(data, offset);
  image.step = ReadUint32(data, offset);
  ReadUint8Array(data, offset, image.data);
  return image;
}

foxglove_msgs__ShrunkImage ReadShrunkImage(const std::byte* data, size_t& offset) {
  foxglove_msgs__ShrunkImage image;
  image.header = ReadHeader(data, offset);
  image.height = ReadUint32(data, offset);
  image.width = ReadUint32(data, offset);
  image.encoding = ReadString(data, offset);
  image.is_bigendian = ReadUint8(data, offset);
  image.step = ReadUint32(data, offset);
  image.format = ReadString(data, offset);
  ReadUint8Array(data, offset, image.data);
  return image;
}

// Writing /////////////////////////////////////////////////////////////////////

void WriteInt8(int8_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteUint8(uint8_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteInt16(int16_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteUint16(uint16_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteInt32(int32_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteUint32(uint32_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteInt64(int64_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteUint64(uint64_t value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteFloat(float value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteDouble(double value, mcap::ByteArray& data) {
  const auto* ptr = reinterpret_cast<const std::byte*>(&value);
  data.insert(data.end(), ptr, ptr + sizeof(value));
}

void WriteString(const std::string& value, mcap::ByteArray& data) {
  WriteUint32(uint32_t(value.size()), data);
  const auto* ptr = reinterpret_cast<const std::byte*>(value.data());
  data.insert(data.end(), ptr, ptr + value.size());
}

void WriteUint8Array(const std::vector<uint8_t>& value, mcap::ByteArray& data) {
  const auto& byteArray = reinterpret_cast<const mcap::ByteArray&>(value);
  WriteUint32(uint32_t(byteArray.size()), data);
  data.insert(data.end(), byteArray.begin(), byteArray.end());
}

void WriteHeader(const Header& header, mcap::ByteArray& data) {
  WriteUint32(header.seq, data);
  WriteInt32(header.stamp.sec, data);
  WriteUint32(header.stamp.nsec, data);
  WriteString(header.frame_id, data);
}

void WriteShrunkImage(const foxglove_msgs__ShrunkImage& image, mcap::ByteArray& data) {
  WriteHeader(image.header, data);
  WriteUint32(image.height, data);
  WriteUint32(image.width, data);
  WriteString(image.encoding, data);
  WriteUint8(image.is_bigendian, data);
  WriteUint32(image.step, data);
  WriteString(image.format, data);
  WriteUint8Array(image.data, data);
}

}  // namespace ros1
