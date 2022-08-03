#include "webrtc.h"

void RTC_API just::webrtc::descriptionCallback(int pc, const char *sdp, const char *type, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
  Isolate* isolate = handler->isolate;
  handler->sdp = strdup(sdp);
  handler->type = strdup(type);
  handler->numCandidates = 0;
  isolate->EnqueueMicrotask(Local<Function>::New(isolate, handler->onDescription));
}

void RTC_API just::webrtc::candidateCallback(int pc, const char *cand, const char *mid, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
  Isolate* isolate = handler->isolate;
  handler->candidates[handler->numCandidates] = strdup(cand);
  handler->mids[handler->numCandidates] = strdup(mid);
  handler->numCandidates++;
  isolate->EnqueueMicrotask(Local<Function>::New(isolate, handler->onCandidate));
}

void RTC_API just::webrtc::closedCallback(int id, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
  Isolate* isolate = handler->isolate;
	Peer *peer = handler->peer;
	peer->connected = false;
  isolate->EnqueueMicrotask(Local<Function>::New(isolate, handler->onClose));
}

void RTC_API just::webrtc::messageCallback(int id, const char *message, int size, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
  Isolate* isolate = handler->isolate;
  handler->messageSize = size;
  if (size > 0) {
    void* m = calloc(1, size);
    memcpy(m, message, size);
    handler->message = (const char*)m;
  } else if (size < 0) {
    handler->message = strdup(message);
  }
  isolate->EnqueueMicrotask(Local<Function>::New(isolate, handler->onMessage));
}

void RTC_API just::webrtc::openCallback(int id, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
  Isolate* isolate = handler->isolate;
	Peer *peer = handler->peer;
	peer->connected = true;
	char buffer[256];
	if (rtcGetDataChannelLabel(peer->dc, buffer, 256) >= 0) {
    handler->label = strdup(buffer);
  }
  isolate->EnqueueMicrotask(Local<Function>::New(isolate, handler->onDataChannel));
}

void RTC_API just::webrtc::dataChannelCallback(int pc, int dc, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
  Isolate* isolate = handler->isolate;
	Peer *peer = handler->peer;
	peer->dc = dc;
	peer->connected = true;
	rtcSetClosedCallback(dc, closedCallback);
	rtcSetMessageCallback(dc, messageCallback);
	char buffer[256];
	if (rtcGetDataChannelLabel(dc, buffer, 256) >= 0) {
    handler->label = strdup(buffer);
  }
  isolate->EnqueueMicrotask(Local<Function>::New(isolate, handler->onDataChannel));
}

static void RTC_API just::webrtc::stateChangeCallback(int pc, rtcState state, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
	Peer *peer = handler->peer;
	peer->state = state;
}

static void RTC_API just::webrtc::gatheringStateCallback(int pc, rtcGatheringState state, void *ptr) {
  rtcConnHandler* handler = (rtcConnHandler*)ptr;
	Peer *peer = handler->peer;
	peer->gatheringState = state;
}

void just::webrtc::Send(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
	Peer *peer = connection->peer;
  if (args[1]->IsString()) {
    String::Utf8Value message(isolate, args[1]);
    rtcSendMessage(peer->dc, *message, -1);
  } else {
    Local<ArrayBuffer> ab = args[1].As<ArrayBuffer>();
    std::shared_ptr<BackingStore> backing = ab->GetBackingStore();
    rtcSendMessage(peer->dc, (const char*)backing->Data(), ab->ByteLength());
  }
}

