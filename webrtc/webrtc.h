#ifndef JUST_WEBRTC_H
#define JUST_WEBRTC_H

#include "just.h"
#include <rtc/rtc.h>

namespace just {
namespace webrtc {

typedef struct {
	rtcState state;
	rtcGatheringState gatheringState;
	int pc;
	int dc;
	bool connected;
} Peer;

struct rtcConnHandler {
	v8::Persistent<Function, v8::NonCopyablePersistentTraits<Function>> onDescription;
	v8::Persistent<Function, v8::NonCopyablePersistentTraits<Function>> onCandidate;
	v8::Persistent<Function, v8::NonCopyablePersistentTraits<Function>> onDataChannel;
	v8::Persistent<Function, v8::NonCopyablePersistentTraits<Function>> onMessage;
	v8::Persistent<Function, v8::NonCopyablePersistentTraits<Function>> onClose;
	Isolate* isolate;
	const char* sdp;
	const char* type;
	const char* candidates[16];
	const char* mids[16];
	const char* label;
	int numCandidates;
	const char* message;
	int messageSize;
	Peer* peer;
};

static void RTC_API dataChannelCallback(int pc, int dc, void *ptr);
static void RTC_API descriptionCallback(int pc, const char *sdp, const char *type, void *ptr);
static void RTC_API candidateCallback(int pc, const char *cand, const char *mid, void *ptr);
static void RTC_API stateChangeCallback(int pc, rtcState state, void *ptr);
static void RTC_API gatheringStateCallback(int pc, rtcGatheringState state, void *ptr);
static void RTC_API closedCallback(int id, void *ptr);
static void RTC_API messageCallback(int id, const char *message, int size, void *ptr);
static void RTC_API openCallback(int id, void *ptr);

void InitLogger(const FunctionCallbackInfo<Value> &args);
void OnMessage(const FunctionCallbackInfo<Value> &args);
void Send(const FunctionCallbackInfo<Value> &args);
void CreateConnection(const FunctionCallbackInfo<Value> &args);
void CreateOffer(const FunctionCallbackInfo<Value> &args);
void SetRemoteDescription(const FunctionCallbackInfo<Value> &args);
void AddCandidate(const FunctionCallbackInfo<Value> &args);
void GetDescription(const FunctionCallbackInfo<Value> &args);
void GetCandidates(const FunctionCallbackInfo<Value> &args);
void GetLabel(const FunctionCallbackInfo<Value> &args);
void GetMessage(const FunctionCallbackInfo<Value> &args);
void GetState(const FunctionCallbackInfo<Value> &args);
void GetGatheringState(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);

}

}

extern "C" {
	void* _register_webrtc() {
		return (void*)just::webrtc::Init;
	}
}

#endif
