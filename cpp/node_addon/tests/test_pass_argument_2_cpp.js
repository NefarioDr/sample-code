const pa2c = require("../build/Release/pass_argument_2_cpp.node");

console.log(pa2c.passBasic(-1));  // 都被判断为　Number　　？？？？？？
console.log(pa2c.passBasic(true));
console.log(pa2c.passBasic("A string from JS"));
console.log(pa2c.passBasic());
console.log(pa2c.passBasic(2.34));

console.log(pa2c.passArray([0, 2, 4, 8]));

console.log(pa2c.passObject({key: "mamimamihong", value: 85}));

console.log(pa2c.passFunction((msg)=>{
    console.log(msg);
}));