void just::webrtc::CreateConnection(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> peerTemplate = ObjectTemplate::New(isolate);
  peerTemplate->SetInternalFieldCount(1);
  Local<Object> peerObj = peerTemplate->NewInstance(context).ToLocalChecked();
	Peer *peer = (Peer *)calloc(1, sizeof(Peer));
  just::webrtc::rtcConnHandler* connection = new just::webrtc::rtcConnHandler();
  connection->isolate = isolate;
  connection->peer = peer;
  String::Utf8Value stun(isolate, args[0]);
	rtcConfiguration config;
	memset(&config, 0, sizeof(config));
  config.iceServersCount = 1;
	const char *iceServers[1] = {*stun};
  config.iceServers = iceServers;
	peer->pc = rtcCreatePeerConnection(&config);
  connection->onDescription.Reset(isolate, args[1].As<Function>());
  connection->onCandidate.Reset(isolate, args[2].As<Function>());
  connection->onDataChannel.Reset(isolate, args[3].As<Function>());
  connection->onMessage.Reset(isolate, args[4].As<Function>());
  connection->onClose.Reset(isolate, args[5].As<Function>());
	rtcSetLocalDescriptionCallback(peer->pc, descriptionCallback);
	rtcSetLocalCandidateCallback(peer->pc, candidateCallback);
	rtcSetDataChannelCallback(peer->pc, dataChannelCallback);
	rtcSetStateChangeCallback(peer->pc, stateChangeCallback);
	rtcSetGatheringStateChangeCallback(peer->pc, gatheringStateCallback);
	rtcSetUserPointer(peer->pc, connection);
  peerObj->SetAlignedPointerInInternalField(0, connection);
  args.GetReturnValue().Set(peerObj);
}

void just::webrtc::SetRemoteDescription(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  int pc = connection->peer->pc;
  String::Utf8Value sdp(isolate, args[1]);
  String::Utf8Value type(isolate, args[2]);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), rtcSetRemoteDescription(pc, *sdp, *type)));
}

void just::webrtc::CreateOffer(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  String::Utf8Value name(isolate, args[1]);
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  Peer* peer = connection->peer;
	peer->dc = rtcCreateDataChannel(peer->pc, *name);
	rtcSetClosedCallback(peer->dc, closedCallback);
	rtcSetMessageCallback(peer->dc, messageCallback);
	rtcSetOpenCallback(peer->dc, openCallback);
}

void just::webrtc::AddCandidate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  int pc = connection->peer->pc;
  String::Utf8Value candidate(isolate, args[1]);
  String::Utf8Value mid(isolate, args[2]);
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), rtcAddRemoteCandidate(pc, *candidate, *mid)));
}

void just::webrtc::GetLabel(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, connection->label, 
    NewStringType::kNormal, strnlen(connection->label, 256)).ToLocalChecked());
}

void just::webrtc::GetMessage(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  if (connection->messageSize < 0) {
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, connection->message, 
      NewStringType::kNormal, strnlen(connection->message, 65536)).ToLocalChecked());
  } else {
    if (connection->messageSize == 0) return;
    std::unique_ptr<BackingStore> backing = ArrayBuffer::NewBackingStore((void*)connection->message, connection->messageSize, 
        just::FreeMemory, nullptr);
    Local<ArrayBuffer> ab =
        ArrayBuffer::New(isolate, std::move(backing));
    args.GetReturnValue().Set(ab);
  }
  connection->messageSize = 0;
}

void just::webrtc::GetState(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(Integer::New(isolate, connection->peer->state));
}

void just::webrtc::GetGatheringState(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  args.GetReturnValue().Set(Integer::New(isolate, connection->peer->gatheringState));
}

void just::webrtc::GetDescription(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  Local<Array> result = Array::New(isolate);
  result->Set(context, 0, String::NewFromUtf8(isolate, connection->sdp, 
    NewStringType::kNormal, strnlen(connection->sdp, 65536)).ToLocalChecked()).Check();
  result->Set(context, 1, String::NewFromUtf8(isolate, connection->type, 
    NewStringType::kNormal, strnlen(connection->type, 256)).ToLocalChecked()).Check();
  args.GetReturnValue().Set(result);
}

