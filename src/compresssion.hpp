#pragma once

#include <png.h>
#include <webp/encode.h>

#include <vector>

#include "ros1.hpp"

struct PngDestructor {
  png_struct* p;

  PngDestructor(png_struct* p)
      : p(p) {}

  ~PngDestructor() {
    if (p) {
      png_destroy_write_struct(&p, nullptr);
    }
  }
};

static void PngWriteCallback(png_structp pngPtr, png_bytep data, png_size_t length) {
  auto* p = reinterpret_cast<std::vector<uint8_t>*>(png_get_io_ptr(pngPtr));
  p->insert(p->end(), data, data + length);
}

bool CompressMono8(const ros1::sensor_msgs__Image& image,
                   ros1::foxglove_msgs__ShrunkImage& output) {
  // Convert grayscale to RGB
  std::vector<uint8_t> rgbData(image.width * image.height * 3);
  for (size_t i = 0; i < image.height; ++i) {
    for (size_t j = 0; j < image.width; ++j) {
      const uint8_t gray = image.data[i * image.step + j];
      const auto rgbOffset = (i * image.width + j) * 3;
      rgbData[rgbOffset + 0] = gray;
      rgbData[rgbOffset + 1] = gray;
      rgbData[rgbOffset + 2] = gray;
    }
  }

  uint8_t* compressedData = nullptr;
  const size_t compressedSize = WebPEncodeLosslessRGB(
      rgbData.data(), int(image.width), int(image.height), int(image.width * 3), &compressedData);

  output.data.resize(compressedSize);
  output.format = "webp";
  std::memcpy(output.data.data(), compressedData, compressedSize);

  WebPFree(compressedData);
  return true;
}

bool CompressMono16(const ros1::sensor_msgs__Image& image,
                    ros1::foxglove_msgs__ShrunkImage& output) {
  auto pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!pngPtr) {
    std::cerr << "Failed to create PNG write struct\n";
    return false;
  }
  PngDestructor destroyPng(pngPtr);

  auto infoPtr = png_create_info_struct(pngPtr);
  if (!infoPtr) {
    std::cerr << "Failed to create PNG info struct\n";
    return false;
  }

  if (0 != setjmp(png_jmpbuf(pngPtr))) {
    std::cerr << "setjmp(png_jmpbuf(*)) failed\n";
    return false;
  }

  png_set_IHDR(pngPtr, infoPtr, image.width, image.height,  // width and height
               16,                                          // bit depth
               PNG_COLOR_TYPE_GRAY,                         // color type
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  std::vector<uint8_t*> rows(image.height);
  for (size_t y = 0; y < image.height; ++y) {
    rows[y] = const_cast<uint8_t*>(image.data.data()) + y * image.step;
  }

  png_set_rows(pngPtr, infoPtr, &rows[0]);
  png_set_write_fn(pngPtr, &output.data, PngWriteCallback, nullptr);
  png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, nullptr);
  output.format = "png";
  return true;
}

bool CompressRGB(const ros1::sensor_msgs__Image& image, ros1::foxglove_msgs__ShrunkImage& output) {
  uint8_t* compressedData = nullptr;
  const size_t compressedSize = WebPEncodeLosslessRGB(
      image.data.data(), int(image.width), int(image.height), int(image.step), &compressedData);

  output.data.resize(compressedSize);
  output.format = "webp";
  std::memcpy(output.data.data(), compressedData, compressedSize);

  WebPFree(compressedData);
  return true;
}

bool CompressBGR(const ros1::sensor_msgs__Image& image, ros1::foxglove_msgs__ShrunkImage& output) {
  uint8_t* compressedData = nullptr;
  const size_t compressedSize = WebPEncodeLosslessBGR(
      image.data.data(), int(image.width), int(image.height), int(image.step), &compressedData);

  output.data.resize(compressedSize);
  output.format = "webp";
  std::memcpy(output.data.data(), compressedData, compressedSize);

  WebPFree(compressedData);
  return true;
}

bool CompressRGBA(const ros1::sensor_msgs__Image& image, ros1::foxglove_msgs__ShrunkImage& output) {
  uint8_t* compressedData = nullptr;
  const size_t compressedSize = WebPEncodeLosslessRGBA(
      image.data.data(), int(image.width), int(image.height), int(image.step), &compressedData);

  output.data.resize(compressedSize);
  output.format = "webp";
  std::memcpy(output.data.data(), compressedData, compressedSize);

  WebPFree(compressedData);
  return true;
}

bool CompressBGRA(const ros1::sensor_msgs__Image& image, ros1::foxglove_msgs__ShrunkImage& output) {
  uint8_t* compressedData = nullptr;
  const size_t compressedSize = WebPEncodeLosslessBGRA(
      image.data.data(), int(image.width), int(image.height), int(image.step), &compressedData);

  output.data.resize(compressedSize);
  output.format = "webp";
  std::memcpy(output.data.data(), compressedData, compressedSize);

  WebPFree(compressedData);
  return true;
}
