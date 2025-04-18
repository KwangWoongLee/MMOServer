// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: struct.proto

#include "struct.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace Protocol {
PROTOBUF_CONSTEXPR Pos::Pos(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.x_)*/0
  , /*decltype(_impl_.y_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct PosDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PosDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~PosDefaultTypeInternal() {}
  union {
    Pos _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PosDefaultTypeInternal _Pos_default_instance_;
PROTOBUF_CONSTEXPR PActor::PActor(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_._has_bits_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}
  , /*decltype(_impl_.position_)*/nullptr
  , /*decltype(_impl_.id_)*/uint64_t{0u}
  , /*decltype(_impl_.type_)*/0
  , /*decltype(_impl_.action_)*/0
  , /*decltype(_impl_.playertype_)*/0
  , /*decltype(_impl_.blocktype_)*/0
  , /*decltype(_impl_.monstertype_)*/0} {}
struct PActorDefaultTypeInternal {
  PROTOBUF_CONSTEXPR PActorDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~PActorDefaultTypeInternal() {}
  union {
    PActor _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 PActorDefaultTypeInternal _PActor_default_instance_;
}  // namespace Protocol
static ::_pb::Metadata file_level_metadata_struct_2eproto[2];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_struct_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_struct_2eproto = nullptr;

const uint32_t TableStruct_struct_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::Pos, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Protocol::Pos, _impl_.x_),
  PROTOBUF_FIELD_OFFSET(::Protocol::Pos, _impl_.y_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_._has_bits_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.id_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.type_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.position_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.action_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.playertype_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.blocktype_),
  PROTOBUF_FIELD_OFFSET(::Protocol::PActor, _impl_.monstertype_),
  ~0u,
  ~0u,
  ~0u,
  ~0u,
  0,
  1,
  2,
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::Protocol::Pos)},
  { 8, 21, -1, sizeof(::Protocol::PActor)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::Protocol::_Pos_default_instance_._instance,
  &::Protocol::_PActor_default_instance_._instance,
};

const char descriptor_table_protodef_struct_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\014struct.proto\022\010Protocol\032\nenum.proto\"\033\n\003"
  "Pos\022\t\n\001x\030\001 \001(\002\022\t\n\001y\030\002 \001(\002\"\264\002\n\006PActor\022\n\n\002"
  "id\030\001 \001(\004\022!\n\004type\030\002 \001(\0162\023.Protocol.ActorT"
  "ype\022\037\n\010position\030\003 \001(\0132\r.Protocol.Pos\022 \n\006"
  "action\030\004 \001(\0162\020.Protocol.Action\022-\n\nplayer"
  "Type\030\005 \001(\0162\024.Protocol.PlayerTypeH\000\210\001\001\022+\n"
  "\tblockType\030\006 \001(\0162\023.Protocol.BlockTypeH\001\210"
  "\001\001\022/\n\013monsterType\030\007 \001(\0162\025.Protocol.Monst"
  "erTypeH\002\210\001\001B\r\n\013_playerTypeB\014\n\n_blockType"
  "B\016\n\014_monsterTypeb\006proto3"
  ;
static const ::_pbi::DescriptorTable* const descriptor_table_struct_2eproto_deps[1] = {
  &::descriptor_table_enum_2eproto,
};
static ::_pbi::once_flag descriptor_table_struct_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_struct_2eproto = {
    false, false, 384, descriptor_table_protodef_struct_2eproto,
    "struct.proto",
    &descriptor_table_struct_2eproto_once, descriptor_table_struct_2eproto_deps, 1, 2,
    schemas, file_default_instances, TableStruct_struct_2eproto::offsets,
    file_level_metadata_struct_2eproto, file_level_enum_descriptors_struct_2eproto,
    file_level_service_descriptors_struct_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_struct_2eproto_getter() {
  return &descriptor_table_struct_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_struct_2eproto(&descriptor_table_struct_2eproto);
namespace Protocol {

// ===================================================================

class Pos::_Internal {
 public:
};

Pos::Pos(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:Protocol.Pos)
}
Pos::Pos(const Pos& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  Pos* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.x_){}
    , decltype(_impl_.y_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&_impl_.x_, &from._impl_.x_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.y_) -
    reinterpret_cast<char*>(&_impl_.x_)) + sizeof(_impl_.y_));
  // @@protoc_insertion_point(copy_constructor:Protocol.Pos)
}

