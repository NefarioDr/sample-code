const addon = require("../build/Release/return_values_2_js.node");

console.log(addon.returnNumber());
console.log(addon.returnString());
console.log(addon.returnBoolean());
console.log(addon.returnArray());

var obj = addon.returnObject();
console.log(obj.name);
console.log(obj.points);
console.log(obj.dynamic);
console.log(obj.myInfo());

var gobj = addon.returnObject(1);
console.log(gobj.gProperty);
console.log(gobj.name);
// console.log(gobj.gMethod());
//
var fn = addon.returnFunction();
console.log(fn());