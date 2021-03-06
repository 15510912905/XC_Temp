// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: comstruct.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "comstruct.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* STReqShowSmooth_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  STReqShowSmooth_reflection_ = NULL;
const ::google::protobuf::Descriptor* adOP_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  adOP_reflection_ = NULL;
const ::google::protobuf::Descriptor* STResShowSmooth_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  STResShowSmooth_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* ComMsgType_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_comstruct_2eproto() {
  protobuf_AddDesc_comstruct_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "comstruct.proto");
  GOOGLE_CHECK(file != NULL);
  STReqShowSmooth_descriptor_ = file->message_type(0);
  static const int STReqShowSmooth_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STReqShowSmooth, count_),
  };
  STReqShowSmooth_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      STReqShowSmooth_descriptor_,
      STReqShowSmooth::default_instance_,
      STReqShowSmooth_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STReqShowSmooth, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STReqShowSmooth, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(STReqShowSmooth));
  adOP_descriptor_ = file->message_type(1);
  static const int adOP_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(adOP, op_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(adOP, ad_),
  };
  adOP_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      adOP_descriptor_,
      adOP::default_instance_,
      adOP_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(adOP, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(adOP, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(adOP));
  STResShowSmooth_descriptor_ = file->message_type(2);
  static const int STResShowSmooth_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STResShowSmooth, cirle_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STResShowSmooth, value_),
  };
  STResShowSmooth_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      STResShowSmooth_descriptor_,
      STResShowSmooth::default_instance_,
      STResShowSmooth_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STResShowSmooth, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(STResShowSmooth, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(STResShowSmooth));
  ComMsgType_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_comstruct_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    STReqShowSmooth_descriptor_, &STReqShowSmooth::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    adOP_descriptor_, &adOP::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    STResShowSmooth_descriptor_, &STResShowSmooth::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_comstruct_2eproto() {
  delete STReqShowSmooth::default_instance_;
  delete STReqShowSmooth_reflection_;
  delete adOP::default_instance_;
  delete adOP_reflection_;
  delete STResShowSmooth::default_instance_;
  delete STResShowSmooth_reflection_;
}

void protobuf_AddDesc_comstruct_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::protobuf_AddDesc_typedefine_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017comstruct.proto\032\020typedefine.proto\" \n\017S"
    "TReqShowSmooth\022\r\n\005count\030\001 \001(\r\"\036\n\004adOP\022\n\n"
    "\002op\030\001 \001(\r\022\n\n\002ad\030\002 \001(\r\"D\n\017STResShowSmooth"
    "\022\033\n\005cirle\030\001 \001(\0162\014.ReactCircle\022\024\n\005value\030\002"
    " \003(\0132\005.adOP*<\n\nComMsgType\022\026\n\021MSG_ReqShow"
    "Smooth\020\241\037\022\026\n\021MSG_ResShowSmooth\020\242\037", 233);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "comstruct.proto", &protobuf_RegisterTypes);
  STReqShowSmooth::default_instance_ = new STReqShowSmooth();
  adOP::default_instance_ = new adOP();
  STResShowSmooth::default_instance_ = new STResShowSmooth();
  STReqShowSmooth::default_instance_->InitAsDefaultInstance();
  adOP::default_instance_->InitAsDefaultInstance();
  STResShowSmooth::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_comstruct_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_comstruct_2eproto {
  StaticDescriptorInitializer_comstruct_2eproto() {
    protobuf_AddDesc_comstruct_2eproto();
  }
} static_descriptor_initializer_comstruct_2eproto_;
const ::google::protobuf::EnumDescriptor* ComMsgType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ComMsgType_descriptor_;
}
bool ComMsgType_IsValid(int value) {
  switch(value) {
    case 4001:
    case 4002:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#ifndef _MSC_VER
const int STReqShowSmooth::kCountFieldNumber;
#endif  // !_MSC_VER

STReqShowSmooth::STReqShowSmooth()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:STReqShowSmooth)
}

void STReqShowSmooth::InitAsDefaultInstance() {
}

