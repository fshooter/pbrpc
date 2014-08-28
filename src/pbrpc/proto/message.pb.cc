// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "message.pb.h"

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

namespace pbrpc {
namespace proto {

namespace {

const ::google::protobuf::Descriptor* RequestMsg_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RequestMsg_reflection_ = NULL;
const ::google::protobuf::Descriptor* ResponseMsg_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ResponseMsg_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* RequestMsgType_descriptor_ = NULL;
const ::google::protobuf::EnumDescriptor* ResponseMsgType_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_message_2eproto() {
  protobuf_AddDesc_message_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "message.proto");
  GOOGLE_CHECK(file != NULL);
  RequestMsg_descriptor_ = file->message_type(0);
  static const int RequestMsg_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, service_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, method_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, request_body_),
  };
  RequestMsg_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RequestMsg_descriptor_,
      RequestMsg::default_instance_,
      RequestMsg_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RequestMsg, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RequestMsg));
  ResponseMsg_descriptor_ = file->message_type(1);
  static const int ResponseMsg_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResponseMsg, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResponseMsg, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResponseMsg, response_body_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResponseMsg, error_text_),
  };
  ResponseMsg_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ResponseMsg_descriptor_,
      ResponseMsg::default_instance_,
      ResponseMsg_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResponseMsg, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResponseMsg, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ResponseMsg));
  RequestMsgType_descriptor_ = file->enum_type(0);
  ResponseMsgType_descriptor_ = file->enum_type(1);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_message_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RequestMsg_descriptor_, &RequestMsg::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ResponseMsg_descriptor_, &ResponseMsg::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_message_2eproto() {
  delete RequestMsg::default_instance_;
  delete RequestMsg_reflection_;
  delete ResponseMsg::default_instance_;
  delete ResponseMsg_reflection_;
}

void protobuf_AddDesc_message_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\rmessage.proto\022\013pbrpc.proto\"\204\001\n\nRequest"
    "Msg\022\n\n\002id\030\001 \002(\005\022)\n\004type\030\002 \002(\0162\033.pbrpc.pr"
    "oto.RequestMsgType\022\024\n\014service_name\030\n \001(\t"
    "\022\023\n\013method_name\030\013 \001(\t\022\024\n\014request_body\030\014 "
    "\001(\014\"p\n\013ResponseMsg\022\n\n\002id\030\001 \002(\005\022*\n\004type\030\002"
    " \002(\0162\034.pbrpc.proto.ResponseMsgType\022\025\n\rre"
    "sponse_body\030\n \001(\014\022\022\n\nerror_text\030\013 \001(\t*(\n"
    "\016RequestMsgType\022\n\n\006NORMAL\020\000\022\n\n\006CANCEL\020\001*"
    "(\n\017ResponseMsgType\022\013\n\007SUCCESS\020\000\022\010\n\004FAIL\020"
    "\001", 361);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "message.proto", &protobuf_RegisterTypes);
  RequestMsg::default_instance_ = new RequestMsg();
  ResponseMsg::default_instance_ = new ResponseMsg();
  RequestMsg::default_instance_->InitAsDefaultInstance();
  ResponseMsg::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_message_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_message_2eproto {
  StaticDescriptorInitializer_message_2eproto() {
    protobuf_AddDesc_message_2eproto();
  }
} static_descriptor_initializer_message_2eproto_;
const ::google::protobuf::EnumDescriptor* RequestMsgType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RequestMsgType_descriptor_;
}
bool RequestMsgType_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ResponseMsgType_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ResponseMsgType_descriptor_;
}
bool ResponseMsgType_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#ifndef _MSC_VER
const int RequestMsg::kIdFieldNumber;
const int RequestMsg::kTypeFieldNumber;
const int RequestMsg::kServiceNameFieldNumber;
const int RequestMsg::kMethodNameFieldNumber;
const int RequestMsg::kRequestBodyFieldNumber;
#endif  // !_MSC_VER

RequestMsg::RequestMsg()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void RequestMsg::InitAsDefaultInstance() {
}

