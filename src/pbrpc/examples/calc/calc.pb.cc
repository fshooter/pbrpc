// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: calc.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "calc.pb.h"

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
namespace examples {
namespace calc {

namespace {

const ::google::protobuf::Descriptor* CalcRequest_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  CalcRequest_reflection_ = NULL;
const ::google::protobuf::Descriptor* CalcResponse_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  CalcResponse_reflection_ = NULL;
const ::google::protobuf::ServiceDescriptor* CalcService_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_calc_2eproto() {
  protobuf_AddDesc_calc_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "calc.proto");
  GOOGLE_CHECK(file != NULL);
  CalcRequest_descriptor_ = file->message_type(0);
  static const int CalcRequest_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CalcRequest, nums_),
  };
  CalcRequest_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      CalcRequest_descriptor_,
      CalcRequest::default_instance_,
      CalcRequest_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CalcRequest, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CalcRequest, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(CalcRequest));
  CalcResponse_descriptor_ = file->message_type(1);
  static const int CalcResponse_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CalcResponse, result_),
  };
  CalcResponse_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      CalcResponse_descriptor_,
      CalcResponse::default_instance_,
      CalcResponse_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CalcResponse, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CalcResponse, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(CalcResponse));
  CalcService_descriptor_ = file->service(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_calc_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    CalcRequest_descriptor_, &CalcRequest::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    CalcResponse_descriptor_, &CalcResponse::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_calc_2eproto() {
  delete CalcRequest::default_instance_;
  delete CalcRequest_reflection_;
  delete CalcResponse::default_instance_;
  delete CalcResponse_reflection_;
}

void protobuf_AddDesc_calc_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\ncalc.proto\022\023pbrpc.examples.calc\"\033\n\013Cal"
    "cRequest\022\014\n\004nums\030\001 \003(\005\"\036\n\014CalcResponse\022\016"
    "\n\006result\030\001 \002(\0052Z\n\013CalcService\022K\n\004Calc\022 ."
    "pbrpc.examples.calc.CalcRequest\032!.pbrpc."
    "examples.calc.CalcResponseB\003\200\001\001", 191);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "calc.proto", &protobuf_RegisterTypes);
  CalcRequest::default_instance_ = new CalcRequest();
  CalcResponse::default_instance_ = new CalcResponse();
  CalcRequest::default_instance_->InitAsDefaultInstance();
  CalcResponse::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_calc_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_calc_2eproto {
  StaticDescriptorInitializer_calc_2eproto() {
    protobuf_AddDesc_calc_2eproto();
  }
} static_descriptor_initializer_calc_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int CalcRequest::kNumsFieldNumber;
#endif  // !_MSC_VER

CalcRequest::CalcRequest()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void CalcRequest::InitAsDefaultInstance() {
}

CalcRequest::CalcRequest(const CalcRequest& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void CalcRequest::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

CalcRequest::~CalcRequest() {
  SharedDtor();
}

void CalcRequest::SharedDtor() {
  if (this != default_instance_) {
  }
}

void CalcRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CalcRequest::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CalcRequest_descriptor_;
}

const CalcRequest& CalcRequest::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_calc_2eproto();
  return *default_instance_;
}

CalcRequest* CalcRequest::default_instance_ = NULL;

CalcRequest* CalcRequest::New() const {
  return new CalcRequest;
}

void CalcRequest::Clear() {
  nums_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool CalcRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated int32 nums = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_nums:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 8, input, this->mutable_nums())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_nums())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(8)) goto parse_nums;
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

void CalcRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated int32 nums = 1;
  for (int i = 0; i < this->nums_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      1, this->nums(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* CalcRequest::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated int32 nums = 1;
  for (int i = 0; i < this->nums_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(1, this->nums(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int CalcRequest::ByteSize() const {
  int total_size = 0;

  // repeated int32 nums = 1;
  {
    int data_size = 0;
    for (int i = 0; i < this->nums_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->nums(i));
    }
    total_size += 1 * this->nums_size() + data_size;
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

void CalcRequest::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const CalcRequest* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const CalcRequest*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void CalcRequest::MergeFrom(const CalcRequest& from) {
  GOOGLE_CHECK_NE(&from, this);
  nums_.MergeFrom(from.nums_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void CalcRequest::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CalcRequest::CopyFrom(const CalcRequest& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CalcRequest::IsInitialized() const {

  return true;
}

void CalcRequest::Swap(CalcRequest* other) {
  if (other != this) {
    nums_.Swap(&other->nums_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata CalcRequest::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = CalcRequest_descriptor_;
  metadata.reflection = CalcRequest_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int CalcResponse::kResultFieldNumber;
#endif  // !_MSC_VER

CalcResponse::CalcResponse()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void CalcResponse::InitAsDefaultInstance() {
}

CalcResponse::CalcResponse(const CalcResponse& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void CalcResponse::SharedCtor() {
  _cached_size_ = 0;
  result_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

CalcResponse::~CalcResponse() {
  SharedDtor();
}

void CalcResponse::SharedDtor() {
  if (this != default_instance_) {
  }
}

void CalcResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CalcResponse::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CalcResponse_descriptor_;
}

const CalcResponse& CalcResponse::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_calc_2eproto();
  return *default_instance_;
}

CalcResponse* CalcResponse::default_instance_ = NULL;

CalcResponse* CalcResponse::New() const {
  return new CalcResponse;
}

void CalcResponse::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    result_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool CalcResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 result = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &result_)));
          set_has_result();
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

void CalcResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 result = 1;
  if (has_result()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->result(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* CalcResponse::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 result = 1;
  if (has_result()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->result(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int CalcResponse::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 result = 1;
    if (has_result()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->result());
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

void CalcResponse::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const CalcResponse* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const CalcResponse*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void CalcResponse::MergeFrom(const CalcResponse& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_result()) {
      set_result(from.result());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void CalcResponse::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void CalcResponse::CopyFrom(const CalcResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CalcResponse::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void CalcResponse::Swap(CalcResponse* other) {
  if (other != this) {
    std::swap(result_, other->result_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata CalcResponse::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = CalcResponse_descriptor_;
  metadata.reflection = CalcResponse_reflection_;
  return metadata;
}


// ===================================================================

CalcService::~CalcService() {}

const ::google::protobuf::ServiceDescriptor* CalcService::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CalcService_descriptor_;
}

const ::google::protobuf::ServiceDescriptor* CalcService::GetDescriptor() {
  protobuf_AssignDescriptorsOnce();
  return CalcService_descriptor_;
}

void CalcService::Calc(::google::protobuf::RpcController* controller,
                         const ::pbrpc::examples::calc::CalcRequest*,
                         ::pbrpc::examples::calc::CalcResponse*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method Calc() not implemented.");
  done->Run();
}

void CalcService::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                             ::google::protobuf::RpcController* controller,
                             const ::google::protobuf::Message* request,
                             ::google::protobuf::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), CalcService_descriptor_);
  switch(method->index()) {
    case 0:
      Calc(controller,
             ::google::protobuf::down_cast<const ::pbrpc::examples::calc::CalcRequest*>(request),
             ::google::protobuf::down_cast< ::pbrpc::examples::calc::CalcResponse*>(response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::google::protobuf::Message& CalcService::GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::pbrpc::examples::calc::CalcRequest::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);
  }
}

const ::google::protobuf::Message& CalcService::GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::pbrpc::examples::calc::CalcResponse::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);
  }
}

CalcService_Stub::CalcService_Stub(::google::protobuf::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
CalcService_Stub::CalcService_Stub(
    ::google::protobuf::RpcChannel* channel,
    ::google::protobuf::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::google::protobuf::Service::STUB_OWNS_CHANNEL) {}
CalcService_Stub::~CalcService_Stub() {
  if (owns_channel_) delete channel_;
}

void CalcService_Stub::Calc(::google::protobuf::RpcController* controller,
                              const ::pbrpc::examples::calc::CalcRequest* request,
                              ::pbrpc::examples::calc::CalcResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace calc
}  // namespace examples
}  // namespace pbrpc

// @@protoc_insertion_point(global_scope)
