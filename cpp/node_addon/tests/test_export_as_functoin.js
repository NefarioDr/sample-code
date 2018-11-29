const fn = require("../build/Release/export_as_function.node");

var ret = fn("Hello world.");
console.log(ret.msg); // print 'Hello world.'