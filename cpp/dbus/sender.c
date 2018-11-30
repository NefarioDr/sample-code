/* compile with:
    gcc -o sender sender.c -I/usr/local/include/ -I/usr//local/Cellar/dbus/1.10.22/include/dbus-1.0/ -I/usr//local/Cellar/dbus/1.10.22/lib/dbus-1.0/include -L/usr//local/lib/ -ldbus-1 -luv
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <getopt.h>

/*
 * listen, wait a call or a signal
 */
#define DBUS_SENDER_BUS_NAME        "name.of.sender.lionel.messi"

#define MODE_SIGNAL                 1
#define MODE_METHOD                 2

DBusMessage* make_dbus_message(int type, const char* busName, const char* objPath, const char* iface,
        const char* msname) {

    DBusMessage* retMsg = NULL;

    switch(type) {
        case MODE_METHOD: {
            retMsg = dbus_message_new_method_call(busName, objPath, iface, msname);
        }
            break;
        case MODE_SIGNAL: {
            retMsg = dbus_message_new_signal(objPath, iface, msname);
        }
            break;
        default:
            break;
    }
    if (retMsg == NULL) {
        printf("Make dbus message failed. \n");
    }
    return retMsg;
}

static void dbus_send_method_call(const char* busName, const char* objPath, const char* iface,
        const char* method, DBusMessage* msg) {
    printf("start dbus_send_method_call function. \n");
    DBusConnection *connection;
    DBusError err;
    DBusMessageIter arg;
    DBusPendingCall *pending;

    char *__value_str;
    int __value_int;
    //Step 1: connecting session bus
    /* initialise the erroes */
    dbus_error_init(&err);

    do {
        /* Connect to Bus*/
        connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
        if (!connection)
        {
            if (dbus_error_is_set(&err))
                printf("Connection Err : %s\n", err.message);

           break;
        }

        //step 2: 设置BUS name，也即连接的名字。
        int ret = dbus_bus_request_name(connection, DBUS_SENDER_BUS_NAME,
                                DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
        if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        {
            if (dbus_error_is_set(&err))
                printf("Name Err : %s\n", err.message);

            break;
        }

        printf("Call app[bus_name]=%s, object[path]=%s, interface=%s, method=%s\n",
                busName, objPath, iface, method);

        //发送消息并获得reply的handle 。Queues a message to send, as with dbus_connection_send() , but also returns a DBusPendingCall used to receive a reply to the message.
        if (!dbus_connection_send_with_reply(connection, msg, &pending, -1))
        {
            printf("Out of Memory!");
            break;
        }

        if (pending == NULL)
        {
            printf("Pending Call NULL: connection is disconnected ");
            break;
        }

        dbus_connection_flush(connection);
        dbus_message_unref(msg);

        //waiting a reply，在发送的时候，已经获取了method reply的handle，类型为DBusPendingCall。
        // block until we receive a reply， Block until the pending call is completed.
        dbus_pending_call_block(pending);
        // get the reply message，Gets the reply, or returns NULL if none has been received yet.
        msg = dbus_pending_call_steal_reply(pending);
        if (msg == NULL)
        {
            printf("Reply Null\n");
            break;
        }

        // free the pending message handle
        dbus_pending_call_unref(pending);

        // read the Arguments
        if (!dbus_message_iter_init(msg, &arg))
        {
            printf("Message has no Argument!\n");
            break;
        }

        do
        {
            ret = dbus_message_iter_get_arg_type(&arg);
            printf("get reply parameter type: %c \n", ret);
            if (DBUS_TYPE_STRING == ret)
            {
                dbus_message_iter_get_basic(&arg, &__value_str);
                printf("get Method return STRING: %s\n", __value_str);
            }
            else if (DBUS_TYPE_INT32 == ret)
            {
                dbus_message_iter_get_basic(&arg, &__value_int);
                printf("get Method return INT32: %d\n", __value_int);
            }
            else if (DBUS_TYPE_ARRAY == ret) {
                printf("DBUS_TYPE_ARRAY returned, should parse it.\n");
            }
            else
            {
                printf("Argument Type ERROR\n");
            }

        } while (dbus_message_iter_next(&arg));

        printf("NO More Argument\n");
    } while (0);

    if (msg != NULL)
        dbus_message_unref(msg);

    dbus_error_free(&err);
}

static void dbus_send_signal(const char* objPath, const char* iface, const char* signal, DBusMessage* msg) {
    DBusConnection *connection;
    DBusError err;
    DBusPendingCall *pending;
    dbus_uint32_t serial;

    //Step 1: connecting session bus
    /* initialise the erroes */
    dbus_error_init(&err);

    /* Connect to Bus*/
    connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    do {

        if (!connection)
        {
            if (dbus_error_is_set(&err))
                printf("Connection Err : %s\n", err.message);

           break;
        }

        //step 2: 设置BUS name，也即连接的名字。
        int ret = dbus_bus_request_name(connection, DBUS_SENDER_BUS_NAME,
                                DBUS_NAME_FLAG_REPLACE_EXISTING, &err);

        if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        {
            if (dbus_error_is_set(&err))
                printf("Name Err : %s\n", err.message);

            break;
        }

        printf("Signal to object[path]=%s, interface=%s, signal=%s\n",
                    objPath, iface, signal);

        //将信号从连接中发送
        if (!dbus_connection_send(connection, msg, &serial))
        {
            printf("Out of Memory!\n");
            break;
        }

        dbus_connection_flush(connection);
        printf("Signal Send Out\n");
    } while (0);

    if (msg != NULL)
        dbus_message_unref(msg);

    dbus_error_free(&err);
}

static void usage(void)
{
#define USAGE "usage: ./send --type=signal|method --bus-name=a.b.c --obj-path=/a/b/c --interface=a.b.c --member=test --args=all about you\n" \
    "\tsignal -- send a signal to dbus server.\n" \
    "\t\tthe dbus server is specified by following parameters.\n" \
    "\tmethod -- call a method from dbus server.\n" \
    "\t\tMUST define bus-name for method call.\n" \
    "\n"
    printf(USAGE);
}

static bool fill_message(DBusMessage* msg, const char* param) {
    if (param == NULL) return true;

    printf("fill_message: msg =%p, param = %s \n", msg, param);
    DBusMessageIter arg;
    dbus_message_iter_init_append(msg, &arg);

    bool ret = dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &param);
    if (!ret) {
        printf("make dbus message write parameters failed. Out of memory.\n");
    }

    return ret;
}

