const addon = require("../build/Release/js_c_js_c.node");

addon.setCallbackFunction(function(args, cpp_callback) {
    console.log("JS_LOG: callback function called: ", args);
    cpp_callback("a", 1, {name: "unknown"});
})

addon.emitChain();