RequestMsg::RequestMsg(const RequestMsg& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void RequestMsg::SharedCtor() {
  _cached_size_ = 0;
  id_ = 0;
  type_ = 0;
  service_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  method_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  request_body_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RequestMsg::~RequestMsg() {
  SharedDtor();
}

void RequestMsg::SharedDtor() {
  if (service_name_ != &::google::protobuf::internal::kEmptyString) {
    delete service_name_;
  }
  if (method_name_ != &::google::protobuf::internal::kEmptyString) {
    delete method_name_;
  }
  if (request_body_ != &::google::protobuf::internal::kEmptyString) {
    delete request_body_;
  }
  if (this != default_instance_) {
  }
}

void RequestMsg::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RequestMsg::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RequestMsg_descriptor_;
}

const RequestMsg& RequestMsg::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_message_2eproto();
  return *default_instance_;
}

RequestMsg* RequestMsg::default_instance_ = NULL;

RequestMsg* RequestMsg::New() const {
  return new RequestMsg;
}

void RequestMsg::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    id_ = 0;
    type_ = 0;
    if (has_service_name()) {
      if (service_name_ != &::google::protobuf::internal::kEmptyString) {
        service_name_->clear();
      }
    }
    if (has_method_name()) {
      if (method_name_ != &::google::protobuf::internal::kEmptyString) {
        method_name_->clear();
      }
    }
    if (has_request_body()) {
      if (request_body_ != &::google::protobuf::internal::kEmptyString) {
        request_body_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool RequestMsg::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
          set_has_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_type;
        break;
      }

      // required .pbrpc.proto.RequestMsgType type = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_type:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::pbrpc::proto::RequestMsgType_IsValid(value)) {
            set_type(static_cast< ::pbrpc::proto::RequestMsgType >(value));
          } else {
            mutable_unknown_fields()->AddVarint(2, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(82)) goto parse_service_name;
        break;
      }

      // optional string service_name = 10;
      case 10: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_service_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_service_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->service_name().data(), this->service_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(90)) goto parse_method_name;
        break;
      }

      // optional string method_name = 11;
      case 11: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_method_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_method_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->method_name().data(), this->method_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(98)) goto parse_request_body;
        break;
      }

      // optional bytes request_body = 12;
      case 12: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_request_body:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_request_body()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void RequestMsg::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 id = 1;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->id(), output);
  }

  // required .pbrpc.proto.RequestMsgType type = 2;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      2, this->type(), output);
  }

  // optional string service_name = 10;
  if (has_service_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->service_name().data(), this->service_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      10, this->service_name(), output);
  }

  // optional string method_name = 11;
  if (has_method_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->method_name().data(), this->method_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      11, this->method_name(), output);
  }

  // optional bytes request_body = 12;
  if (has_request_body()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      12, this->request_body(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* RequestMsg::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 id = 1;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->id(), target);
  }

  // required .pbrpc.proto.RequestMsgType type = 2;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      2, this->type(), target);
  }

  // optional string service_name = 10;
  if (has_service_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->service_name().data(), this->service_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        10, this->service_name(), target);
  }

  // optional string method_name = 11;
  if (has_method_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->method_name().data(), this->method_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        11, this->method_name(), target);
  }

  // optional bytes request_body = 12;
  if (has_request_body()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        12, this->request_body(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int RequestMsg::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 id = 1;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->id());
    }

    // required .pbrpc.proto.RequestMsgType type = 2;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }

    // optional string service_name = 10;
    if (has_service_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->service_name());
    }

    // optional string method_name = 11;
    if (has_method_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->method_name());
    }

    // optional bytes request_body = 12;
    if (has_request_body()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->request_body());
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

