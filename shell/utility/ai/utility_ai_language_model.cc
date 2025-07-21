// Copyright (c) 2025 Microsoft, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/utility/ai/utility_ai_language_model.h"

#include "base/notimplemented.h"
#include "shell/browser/javascript_environment.h"
#include "shell/common/gin_converters/callback_converter.h"
#include "shell/common/gin_converters/std_converter.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/event_emitter_caller.h"
#include "third_party/blink/public/mojom/ai/ai_common.mojom.h"
#include "third_party/blink/public/mojom/ai/ai_language_model.mojom.h"
#include "third_party/blink/public/mojom/ai/model_streaming_responder.mojom.h"

namespace gin {

template <>
struct Converter<on_device_model::mojom::ResponseConstraintPtr> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const on_device_model::mojom::ResponseConstraintPtr& val) {
    // TODO - A proper implementation here
    return v8::Undefined(isolate);
  }
};

template <>
struct Converter<blink::mojom::AILanguageModelPromptRole> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      blink::mojom::AILanguageModelPromptRole value) {
    switch (value) {
      case blink::mojom::AILanguageModelPromptRole::kSystem:
        return StringToV8(isolate, "system");
      case blink::mojom::AILanguageModelPromptRole::kUser:
        return StringToV8(isolate, "user");
      case blink::mojom::AILanguageModelPromptRole::kAssistant:
        return StringToV8(isolate, "assistant");
      default:
        return StringToV8(isolate, "unknown");
    }
  }
};

template <>
struct Converter<blink::mojom::AILanguageModelPromptContentPtr> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const blink::mojom::AILanguageModelPromptContentPtr& val) {
    if (val.is_null())
      return v8::Undefined(isolate);

    auto dict = gin::Dictionary::CreateEmpty(isolate);

    if (val->is_text()) {
      dict.Set("type", "text");
      dict.Set("text", val->get_text());
    } else if (val->is_bitmap()) {
      dict.Set("type", "image");
      // TODO - Need to convert this to an ArrayBuffer
      // dict.Set("image", val->get_bitmap());
    } else if (val->is_audio()) {
      dict.Set("type", "audio");
      // TODO - Need to convert this to an ArrayBuffer
      // dict.Set("bytes", val->get_audio());
    }

    return ConvertToV8(isolate, dict);
  }
};

template <>
struct Converter<blink::mojom::AILanguageModelPromptPtr> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const blink::mojom::AILanguageModelPromptPtr& val) {
    if (val.is_null())
      return v8::Undefined(isolate);

    auto dict = gin::Dictionary::CreateEmpty(isolate);

    dict.Set("role", val->role);
    dict.Set("content", val->content);
    dict.Set("prefix", val->is_prefix);

    return ConvertToV8(isolate, dict);
  }
};

}  // namespace gin

namespace electron {

UtilityAILanguageModel::UtilityAILanguageModel(
    v8::Local<v8::Object> language_model) {
  v8::Isolate* isolate = JavascriptEnvironment::GetIsolate();
  language_model_.Reset(isolate, language_model);
}

UtilityAILanguageModel::~UtilityAILanguageModel() = default;

void UtilityAILanguageModel::Prompt(
    std::vector<blink::mojom::AILanguageModelPromptPtr> prompts,
    on_device_model::mojom::ResponseConstraintPtr constraint,
    mojo::PendingRemote<blink::mojom::ModelStreamingResponder>
        pending_responder) {
  // TODO - Implement this

  // TODO - Add v8::TryCatch?
  v8::Isolate* isolate = JavascriptEnvironment::GetIsolate();
  v8::HandleScope scope{isolate};

  v8::Local<v8::Value> val = gin_helper::CallMethod(
      isolate, language_model_.Get(isolate), "prompt", prompts, constraint);

  // It's supposed to return a promise, but for convenience
  // allow developers to return a value directly as well
  if (val->IsString()) {
    mojo::Remote<blink::mojom::ModelStreamingResponder> responder(
        std::move(pending_responder));
    std::string response;
    gin::ConvertFromV8(isolate, val, &response);
    responder->OnStreaming(response);
    // TODO - Pull real tokens count - need to worry about parallel prompts?
    responder->OnCompletion(blink::mojom::ModelExecutionContextInfo::New(0));
    return;
  } else if (val->IsPromise()) {
    auto promise = val.As<v8::Promise>();

    auto then_cb = base::BindOnce(
        [](v8::Isolate* isolate, v8::Local<v8::Value> result) {
          // TODO - Implementation
        },
        isolate);

    auto catch_cb = base::BindOnce([](v8::Local<v8::Value> result) {
      // TODO - Error is here
      // TODO - An error here is killing the utility process
    });

    std::ignore = promise->Then(
        isolate->GetCurrentContext(),
        gin::ConvertToV8(isolate, std::move(then_cb)).As<v8::Function>(),
        gin::ConvertToV8(isolate, std::move(catch_cb)).As<v8::Function>());
  } else {
    // TODO - Error handling
    mojo::Remote<blink::mojom::ModelStreamingResponder> responder(
        std::move(pending_responder));
    responder->OnError(
        blink::mojom::ModelStreamingResponseStatus::kErrorUnknown,
        /*quota_error_info=*/nullptr);
  }

  // TODO - Respond
}

void UtilityAILanguageModel::Append(
    std::vector<blink::mojom::AILanguageModelPromptPtr> prompts,
    mojo::PendingRemote<blink::mojom::ModelStreamingResponder>
        pending_responder) {
  // TODO - Implement this
  NOTIMPLEMENTED();
}

void UtilityAILanguageModel::Fork(
    mojo::PendingRemote<blink::mojom::AIManagerCreateLanguageModelClient>
        client) {
  // TODO - Implement this
  NOTIMPLEMENTED();
}

void UtilityAILanguageModel::Destroy() {
  // TODO - Implement this
  NOTIMPLEMENTED();
}

void UtilityAILanguageModel::MeasureInputUsage(
    std::vector<blink::mojom::AILanguageModelPromptPtr> input,
    MeasureInputUsageCallback callback) {
  // TODO - Implement this
  NOTIMPLEMENTED();
}

}  // namespace electron
