#include "cupkee.h"
#include "rbuff.h"

#define BELL    "\007"
#define CRLF    "\r\n"
#define RIGHT   "\033[C"
#define LEFT    "\010"
#define PROMPT  "> "

#define CONSOLE_IN       0
#define CONSOLE_OUT      1
#define CONSOLE_BUF_NUM  2

#define CONSOLE_BUF_SIZE 128

static cupkee_device_t *console_dev = NULL;
static console_handle_t user_handle = NULL;

static uint32_t console_total_recv = 0;
static uint32_t console_total_send = 0;

//static uint8_t  console_logo_show;
static uint16_t console_cursor = 0;

static rbuff_t  console_buff[CONSOLE_BUF_NUM];
static char     console_buff_mem[CONSOLE_BUF_NUM][CONSOLE_BUF_SIZE];
static const char *console_prompt = PROMPT;

static int console_buf_write_byte(int x, char c)
{
    rbuff_t *rb = &console_buff[x];
    int pos = rbuff_push(rb);
    if (pos < 0) {
        return 0;
    }
    console_buff_mem[x][pos] = c;
    return 1;
}

static int console_buf_read_byte(int x, char *c)
{
    rbuff_t *rb = &console_buff[x];
    int pos = rbuff_shift(rb);
    if (pos < 0) {
        return 0;
    }
    *c = console_buff_mem[x][pos];
    return 1;
}

static int console_buf_unread_byte(int x, char c)
{
    rbuff_t *rb = &console_buff[x];
    int pos = rbuff_unshift(rb);
    if (pos < 0) {
        return 0;
    }
    console_buff_mem[x][pos] = c;
    return 1;
}

static int console_input_peek(int pos)
{
    rbuff_t *rb = &console_buff[CONSOLE_IN];
    char *ptr = console_buff_mem[CONSOLE_IN];

    if (pos < rb->cnt) {
        return ptr[rbuff_get(rb, pos)];
    } else {
        return 0;
    }
}

static void console_input_delete(void)
{
    rbuff_t *rb = &console_buff[CONSOLE_IN];
    char *ptr = console_buff_mem[CONSOLE_IN];

    if (console_cursor <= 0) {
        console_puts(BELL);
        return;
    }

    int pos = console_cursor--;
    int i, n = rbuff_end(rb) - pos;
    int dst = rbuff_get(rb, pos - 1);

    console_puts(LEFT);
    for (i = 0; i < n; i++) {
        int src = rbuff_get(rb, pos + i);

        ptr[dst] = ptr[src];
        dst = src;

        console_putc(ptr[src]);
    }
    console_putc(' ');
    while (n-- >= 0) {
        console_puts(LEFT);
    }

    rbuff_remove(rb, 1);
}

static void console_input_enter(void)
{
    console_cursor = 0;
    rbuff_reset(&console_buff[CONSOLE_IN]);
    console_puts(console_prompt);
}

static void console_input_seek(int n)
{
    int pos = console_cursor + n;

    if (pos < 0 || pos > rbuff_end(&console_buff[CONSOLE_IN])) {
        console_puts(BELL);
    } else {
        console_cursor = pos;
        if (n > 0) {
            while(n--)
                console_puts(RIGHT);
        } else {
            while(n++)
                console_puts(LEFT);
        }
    }
}