void RequestMsg::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RequestMsg* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RequestMsg*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void RequestMsg::MergeFrom(const RequestMsg& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_service_name()) {
      set_service_name(from.service_name());
    }
    if (from.has_method_name()) {
      set_method_name(from.method_name());
    }
    if (from.has_request_body()) {
      set_request_body(from.request_body());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void RequestMsg::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RequestMsg::CopyFrom(const RequestMsg& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RequestMsg::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void RequestMsg::Swap(RequestMsg* other) {
  if (other != this) {
    std::swap(id_, other->id_);
    std::swap(type_, other->type_);
    std::swap(service_name_, other->service_name_);
    std::swap(method_name_, other->method_name_);
    std::swap(request_body_, other->request_body_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata RequestMsg::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RequestMsg_descriptor_;
  metadata.reflection = RequestMsg_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ResponseMsg::kIdFieldNumber;
const int ResponseMsg::kTypeFieldNumber;
const int ResponseMsg::kResponseBodyFieldNumber;
const int ResponseMsg::kErrorTextFieldNumber;
#endif  // !_MSC_VER

ResponseMsg::ResponseMsg()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ResponseMsg::InitAsDefaultInstance() {
}

ResponseMsg::ResponseMsg(const ResponseMsg& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ResponseMsg::SharedCtor() {
  _cached_size_ = 0;
  id_ = 0;
  type_ = 0;
  response_body_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  error_text_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ResponseMsg::~ResponseMsg() {
  SharedDtor();
}

void ResponseMsg::SharedDtor() {
  if (response_body_ != &::google::protobuf::internal::kEmptyString) {
    delete response_body_;
  }
  if (error_text_ != &::google::protobuf::internal::kEmptyString) {
    delete error_text_;
  }
  if (this != default_instance_) {
  }
}

void ResponseMsg::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ResponseMsg::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ResponseMsg_descriptor_;
}

const ResponseMsg& ResponseMsg::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_message_2eproto();
  return *default_instance_;
}

ResponseMsg* ResponseMsg::default_instance_ = NULL;

ResponseMsg* ResponseMsg::New() const {
  return new ResponseMsg;
}

void ResponseMsg::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    id_ = 0;
    type_ = 0;
    if (has_response_body()) {
      if (response_body_ != &::google::protobuf::internal::kEmptyString) {
        response_body_->clear();
      }
    }
    if (has_error_text()) {
      if (error_text_ != &::google::protobuf::internal::kEmptyString) {
        error_text_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ResponseMsg::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
          set_has_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_type;
        break;
      }

      // required .pbrpc.proto.ResponseMsgType type = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_type:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::pbrpc::proto::ResponseMsgType_IsValid(value)) {
            set_type(static_cast< ::pbrpc::proto::ResponseMsgType >(value));
          } else {
            mutable_unknown_fields()->AddVarint(2, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(82)) goto parse_response_body;
        break;
      }

      // optional bytes response_body = 10;
      case 10: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_response_body:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_response_body()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(90)) goto parse_error_text;
        break;
      }

      // optional string error_text = 11;
      case 11: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_error_text:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_error_text()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->error_text().data(), this->error_text().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ResponseMsg::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 id = 1;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->id(), output);
  }

  // required .pbrpc.proto.ResponseMsgType type = 2;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      2, this->type(), output);
  }

  // optional bytes response_body = 10;
  if (has_response_body()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      10, this->response_body(), output);
  }

  // optional string error_text = 11;
  if (has_error_text()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->error_text().data(), this->error_text().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      11, this->error_text(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ResponseMsg::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 id = 1;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->id(), target);
  }

  // required .pbrpc.proto.ResponseMsgType type = 2;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      2, this->type(), target);
  }

  // optional bytes response_body = 10;
  if (has_response_body()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        10, this->response_body(), target);
  }

  // optional string error_text = 11;
  if (has_error_text()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->error_text().data(), this->error_text().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        11, this->error_text(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ResponseMsg::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 id = 1;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->id());
    }

    // required .pbrpc.proto.ResponseMsgType type = 2;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }

    // optional bytes response_body = 10;
    if (has_response_body()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->response_body());
    }

    // optional string error_text = 11;
    if (has_error_text()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->error_text());
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

void ResponseMsg::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ResponseMsg* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ResponseMsg*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ResponseMsg::MergeFrom(const ResponseMsg& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_response_body()) {
      set_response_body(from.response_body());
    }
    if (from.has_error_text()) {
      set_error_text(from.error_text());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ResponseMsg::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ResponseMsg::CopyFrom(const ResponseMsg& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ResponseMsg::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void ResponseMsg::Swap(ResponseMsg* other) {
  if (other != this) {
    std::swap(id_, other->id_);
    std::swap(type_, other->type_);
    std::swap(response_body_, other->response_body_);
    std::swap(error_text_, other->error_text_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ResponseMsg::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ResponseMsg_descriptor_;
  metadata.reflection = ResponseMsg_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto
}  // namespace pbrpc

// @@protoc_insertion_point(global_scope)
