/* compile with:
   gcc -o server_uv server_uv.c -I/usr/local/include/ -I/usr//local/Cellar/dbus/1.10.22/include/dbus-1.0/ -I/usr//local/Cellar/dbus/1.10.22/lib/dbus-1.0/include -L/usr/local/lib/ -ldbus-1 -luv
*/
#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <uv.h>

#define MAX_WATCHES 100

#define SERVER_BUS_NAME "sample.server.bus_name"
#define SERVER_OBJ_PATH "/sample/server/bus/path"~
#define SERVER_INTERFACE "sample.server.bus.interface"

static DBusConnection *conn;
static int watches_count = 0;
static char *progname;

uv_loop_t* loop;
typedef struct {
    uv_poll_t phandler;
    int fd;
} watch_context_t;

watch_context_t* g_watches_list[MAX_WATCHES];

/**
 * 处理函数，用来处理经过rule处理过的消息。
 * 如果所有的处理函数都返回DBUS_HANDLER_RESULT_NOT_YET_HANDLED，那调用方会收到错误信息“服务端没有这个方法”
 * @param  connection [description]
 * @param  message    [description]
 * @param  userData   [dbus_connection_register_object_path()方法中指定的数据，可以用来回调。]
 * @return            [DBUS_HANDLER_RESULT_HANDLED　：　后续的过滤器不会再收到这个消息了。
 *               　　　DBUS_HANDLER_RESULT_NOT_YET_HANDLED　：　后续的过滤器还能收到这个消息。]
 */
