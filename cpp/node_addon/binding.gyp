# 关于node-gyp和binding.gyp的参考资料，可以参考如下网址：
# node-gyp的安装和使用说明
# 1.　https://github.com/nodejs/node-gyp#installation
# .gyp文件的格式的用户手册
# 2.　https://gyp.gsrc.io/docs/UserDocumentation.md
# 使用.gyp文件的一些样例项目
# 3.　https://github.com/nodejs/node-gyp/wiki/%22binding.gyp%22-files-out-in-the-wild

{
    "make_global_settings": [
        ['CXX','/usr/bin/g++'],
        ['LINK','/usr/bin/g++'],
      ],
    'conditions': [
        # 在MAC平台上，定义这个condition，否则c++11的设定不起作用。
        [ 'OS=="mac"',
            {
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                    'OTHER_LDFLAGS': ['-stdlib=libc++'],
                },
            }
        ],
    ],
    # targets 是一个array，可以同时生成多个对象
    "targets":[
        {
            "target_name": "export_as_function",
            "sources": [
                "export_as_function.cc"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        },
        {
            "target_name": "functions_set",
            "sources": [
                "functions_set.cc"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        },
        {
            #　target_name：　生成的addon的名字，如sample.node
            "target_name": "pass_argument_2_cpp",
            #　源文件列表，可以使用相对路径或绝对路径列出文件
            "sources": [
                "pass_argument_2_cpp.cc"
            ],
            # 宏定义
            "defines": [
                # 'DEFINE_TEST',
                # 'DEFINE_A_VALUE=value'
            ],
            # 头文件路径，类似gcc中的'-I'指示符
            "include_dirs": [
                # 以<!(node -e ...)格式的头文件
                "<!(node -e \"require('nan')\")"
                #　普通的头文件，通过相对/绝对路径的方式引入。
                # "./",
                # "/usr/local/include"
            ],
            # "libraries": [ "/usr/local/lib/mylib.a" ] 静态链接库
            # 依赖的.so, .a库文件，'-L'指定库文件的搜索路径，'-l'指明库名
            # 如果是静态库，则全路径指明文件名(相对/绝对路径)
            "libraries": [
                # "-L/usr/local/lib/",
                # "-ldbus-1",
                # "-luv"
            ],
            #　编译标志，编译选项在这里说明。
            "cflags": [ "-std=c++11 -Wall" ]
        },
        {
            "target_name": "return_values_2_js",
            "sources": [
                "return_values_2_js.cc"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        },
        {
            "target_name": "js_c_js_c",
            "sources": [
                "js_c_js_c.cpp"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        },
    ]
}
