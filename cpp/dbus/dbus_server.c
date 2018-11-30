/* compile with:
   gcc `pkg-config dbus-1 --cflags --libs` own-mainloop.c -o own-
mainloop
*/
#include <dbus/dbus.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>
#define MAX_WATCHES 100
static DBusConnection *conn;
static struct pollfd pollfds[MAX_WATCHES];
static DBusWatch *watches[MAX_WATCHES];
static int max_i;
static char *progname;

static DBusHandlerResult handleMessage(DBusConnection *conn, DBusMessage *message,
        void *userData) {
    printf("[%s] handleMessage get connection send in.\n", __FUNCTION__);

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static void unregisterMessageHandler(DBusConnection *conn, void *userData) {
    printf("[%s] unregisterMessageHandler. \n", __FUNCTION__);
}

DBusObjectPathVTable vtable = {
    .message_function = handleMessage,
    .unregister_function = unregisterMessageHandler
};

static DBusHandlerResult filter_func(DBusConnection *c, DBusMessage *m, void *data)
{
    char * msgtype;
    int type = dbus_message_get_type(m);
    switch(type) {
        case DBUS_MESSAGE_TYPE_METHOD_CALL:
            msgtype = "Method_Call";
            break;
        case DBUS_MESSAGE_TYPE_METHOD_RETURN:
            msgtype = "Method_Return";
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
    // return DBUS_HANDLER_RESULT_HANDLED;
}

static dbus_bool_t add_watch(DBusWatch *watch, void *data)
{
        short cond = POLLHUP | POLLERR;
        int fd;
        unsigned int flags;
        printf("[%s][%s] add watch %p\n", progname, __FUNCTION__, (void*)watch);
        fd = dbus_watch_get_fd(watch);
        flags = dbus_watch_get_flags(watch);

        if (flags & DBUS_WATCH_READABLE)
                cond |= POLLIN;
        if (flags & DBUS_WATCH_WRITABLE)
                cond |= POLLOUT;
        ++max_i;
        pollfds[max_i].fd = fd;
        pollfds[max_i].events = cond;
        watches[max_i] = watch;
        return 1;
}

static void remove_watch(DBusWatch *watch, void *data)
{
        int i, found = 0;
        printf("[%s][%s] remove watch %p\n", progname, __FUNCTION__, (void*)watch);
        for (i = 0; i <= max_i; ++i) {
                if (watches[i] == watch) {
                        found = 1;
                        break;
                }
        }
        if (!found) {
                printf("watch %p not found\n", (void*)watch);
                return;
        }
        memset(&pollfds[i], 0, sizeof(pollfds[i]));
        watches[i] = NULL;
        if (i == max_i && max_i > 0) --max_i;
}
static void fd_handler(short events, DBusWatch *watch)
{
        printf("[%s][%s] fd_handler : %d \n", progname, __FUNCTION__, events);
        unsigned int flags = 0;
        if (events & POLLIN)
                flags |= DBUS_WATCH_READABLE;
        if (events & POLLOUT)
                flags |= DBUS_WATCH_WRITABLE;
        if (events & POLLHUP)
                flags |= DBUS_WATCH_HANGUP;
        if (events & POLLERR)
                flags |= DBUS_WATCH_ERROR;
        while (!dbus_watch_handle(watch, flags)) {
                printf("dbus_watch_handle needs more memory\n");
                sleep(1);
        }

        dbus_connection_ref(conn);
        while (dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS);
        dbus_connection_unref(conn);
}

int main(int argc, char *argv[])
{
        DBusError error;
        progname = argv[0];
        dbus_error_init(&error);
        conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
        if (conn == NULL) {
                printf("Error when connecting to the bus: %s\n",
                       error.message);
                return 1;
        }
        int ret = dbus_bus_request_name(conn, "dbus.sample1",
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
        // dbus_bus_add_match(conn, "type='method_call',interface='com.liangqin.fan',path='/com/liangqin/fan', member='ListNames'", &error);
        dbus_bus_add_match(conn, "type='method_call'", &error);

        ret = dbus_connection_register_object_path(
                    conn,
                    "/com/liangqin/fan",
                    &vtable,
                    NULL);

        if (!ret) {
            printf("[%s][ERROR]: register object path faild.\n", __FUNCTION__);
            return 1;
        }

        dbus_connection_flush(conn);
        while (1) {
            struct pollfd fds[MAX_WATCHES];
            DBusWatch *watch[MAX_WATCHES];
            int nfds, i;
            for (nfds = i = 0; i <= max_i; ++i) {
                if (pollfds[i].fd == 0 ||
                    !dbus_watch_get_enabled(watches[i])) {
                        continue;
                }
                fds[nfds].fd = pollfds[i].fd;
                fds[nfds].events = pollfds[i].events;
                fds[nfds].revents = 0;
                watch[nfds] = watches[i];
                ++nfds;
            }
            if (poll(fds, nfds, -1) <= 0) {
                perror("poll");
                break;
            }
            for (i = 0; i < nfds; ++i) {
                if (fds[i].revents) {
                        fd_handler(fds[i].revents, watch[i]);
                }
            }
        }
        return 0;
}