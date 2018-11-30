#include <nan.h>
#include <string>



NAN_METHOD(SetCallbackFunction) {
    printf("Hello world from C++\n");
}

void InitAll(v8::Local<v8::Object> exports) {
    Nan::SetMethod(exports, "hello", SetCallbackFunction);
}

NODE_MODULE(js_c_js_c, InitAll)