inline void Pos::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.x_){0}
    , decltype(_impl_.y_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

Pos::~Pos() {
  // @@protoc_insertion_point(destructor:Protocol.Pos)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Pos::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void Pos::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Pos::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.Pos)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.x_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.y_) -
      reinterpret_cast<char*>(&_impl_.x_)) + sizeof(_impl_.y_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Pos::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // float x = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 13)) {
          _impl_.x_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else
          goto handle_unusual;
        continue;
      // float y = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 21)) {
          _impl_.y_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Pos::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.Pos)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // float x = 1;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_x = this->_internal_x();
  uint32_t raw_x;
  memcpy(&raw_x, &tmp_x, sizeof(tmp_x));
  if (raw_x != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(1, this->_internal_x(), target);
  }

  // float y = 2;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_y = this->_internal_y();
  uint32_t raw_y;
  memcpy(&raw_y, &tmp_y, sizeof(tmp_y));
  if (raw_y != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(2, this->_internal_y(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.Pos)
  return target;
}

size_t Pos::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.Pos)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // float x = 1;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_x = this->_internal_x();
  uint32_t raw_x;
  memcpy(&raw_x, &tmp_x, sizeof(tmp_x));
  if (raw_x != 0) {
    total_size += 1 + 4;
  }

  // float y = 2;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_y = this->_internal_y();
  uint32_t raw_y;
  memcpy(&raw_y, &tmp_y, sizeof(tmp_y));
  if (raw_y != 0) {
    total_size += 1 + 4;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Pos::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    Pos::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Pos::GetClassData() const { return &_class_data_; }


void Pos::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<Pos*>(&to_msg);
  auto& from = static_cast<const Pos&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Protocol.Pos)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_x = from._internal_x();
  uint32_t raw_x;
  memcpy(&raw_x, &tmp_x, sizeof(tmp_x));
  if (raw_x != 0) {
    _this->_internal_set_x(from._internal_x());
  }
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_y = from._internal_y();
  uint32_t raw_y;
  memcpy(&raw_y, &tmp_y, sizeof(tmp_y));
  if (raw_y != 0) {
    _this->_internal_set_y(from._internal_y());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Pos::CopyFrom(const Pos& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.Pos)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Pos::IsInitialized() const {
  return true;
}

void Pos::InternalSwap(Pos* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Pos, _impl_.y_)
      + sizeof(Pos::_impl_.y_)
      - PROTOBUF_FIELD_OFFSET(Pos, _impl_.x_)>(
          reinterpret_cast<char*>(&_impl_.x_),
          reinterpret_cast<char*>(&other->_impl_.x_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Pos::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_struct_2eproto_getter, &descriptor_table_struct_2eproto_once,
      file_level_metadata_struct_2eproto[0]);
}

// ===================================================================

class PActor::_Internal {
 public:
  using HasBits = decltype(std::declval<PActor>()._impl_._has_bits_);
  static const ::Protocol::Pos& position(const PActor* msg);
  static void set_has_playertype(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_blocktype(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_monstertype(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

const ::Protocol::Pos&
PActor::_Internal::position(const PActor* msg) {
  return *msg->_impl_.position_;
}
PActor::PActor(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:Protocol.PActor)
}
PActor::PActor(const PActor& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  PActor* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.position_){nullptr}
    , decltype(_impl_.id_){}
    , decltype(_impl_.type_){}
    , decltype(_impl_.action_){}
    , decltype(_impl_.playertype_){}
    , decltype(_impl_.blocktype_){}
    , decltype(_impl_.monstertype_){}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  if (from._internal_has_position()) {
    _this->_impl_.position_ = new ::Protocol::Pos(*from._impl_.position_);
  }
  ::memcpy(&_impl_.id_, &from._impl_.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.monstertype_) -
    reinterpret_cast<char*>(&_impl_.id_)) + sizeof(_impl_.monstertype_));
  // @@protoc_insertion_point(copy_constructor:Protocol.PActor)
}

inline void PActor::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.position_){nullptr}
    , decltype(_impl_.id_){uint64_t{0u}}
    , decltype(_impl_.type_){0}
    , decltype(_impl_.action_){0}
    , decltype(_impl_.playertype_){0}
    , decltype(_impl_.blocktype_){0}
    , decltype(_impl_.monstertype_){0}
  };
}

PActor::~PActor() {
  // @@protoc_insertion_point(destructor:Protocol.PActor)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void PActor::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  if (this != internal_default_instance()) delete _impl_.position_;
}

