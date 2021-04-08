// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: privatestruct.proto

#ifndef PROTOBUF_privatestruct_2eproto__INCLUDED
#define PROTOBUF_privatestruct_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_privatestruct_2eproto();
void protobuf_AssignDesc_privatestruct_2eproto();
void protobuf_ShutdownFile_privatestruct_2eproto();

class STRAct;
class STSubActInfo;

// ===================================================================

class STRAct : public ::google::protobuf::Message {
 public:
  STRAct();
  virtual ~STRAct();

  STRAct(const STRAct& from);

  inline STRAct& operator=(const STRAct& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const STRAct& default_instance();

  void Swap(STRAct* other);

  // implements Message ----------------------------------------------

  STRAct* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const STRAct& from);
  void MergeFrom(const STRAct& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint32 uiSubSys = 1;
  inline bool has_uisubsys() const;
  inline void clear_uisubsys();
  static const int kUiSubSysFieldNumber = 1;
  inline ::google::protobuf::uint32 uisubsys() const;
  inline void set_uisubsys(::google::protobuf::uint32 value);

  // optional uint32 uiActType = 2;
  inline bool has_uiacttype() const;
  inline void clear_uiacttype();
  static const int kUiActTypeFieldNumber = 2;
  inline ::google::protobuf::uint32 uiacttype() const;
  inline void set_uiacttype(::google::protobuf::uint32 value);

  // optional uint32 uiActIndex = 3;
  inline bool has_uiactindex() const;
  inline void clear_uiactindex();
  static const int kUiActIndexFieldNumber = 3;
  inline ::google::protobuf::uint32 uiactindex() const;
  inline void set_uiactindex(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:STRAct)
 private:
  inline void set_has_uisubsys();
  inline void clear_has_uisubsys();
  inline void set_has_uiacttype();
  inline void clear_has_uiacttype();
  inline void set_has_uiactindex();
  inline void clear_has_uiactindex();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 uisubsys_;
  ::google::protobuf::uint32 uiacttype_;
  ::google::protobuf::uint32 uiactindex_;
  friend void  protobuf_AddDesc_privatestruct_2eproto();
  friend void protobuf_AssignDesc_privatestruct_2eproto();
  friend void protobuf_ShutdownFile_privatestruct_2eproto();

  void InitAsDefaultInstance();
  static STRAct* default_instance_;
};
// -------------------------------------------------------------------

class STSubActInfo : public ::google::protobuf::Message {
 public:
  STSubActInfo();
  virtual ~STSubActInfo();

  STSubActInfo(const STSubActInfo& from);