STReqShowSmooth::STReqShowSmooth(const STReqShowSmooth& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:STReqShowSmooth)
}

void STReqShowSmooth::SharedCtor() {
  _cached_size_ = 0;
  count_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

STReqShowSmooth::~STReqShowSmooth() {
  // @@protoc_insertion_point(destructor:STReqShowSmooth)
  SharedDtor();
}

void STReqShowSmooth::SharedDtor() {
  if (this != default_instance_) {
  }
}

void STReqShowSmooth::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* STReqShowSmooth::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return STReqShowSmooth_descriptor_;
}

const STReqShowSmooth& STReqShowSmooth::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_comstruct_2eproto();
  return *default_instance_;
}

STReqShowSmooth* STReqShowSmooth::default_instance_ = NULL;

STReqShowSmooth* STReqShowSmooth::New() const {
  return new STReqShowSmooth;
}

void STReqShowSmooth::Clear() {
  count_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool STReqShowSmooth::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:STReqShowSmooth)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 count = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &count_)));
          set_has_count();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:STReqShowSmooth)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:STReqShowSmooth)
  return false;
#undef DO_
}

void STReqShowSmooth::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:STReqShowSmooth)
  // optional uint32 count = 1;
  if (has_count()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->count(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:STReqShowSmooth)
}

::google::protobuf::uint8* STReqShowSmooth::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:STReqShowSmooth)
  // optional uint32 count = 1;
  if (has_count()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->count(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:STReqShowSmooth)
  return target;
}

int STReqShowSmooth::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional uint32 count = 1;
    if (has_count()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->count());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void STReqShowSmooth::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const STReqShowSmooth* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const STReqShowSmooth*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void STReqShowSmooth::MergeFrom(const STReqShowSmooth& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_count()) {
      set_count(from.count());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void STReqShowSmooth::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void STReqShowSmooth::CopyFrom(const STReqShowSmooth& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool STReqShowSmooth::IsInitialized() const {

  return true;
}

void STReqShowSmooth::Swap(STReqShowSmooth* other) {
  if (other != this) {
    std::swap(count_, other->count_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata STReqShowSmooth::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = STReqShowSmooth_descriptor_;
  metadata.reflection = STReqShowSmooth_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int adOP::kOpFieldNumber;
const int adOP::kAdFieldNumber;
#endif  // !_MSC_VER

adOP::adOP()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:adOP)
}

void adOP::InitAsDefaultInstance() {
}

adOP::adOP(const adOP& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:adOP)
}

void adOP::SharedCtor() {
  _cached_size_ = 0;
  op_ = 0u;
  ad_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

adOP::~adOP() {
  // @@protoc_insertion_point(destructor:adOP)
  SharedDtor();
}

void adOP::SharedDtor() {
  if (this != default_instance_) {
  }
}

void adOP::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* adOP::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return adOP_descriptor_;
}

const adOP& adOP::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_comstruct_2eproto();
  return *default_instance_;
}

adOP* adOP::default_instance_ = NULL;

adOP* adOP::New() const {
  return new adOP;
}

void adOP::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<adOP*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  ZR_(op_, ad_);

#undef OFFSET_OF_FIELD_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool adOP::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:adOP)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 op = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &op_)));
          set_has_op();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_ad;
        break;
      }

      // optional uint32 ad = 2;
      case 2: {
        if (tag == 16) {
         parse_ad:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &ad_)));
          set_has_ad();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:adOP)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:adOP)
  return false;
#undef DO_
}

void adOP::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:adOP)
  // optional uint32 op = 1;
  if (has_op()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->op(), output);
  }

  // optional uint32 ad = 2;
  if (has_ad()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->ad(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:adOP)
}

