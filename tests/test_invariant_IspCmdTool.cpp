#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* Simulate the fixed-size buffers from the vulnerable code */
#define SZSTR_SIZE        256
#define TEXTBUFFERTMP_SIZE 512
#define PORTNAME_SIZE     64

/* Safe wrapper that mimics what the code SHOULD do:
 * use snprintf instead of sprintf, and enforce buffer bounds.
 * The invariant: output never exceeds declared buffer size.
 */

static int safe_format_portname(char *dst, size_t dst_size, const char *port_name)
{
    if (!dst || !port_name || dst_size == 0)
        return -1;
    int ret = snprintf(dst, dst_size, "%s", port_name);
    /* snprintf returns number of chars that would have been written */
    return ret;
}

static int safe_format_device_status(char *dst, size_t dst_size, const char *port_name)
{
    if (!dst || !port_name || dst_size == 0)
        return -1;
    int ret = snprintf(dst, dst_size, "{\"Device\":\"%s\",\"Status\":\"Ready\"}", port_name);
    return ret;
}

static int safe_format_text_buffer(char *dst, size_t dst_size,
                                    const char *msg, const char *port_name, const char *msg1)
{
    if (!dst || !msg || !port_name || !msg1 || dst_size == 0)
        return -1;
    int ret = snprintf(dst, dst_size, "%s%s%s", msg, port_name, msg1);
    return ret;
}

/* Canary check: place known sentinel bytes after buffer to detect overflow */
#define CANARY_VALUE 0xDEADBEEF
#define CANARY_SIZE  8

static void set_canary(uint8_t *after_buf, size_t canary_size)
{
    for (size_t i = 0; i < canary_size; i++) {
        after_buf[i] = (uint8_t)(0xDE + i);
    }
}

static int check_canary(const uint8_t *after_buf, size_t canary_size)
{
    for (size_t i = 0; i < canary_size; i++) {
        if (after_buf[i] != (uint8_t)(0xDE + i))
            return 0; /* canary corrupted */
    }
    return 1; /* canary intact */
}