  inline STSubActInfo& operator=(const STSubActInfo& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const STSubActInfo& default_instance();

  void Swap(STSubActInfo* other);

  // implements Message ----------------------------------------------

  STSubActInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const STSubActInfo& from);
  void MergeFrom(const STSubActInfo& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bool bDone = 1 [default = true];
  inline bool has_bdone() const;
  inline void clear_bdone();
  static const int kBDoneFieldNumber = 1;
  inline bool bdone() const;
  inline void set_bdone(bool value);

  // optional uint32 uiActType = 2;
  inline bool has_uiacttype() const;
  inline void clear_uiacttype();
  static const int kUiActTypeFieldNumber = 2;
  inline ::google::protobuf::uint32 uiacttype() const;
  inline void set_uiacttype(::google::protobuf::uint32 value);

  // repeated .STRAct stRelyAct = 3;
  inline int strelyact_size() const;
  inline void clear_strelyact();
  static const int kStRelyActFieldNumber = 3;
  inline const ::STRAct& strelyact(int index) const;
  inline ::STRAct* mutable_strelyact(int index);
  inline ::STRAct* add_strelyact();
  inline const ::google::protobuf::RepeatedPtrField< ::STRAct >&
      strelyact() const;
  inline ::google::protobuf::RepeatedPtrField< ::STRAct >*
      mutable_strelyact();

  // @@protoc_insertion_point(class_scope:STSubActInfo)
 private:
  inline void set_has_bdone();
  inline void clear_has_bdone();
  inline void set_has_uiacttype();
  inline void clear_has_uiacttype();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  bool bdone_;
  ::google::protobuf::uint32 uiacttype_;
  ::google::protobuf::RepeatedPtrField< ::STRAct > strelyact_;
  friend void  protobuf_AddDesc_privatestruct_2eproto();
  friend void protobuf_AssignDesc_privatestruct_2eproto();
  friend void protobuf_ShutdownFile_privatestruct_2eproto();

  void InitAsDefaultInstance();
  static STSubActInfo* default_instance_;
};
// ===================================================================


// ===================================================================

// STRAct

// optional uint32 uiSubSys = 1;
inline bool STRAct::has_uisubsys() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void STRAct::set_has_uisubsys() {
  _has_bits_[0] |= 0x00000001u;
}
inline void STRAct::clear_has_uisubsys() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void STRAct::clear_uisubsys() {
  uisubsys_ = 0u;
  clear_has_uisubsys();
}
inline ::google::protobuf::uint32 STRAct::uisubsys() const {
  // @@protoc_insertion_point(field_get:STRAct.uiSubSys)
  return uisubsys_;
}
inline void STRAct::set_uisubsys(::google::protobuf::uint32 value) {
  set_has_uisubsys();
  uisubsys_ = value;
  // @@protoc_insertion_point(field_set:STRAct.uiSubSys)
}

// optional uint32 uiActType = 2;
inline bool STRAct::has_uiacttype() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void STRAct::set_has_uiacttype() {
  _has_bits_[0] |= 0x00000002u;
}
inline void STRAct::clear_has_uiacttype() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void STRAct::clear_uiacttype() {
  uiacttype_ = 0u;
  clear_has_uiacttype();
}
inline ::google::protobuf::uint32 STRAct::uiacttype() const {
  // @@protoc_insertion_point(field_get:STRAct.uiActType)
  return uiacttype_;
}
inline void STRAct::set_uiacttype(::google::protobuf::uint32 value) {
  set_has_uiacttype();
  uiacttype_ = value;
  // @@protoc_insertion_point(field_set:STRAct.uiActType)
}

// optional uint32 uiActIndex = 3;
inline bool STRAct::has_uiactindex() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void STRAct::set_has_uiactindex() {
  _has_bits_[0] |= 0x00000004u;
}
inline void STRAct::clear_has_uiactindex() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void STRAct::clear_uiactindex() {
  uiactindex_ = 0u;
  clear_has_uiactindex();
}
inline ::google::protobuf::uint32 STRAct::uiactindex() const {
  // @@protoc_insertion_point(field_get:STRAct.uiActIndex)
  return uiactindex_;
}
inline void STRAct::set_uiactindex(::google::protobuf::uint32 value) {
  set_has_uiactindex();
  uiactindex_ = value;
  // @@protoc_insertion_point(field_set:STRAct.uiActIndex)
}

// -------------------------------------------------------------------

// STSubActInfo

// optional bool bDone = 1 [default = true];
inline bool STSubActInfo::has_bdone() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void STSubActInfo::set_has_bdone() {
  _has_bits_[0] |= 0x00000001u;
}
inline void STSubActInfo::clear_has_bdone() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void STSubActInfo::clear_bdone() {
  bdone_ = true;
  clear_has_bdone();
}
inline bool STSubActInfo::bdone() const {
  // @@protoc_insertion_point(field_get:STSubActInfo.bDone)
  return bdone_;
}
inline void STSubActInfo::set_bdone(bool value) {
  set_has_bdone();
  bdone_ = value;
  // @@protoc_insertion_point(field_set:STSubActInfo.bDone)
}

// optional uint32 uiActType = 2;
inline bool STSubActInfo::has_uiacttype() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void STSubActInfo::set_has_uiacttype() {
  _has_bits_[0] |= 0x00000002u;
}
inline void STSubActInfo::clear_has_uiacttype() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void STSubActInfo::clear_uiacttype() {
  uiacttype_ = 0u;
  clear_has_uiacttype();
}
inline ::google::protobuf::uint32 STSubActInfo::uiacttype() const {
  // @@protoc_insertion_point(field_get:STSubActInfo.uiActType)
  return uiacttype_;
}
inline void STSubActInfo::set_uiacttype(::google::protobuf::uint32 value) {
  set_has_uiacttype();
  uiacttype_ = value;
  // @@protoc_insertion_point(field_set:STSubActInfo.uiActType)
}

// repeated .STRAct stRelyAct = 3;
inline int STSubActInfo::strelyact_size() const {
  return strelyact_.size();
}
inline void STSubActInfo::clear_strelyact() {
  strelyact_.Clear();
}
inline const ::STRAct& STSubActInfo::strelyact(int index) const {
  // @@protoc_insertion_point(field_get:STSubActInfo.stRelyAct)
  return strelyact_.Get(index);
}
inline ::STRAct* STSubActInfo::mutable_strelyact(int index) {
  // @@protoc_insertion_point(field_mutable:STSubActInfo.stRelyAct)
  return strelyact_.Mutable(index);
}
inline ::STRAct* STSubActInfo::add_strelyact() {
  // @@protoc_insertion_point(field_add:STSubActInfo.stRelyAct)
  return strelyact_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::STRAct >&
STSubActInfo::strelyact() const {
  // @@protoc_insertion_point(field_list:STSubActInfo.stRelyAct)
  return strelyact_;
}
inline ::google::protobuf::RepeatedPtrField< ::STRAct >*
STSubActInfo::mutable_strelyact() {
  // @@protoc_insertion_point(field_mutable_list:STSubActInfo.stRelyAct)
  return &strelyact_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_privatestruct_2eproto__INCLUDED