void just::webrtc::GetCandidates(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> peerObj = args[0].As<Object>();
  just::webrtc::rtcConnHandler* connection = (just::webrtc::rtcConnHandler*)peerObj->GetAlignedPointerFromInternalField(0);
  Local<Array> result = Array::New(isolate);
  for (int i = 0; i < connection->numCandidates; i++) {
    Local<Array> candidate = Array::New(isolate);
    candidate->Set(context, 0, String::NewFromUtf8(isolate, connection->candidates[i], 
      NewStringType::kNormal, strnlen(connection->candidates[i], 1024)).ToLocalChecked()).Check();
    candidate->Set(context, 1, String::NewFromUtf8(isolate, connection->mids[i], 
      NewStringType::kNormal, strnlen(connection->mids[i], 256)).ToLocalChecked()).Check();
    result->Set(context, i, candidate).Check();
  }
  args.GetReturnValue().Set(result);
}

void just::webrtc::InitLogger(const FunctionCallbackInfo<Value> &args) {
  int loglevel = Local<Integer>::Cast(args[0])->Value();
	rtcInitLogger((rtcLogLevel)loglevel, NULL);
}

void just::webrtc::Init(Isolate* isolate, Local<ObjectTemplate> target) {
  Local<ObjectTemplate> module = ObjectTemplate::New(isolate);

  SET_METHOD(isolate, module, "initLogger", InitLogger);

  SET_METHOD(isolate, module, "send", Send);
  SET_METHOD(isolate, module, "createConnection", CreateConnection);
  SET_METHOD(isolate, module, "createOffer", CreateOffer);
  SET_METHOD(isolate, module, "setRemoteDescription", SetRemoteDescription);
  SET_METHOD(isolate, module, "addCandidate", AddCandidate);
  SET_METHOD(isolate, module, "getDescription", GetDescription);
  SET_METHOD(isolate, module, "getCandidates", GetCandidates);
  SET_METHOD(isolate, module, "getLabel", GetLabel);
  SET_METHOD(isolate, module, "getMessage", GetMessage);
  SET_METHOD(isolate, module, "getState", GetState);
  SET_METHOD(isolate, module, "getGatheringState", GetGatheringState);

  SET_VALUE(isolate, module, "RTC_LOG_NONE", Integer::New(isolate, RTC_LOG_NONE));
  SET_VALUE(isolate, module, "RTC_LOG_FATAL", Integer::New(isolate, RTC_LOG_FATAL));
  SET_VALUE(isolate, module, "RTC_LOG_ERROR", Integer::New(isolate, RTC_LOG_ERROR));
  SET_VALUE(isolate, module, "RTC_LOG_WARNING", Integer::New(isolate, RTC_LOG_WARNING));
  SET_VALUE(isolate, module, "RTC_LOG_INFO", Integer::New(isolate, RTC_LOG_INFO));
  SET_VALUE(isolate, module, "RTC_LOG_DEBUG", Integer::New(isolate, RTC_LOG_DEBUG));
  SET_VALUE(isolate, module, "RTC_LOG_VERBOSE", Integer::New(isolate, RTC_LOG_VERBOSE));

  SET_VALUE(isolate, module, "RTC_NEW", Integer::New(isolate, RTC_NEW));
  SET_VALUE(isolate, module, "RTC_CONNECTING", Integer::New(isolate, RTC_CONNECTING));
  SET_VALUE(isolate, module, "RTC_CONNECTED", Integer::New(isolate, RTC_CONNECTED));
  SET_VALUE(isolate, module, "RTC_DISCONNECTED", Integer::New(isolate, RTC_DISCONNECTED));
  SET_VALUE(isolate, module, "RTC_FAILED", Integer::New(isolate, RTC_FAILED));
  SET_VALUE(isolate, module, "RTC_CLOSED", Integer::New(isolate, RTC_CLOSED));

  SET_VALUE(isolate, module, "RTC_GATHERING_NEW", Integer::New(isolate, RTC_GATHERING_NEW));
  SET_VALUE(isolate, module, "RTC_GATHERING_INPROGRESS", Integer::New(isolate, RTC_GATHERING_INPROGRESS));
  SET_VALUE(isolate, module, "RTC_GATHERING_COMPLETE", Integer::New(isolate, RTC_GATHERING_COMPLETE));

  SET_MODULE(isolate, target, "webrtc", module);
}
