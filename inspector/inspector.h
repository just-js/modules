#ifndef JUST_INSPECTOR_H
#define JUST_INSPECTOR_H

#include <just.h>
#include <v8-inspector.h>

namespace just {

namespace inspector {

using v8_inspector::V8InspectorClient;
using v8_inspector::V8Inspector;
using v8_inspector::StringBuffer;
using v8_inspector::StringView;
using v8_inspector::V8ContextInfo;
using v8_inspector::V8InspectorSession;
using v8_inspector::V8Inspector;
using v8::Global;

const int kInspectorClientIndex = 33;

class SymbolInfo {
  public:
  std::string name;
  std::string filename;
  size_t line = 0;
  size_t dis = 0;
};

class InspectorFrontend final : public V8Inspector::Channel {
 public:

  explicit InspectorFrontend(Local<Context> context) {
    isolate_ = context->GetIsolate();
    context_.Reset(isolate_, context);
  }

  ~InspectorFrontend() override = default;

 private:

  void sendResponse(int callId, std::unique_ptr<StringBuffer> message) override {
    Send(message->string());
  }

  void sendNotification(std::unique_ptr<StringBuffer> message) override {
    Send(message->string());
  }

  void flushProtocolNotifications() override {}

  void Send(const v8_inspector::StringView& string) {
    v8::Isolate::AllowJavascriptExecutionScope allow_script(isolate_);
    int length = static_cast<int>(string.length());
    Local<String> message = (string.is8Bit() ? 
      v8::String::NewFromOneByte(isolate_, 
      reinterpret_cast<const uint8_t*>(string.characters8()), 
      v8::NewStringType::kNormal, length) : 
      v8::String::NewFromTwoByte(isolate_, 
      reinterpret_cast<const uint16_t*>(string.characters16()), 
      v8::NewStringType::kNormal, length)).ToLocalChecked();
    Local<String> callback_name = v8::String::NewFromUtf8Literal(isolate_, 
      "receive", v8::NewStringType::kNormal);
    Local<Context> context = context_.Get(isolate_);
    Local<Value> callback = context->Global()->Get(context, 
      callback_name).ToLocalChecked();
    if (callback->IsFunction()) {
      v8::TryCatch try_catch(isolate_);
      Local<Value> args[] = {message};
      Local<Function>::Cast(callback)->Call(context, Undefined(isolate_), 1, 
        args).FromMaybe(Local<Value>());
    }
  }

  Isolate* isolate_;
  Global<Context> context_;
};

class InspectorClient : public V8InspectorClient {
 public:
  InspectorClient(Local<Context> context, bool connect) {
    if (!connect) return;
    isolate_ = context->GetIsolate();
    channel_.reset(new InspectorFrontend(context));
    inspector_ = V8Inspector::create(isolate_, this);
    session_ = inspector_->connect(1, channel_.get(), StringView());
    context->SetAlignedPointerInEmbedderData(kInspectorClientIndex, this);
    inspector_->contextCreated(V8ContextInfo(context, kContextGroupId, 
      StringView()));

    Local<Value> function = FunctionTemplate::New(isolate_, 
      SendInspectorMessage)->GetFunction(context).ToLocalChecked();
    Local<String> function_name = String::NewFromUtf8Literal(isolate_, 
      "send", NewStringType::kNormal);
    context->Global()->Set(context, function_name, function).FromJust();
    context_.Reset(isolate_, context);
  }

  void runMessageLoopOnPause(int context_group_id) override {
    Local<String> callback_name = v8::String::NewFromUtf8Literal(isolate_, 
      "onRunMessageLoop", v8::NewStringType::kNormal);
    Local<Context> context = context_.Get(isolate_);
    Local<Value> callback = context->Global()->Get(context, 
      callback_name).ToLocalChecked();
    if (callback->IsFunction()) {
      v8::TryCatch try_catch(isolate_);
      Local<Value> args[1] = {Integer::New(isolate_, 0)};
      Local<Function>::Cast(callback)->Call(context, Undefined(isolate_), 0, 
        args).FromMaybe(Local<Value>());
    }
  }

  void quitMessageLoopOnPause() override {
    Local<String> callback_name = v8::String::NewFromUtf8Literal(isolate_, 
      "onQuitMessageLoop", v8::NewStringType::kNormal);
    Local<Context> context = context_.Get(isolate_);
    Local<Value> callback = context->Global()->Get(context, 
      callback_name).ToLocalChecked();
    if (callback->IsFunction()) {
      v8::TryCatch try_catch(isolate_);
      Local<Value> args[1] = {Integer::New(isolate_, 0)};
      Local<Function>::Cast(callback)->Call(context, Undefined(isolate_), 0, 
        args).FromMaybe(Local<Value>());
    }
  }

 private:

  static V8InspectorSession* GetSession(Local<Context> context) {
    InspectorClient* inspector_client = 
      static_cast<InspectorClient*>(
        context->GetAlignedPointerFromEmbedderData(kInspectorClientIndex));
    return inspector_client->session_.get();
  }

  Local<Context> ensureDefaultContextInGroup(int group_id) override {
    return context_.Get(isolate_);
  }

  static void SendInspectorMessage(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope handle_scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    args.GetReturnValue().Set(Undefined(isolate));
    Local<String> message = args[0]->ToString(context).ToLocalChecked();
    V8InspectorSession* session = InspectorClient::GetSession(context);
    int length = message->Length();
    std::unique_ptr<uint16_t[]> buffer(new uint16_t[length]);
    message->Write(isolate, buffer.get(), 0, length);
    StringView message_view(buffer.get(), length);
    session->dispatchProtocolMessage(message_view);
    args.GetReturnValue().Set(True(isolate));
  }

  static const int kContextGroupId = 1;
  std::unique_ptr<V8Inspector> inspector_;
  std::unique_ptr<V8InspectorSession> session_;
  std::unique_ptr<V8Inspector::Channel> channel_;
  Global<Context> context_;
  Isolate* isolate_;
};

void Enable(const FunctionCallbackInfo<Value> &args);
void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_inspector() {
		return (void*)just::inspector::Init;
	}
}

#endif