::google::protobuf::uint8* adOP::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:adOP)
  // optional uint32 op = 1;
  if (has_op()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->op(), target);
  }

  // optional uint32 ad = 2;
  if (has_ad()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(2, this->ad(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:adOP)
  return target;
}

int adOP::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional uint32 op = 1;
    if (has_op()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->op());
    }

    // optional uint32 ad = 2;
    if (has_ad()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->ad());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void adOP::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const adOP* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const adOP*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void adOP::MergeFrom(const adOP& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_op()) {
      set_op(from.op());
    }
    if (from.has_ad()) {
      set_ad(from.ad());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void adOP::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void adOP::CopyFrom(const adOP& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool adOP::IsInitialized() const {

  return true;
}

void adOP::Swap(adOP* other) {
  if (other != this) {
    std::swap(op_, other->op_);
    std::swap(ad_, other->ad_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata adOP::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = adOP_descriptor_;
  metadata.reflection = adOP_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int STResShowSmooth::kCirleFieldNumber;
const int STResShowSmooth::kValueFieldNumber;
#endif  // !_MSC_VER

STResShowSmooth::STResShowSmooth()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:STResShowSmooth)
}

void STResShowSmooth::InitAsDefaultInstance() {
}

STResShowSmooth::STResShowSmooth(const STResShowSmooth& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:STResShowSmooth)
}

void STResShowSmooth::SharedCtor() {
  _cached_size_ = 0;
  cirle_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

STResShowSmooth::~STResShowSmooth() {
  // @@protoc_insertion_point(destructor:STResShowSmooth)
  SharedDtor();
}

void STResShowSmooth::SharedDtor() {
  if (this != default_instance_) {
  }
}

void STResShowSmooth::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* STResShowSmooth::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return STResShowSmooth_descriptor_;
}

const STResShowSmooth& STResShowSmooth::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_comstruct_2eproto();
  return *default_instance_;
}

STResShowSmooth* STResShowSmooth::default_instance_ = NULL;

STResShowSmooth* STResShowSmooth::New() const {
  return new STResShowSmooth;
}

void STResShowSmooth::Clear() {
  cirle_ = 0;
  value_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool STResShowSmooth::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:STResShowSmooth)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .ReactCircle cirle = 1;
      case 1: {
        if (tag == 8) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::ReactCircle_IsValid(value)) {
            set_cirle(static_cast< ::ReactCircle >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_value;
        break;
      }

      // repeated .adOP value = 2;
      case 2: {
        if (tag == 18) {
         parse_value:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_value()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_value;
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:STResShowSmooth)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:STResShowSmooth)
  return false;
#undef DO_
}

void STResShowSmooth::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:STResShowSmooth)
  // optional .ReactCircle cirle = 1;
  if (has_cirle()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->cirle(), output);
  }

  // repeated .adOP value = 2;
  for (int i = 0; i < this->value_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->value(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:STResShowSmooth)
}

::google::protobuf::uint8* STResShowSmooth::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:STResShowSmooth)
  // optional .ReactCircle cirle = 1;
  if (has_cirle()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->cirle(), target);
  }

  // repeated .adOP value = 2;
  for (int i = 0; i < this->value_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->value(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:STResShowSmooth)
  return target;
}

int STResShowSmooth::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .ReactCircle cirle = 1;
    if (has_cirle()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->cirle());
    }

  }
  // repeated .adOP value = 2;
  total_size += 1 * this->value_size();
  for (int i = 0; i < this->value_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->value(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void STResShowSmooth::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const STResShowSmooth* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const STResShowSmooth*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void STResShowSmooth::MergeFrom(const STResShowSmooth& from) {
  GOOGLE_CHECK_NE(&from, this);
  value_.MergeFrom(from.value_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_cirle()) {
      set_cirle(from.cirle());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void STResShowSmooth::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void STResShowSmooth::CopyFrom(const STResShowSmooth& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool STResShowSmooth::IsInitialized() const {

  return true;
}

void STResShowSmooth::Swap(STResShowSmooth* other) {
  if (other != this) {
    std::swap(cirle_, other->cirle_);
    value_.Swap(&other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata STResShowSmooth::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = STResShowSmooth_descriptor_;
  metadata.reflection = STResShowSmooth_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
