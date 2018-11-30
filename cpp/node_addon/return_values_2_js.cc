#include <nan.h>

NAN_METHOD(ReturnNumber) {
    info.GetReturnValue().Set(Nan::New(3.14));
}

NAN_METHOD(ReturnString) {
    info.GetReturnValue().Set(Nan::New("I am a string from C++.").ToLocalChecked());
}

NAN_METHOD(ReturnBoolean) {
    info.GetReturnValue().Set(Nan::True());
}

NAN_METHOD(ReturnArray) {
    v8::Local<v8::Array> retVal = Nan::New<v8::Array>();
    for (unsigned int i = 0; i < 4; ++i) {
        retVal->Set(i, Nan::New(i));
    }
    info.GetReturnValue().Set(retVal);
}

static void ObjectNameGetter(v8::Local< v8::Name > property,
        const v8::PropertyCallbackInfo< v8::Value > &info) {
    //　可以通过info设置返回值，拿到Holder
    info.GetReturnValue().Set(Nan::New("get dynamic property value.").ToLocalChecked());
}

static void ObjectNameSetter(v8::Local< v8::Name > property, v8::Local< v8::Value > value,
        const v8::PropertyCallbackInfo< void > &info) {

}

static void ObjectMethodCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    info.GetReturnValue().Set(Nan::New("myInfo is : dabao is the best!").ToLocalChecked());
}

static void GlobalMethodCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    info.GetReturnValue().Set(Nan::New("this is a global function for ObjectTemplate.").ToLocalChecked());
}

NAN_METHOD(ReturnObject) {

    int type = 0;
    if (info.Length() > 0 && info[0]->IsInt32()) {
        type = info[0]->Int32Value() % 2;
    }

    if (type == 0) {
        v8::Local<v8::Object> obj = Nan::New<v8::Object>();
        // 只读属性
        obj->Set(Nan::New("name").ToLocalChecked(), Nan::New("dabao").ToLocalChecked());
        obj->Set(Nan::New("points").ToLocalChecked(), Nan::New(100));
        // 调用这个object的dynamic属性时，会通过方法来读取和设置。
        obj->SetAccessor(Nan::New("dynamic").ToLocalChecked(), ObjectNameGetter, ObjectNameSetter);
        // 添加方法,　只有这个object才有。
        obj->Set(Nan::New("myInfo").ToLocalChecked(),
                Nan::New<v8::FunctionTemplate>(ObjectMethodCallback)->GetFunction());

        info.GetReturnValue().Set(obj);
    } else {
        v8::Local<v8::ObjectTemplate> objTemp = Nan::New<v8::ObjectTemplate>();
        // 所有通过这个ObjectTemplate，new出来的Instance，调用这个属性的时候，都会调到这些方法
        // Nan::SetAccessor(objTemp, Nan::New("name").ToLocalChecked(), ObjectNameGetter, ObjectNameSetter);
        objTemp->SetAccessor(Nan::New("name").ToLocalChecked(), ObjectNameGetter, ObjectNameSetter);

        objTemp->Set(Nan::New("gProperty").ToLocalChecked(), Nan::New("global property").ToLocalChecked());
        // 通过下面的方法给ObjectTemplate添加方法已经被标注为deprecate
        // objTemp->Set(Nan::New("gMethod").ToLocalChecked(),
        //         Nan::New<v8::FunctionTemplate>(GlobalMethodCallback)->GetFunction());
        // NewInstance的对象，可以针对它再单独设置属性。
        v8::Local<v8::Object> obj = objTemp->NewInstance();
            //　之后这里设置的属性和方法，就和上面的对象一样
        info.GetReturnValue().Set(obj);
    }
}

static void RetFunction(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    info.GetReturnValue().Set(Nan::New("say hello from return function.").ToLocalChecked());
}

NAN_METHOD(ReturnFunction) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(RetFunction);
    v8::Local<v8::Function> fn = tpl->GetFunction();
    info.GetReturnValue().Set(fn);
}

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    Nan::SetMethod(exports, "returnNumber", ReturnNumber);
    Nan::SetMethod(exports, "returnString", ReturnString);
    Nan::SetMethod(exports, "returnBoolean", ReturnBoolean);
    Nan::SetMethod(exports, "returnArray", ReturnArray);
    Nan::SetMethod(exports, "returnObject", ReturnObject);
    Nan::SetMethod(exports, "returnFunction", ReturnFunction);
}

NODE_MODULE(return_values_2_js, Init)
