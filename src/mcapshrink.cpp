#define MCAP_IMPLEMENTATION

#include <iostream>
#include <mcap/mcap.hpp>
#include <unordered_set>

#include "compresssion.hpp"
#include "ros1.hpp"

static std::unordered_set<std::string> unhandledFormats;

bool CompressRos1Image(const mcap::Message& message, ros1::foxglove_msgs__ShrunkImage& output) {
  // Decode the ROS1 sensor_msgs/Image message
  size_t offset = 0;
  const auto image = ros1::ReadImage(message.data, offset);

  output.header = image.header;
  output.height = image.height;
  output.width = image.width;
  output.encoding = image.encoding;
  output.is_bigendian = image.is_bigendian;
  output.step = image.step;

  if (image.encoding == "mono8" || image.encoding == "8UC1") {
    return CompressMono8(image, output);
  } else if (image.encoding == "mono16") {
    return CompressMono16(image, output);
  } else if (image.encoding == "rgb8" || image.encoding == "8UC3") {
    return CompressRGB(image, output);
  } else if (image.encoding == "bgr8") {
    return CompressBGR(image, output);
  } else if (image.encoding == "rgba8" || image.encoding == "8UC4") {
    return CompressRGBA(image, output);
  } else if (image.encoding == "bgra8") {
    return CompressBGRA(image, output);
  }

  if (unhandledFormats.count(image.encoding) == 0) {
    std::cerr << "Image format \"" << image.encoding << "\" will not be compressed\n";
    unhandledFormats.insert(image.encoding);
  }

  output.data = image.data;
  return true;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input.mcap> <output.mcap>\n";
    return 1;
  }

  const std::string inputFilename = argv[1];
  const std::string outputFilename = argv[2];

  mcap::McapReader reader;
  auto status = reader.open(inputFilename);
  if (!status.ok()) {
    std::cerr << "Failed to open input file: " << status.message << "\n";
    return 1;
  }

  const auto& profile = reader.header()->profile;

  mcap::McapWriter writer;
  mcap::McapWriterOptions writerOpts{profile};
  status = writer.open(outputFilename, writerOpts);
  if (!status.ok()) {
    std::cerr << "Failed to open output file: " << status.message << "\n";
    return 1;
  }

  std::unordered_map<mcap::ChannelId, mcap::Channel> remappedChannels;
  std::unordered_map<std::string, mcap::Schema> remappedSchemas;

  for (const auto& msg : reader.readMessages()) {
    const auto& inputMessage = msg.message;
    const auto& dataType = msg.schema->name;

    // Find the remapped channel associated with this message
    auto channelIt = remappedChannels.find(msg.channel->id);
    if (channelIt == remappedChannels.end()) {
      // Find the remapped schema associated with this channel
      auto schemaIt = remappedSchemas.find(dataType);
      if (schemaIt == remappedSchemas.end()) {
        const auto& inputSchema = *msg.schema;
        const auto& encoding = msg.schema->encoding;

        if (encoding == "ros1msg" && dataType == "sensor_msgs/Image") {
          mcap::Schema schema{"foxglove_msgs/ShrunkImage", "ros1msg", ros1::SHRUNK_IMAGE_MSGDEF};
          writer.addSchema(schema);
          remappedSchemas.emplace(dataType, schema);
        } else {
          mcap::Schema schema{dataType, inputSchema.encoding, inputSchema.data};
          writer.addSchema(schema);
          remappedSchemas.emplace(dataType, schema);
        }

        schemaIt = remappedSchemas.find(dataType);
        assert(schemaIt != remappedSchemas.end());
        // std::cout << "Added schema for " << dataType << "\n";
      }

      const auto& schema = schemaIt->second;

      // Create a new channel
      mcap::Channel channel{msg.channel->topic, msg.channel->messageEncoding, schema.id};
      writer.addChannel(channel);
      // std::cout << "Added channel mapping for " << channel.topic << " (" << msg.channel->id
      //           << " -> " << channel.id << ")\n";
      remappedChannels.emplace(msg.channel->id, channel);
      channelIt = remappedChannels.find(msg.channel->id);
      assert(channelIt != remappedChannels.end());
    }

    // Create a message for writing to the output file
    mcap::Message message;
    message.channelId = channelIt->second.id;
    message.sequence = inputMessage.sequence;
    message.logTime = inputMessage.logTime;
    message.publishTime = inputMessage.publishTime;

    if (dataType == "sensor_msgs/Image") {
      // Create a foxglove_msgs/ShrunkImage message from the ROS1 sensor_msgs/Image message
      ros1::foxglove_msgs__ShrunkImage shrunkImage;
      if (!CompressRos1Image(inputMessage, shrunkImage)) {
        std::cerr << "Failed to compress ROS1 image\n";
        return 1;
      }

      // Serialize the ShrunkImage message
      mcap::ByteArray buffer;
      buffer.reserve(shrunkImage.sizeInBytes());
      ros1::WriteShrunkImage(shrunkImage, buffer);
      message.dataSize = buffer.size();
      message.data = buffer.data();
    } else {
      // Pass this message straight through to the writer without decoding
      message.dataSize = inputMessage.dataSize;
      message.data = inputMessage.data;
    }

    // Write the message to the output file
    status = writer.write(message);
    if (!status.ok()) {
      std::cerr << "Failed to write message: " << status.message << "\n";
      writer.close();
      return 1;
    }
  }

  writer.close();
  return 0;
}
