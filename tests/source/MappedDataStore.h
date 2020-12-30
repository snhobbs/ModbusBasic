#pragma once

#include <Modbus/DataStore.h>
#include <Modbus/MappedRegisterDataStore.h>
#include <algorithm>
#include <array>
#include <cstdint>

struct MemoryMap {
  uint8_t str[4];
  uint32_t int32;
  uint64_t int641;
  uint64_t int642;
  uint64_t int643;
  uint64_t int644;
  uint64_t int645;
  uint64_t int646;
  uint64_t int647;
  uint64_t int648;
  uint64_t int649;
  uint64_t int640;
  uint64_t int6411;
  uint64_t int6412;
  uint64_t int6413;
  uint64_t int6422;
};
enum class MemoryMapEntryIdentifier {
  str,
  int32,
  int641,
  int642,
  int643,
  int644,
  int645,
  int646,
  int647,
  int648,
  int649,
  int640,
  int6411,
  int6412,
  int6413,
  int6422,
  unknown
};
class DataMap {
  MemoryMap data_bank_{};
  static const constexpr std::size_t map_entry_count_ = 16;
  std::array<bool, map_entry_count_> new_data_flags_{};
  using MapEntry = Modbus::MemoryMapEntry<MemoryMapEntryIdentifier>;
  static const constexpr std::array<MapEntry, map_entry_count_> memory_entries_{
      MapEntry{MemoryMapEntryIdentifier::str, offsetof(MemoryMap, str),
               sizeof(MemoryMap::str)},
      MapEntry{MemoryMapEntryIdentifier::int32, offsetof(MemoryMap, int32),
               sizeof(MemoryMap::int32)},
      MapEntry{MemoryMapEntryIdentifier::int641, offsetof(MemoryMap, int641),
               sizeof(MemoryMap::int641)},
      MapEntry{MemoryMapEntryIdentifier::int642, offsetof(MemoryMap, int642),
               sizeof(MemoryMap::int642)},
      MapEntry{MemoryMapEntryIdentifier::int643, offsetof(MemoryMap, int643),
               sizeof(MemoryMap::int643)},
      MapEntry{MemoryMapEntryIdentifier::int644, offsetof(MemoryMap, int644),
               sizeof(MemoryMap::int644)},
      MapEntry{MemoryMapEntryIdentifier::int645, offsetof(MemoryMap, int645),
               sizeof(MemoryMap::int645)},
      MapEntry{MemoryMapEntryIdentifier::int646, offsetof(MemoryMap, int646),
               sizeof(MemoryMap::int646)},
      MapEntry{MemoryMapEntryIdentifier::int647, offsetof(MemoryMap, int647),
               sizeof(MemoryMap::int647)},
      MapEntry{MemoryMapEntryIdentifier::int648, offsetof(MemoryMap, int648),
               sizeof(MemoryMap::int648)},
      MapEntry{MemoryMapEntryIdentifier::int649, offsetof(MemoryMap, int649),
               sizeof(MemoryMap::int649)},
      MapEntry{MemoryMapEntryIdentifier::int640, offsetof(MemoryMap, int640),
               sizeof(MemoryMap::int640)},
      MapEntry{MemoryMapEntryIdentifier::int6411, offsetof(MemoryMap, int6411),
               sizeof(MemoryMap::int6411)},
      MapEntry{MemoryMapEntryIdentifier::int6412, offsetof(MemoryMap, int6412),
               sizeof(MemoryMap::int6412)},
      MapEntry{MemoryMapEntryIdentifier::int6413, offsetof(MemoryMap, int6413),
               sizeof(MemoryMap::int6413)},
      MapEntry{MemoryMapEntryIdentifier::int6422, offsetof(MemoryMap, int6422),
               sizeof(MemoryMap::int6422)},
  };

private:
  static constexpr std::size_t GetMemoryMapEntryIndex(std::size_t address) {
    for (std::size_t i = 0; i < memory_entries_.size(); i++) {
      if (address >= memory_entries_[i].offset) {
        return i;
      }
    }
    return 0;
  }

public:
  void SetField(MemoryMapEntryIdentifier identifier,
                const ArrayView<const uint8_t> &data_view) {
    switch (identifier) {
    case (MemoryMapEntryIdentifier::unknown): {
      // Ignore this case, should be scrubed
      break;
    }
    case (MemoryMapEntryIdentifier::str): {
      for (std::size_t i = 0;
           i < std::min(sizeof(data_bank_.str), data_view.size()); i++) {
        data_bank_.str[i] = data_view[i];
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int32): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint32_t>(
          data_view.data(), data_view.size());
      data_bank_.int32 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int641): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int641 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int642): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int642 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int643): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int643 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int644): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int644 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int645): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int645 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int646): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int646 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int647): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int647 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int648): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int648 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int649): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int649 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int640): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int640 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int6411): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int6411 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int6412): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int6412 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int6413): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int6413 = value;
      break;
    }
    case (MemoryMapEntryIdentifier::int6422): {
      const auto value = Utilities::Make_LSB_IntegerTypeFromU8Array<uint64_t>(
          data_view.data(), data_view.size());
      data_bank_.int6422 = value;
      break;
    }
    default: { assert(0); }
    }
  }
  void GetField(MemoryMapEntryIdentifier identifier,
                ArrayView<uint8_t> *data_view) const {
    switch (identifier) {
    case (MemoryMapEntryIdentifier::unknown): {
      // Ignore this case, should be scrubed
      break;
    }
    case (MemoryMapEntryIdentifier::str): {
      for (std::size_t i = 0;
           i < std::min(sizeof(data_bank_.str), data_view->size()); i++) {
        data_view->at(i) = data_bank_.str[i];
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int32): {
      const auto value = data_bank_.int32;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int641): {
      const auto value = data_bank_.int641;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int642): {
      const auto value = data_bank_.int642;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int643): {
      const auto value = data_bank_.int643;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int644): {
      const auto value = data_bank_.int644;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int645): {
      const auto value = data_bank_.int645;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int646): {
      const auto value = data_bank_.int646;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int647): {
      const auto value = data_bank_.int647;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int648): {
      const auto value = data_bank_.int648;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int649): {
      const auto value = data_bank_.int649;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int640): {
      const auto value = data_bank_.int640;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int6411): {
      const auto value = data_bank_.int6411;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int6412): {
      const auto value = data_bank_.int6412;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int6413): {
      const auto value = data_bank_.int6413;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    case (MemoryMapEntryIdentifier::int6422): {
      const auto value = data_bank_.int6422;
      const auto array = Utilities::MakeLSBU8Array(value);
      for (std::size_t i = 0; i < std::min(array.size(), data_view->size());
           i++) {
        data_view->at(i) = array.at(i);
      }
      break;
    }
    default: { assert(0); }
    }
  }
  ArrayView<const uint8_t> get_str(void) const {
    return ArrayView<const uint8_t>{4, GetDataBank().str};
  }
  void set_str(const uint8_t *const value, std::size_t length) {
    for (std::size_t i = 0; i < length; i++) {
      GetDataBank().str[i] = value[i];
    }
  }
  uint32_t get_int32(void) const { return GetDataBank().int32; }
  void set_int32(uint32_t value) { GetDataBank().int32 = value; }
  uint64_t get_int641(void) const { return GetDataBank().int641; }
  void set_int641(uint64_t value) { GetDataBank().int641 = value; }
  uint64_t get_int642(void) const { return GetDataBank().int642; }
  void set_int642(uint64_t value) { GetDataBank().int642 = value; }
  uint64_t get_int643(void) const { return GetDataBank().int643; }
  void set_int643(uint64_t value) { GetDataBank().int643 = value; }
  uint64_t get_int644(void) const { return GetDataBank().int644; }
  void set_int644(uint64_t value) { GetDataBank().int644 = value; }
  uint64_t get_int645(void) const { return GetDataBank().int645; }
  void set_int645(uint64_t value) { GetDataBank().int645 = value; }
  uint64_t get_int646(void) const { return GetDataBank().int646; }
  void set_int646(uint64_t value) { GetDataBank().int646 = value; }
  uint64_t get_int647(void) const { return GetDataBank().int647; }
  void set_int647(uint64_t value) { GetDataBank().int647 = value; }
  uint64_t get_int648(void) const { return GetDataBank().int648; }
  void set_int648(uint64_t value) { GetDataBank().int648 = value; }
  uint64_t get_int649(void) const { return GetDataBank().int649; }
  void set_int649(uint64_t value) { GetDataBank().int649 = value; }
  uint64_t get_int640(void) const { return GetDataBank().int640; }
  void set_int640(uint64_t value) { GetDataBank().int640 = value; }
  uint64_t get_int6411(void) const { return GetDataBank().int6411; }
  void set_int6411(uint64_t value) { GetDataBank().int6411 = value; }
  uint64_t get_int6412(void) const { return GetDataBank().int6412; }
  void set_int6412(uint64_t value) { GetDataBank().int6412 = value; }
  uint64_t get_int6413(void) const { return GetDataBank().int6413; }
  void set_int6413(uint64_t value) { GetDataBank().int6413 = value; }
  uint64_t get_int6422(void) const { return GetDataBank().int6422; }
  void set_int6422(uint64_t value) { GetDataBank().int6422 = value; }

public:
  static std::size_t size(void) { return sizeof(MemoryMap); }
  static MemoryMapEntryIdentifier
  GetIdentifierFromAddress(std::size_t address) {
    const auto &entry = memory_entries_.at(GetMemoryMapEntryIndex(address));
    if (entry.offset != address) {
      return MemoryMapEntryIdentifier::unknown;
    }
    return entry.identifier;
  }

  static constexpr bool WriteLocationValid(std::size_t address,
                                           std::size_t count) {
    const auto &entry = memory_entries_.at(GetMemoryMapEntryIndex(address));
    return (address == entry.offset) && (entry.size == count);
  }
  const MemoryMap &GetDataBank(void) const { return data_bank_; }
  void SetFieldFromAddress(std::size_t address,
                           const ArrayView<const uint8_t> &data_view) {
    const auto identifier = GetIdentifierFromAddress(address);
    SetField(identifier, data_view);
  }
  void GetFieldFromAddress(std::size_t address,
                           ArrayView<uint8_t> *data_view) const {
    const auto identifier = GetIdentifierFromAddress(address);
    GetField(identifier, data_view);
  }
  MemoryMap &GetDataBank(void) { return data_bank_; }

  DataMap(void) {}
};