static DBusHandlerResult handleMessage(DBusConnection *connection, DBusMessage *message,
        void *userData) {
    printf("[%s] handleMessage get connection send in.\n", __FUNCTION__);

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static void unregisterMessageHandler(DBusConnection *connection, void *userData) {
    printf("[%s] unregisterMessageHandler. \n", __FUNCTION__);
}

DBusObjectPathVTable vtable = {
    .message_function = handleMessage,
    .unregister_function = unregisterMessageHandler
};

/**
 * 过滤器函数，优先于dbus_connection_register_object_path()中DBusObjectPathVTable指定的回调方法。
 * @param  c    [description]
 * @param  m    [description]
 * @param  data [函数中被传递的数据]
 * @return      [DBUS_HANDLER_RESULT_HANDLED　：　后续的过滤器不会再收到这个消息了。
 *               DBUS_HANDLER_RESULT_NOT_YET_HANDLED　：　后续的过滤器还能收到这个消息。]
 */
static DBusHandlerResult filter_func(DBusConnection *c, DBusMessage *m, void *data)
{
    char * msgtype;
    int type = dbus_message_get_type(m);
    switch(type) {
        case DBUS_MESSAGE_TYPE_METHOD_CALL:
            msgtype = "Type_Method_Call";
            break;
        case DBUS_MESSAGE_TYPE_METHOD_RETURN:
            msgtype = "Type_Method_Return";
            break;
        case DBUS_MESSAGE_TYPE_ERROR:
            msgtype = "Type_Error";
            break;
        case DBUS_MESSAGE_TYPE_SIGNAL:
            msgtype = "Type_Signal";
            break;
        default:
            msgtype = "Type_Invalid";
            break;
    }

    printf("[%s][%s] type: %s, if: %s, member: %s, path: %s\n", progname, __FUNCTION__,
            msgtype,
           dbus_message_get_interface(m),
           dbus_message_get_member(m),
           dbus_message_get_path(m));

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

/**
 * uv_poll_start()中指定的回调方法，当满足条件的poll()事件发生时，调用这个方法
 * @param req    [uv_poll_start()方法中的uv_poll_t参数，可以通过data指针传递需要的数据]
 * @param status [如果uv_poll失败了，那么status这个值是一个<0的数，代表UV_E的某个值。uv_poll成功，status值是0]
 * @param events [发生的事件，可能是UV_READABLE, UV_WRITABLE, UV_PRIORITIZED, UV_DISCONNECT]
 */
static void incoming_msg_handler(uv_poll_t *req, int status, int events) {
    printf("[%s][%s] fd_handler : %d \n", progname, __FUNCTION__, events);
    DBusWatch* watch = (DBusWatch *)req->data;
    while (!dbus_watch_handle(watch, events)) {
            printf("dbus_watch_handle needs more memory\n");
            sleep(1);
    }

    dbus_connection_ref(conn);
    while (dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS);
    dbus_connection_unref(conn);
}

/**
 * 添加watch的回调方法
 *
 * @param  watch [观察器]
 * @param  data  [dbus_connection_set_watch_functions()中传递过来的数据，用来异步传递参数]
 * @return       [必须返回true, 否则会导致dbus_connection_set_watch_functions()调用失败]
 */
static dbus_bool_t add_watch(DBusWatch *watch, void *data)
{
    int fd;
    unsigned int flags;
    fd = dbus_watch_get_unix_fd(watch);
    flags = dbus_watch_get_flags(watch);
    printf("[%s][%s] add watch %p, flags: %d \n", progname, __FUNCTION__, (void*)watch, flags);

    if (flags & DBUS_WATCH_READABLE) {
        watch_context_t* watch_ctx = (watch_context_t *)dbus_malloc(sizeof(watch_context_t));
        watch_ctx->fd = fd;
        int r = uv_poll_init(loop, &watch_ctx->phandler, fd);
        if (r != 0) {
            printf("add watch failed: %d\n", r);
            return 1;
        }
        watch_ctx->phandler.data = watch;
        uv_poll_start(&watch_ctx->phandler, UV_READABLE, incoming_msg_handler);
        g_watches_list[watches_count++] = watch_ctx;
    }
    return 1;
}

static void remove_watch(DBusWatch *watch, void *data)
{
    printf("[%s][%s] remove watch %p\n", progname, __FUNCTION__, (void*)watch);
    int i;
    for (i = 0; i < watches_count; ++i) {
        if (g_watches_list[i]->phandler.data == watch) {
            uv_poll_stop(&g_watches_list[i]->phandler);
            dbus_free(g_watches_list[i]);
            g_watches_list[i] = NULL;
            break;
        }
    }

    if (i < watches_count - 1) { // move NULL value to the end of list queue.
        --watches_count;
        g_watches_list[i] = g_watches_list[watches_count];
        g_watches_list[watches_count] = NULL;
    }
}

int main(int argc, char *argv[])
{
    progname = argv[0];
    loop = uv_default_loop();

    DBusError error;
    dbus_error_init(&error);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (conn == NULL) {
        printf("Error when connecting to the bus: %s\n",
               error.message);
        return 1;
    }
    int ret = dbus_bus_request_name(conn, SERVER_BUS_NAME,
                        DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        if (dbus_error_is_set(&error))
            printf("Name Error %s\n", error.message);

        return 1;
    }

    if (!dbus_connection_set_watch_functions(conn, add_watch,
             remove_watch, NULL, NULL, NULL)) {
        printf("dbus_connection_set_watch_functions failed\n");
        return 1;
    }
    if (!dbus_connection_add_filter(conn, filter_func, NULL, NULL)) {
        printf("Failed to register signal handler callback\n");
        return 1;
    }
    dbus_bus_add_match(conn, "type='signal'", &error);
    const char* method_fmt = "type='method_call',interface='%s',path='%s', member='%s'";

    int len = snprintf(NULL, 0, method_fmt, SERVER_INTERFACE, SERVER_OBJ_PATH, "ListNames");
    char rule[len + 1];
    snprintf(rule, len, method_fmt, SERVER_INTERFACE, SERVER_OBJ_PATH, "ListNames");
    dbus_bus_add_match(conn, rule, &error);
    // dbus_bus_add_match(conn, "type='method_call'", &error);

    ret = dbus_connection_register_object_path(
                conn,
                SERVER_OBJ_PATH,
                &vtable,
                NULL);

    if (!ret) {
        printf("[%s][ERROR]: register object path faild.\n", __FUNCTION__);
        return 1;
    }

    dbus_connection_flush(conn);
    // call uv_stop() to quit loop.
    return uv_run(loop, UV_RUN_DEFAULT);
}