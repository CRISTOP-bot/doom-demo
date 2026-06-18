#include <node.h>
#include <v8.h>
#include <cstring>
#include "doom_core.h"

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Number;
using v8::Array;
using v8::Uint32Array;
using v8::ArrayBuffer;

static uint32_t* g_frameBuffer = nullptr;
static int g_bufferSize = 0;

void Init(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() < 2) {
    isolate->ThrowException(v8::Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
    return;
  }

  int width = args[0]->Int32Value(isolate->GetCurrentContext()).FromJust();
  int height = args[1]->Int32Value(isolate->GetCurrentContext()).FromJust();

  /* Allocate frame buffer */
  if (g_frameBuffer != nullptr) {
    delete[] g_frameBuffer;
  }
  g_bufferSize = width * height;
  g_frameBuffer = new uint32_t[g_bufferSize];

  int result = game_init(width, height);
  args.GetReturnValue().Set(Number::New(isolate, result));
}

void Update(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() < 1 || !args[0]->IsObject()) {
    isolate->ThrowException(v8::Exception::TypeError(
        String::NewFromUtf8(isolate, "Input object required").ToLocalChecked()));
    return;
  }

  Local<Object> inputObj = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
  Local<v8::Context> context = isolate->GetCurrentContext();

  InputState input = {0};
  input.w = inputObj->Get(context, String::NewFromUtf8(isolate, "w").ToLocalChecked())
      .ToLocalChecked()->Int32Value(context).FromJust();
  input.a = inputObj->Get(context, String::NewFromUtf8(isolate, "a").ToLocalChecked())
      .ToLocalChecked()->Int32Value(context).FromJust();
  input.s = inputObj->Get(context, String::NewFromUtf8(isolate, "s").ToLocalChecked())
      .ToLocalChecked()->Int32Value(context).FromJust();
  input.d = inputObj->Get(context, String::NewFromUtf8(isolate, "d").ToLocalChecked())
      .ToLocalChecked()->Int32Value(context).FromJust();
  input.mouseX = inputObj->Get(context, String::NewFromUtf8(isolate, "mouseX").ToLocalChecked())
      .ToLocalChecked()->Int32Value(context).FromJust();
  input.mouseY = inputObj->Get(context, String::NewFromUtf8(isolate, "mouseY").ToLocalChecked())
      .ToLocalChecked()->Int32Value(context).FromJust();

  game_update(&input);
  args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
}

void Render(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (g_frameBuffer == nullptr) {
    args.GetReturnValue().SetNull();
    return;
  }

  game_render(g_frameBuffer);

  GameState* state = game_get_state();
  Local<Object> result = Object::New(isolate);
  Local<v8::Context> context = isolate->GetCurrentContext();

  result->Set(context, String::NewFromUtf8(isolate, "score").ToLocalChecked(),
      Number::New(isolate, state->player.score)).FromJust();
  result->Set(context, String::NewFromUtf8(isolate, "health").ToLocalChecked(),
      Number::New(isolate, state->player.health)).FromJust();
  result->Set(context, String::NewFromUtf8(isolate, "ammo").ToLocalChecked(),
      Number::New(isolate, state->player.ammo)).ToLocalChecked();

  /* Create typed array for frame buffer */
  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(isolate, g_bufferSize * sizeof(uint32_t));
  std::memcpy(arrayBuffer->GetBackingStore()->Data(), g_frameBuffer, g_bufferSize * sizeof(uint32_t));
  Local<Uint32Array> typedArray = Uint32Array::New(arrayBuffer, 0, g_bufferSize);

  result->Set(context, String::NewFromUtf8(isolate, "frameBuffer").ToLocalChecked(),
      typedArray).FromJust();

  args.GetReturnValue().Set(result);
}

void Cleanup(const FunctionCallbackInfo<Value>& args) {
  game_cleanup();

  if (g_frameBuffer != nullptr) {
    delete[] g_frameBuffer;
    g_frameBuffer = nullptr;
  }
  g_bufferSize = 0;

  args.GetReturnValue().SetUndefined();
}

void Initialize(Local<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  Local<v8::Context> context = isolate->GetCurrentContext();

  exports->Set(context,
      String::NewFromUtf8(isolate, "init").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, Init)->GetFunction(context).ToLocalChecked())
      .FromJust();

  exports->Set(context,
      String::NewFromUtf8(isolate, "update").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, Update)->GetFunction(context).ToLocalChecked())
      .FromJust();

  exports->Set(context,
      String::NewFromUtf8(isolate, "render").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, Render)->GetFunction(context).ToLocalChecked())
      .FromJust();

  exports->Set(context,
      String::NewFromUtf8(isolate, "cleanup").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, Cleanup)->GetFunction(context).ToLocalChecked())
      .FromJust();
}

NODE_MODULE(doom_native, Initialize)
