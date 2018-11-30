#!/bin/bash

# Special Arguments List:
#
# $0 : file name of this script
# $1 : the first argument.
# $2 : the second argument.
# $3 : the third argument.
# ...

# $# : the number of arguments
# 关于$@和$*的使用，参考ABS_guild.pdf P103
# $@ : 所有的参数列表，放在""中使用，每个参数都是单独的string
# $* : 所有的参数列表，放在""中使用，所有的参数组合成一个string
#
#
# $$ : 运行当前脚本的进程的PID，常用于构建一个临时文件文件名
# $! : 最后一个后台进程的PID
# $_ : 最后执行命令的最后一个命令、参数、符号。如果这个命令没有参数，那么它是最后的命令名
# $? : 上一次命令、函数、脚本的退出状态


function arguments_with_switch() {
    while getopts u:d:p:f: option
    do
    case "${option}"
    in
    u) USER=${OPTARG};;
    d) DATE=${OPTARG};;
    p) PRODUCT=${OPTARG};;
    f) FORMAT=$OPTARG;;
    esac
    done

    echo "FINISHED: user: ${USER}, DATE: ${DATE}, PRODUCT: ${PRODUCT}, FORMAT: ${FORMAT}"
    return 0
}

function iterate_arguments() {
    index=1
    for arg in "$@"
    do
        echo '$@ with "": '"Arg$index is $arg"
        let "index+=1"
    done

    index=1
    for arg in $@
    do
        echo '$@ without "": '"Arg$index is $arg"
        let "index+=1"
    done

    index=1
    for arg in "$*"
    do
        echo '$* with "": '"Arg$index is $arg"
        let "index+=1"
    done

    index=1
    for arg in $*
    do
        echo '$* without "": '"Arg$index is $arg"
        ((index+=1))
    done

    return 1
}

arguments_with_switch -u jsmith -p notebooks -d 10-20-2011 -f pdf
echo "switch $?"
iterate_arguments -u jsmith -p notebooks -d 10-20-2011 -f pdf
echo "iterator $?"