static int console_input_parse(char *input, int end, int *ppos, int *pch)
{
    int  type = CON_CTRL_IDLE;
    int  pos = *ppos;
    char key = input[pos++];

    if (key >= 32 && key < 127) {
        *pch = key;
        type = CON_CTRL_CHAR;
    } else {
        if (key == 8) {
            type = CON_CTRL_BACKSPACE;
        } else
        if (key == 9) {
            type = CON_CTRL_TABLE;
        } else
        if (key == 13) {
            type = CON_CTRL_ENTER;
        } else
        if (key == 127) {
            type = CON_CTRL_DELETE;
        } else
        if (key == 27) {
            if (pos == end) {
                type = CON_CTRL_ESCAPE;
            } else
            if (input[pos] == 91) {
                key = input[pos + 1];
                if (key == 65) {
                    type = CON_CTRL_UP;
                } else
                if (key == 66) {
                    type = CON_CTRL_DOWN;
                } else
                if (key == 67) {
                    type = CON_CTRL_RIGHT;
                } else
                if (key == 68) {
                    type = CON_CTRL_LEFT;
                }
            } else
            if (input[pos] == 79) {
                key = input[pos + 1];
                if (key == 80) {
                    type = CON_CTRL_F1;
                } else
                if (key == 81) {
                    type = CON_CTRL_F2;
                } else
                if (key == 82) {
                    type = CON_CTRL_F3;
                } else
                if (key == 83) {
                    type = CON_CTRL_F4;
                }
            }
            pos = end; // Skip all left input
        }
    }
    *ppos = pos;

    return type;
}

static void console_input_proc(int type, int c)
{
    if (type == CON_CTRL_ENTER) {
        console_puts(CRLF);
    }

    if (user_handle && user_handle(type, c)) {
        return;
    }

    switch (type) {
    case CON_CTRL_IDLE:         break;
    case CON_CTRL_CHAR:         console_input_char(c); break;
    case CON_CTRL_BACKSPACE:    console_input_delete(); break;
    case CON_CTRL_DELETE:       console_input_delete(); break;
    case CON_CTRL_TABLE:        console_puts(BELL); break;
    case CON_CTRL_ENTER:        console_input_enter(); break;
    case CON_CTRL_ESCAPE:       console_puts(BELL); break;
    case CON_CTRL_UP:           console_puts(BELL); break;
    case CON_CTRL_DOWN:         console_puts(BELL); break;
    case CON_CTRL_RIGHT:        console_input_seek(1);  break;
    case CON_CTRL_LEFT:         console_input_seek(-1); break;
    case CON_CTRL_F1:
    case CON_CTRL_F2:
    case CON_CTRL_F3:
    case CON_CTRL_F4:           console_puts(BELL); break;
    default: break;
    }
}

static void console_input_handle(int n, void *data)
{
    int pos = 0;
    int ch = '.';

    while (pos < n) {
        int type = console_input_parse(data, n, &pos, &ch);

        console_input_proc(type, ch);
    }
}

static char recv_buf[16];

static void console_do_recv(cupkee_device_t *dev)
{
    int n;

    while (0 < (n = cupkee_device_recv(dev, 16, recv_buf))) {
        console_total_recv += n;
        console_input_handle(n, recv_buf);
    }
}

static void console_do_send(cupkee_device_t *dev)
{
    char c;

    while (console_buf_read_byte(CONSOLE_OUT, &c)) {
        if (!cupkee_device_send(dev, 1, &c)) {
            console_buf_unread_byte(CONSOLE_OUT, c);
            break;
        } else {
            console_total_send++;
        }
    }
}

static void console_device_handle(cupkee_device_t *dev, uint8_t code, void *param)
{
    (void) param;

    if (code == DEVICE_EVENT_DATA) {
        console_do_recv(dev);
    } else
    if (code == DEVICE_EVENT_DRAIN) {
        console_do_send(dev);
    }
}

int cupkee_console_init(cupkee_device_t *con_dev, console_handle_t handle)
{
    console_cursor = 0;
    console_total_recv = 0;

    rbuff_init(&console_buff[CONSOLE_IN],  CONSOLE_BUF_SIZE);
    rbuff_init(&console_buff[CONSOLE_OUT], CONSOLE_BUF_SIZE);

    if (!con_dev) {
        return -1;
    }

    con_dev->handle = console_device_handle;
    con_dev->handle_param = NULL;

    user_handle = handle;
    console_dev = con_dev;

    return 0;
}

int console_input_clean(void)
{
    rbuff_t *rb = &console_buff[CONSOLE_IN];
    int n = rbuff_end(rb);
    int i;

    if (n <= 0) {
        return 0;
    }

    rbuff_reset(rb);
    while (console_cursor) {
        console_putc(8);
        console_cursor--;
    }

    for (i = 0; i < n; i++) {
        console_putc(' ');
    }
    for (i = 0; i < n; i++) {
        console_putc(8);
    }

    return n;
}