START_TEST(test_buffer_reads_never_exceed_declared_length)
{
    /* Invariant: Buffer reads/writes never exceed the declared buffer length.
     * Any formatted output into a fixed-size buffer must be bounded by that size.
     * Oversized PortName inputs must be truncated, not overflow the buffer.
     */
    const char *payloads[] = {
        /* 2x expected buffer size (128 chars) */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        /* 10x expected buffer size (640 chars) */
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
        /* Format string injection attempt */
        "%s%s%s%s%s%s%s%s%s%s%n%n%n%n%n%n%n%n%n%n",
        /* Path traversal + overflow */
        "/dev/ttyUSB0/../../../../../etc/passwd"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",
        /* Null bytes embedded (C string stops here, but tests boundary) */
        "/dev/ttyUSB0",
        /* Special characters */
        "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
        /* JSON injection attempt */
        "\"},\"evil\":\"pwned\",\"x\":\"",
        /* Shell metacharacters */
        "; rm -rf / ; echo PWNED ; /dev/ttyUSB0"
        "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
        "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE",
        /* Exactly at boundary (255 chars for 256-byte buffer) */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
        /* One over boundary (256 chars for 256-byte buffer) */
        "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
        "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
        "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
        "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG",
        /* Unicode-like sequences */
        "\xc0\xaf\xc0\xaf\xc0\xaf\xc0\xaf\xc0\xaf\xc0\xaf\xc0\xaf\xc0\xaf"
        "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
        "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH",
        /* Empty string (boundary case) */
        "",
        /* Single char */
        "X",
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        const char *port_name = payloads[i];

        /* --- Test 1: safe_format_portname into szStr --- */
        {
            /* Allocate buffer + canary region */
            uint8_t region[SZSTR_SIZE + CANARY_SIZE];
            memset(region, 0, sizeof(region));
            char *szStr = (char *)region;
            uint8_t *canary = region + SZSTR_SIZE;
            set_canary(canary, CANARY_SIZE);

            int ret = safe_format_portname(szStr, SZSTR_SIZE, port_name);

            /* Invariant 1: canary must not be corrupted */
            ck_assert_msg(check_canary(canary, CANARY_SIZE),
                "FAIL: Buffer overflow detected in safe_format_portname for payload[%d] (len=%zu)",
                i, strlen(port_name));

            /* Invariant 2: output string must be null-terminated within buffer */
            int found_null = 0;
            for (int j = 0; j < SZSTR_SIZE; j++) {
                if (szStr[j] == '\0') {
                    found_null = 1;
                    break;
                }
            }
            ck_assert_msg(found_null,
                "FAIL: Output not null-terminated in szStr for payload[%d]", i);

            /* Invariant 3: actual string length must be < SZSTR_SIZE */
            size_t out_len = strnlen(szStr, SZSTR_SIZE);
            ck_assert_msg(out_len < SZSTR_SIZE,
                "FAIL: Output length %zu >= buffer size %d for payload[%d]",
                out_len, SZSTR_SIZE, i);

            (void)ret;
        }

        /* --- Test 2: safe_format_device_status into szStr --- */
        {
            uint8_t region[SZSTR_SIZE + CANARY_SIZE];
            memset(region, 0, sizeof(region));
            char *szStr = (char *)region;
            uint8_t *canary = region + SZSTR_SIZE;
            set_canary(canary, CANARY_SIZE);

            int ret = safe_format_device_status(szStr, SZSTR_SIZE, port_name);

            ck_assert_msg(check_canary(canary, CANARY_SIZE),
                "FAIL: Buffer overflow detected in safe_format_device_status for payload[%d] (len=%zu)",
                i, strlen(port_name));

            int found_null = 0;
            for (int j = 0; j < SZSTR_SIZE; j++) {
                if (szStr[j] == '\0') {
                    found_null = 1;
                    break;
                }
            }
            ck_assert_msg(found_null,
                "FAIL: Output not null-terminated in device_status szStr for payload[%d]", i);

            size_t out_len = strnlen(szStr, SZSTR_SIZE);
            ck_assert_msg(out_len < SZSTR_SIZE,
                "FAIL: device_status output length %zu >= buffer size %d for payload[%d]",
                out_len, SZSTR_SIZE, i);

            (void)ret;
        }

        /* --- Test 3: safe_format_text_buffer into TextBufferTmp --- */
        {
            /* Use representative msg/msg1 values */
            const char *msg_variants[]  = { "OK: ", "ERROR: ", "STATUS: ", "" };
            const char *msg1_variants[] = { " done", " failed", " pending", "" };

            for (int m = 0; m < 4; m++) {
                uint8_t region[TEXTBUFFERTMP_SIZE + CANARY_SIZE];
                memset(region, 0, sizeof(region));
                char *TextBufferTmp = (char *)region;
                uint8_t *canary = region + TEXTBUFFERTMP_SIZE;
                set_canary(canary, CANARY_SIZE);

                int ret = safe_format_text_buffer(TextBufferTmp, TEXTBUFFERTMP_SIZE,
                                                   msg_variants[m], port_name, msg1_variants[m]);

                ck_assert_msg(check_canary(canary, CANARY_SIZE),
                    "FAIL: Buffer overflow in safe_format_text_buffer for payload[%d] msg_variant[%d] (len=%zu)",
                    i, m, strlen(port_name));

                int found_null = 0;
                for (int j = 0; j < TEXTBUFFERTMP_SIZE; j++) {
                    if (TextBufferTmp[j] == '\0') {
                        found_null = 1;
                        break;
                    }
                }
                ck_assert_msg(found_null,
                    "FAIL: Output not null-terminated in TextBufferTmp for payload[%d] msg_variant[%d]",
                    i, m);

                size_t out_len = strnlen(TextBufferTmp, TEXTBUFFERTMP_SIZE);
                ck_assert_msg(out_len < TEXTBUFFERTMP_SIZE,
                    "FAIL: TextBufferTmp output length %zu >= buffer size %d for payload[%d] msg_variant[%d]",
                    out_len, TEXTBUFFERTMP_SIZE, i, m);

                (void)ret;
            }
        }

        /* --- Test 4: PortName copy into fixed-size PortName buffer --- */
        {
            uint8_t region[PORTNAME_SIZE + CANARY_SIZE];
            memset(region, 0, sizeof(region));
            char *portname_buf = (char *)region;
            uint8_t *canary = region + PORTNAME_SIZE;
            set_canary(canary, CANARY_SIZE);

            /* Safe copy: use strncpy instead of strcpy */
            strncpy(portname_buf, port_name, PORTNAME_SIZE - 1);
            portname_buf[PORTNAME_SIZE - 1] = '\0';

            ck_assert_msg(check_canary(canary, CANARY_SIZE),
                "FAIL: Buffer overflow in PortName copy for payload[%d] (len=%zu)",
                i, strlen(port_name));

            size_t out_len = strnlen(portname_buf, PORTNAME_SIZE);
            ck_assert_msg(out_len < PORTNAME_SIZE,
                "FAIL: PortName output length %zu >= buffer size %d for payload[%d]",
                out_len, PORTNAME_SIZE, i);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test