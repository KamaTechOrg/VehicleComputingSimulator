#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using FieldValue = std::variant<unsigned int, std::string, float, unsigned long,
                                double, int, bool>;

enum class FieldType {
  UNSIGNED_INT,
  SIGNED_INT,
  CHAR_ARRAY,
  FLOAT_FIXED,
  FLOAT_MANTISSA,
  BIT_FIELD,
  DOUBLE,
  BOOLEAN,
  UNKNOWN
};

struct Field {
  std::string name;
  std::string type;
  size_t size;
  size_t offset;
};

struct BitField {
  std::string name;
  size_t size;
  size_t offset;
  std::vector<Field> fields;
};

class PacketParser {
public:
  PacketParser(const std::string &jsonFilePath);
  void *getFieldValue(const std::string &fieldName) const;
  std::map<std::string, FieldValue> getAllFieldValues() const;
  const std::vector<Field> &getFields() const {
    return fields; 
  }
  const void *getBuffer() const { return _buffer; }
  FieldType getFieldType(const std::string &typeName) const;
  void printFieldValues(const std::map<std::string, FieldValue> &fieldValues);
  void setBuffer(const void *buffer);

private:
  std::vector<Field> fields;
  std::vector<BitField> bitFields;
  std::string endianness;
  const void *_buffer;

  void validateFieldSize(const std::string &type, size_t bitLength) const;
  void loadJson(const std::string &jsonFilePath);
  uint32_t decodeUnsignedInt(const uint8_t *data, size_t bitLength) const;
  int32_t decodeSignedInt(const uint8_t *data, size_t bitLength) const;
  std::string decodeCharArray(const uint8_t *data, size_t byteLength) const;
  float decodeFloat(const uint8_t *data, size_t bitLength) const;
  double decodeDouble(const uint8_t *data, size_t bitLength) const;
  bool decodeBoolean(const uint8_t *data, size_t bitLength) const;
  uint8_t *extractBits(const uint8_t *bitFieldData, size_t offset,
                       size_t size) const;
  friend class PacketParserTest_ExtractBits_Test;
};

#endif // PACKET_PARSER_H