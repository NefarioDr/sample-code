#include <nan.h>

NAN_METHOD(Method1) { // NAN_METHOD 的参数被命名为info
    v8::Local<v8::Value> obj = Nan::New("Hello").ToLocalChecked();
    info.GetReturnValue().Set(obj);
}

NAN_METHOD(Method2) { // NAN_METHOD 的参数被命名为info
    v8::Local<v8::Value> obj = Nan::New("World").ToLocalChecked();
    info.GetReturnValue().Set(obj);
}

// 导出一批方法到JS，没有私有信息
void InitAll(v8::Local<v8::Object> exports) {
    Nan::SetMethod(exports, "method1", Method1);
    Nan::SetMethod(exports, "method2", Method2);
}

NODE_MODULE(functions_set, InitAll)
