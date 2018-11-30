#include <nan.h>
#include <string>

Nan::Persistent<v8::Function> CallbackToJsHandler;

static void JsCallbackToCppHandler(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    unsigned int len = info.Length();
    printf("CPP_LOG: JsCallbackToCppHandler called, arguments length is: %d\n", len);
}

NAN_METHOD(emitChain) {
    v8::Local<v8::FunctionTemplate> ftp = Nan::New<v8::FunctionTemplate>(JsCallbackToCppHandler);
    ftp->SetClassName(Nan::New("jsCallCppHandler").ToLocalChecked());

    const unsigned argc = 2;
    v8::Local<v8::Value> argv[argc] = {
        Nan::New("I call you back from C++").ToLocalChecked(), // 传递到JS中的参数
        ftp->GetFunction() // JS回调C的方法
    };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), Nan::New(CallbackToJsHandler), argc, argv);
    // 在不再需要的时候，释放这个Callback，否则会有内存泄漏。
    // CallbackToJsHandler.Reset();
}

NAN_METHOD(SetCallbackFunction) {
    CallbackToJsHandler.Reset(info[0].As<v8::Function>());
    // 看实际情况看是不是需要返回值。
    info.GetReturnValue().Set(Nan::New("Javascript callback function saved.").ToLocalChecked());
}

void InitAll(v8::Local<v8::Object> exports) {
    Nan::SetMethod(exports, "setCallbackFunction", SetCallbackFunction);
    Nan::SetMethod(exports, "emitChain", emitChain);
}

NODE_MODULE(js_c_js_c, InitAll)