int console_input_char(int ch)
{
    char c = ch;

    return console_input_insert(1, &c);
}

int console_input_token(int size, char *buf)
{
    if (console_cursor == 0) {
        return 0;
    }

    rbuff_t *rb = &console_buff[CONSOLE_IN];
    char *ptr = console_buff_mem[CONSOLE_IN];
    int start = console_cursor;
    int pos = 0;

    while ((pos = rbuff_get(rb, start - 1)) >= 0) {
        int c = ptr[pos];

        if (!isalnum(c) && c != '_') {
            break;
        }
        start--;
    }

    for (pos = 0; pos < size && start < console_cursor; pos++, start++) {
        buf[pos] = ptr[rbuff_get(rb, start)];
    }
    buf[pos] = 0;

    return pos;
}

int console_input_insert(int len, char *s)
{
    rbuff_t *rb = &console_buff[CONSOLE_IN];
    char *ptr = console_buff_mem[CONSOLE_IN];
    int pos, n, i;

    if (len == 0 || !s) {
        return -1;
    }

    if (!rbuff_has_space(rb, len)) {
        //Todo: post console error event
        return -1;
    }

    n = rbuff_end(rb) - console_cursor;
    pos = console_cursor;
    rbuff_append(rb, len);
    for (i = n - 1; i >= 0; i--)
        ptr[rbuff_get(rb, pos + i + len)] = ptr[rbuff_get(rb, pos + i)];
    for (i = 0; i < len; i++)
        ptr[rbuff_get(rb, console_cursor++)] = s[i];

    // echo back
    for (i = 0; i < n + len; i++) {
        console_putc(ptr[rbuff_get(rb, pos + i)]);
    }
    while (n--) {
        console_puts(LEFT);
    }

    return console_cursor;
}

int console_input_refresh(void)
{
    int i = 0, ch;

    console_puts(console_prompt);

    while ((ch = console_input_peek(i++)) > 0) {
        console_putc(ch);
    }

    return i;
}

int console_input_load(int size, char *buf)
{
    int n = console_buff[CONSOLE_IN].cnt;

    n = n > size ? size : n;
    memcpy(buf, console_buff_mem[CONSOLE_IN], n);

    return n;
}

int console_putc(int c)
{
    if (rbuff_is_empty(&console_buff[CONSOLE_OUT])) {
        char buf = c;
        if (cupkee_device_send(console_dev, 1, &buf)) {
            console_total_send++;
            return 1;
        }
    }

    return console_buf_write_byte(CONSOLE_OUT, c);
}

int console_puts(const char *s)
{
    const char *p = s;
    int len = strlen(s);

    if (rbuff_is_empty(&console_buff[CONSOLE_OUT])) {
        int n = cupkee_device_send(console_dev, len, s);
        if (n > 0) {
            console_total_send += n;
            p += n;
        }
    }

    while(*p && console_buf_write_byte(CONSOLE_OUT, *p))
        p++;

    return p - s;
}

int console_log(const char *fmt, ...)
{
    char buf[128];
    int  n;

    va_list va;
    va_start(va, fmt);

    n = vsnprintf(buf, 128, fmt, va);

    va_end(va);
    buf[n] = 0;

    return console_puts(buf);
}

int console_putc_sync(int c)
{
    char buf = c;

    return cupkee_device_send_sync(console_dev, 1, &buf);
}

int console_puts_sync(const char *s)
{
    int len = strlen(s);

    return cupkee_device_send_sync(console_dev, len, s);
}

int console_log_sync(const char *fmt, ...)
{
    char buf[256];
    int  n;

    va_list va;
    va_start(va, fmt);

    n = vsnprintf(buf, 255, fmt, va);

    va_end(va);
    buf[n] = 0;

    return console_puts_sync(buf);
}

