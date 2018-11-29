#include <nan.h>
#include <string>

NAN_METHOD(PassBasic) { // NAN_METHOD 的参数被命名为info
    std::string ret;
    if (info.Length() == 0) {
        ret = "no argument passed over.";
    }

    if (info[0]->IsInt32()) {
        ret = "Int32 value : ";
        ret.append(std::to_string(info[0]->Int32Value()));
    }

    if (info[0]->IsUint32()) {
        ret = "Uint32 value : ";
        ret.append(std::to_string(info[0]->Uint32Value()));
    }

    if (info[0]->IsString()) {
        ret = "String value : ";
        ret.append(*Nan::Utf8String(info[0]->ToString()));
    }

    if (info[0]->IsBoolean()) {
        ret = "Boolean value : ";
        ret.append(std::to_string(info[0]->BooleanValue()));
    }

    if (info[0]->IsNumber()) {
        ret = "Number value : ";
        ret.append(std::to_string(info[0]->NumberValue()));
    }


    info.GetReturnValue().Set(Nan::New(ret.c_str()).ToLocalChecked());
}

NAN_METHOD(PassArray) {
    std::string ret;
    if (info[0]->IsArray()) {
        v8::Local<v8::Array> argsArr = v8::Local<v8::Array>::Cast(info[0]);
        ret = "array length is : ";
        ret.append(std::to_string(argsArr->Length()));

        // 读取Array中的数据。
        // v8::Local<v8::Value> val = argsArr->Get(0);
        // 可以处理val了。
    } else {
        ret = "argument is not Array.";
    }

    info.GetReturnValue().Set(Nan::New(ret.c_str()).ToLocalChecked());
}

NAN_METHOD(PassObject) {
    std::string ret;
    if (info[0]->IsObject()) {
        v8::Local<v8::Object> obj = info[0]->ToObject();
        v8::MaybeLocal<v8::Value> rawObj = obj->Get(Nan::New("key").ToLocalChecked());
        if (rawObj.IsEmpty()) {
            ret = "object 'key' value is not defined.";
        } else {
            ret = "object 'key' value is : ";
            v8::Local<v8::Value> value = rawObj.ToLocalChecked();
            ret.append(*Nan::Utf8String(value->ToString()));
        }
    } else {
        ret = "argument is not Object";
    }
    info.GetReturnValue().Set(Nan::New(ret.c_str()).ToLocalChecked());
}

NAN_METHOD(PassFunction) {
    v8::Local<v8::Function> cb = info[0].As<v8::Function>();
    const unsigned argc = 1;
    v8::Local<v8::Value> argv[argc] = { Nan::New("I call you back from C++").ToLocalChecked() };
    Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);

    info.GetReturnValue().Set(Nan::New("I handled function call from c++").ToLocalChecked());
}

void InitAll(v8::Local<v8::Object> exports) {
    Nan::SetMethod(exports, "passBasic", PassBasic);
    Nan::SetMethod(exports, "passArray", PassArray);
    Nan::SetMethod(exports, "passObject", PassObject);
    Nan::SetMethod(exports, "passFunction", PassFunction);
}

NODE_MODULE(pass_argument_2_cpp, InitAll)