void PActor::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void PActor::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.PActor)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (GetArenaForAllocation() == nullptr && _impl_.position_ != nullptr) {
    delete _impl_.position_;
  }
  _impl_.position_ = nullptr;
  ::memset(&_impl_.id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.action_) -
      reinterpret_cast<char*>(&_impl_.id_)) + sizeof(_impl_.action_));
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    ::memset(&_impl_.playertype_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&_impl_.monstertype_) -
        reinterpret_cast<char*>(&_impl_.playertype_)) + sizeof(_impl_.monstertype_));
  }
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* PActor::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // uint64 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _impl_.id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .Protocol.ActorType type = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_type(static_cast<::Protocol::ActorType>(val));
        } else
          goto handle_unusual;
        continue;
      // .Protocol.Pos position = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          ptr = ctx->ParseMessage(_internal_mutable_position(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .Protocol.Action action = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 32)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_action(static_cast<::Protocol::Action>(val));
        } else
          goto handle_unusual;
        continue;
      // optional .Protocol.PlayerType playerType = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 40)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_playertype(static_cast<::Protocol::PlayerType>(val));
        } else
          goto handle_unusual;
        continue;
      // optional .Protocol.BlockType blockType = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 48)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_blocktype(static_cast<::Protocol::BlockType>(val));
        } else
          goto handle_unusual;
        continue;
      // optional .Protocol.MonsterType monsterType = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 56)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_monstertype(static_cast<::Protocol::MonsterType>(val));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _impl_._has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* PActor::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.PActor)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->_internal_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt64ToArray(1, this->_internal_id(), target);
  }

  // .Protocol.ActorType type = 2;
  if (this->_internal_type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      2, this->_internal_type(), target);
  }

  // .Protocol.Pos position = 3;
  if (this->_internal_has_position()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(3, _Internal::position(this),
        _Internal::position(this).GetCachedSize(), target, stream);
  }

  // .Protocol.Action action = 4;
  if (this->_internal_action() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      4, this->_internal_action(), target);
  }

  // optional .Protocol.PlayerType playerType = 5;
  if (_internal_has_playertype()) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      5, this->_internal_playertype(), target);
  }

  // optional .Protocol.BlockType blockType = 6;
  if (_internal_has_blocktype()) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      6, this->_internal_blocktype(), target);
  }

  // optional .Protocol.MonsterType monsterType = 7;
  if (_internal_has_monstertype()) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      7, this->_internal_monstertype(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.PActor)
  return target;
}

size_t PActor::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.PActor)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .Protocol.Pos position = 3;
  if (this->_internal_has_position()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *_impl_.position_);
  }

  // uint64 id = 1;
  if (this->_internal_id() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt64SizePlusOne(this->_internal_id());
  }

  // .Protocol.ActorType type = 2;
  if (this->_internal_type() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_type());
  }

  // .Protocol.Action action = 4;
  if (this->_internal_action() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_action());
  }

  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    // optional .Protocol.PlayerType playerType = 5;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::_pbi::WireFormatLite::EnumSize(this->_internal_playertype());
    }

    // optional .Protocol.BlockType blockType = 6;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::_pbi::WireFormatLite::EnumSize(this->_internal_blocktype());
    }

    // optional .Protocol.MonsterType monsterType = 7;
    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::_pbi::WireFormatLite::EnumSize(this->_internal_monstertype());
    }

  }
  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData PActor::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    PActor::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*PActor::GetClassData() const { return &_class_data_; }


void PActor::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<PActor*>(&to_msg);
  auto& from = static_cast<const PActor&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Protocol.PActor)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_position()) {
    _this->_internal_mutable_position()->::Protocol::Pos::MergeFrom(
        from._internal_position());
  }
  if (from._internal_id() != 0) {
    _this->_internal_set_id(from._internal_id());
  }
  if (from._internal_type() != 0) {
    _this->_internal_set_type(from._internal_type());
  }
  if (from._internal_action() != 0) {
    _this->_internal_set_action(from._internal_action());
  }
  cached_has_bits = from._impl_._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      _this->_impl_.playertype_ = from._impl_.playertype_;
    }
    if (cached_has_bits & 0x00000002u) {
      _this->_impl_.blocktype_ = from._impl_.blocktype_;
    }
    if (cached_has_bits & 0x00000004u) {
      _this->_impl_.monstertype_ = from._impl_.monstertype_;
    }
    _this->_impl_._has_bits_[0] |= cached_has_bits;
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void PActor::CopyFrom(const PActor& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.PActor)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PActor::IsInitialized() const {
  return true;
}

void PActor::InternalSwap(PActor* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(PActor, _impl_.monstertype_)
      + sizeof(PActor::_impl_.monstertype_)
      - PROTOBUF_FIELD_OFFSET(PActor, _impl_.position_)>(
          reinterpret_cast<char*>(&_impl_.position_),
          reinterpret_cast<char*>(&other->_impl_.position_));
}

::PROTOBUF_NAMESPACE_ID::Metadata PActor::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_struct_2eproto_getter, &descriptor_table_struct_2eproto_once,
      file_level_metadata_struct_2eproto[1]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace Protocol
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Protocol::Pos*
Arena::CreateMaybeMessage< ::Protocol::Pos >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::Pos >(arena);
}
template<> PROTOBUF_NOINLINE ::Protocol::PActor*
Arena::CreateMaybeMessage< ::Protocol::PActor >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::PActor >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