/*
    处理命令行参数
 */

static const char* short_opts = ":t:b:o:i:m:a::";
struct option long_opts[] = {
    {"type", required_argument, NULL, 't'},
    {"bus-name", optional_argument, NULL, 'b'},
    {"obj-path", required_argument, NULL, 'o'},
    {"interface", required_argument, NULL, 'i'},
    {"member", required_argument, NULL, 'm'},
    {"arg", optional_argument, NULL, 'a'},
    {0, 0, 0, 0}
};

char* opt_type = NULL;
char* opt_bus_name = NULL;
char* opt_obj_path = NULL;
char* opt_iface = NULL;
char* opt_member = NULL;
char* opt_arg = NULL;

static void release_opt_args() {
    if (opt_type != NULL)
        free(opt_type);

    if (opt_bus_name != NULL)
        free(opt_bus_name);

    if (opt_obj_path != NULL)
        free(opt_obj_path);

    if (opt_iface != NULL)
        free(opt_iface);

    if (opt_member != NULL)
        free(opt_member);

    if (opt_arg != NULL)
        free(opt_arg);
}

static bool parse_option(int argc, char* argv[]) {
    int c;
    int idx;
    bool parsed_success = true;
    while((c = getopt_long(argc, argv, short_opts, long_opts, &idx)) != -1) {
        switch(c) {
            case 't':
                opt_type = strdup(optarg);
                break;
            case 'b':
                if (optarg != NULL)
                    opt_bus_name = strdup(optarg);
                break;
            case 'o':
                opt_obj_path = strdup(optarg);
                break;
            case 'i':
                opt_iface = strdup(optarg);
                break;
            case 'm':
                if (optarg != NULL)
                    opt_member = strdup(optarg);
                break;
            case 'a':
                if (optarg != NULL)
                    opt_arg = strdup(optarg);
                break;
            case ':':
                printf("option '%c' requires argument. \n", (char)optopt);
                parsed_success = false;
                break;
            case '?':
            default:
                printf("option '%c' is unknown.\n", (char)optopt);
                parsed_success = false;
                break;
        }
    }
    return parsed_success;
};


int main(int argc, char *argv[])
{
    do {
        if (!parse_option(argc, argv)) {
            usage();
            break;
        }

        if (!strcasecmp(opt_type, "SIGNAL")) {
            DBusMessage* msg = make_dbus_message(MODE_SIGNAL, NULL, opt_obj_path, opt_iface, opt_member);
            if (msg == NULL) {
                break;
            }

            bool ret = fill_message(msg, opt_arg);
            if (!ret) {
                printf("fill_message failed. \n");
                break;
            }
            dbus_send_signal(opt_obj_path, opt_iface, opt_member, msg);
        }
        else if (!strcasecmp(opt_type, "METHOD")) {
            if (opt_bus_name == NULL) {
                usage();
                break;
            }
            DBusMessage* msg = make_dbus_message(MODE_METHOD, opt_bus_name, opt_obj_path, opt_iface, opt_member);
            if (msg == NULL) {
                break;
            }

            bool ret = fill_message(msg,opt_arg);
            if (!ret) {
                printf("fill_message failed. \n");
                break;
            }
            dbus_send_method_call(opt_bus_name, opt_obj_path, opt_iface, opt_member, msg);
        }
        else {
            usage();
        }
    } while (0);

    release_opt_args();
    return 0;
}