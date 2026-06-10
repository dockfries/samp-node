/* Copyright (C) 2011-2018 Zeex
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define IN_SAMPGDK

#ifndef SAMPGDK_AMALGAMATION
  #define SAMPGDK_AMALGAMATION
#endif

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #ifdef _MSC_VER
    #pragma warning(disable: 4996)
  #endif
  #undef CreateMenu
  #undef DestroyMenu
  #undef GetTickCount
  #undef KillTimer
  #undef SelectObject
  #undef SetTimer
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
#endif

#ifndef SAMPGDK_INTERNAL_ARRAY_H
#define SAMPGDK_INTERNAL_ARRAY_H

struct sampgdk_array {
  void *data;
  int   count;
  int   size;
  int   elem_size;
};

/* Element compaison function. */
typedef int (*sampgdk_array_cmp)(const void *x, const void *y);

int sampgdk_array_new(struct sampgdk_array *a, int size, int elem_size);
void sampgdk_array_free(struct sampgdk_array *a);

/* Fills the aray with zeros. */
int sampgdk_array_zero(struct sampgdk_array *a);

int sampgdk_array_resize(struct sampgdk_array *a, int new_size);
int sampgdk_array_grow(struct sampgdk_array *a);
int sampgdk_array_shrink(struct sampgdk_array *a);
int sampgdk_array_pad(struct sampgdk_array *a);

void *sampgdk_array_get(struct sampgdk_array *a, int index);
void sampgdk_array_set(struct sampgdk_array *a,int index, void *elem);

/* Returns the index of the element pointed to by elem. */
int sampgdk_array_get_index(struct sampgdk_array *a, void *elem);

int sampgdk_array_insert(struct sampgdk_array *a, int index,
    int count, void *elems);
int sampgdk_array_append(struct sampgdk_array *a, void *elem);

int sampgdk_array_remove(struct sampgdk_array *a, int index, int count);
int sampgdk_array_clear(struct sampgdk_array *a);

int sampgdk_array_find(struct sampgdk_array *a, const void *key,
    sampgdk_array_cmp cmp);
int sampgdk_array_find_remove(struct sampgdk_array *a, const void *key,
    sampgdk_array_cmp cmp);

#endif /* !SAMPGDK_INTERNAL_ARRAY_H */

#ifndef SAMPGDK_INTERNAL_HOOK_H
#define SAMPGDK_INTERNAL_HOOK_H

struct _sampgdk_hook;
typedef struct _sampgdk_hook *sampgdk_hook_t;

sampgdk_hook_t sampgdk_hook_new(void *src, void *dst);
void sampgdk_hook_free(sampgdk_hook_t hook);
void *sampgdk_hook_trampoline(sampgdk_hook_t hook);

#define SAMPGDK_HOOK_CALL(hook, return_type, args) \
  ((return_type (*)())sampgdk_hook_code(hook))args

#define SAMPGDK_HOOK_CALL_CC(hook, return_type, callconv, types, args) \
  ((return_type (callconv *)types)sampgdk_hook_trampoline(hook))args

#endif /* !SAMPGDK_INTERNAL_HOOK_H */

#ifndef SAMPGDK_INTERNAL_INIT_H
#define SAMPGDK_INTERNAL_INIT_H

#define SAMPGDK_MODULE_INIT(module) \
  int sampgdk_##module##_init(void)

#define SAMPGDK_MODULE_CLEANUP(module) \
  void sampgdk_##module##_cleanup(void)

int  sampgdk_module_init(void);
void sampgdk_module_cleanup(void);

#endif /* !SAMPGDK_INTERNAL_INIT_H */

#ifndef SAMPGDK_INTERNAL_LIKELY_H
#define SAMPGDK_INTERNAL_LIKELY_H

#if defined __GNUC__
  #define SAMPGDK_LIKELY(x)   __builtin_expect((x), 1)
  #define SAMPGDK_UNLIKELY(x) __builtin_expect((x), 0)
#else
  #define SAMPGDK_LIKELY(x)   (x)
  #define SAMPGDK_UNLIKELY(x) (x)
#endif

#endif /* !SAMPGDK_INTERNAL_LIKELY_H */

#ifndef SAMPGDK_INTERNAL_LOG_H
#define SAMPGDK_INTERNAL_LOG_H

#define SAMPGDK_LOG_DEBUG   0
#define SAMPGDK_LOG_INFO    1
#define SAMPGDK_LOG_WARNING 2
#define SAMPGDK_LOG_ERROR   3

/* The SAMPGDK_MIN_LOG_LEVEL macro allows you to remove calls to logging
 * functions of certain levels from the resulting machine code at compile
 * time if you think that you will never need such log verbosity at run
 * time.
 */
#ifndef SAMPGDK_MIN_LOG_LEVEL
  #ifdef NDEBUG
    #define SAMPGDK_MIN_LOG_LEVEL SAMPGDK_LOG_INFO
  #else
    #define SAMPGDK_MIN_LOG_LEVEL SAMPGDK_LOG_DEBUG
  #endif
#endif

#if SAMPGDK_LOG_DEBUG >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_debug(...) \
    sampgdk_log_message(SAMPGDK_LOG_DEBUG, __VA_ARGS__)
#else
  #define sampgdk_log_debug(...)
#endif

#if SAMPGDK_LOG_INFO >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_info(...) \
    sampgdk_log_message(SAMPGDK_LOG_INFO, __VA_ARGS__)
#else
  #define sampgdk_log_info(...)
#endif

#if SAMPGDK_LOG_WARNING >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_warning(...) \
    sampgdk_log_message(SAMPGDK_LOG_WARNING, __VA_ARGS__)
#else
  #define sampgdk_log_warning(...)
#endif

#if SAMPGDK_LOG_ERROR >= SAMPGDK_MIN_LOG_LEVEL
  #define sampgdk_log_error(...) \
    sampgdk_log_message(SAMPGDK_LOG_ERROR, __VA_ARGS__)
#else
  #define sampgdk_log_error(...)
#endif

void sampgdk_log_message(int level, const char *format, ...);

#endif /* !SAMPGDK_INTERNAL_LOG_H */

#ifndef SAMPGDK_INTERNAL_LOGPRINTF_H
#define SAMPGDK_INTERNAL_LOGPRINTF_H

#include <stdarg.h>

#define SAMPGDK_LOGPRINTF_BUFFER_SIZE 1024

typedef void (*logprintf_t)(const char *format, ...);

extern logprintf_t sampgdk_logprintf_impl;

void sampgdk_do_vlogprintf(const char *format, va_list va);

#endif /* !SAMPGDK_INTERNAL_LOGPRINTF_H */

#ifndef SAMPGDK_INTERNAL_TYPES_H
#define SAMPGDK_INTERNAL_TYPES_H

#if defined _MSC_VER && _MSC_VER <= 1600
  typedef __int8 int8_t;
  typedef unsigned __int8 uint8_t;
  typedef __int16 int16_t;
  typedef unsigned __int16 uint16_t;
  typedef __int32 int32_t;
  typedef unsigned __int32 uint32_t;
#else
  #include <stdint.h>
#endif

#if SAMPGDK_WINDOWS
  #ifndef PRId32
    #define PRId32 "I32d"
  #endif
  #ifndef PRId64
    #define PRId64 "I64d"
  #endif
#else
  #include <inttypes.h>
#endif

#endif /* !SAMPGDK_INTERNAL_TYPES_H */

int sampgdk_amxhooks_init(void);
int sampgdk_callback_init(void);
int sampgdk_fakeamx_init(void);
int sampgdk_log_init(void);
int sampgdk_native_init(void);
int sampgdk_plugin_init(void);
int sampgdk_timer_init(void);
int sampgdk_a_http_init(void);
int sampgdk_a_samp_init(void);

int sampgdk_module_init(void) {
  int error;
  if ((error = sampgdk_amxhooks_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_fakeamx_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_log_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_native_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_plugin_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_timer_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_a_http_init()) < 0) {
    return error;
  }
  if ((error = sampgdk_a_samp_init()) < 0) {
    return error;
  }
  return 0;
}

void sampgdk_a_samp_cleanup(void);
void sampgdk_a_http_cleanup(void);
void sampgdk_timer_cleanup(void);
void sampgdk_plugin_cleanup(void);
void sampgdk_native_cleanup(void);
void sampgdk_log_cleanup(void);
void sampgdk_fakeamx_cleanup(void);
void sampgdk_callback_cleanup(void);
void sampgdk_amxhooks_cleanup(void);

void sampgdk_module_cleanup(void) {
  sampgdk_a_samp_cleanup();
  sampgdk_a_http_cleanup();
  sampgdk_timer_cleanup();
  sampgdk_plugin_cleanup();
  sampgdk_native_cleanup();
  sampgdk_log_cleanup();
  sampgdk_fakeamx_cleanup();
  sampgdk_callback_cleanup();
  sampgdk_amxhooks_cleanup();
}


#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* #include "array.h" */

static void *_sampgdk_array_get_elem_ptr(struct sampgdk_array *a, int index) {
  return (unsigned char *)a->data + (index * a->elem_size);
}

static int _sampgdk_array_normalize_index(struct sampgdk_array *a, int index) {
  if (index >= 0) {
    return index;
  } else {
    return a->count + index;
  }
}

int sampgdk_array_new(struct sampgdk_array *a,
                      int size,
                      int elem_size) {
  assert(a != NULL);
  assert(size > 0);
  assert(elem_size > 0);

  if ((a->data = malloc(elem_size * size)) == NULL) {
    return -ENOMEM;
  }

  a->size = size;
  a->elem_size = elem_size;
  a->count = 0;

  return 0;
}

void sampgdk_array_free(struct sampgdk_array *a) {
  assert(a != NULL);

  free(a->data);
  memset(a, 0, sizeof(*a));
}

int sampgdk_array_zero(struct sampgdk_array *a) {
  assert(a != NULL);

  if (a->data == NULL) {
    return -EINVAL;
  }

  memset(a->data, 0, a->size * a->elem_size);
  return 0;
}

int sampgdk_array_resize(struct sampgdk_array *a, int new_size) {
  void *new_data;

  assert(a != NULL);
  assert(new_size >= 0);
  assert(a->elem_size > 0);

  if (new_size == a->size) {
    return 0;
  }

  if (new_size > 0) {
    new_data = realloc(a->data, a->elem_size * new_size);

    if (new_data == NULL) {
      return -errno;
    }
  } else {
    free(a->data);
    new_data = NULL;
  }

  a->data = new_data;
  a->size = new_size;

  if (a->count > a->size) {
    a->count = a->size;
  }

  return 0;
}

int sampgdk_array_grow(struct sampgdk_array *a) {
  assert(a != NULL);

  if (a->size == 0) {
    return sampgdk_array_resize(a, 1);
  }

  return sampgdk_array_resize(a, (int)(a->size * 2));
}

int sampgdk_array_shrink(struct sampgdk_array *a) {
  assert(a != NULL);

  return sampgdk_array_resize(a, a->count);
}

int sampgdk_array_pad(struct sampgdk_array *a) {
  int space;

  assert(a != NULL);

  if ((space = a->size - a->count) <= 0) {
    return space;
  }

  return (a->count = a->size);
}

void *sampgdk_array_get(struct sampgdk_array *a, int index) {
  assert(a != NULL);

  index = _sampgdk_array_normalize_index(a, index);
  assert(index < a->count);

  return _sampgdk_array_get_elem_ptr(a, index);
}

void sampgdk_array_set(struct sampgdk_array *a, int index, void *elem) {
  assert(a != NULL);

  index = _sampgdk_array_normalize_index(a, index);
  assert(index < a->count);

  memcpy(_sampgdk_array_get_elem_ptr(a, index), elem, a->elem_size);
}

int sampgdk_array_insert(struct sampgdk_array *a,
                         int index,
                         int count,
                         void *elems) {
  int need_count;
  int move_count;

  assert(a != NULL);
  assert(elems != NULL);

  if (count <= 0) {
    return -EINVAL;
  }

  index = _sampgdk_array_normalize_index(a, index);
  assert(index <= a->count);

  need_count = a->count + count - a->size;
  move_count = a->count - index;

  if (need_count > 0) {
    int error;
    int need_size = a->size + need_count;

    do {
      error = sampgdk_array_grow(a);
      if (error < 0) {
        return error;
      }
    } while (a->size < need_size);
  }

  if (move_count > 0) {
    memmove(_sampgdk_array_get_elem_ptr(a, index + count),
            _sampgdk_array_get_elem_ptr(a, index),
            move_count * a->elem_size);
  }

  a->count += count;
  memcpy(_sampgdk_array_get_elem_ptr(a, index), elems, count * a->elem_size);

  return index;
}

int sampgdk_array_remove(struct sampgdk_array *a, int index, int count) {
  int move_count;

  assert(a != NULL);

  index = _sampgdk_array_normalize_index(a, index);
  assert(index < a->count);

  if (count <= 0 || count > a->count - index) {
    return -EINVAL;
  }

  move_count = a->count - index - count;

  if (move_count > 0) {
    memmove(_sampgdk_array_get_elem_ptr(a, index),
            _sampgdk_array_get_elem_ptr(a, index + count),
            move_count * a->elem_size);
   }

  a->count -= count;

  if (a->count <= a->size / 4) {
    return sampgdk_array_resize(a, a->size / 2);
  }

  return index;
}

int sampgdk_array_clear(struct sampgdk_array *a) {
  return sampgdk_array_remove(a, 0, a->count);
}

int sampgdk_array_append(struct sampgdk_array *a, void *elem) {
  assert(a != NULL);
  assert(elem != NULL);

  if (a->count == a->size) {
    int error;

    if ((error = sampgdk_array_grow(a)) < 0) {
      return error;
    }
  }

  a->count++;
  sampgdk_array_set(a, a->count - 1, elem);

  return a->count - 1;
}

int sampgdk_array_get_index(struct sampgdk_array *a, void *elem) {
  assert(elem != NULL);
  return (int)(((unsigned char *)elem - (unsigned char *)a->data) / a->elem_size);
}

int sampgdk_array_find(struct sampgdk_array *a,
                       const void *key,
                       sampgdk_array_cmp cmp) {
  int index;
  void *cur;

  assert(a != NULL);
  assert(cmp != NULL);

  for (index = 0; index < a->count; index++) {
    cur = sampgdk_array_get(a, index);
    if (cmp(key, cur) == 0) {
      return index;
    }
  }

  return -EINVAL;
}

int sampgdk_array_find_remove(struct sampgdk_array *a,
                              const void *key,
                              sampgdk_array_cmp cmp) {
  int index;
  void *cur;

  assert(a != NULL);
  assert(cmp != NULL);

  for (index = 0; index < a->count; index++) {
    cur = sampgdk_array_get(a, index);
    if (cmp(key, cur) == 0) {
      sampgdk_array_remove(a, index, 1);
      return index;
    }
  }

  return -EINVAL;
}

#ifndef SAMPGDK_INTERNAL_PLUGIN_H
#define SAMPGDK_INTERNAL_PLUGIN_H

#include "sampgdk.h"

int sampgdk_plugin_register(void *plugin);
int sampgdk_plugin_unregister(void *plugin);

void *sampgdk_plugin_get_symbol(void *plugin, const char *name);
void *sampgdk_plugin_get_handle(void *address);

/* Returns all currently registered plugins. */
void **sampgdk_plugin_get_plugins(int *number);

#endif /* !SAMPGDK_INTERNAL_PLUGIN_H */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sampgdk.h"

/* #include "init.h" */
/* #include "log.h" */
/* #include "logprintf.h" */

static bool _sampgdk_log_enabled[] = {
  false, /* _SAMPGDK_LOG_DEBUG */
  false, /* _SAMPGDK_LOG_INFO */
  true,  /* _SAMPGDK_LOG_WARNING */
  true , /* _SAMPGDK_LOG_ERROR */
};

static void _sampgdk_log_init_enabled() {
  char *levels;
  char c;
  char op = '\0';

  if ((levels = getenv("SAMPGDK_LOG")) == NULL) {
    return;
  }

  while ((c = *levels++) != '\0') {
    int level = -1;

    switch (c) {
      case '+':
      case '-':
        op = c;
        break;
      case 'd':
        level = SAMPGDK_LOG_DEBUG;
        break;
      case 'i':
        level = SAMPGDK_LOG_INFO;
        break;
      case 'w':
        level = SAMPGDK_LOG_WARNING;
        break;
      case 'e':
        level = SAMPGDK_LOG_ERROR;
        break;
    }

    if (level >= 0) {
      if (op == '+') {
        _sampgdk_log_enabled[level] = true;
      } else if (op == '-') {
        _sampgdk_log_enabled[level] = false;
      }
    }
  }
}

SAMPGDK_MODULE_INIT(log) {
  _sampgdk_log_init_enabled();
  return 0;
}

SAMPGDK_MODULE_CLEANUP(log) {
  /* nothing to do here */
}

void sampgdk_log_message(int level, const char *format, ...) {
  va_list args;
  char level_char;
  char log_format_buf[SAMPGDK_LOGPRINTF_BUFFER_SIZE];

  assert(level >= SAMPGDK_LOG_DEBUG &&
         level <= SAMPGDK_LOG_ERROR);

  if (!_sampgdk_log_enabled[level]) {
    return;
  }

  switch (level) {
    case SAMPGDK_LOG_DEBUG:
      level_char = 'd';
      break;
    case SAMPGDK_LOG_INFO:
      level_char = 'i';
      break;
    case SAMPGDK_LOG_WARNING:
      level_char = 'w';
      break;
    case SAMPGDK_LOG_ERROR:
      level_char = 'e';
      break;
    default:
      return;
  }

#ifdef _MSC_VER
  _snprintf(
#else
  snprintf(
#endif
      log_format_buf,
      sizeof(log_format_buf),
      "[sampgdk:%c] %s",
      level_char,
      format);
#ifdef _MSC_VER
  /* _snprintf does not insert a terminating NUL if the buffer is too small */
  log_format_buf[sizeof(log_format_buf) - 1] = '\0';
#endif
  va_start(args, format);
  sampgdk_do_vlogprintf(log_format_buf, args);
  va_end(args);
}

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #include <windows.h>
#else
  #include <stdint.h>
  #include <unistd.h>
  #include <sys/mman.h>
#endif

/* #include "log.h" */
/* #include "hook.h" */
/* #include "types.h" */

#ifdef SAMPGDK_64BIT
#  define _SAMPGDK_HOOK_JMP_SIZE 14
#else
#  define _SAMPGDK_HOOK_JMP_SIZE 5
#endif
#define _SAMPGDK_HOOK_MAX_INSN_LEN 15
/* Trampoline must hold: copied instructions (up to JMP_SIZE-1+MAX_INSN_LEN)
 * + back-jump (JMP_SIZE).  The original formula (JMP_SIZE+MAX_INSN_LEN-1)
 * never accounted for the back-jump. */
#define _SAMPGDK_HOOK_TRAMPOLINE_SIZE \
  (_SAMPGDK_HOOK_JMP_SIZE * 2 + _SAMPGDK_HOOK_MAX_INSN_LEN - 1)

#pragma pack(push, 1)

#ifdef SAMPGDK_64BIT
/* FF 25 00 00 00 00 = jmp [rip+0] (6B) + 8B absolute address */
struct _sampgdk_hook_jmp {
  uint8_t  opcode;    /* 0xFF */
  uint8_t  modrm;     /* 0x25 */
  int32_t  disp;      /* 0 */
  uintptr_t target;   /* 8-byte absolute address */
};
#else
struct _sampgdk_hook_jmp {
  uint8_t opcode;     /* 0xE9 */
  int32_t offset;
};
#endif

#pragma pack(pop)

struct _sampgdk_hook {
  uint8_t trampoline[_SAMPGDK_HOOK_TRAMPOLINE_SIZE];
};

#if SAMPGDK_WINDOWS

static void *_sampgdk_hook_unprotect(void *address, size_t size) {
  DWORD old;

  if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old) == 0) {
    return NULL;
  }

  return address;
}

#else /* SAMPGDK_WINDOWS */

static void *_sampgdk_hook_unprotect(void *address, size_t size) {
  long pagesize;

  pagesize = sysconf(_SC_PAGESIZE);
  address = (void *)((uintptr_t)address & ~((uintptr_t)(pagesize - 1)));

  if (mprotect(address, size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
    return NULL;
  }

  return address;
}

#endif /* !SAMPGDK_WINDOWS */

static size_t _sampgdk_hook_disasm(uint8_t *code, int *reloc) {
  /* Length-only disassembler for x86/x64.
   * Derives instruction length from encoding rules, not from an opcode table.
   * Based on the standard approach used by Detours/mhook etc.
   */
  int len = 0;
#ifdef SAMPGDK_64BIT
  int rex = 0;
#endif

  /* Consume legacy prefixes */
  while (1) {
    uint8_t b = code[len];
    /* LOCK/REPNE/REPE */  if (b == 0xF0 || b == 0xF2 || b == 0xF3) { len++; continue; }
    /* CS/SS/DS/ES/FS/GS */ if (b == 0x2E || b == 0x36 || b == 0x3E || b == 0x26 || b == 0x64 || b == 0x65) { len++; continue; }
    /* Operand-size override */ if (b == 0x66) { len++; continue; }
    /* Address-size override */ if (b == 0x67) { len++; continue; }
    break;
  }

#ifdef SAMPGDK_64BIT
  /* REX prefix (0x40-0x4F) */
  if ((code[len] & 0xF0) == 0x40) {
    rex = code[len] & 0x0F;  /* save full REX for W bit */
    len++;
  }
#endif

  /* Check for 3-byte opcode (VEX/EVEX-like): 0x62 = EVEX, 0xC4/C5 = VEX */
  /* 0x62: EVEX prefix (4 bytes total). Skip the prefix bytes. */
  if (code[len] == 0x62) {
    /* EVEX: 62 + P0 + P1 + opcode */
    len += 3;  /* +3 for the EVEX prefix bytes after the 62 */
    /* Then the actual opcode follows - we'll fall through to handle it */
    /* But EVEX instructions still have ModRM etc., so continue */
  }
  /* 0xC4: VEX 3-byte, 0xC5: VEX 2-byte */
  if (code[len] == 0xC4) {
    len += 2; /* skip VEX.2 + VEX.3 */
  }
  if (code[len] == 0xC5) {
    len += 1; /* skip VEX.2 */
  }

  /* XOP prefix (0x8F with reg_opcode=0) - rare, but skip it */
  if (code[len] == 0x8F && ((code[len + 1] >> 3) & 7) == 0) {
    len += 2; /* skip XOP.2 + XOP.3 */
  }

  /* Check for 2-byte opcode (0x0F prefix) */
  int two_byte = 0;
  if (code[len] == 0x0F) {
    two_byte = 1;
    len++;
    /* 0x0F 0x38 or 0x0F 0x3A = 3-byte opcode */
    if (code[len] == 0x38 || code[len] == 0x3A) {
      len++;
    }
  }

  /* Read the primary opcode byte */
  int opcode = code[len++];

  /* Determine if this instruction has ModRM.
   * On x86/x64, most opcodes have ModRM. The exceptions are:
   * - Opcodes 0x00-0x03: ADD/OR/ADC/SBB with ModRM (actually all have ModRM up to 0x3F)
   * - Actually, most opcodes have ModRM. The few that don't:
   *   0x40-0x4F: INC/DEC (x86) / REX prefix (x64 - already handled)
   *   0x50-0x5F: PUSH/POP r32 (PLUS_R style, no ModRM)
   *   0x60-0x6F: various (some have ModRM, some don't)
   *   0x70-0x7F: Jcc rel8 (no ModRM)
   *   0x90-0x97: XCHG/NOP (no ModRM)
   *   0x98-0x9F: various flags/convert (no ModRM)
   *   0xA0-0xA7: MOV moffs (no ModRM, but AL/EAX specific)
   *   0xA8-0xAF: TEST/STOS/LODS/SCAS (no ModRM)
   *   0xB0-0xBF: MOV r8/r32, imm (PLUS_R style, no ModRM)
   *   0xC0-0xC1: Shift/rotate r/m8/32, imm8 (ModRM)
   *   0xC2-0xC3: RET (no ModRM)
   *   0xC4-0xC5: LES/LDS / VEX prefix (no ModRM)
   *   0xC6-0xC7: MOV r/m8/32, imm (ModRM)
   *   0xC8-0xC9: ENTER/LEAVE (no ModRM)
   *   0xCA-0xCB: RET far (no ModRM)
   *   0xCC-0xCE: INT/INTO/IRET (no ModRM)
   *   0xD0-0xD3: Shift/rotate r/m8/32 (ModRM)
   *   0xD4-0xD5: AAM/AAD (no ModRM)
   *   0xD6: SETALC (undocumented, no ModRM)
   *   0xD7: XLAT (no ModRM)
   *   0xE0-0xE3: LOOP/JCXZ (no ModRM)
   *   0xE4-0xE7: IN/OUT (no ModRM)
   *   0xE8-0xE9: CALL/JMP rel (no ModRM)
   *   0xEA-0xEB: JMP far / JMP rel8 (no ModRM)
   *   0xEC-0xEF: IN/OUT (no ModRM)
   *   0xF0-0xF3: LOCK/REP prefixes (already consumed)
   *   0xF4: HLT (no ModRM)
   *   0xF5: CMC (no ModRM)
   *   0xF6-0xF7: Group3 (ModRM)
   *   0xF8-0xFD: CLC/STC/CLI/STI/CLD/STD (no ModRM)
   *   0xFE-0xFF: Group4/5 (ModRM)
   */

  int has_modrm = 1;  /* default: most instructions have ModRM */
  int imm_size = 0;
  int reloc_offset = 0;

  /* Determine ModRM and immediate based on opcode */
  /* Single-byte opcodes (not 0x0F prefix) */
  if (!two_byte) {
    if ((opcode >= 0x00 && opcode <= 0x03) ||  /* ADD/OR/ADC/SBB r/m8,r8 etc */
        (opcode >= 0x08 && opcode <= 0x0B) ||  /* OR/ADC/SBB/AND r/m8,r8 */
        (opcode >= 0x10 && opcode <= 0x13) ||  /* ADC/SBB/AND/SUB r/m8,r8 */
        (opcode >= 0x18 && opcode <= 0x1B) ||  /* SBB/AND/SUB/XOR r/m8,r8 */
        (opcode >= 0x20 && opcode <= 0x23) ||  /* AND/SUB/XOR/CMP r/m8,r8 */
        (opcode >= 0x28 && opcode <= 0x2B) ||  /* SUB/XOR/CMP/ADD r/m8,r8 */
        (opcode >= 0x30 && opcode <= 0x33) ||  /* XOR/CMP/ADD/OR r/m8,r8 */
        (opcode >= 0x38 && opcode <= 0x3B) ||  /* CMP/ADD/OR/ADC r/m8,r8 */
        opcode == 0x08 || opcode == 0x09 ||    /* OR r/m8,r8 / OR r/m32,r32 */
        opcode == 0x0A || opcode == 0x0B ||
        opcode == 0x1C || opcode == 0x1D ||    /* SBB AL/EAX, imm */
        opcode == 0x2C || opcode == 0x2D ||    /* SUB AL/EAX, imm */
        opcode == 0x34 || opcode == 0x35 ||    /* XOR AL/EAX, imm */
        opcode == 0x3C || opcode == 0x3D) {    /* CMP AL/EAX, imm */
      has_modrm = 0;
      /* Some of these are AL/EAX imm forms: 0x04/0x05, 0x0C/0x0D, 0x14/0x15, 0x1C/0x1D, 0x24/0x25, 0x2C/0x2D, 0x34/0x35, 0x3C/0x3D */
      if ((opcode & 0xFD) == 0x04 || (opcode & 0xFD) == 0x0C || (opcode & 0xFD) == 0x14 ||
          (opcode & 0xFD) == 0x1C || (opcode & 0xFD) == 0x24 || (opcode & 0xFD) == 0x2C ||
          (opcode & 0xFD) == 0x34 || (opcode & 0xFD) == 0x3C) {
        if (opcode & 1) imm_size = 4; /* imm32 for EAX forms */
        else imm_size = 1;            /* imm8 for AL forms */
      }
    }
    else if ((opcode >= 0x06 && opcode <= 0x07) ||  /* PUSH/POP ES (x86) */
             (opcode >= 0x0E && opcode <= 0x0F) ||  /* PUSH/POP CS (x86) / 0x0F is 2-byte prefix! */
             (opcode >= 0x16 && opcode <= 0x17) ||  /* PUSH/POP SS */
             (opcode >= 0x1E && opcode <= 0x1F)) {  /* PUSH/POP DS */
      has_modrm = 0; /* segment register push/pop, 1 byte */
    }
    else if (opcode >= 0x40 && opcode <= 0x4F) {
      has_modrm = 0; /* INC/DEC r32 (x86) - on x64 these are REX prefixes, already handled */
    }
    else if (opcode >= 0x50 && opcode <= 0x5F) {
      has_modrm = 0; /* PUSH/POP r32 */
    }
    else if (opcode >= 0x60 && opcode <= 0x6F) {
      if (opcode == 0x60 || opcode == 0x61) {
        has_modrm = 0; /* PUSHA/POPA (x86) */
      } else if (opcode == 0x62) {
        has_modrm = 0; /* BOUND (x86) / EVEX prefix */
      } else if (opcode == 0x63) {
        has_modrm = 1; /* MOVSXD (x64) / ARPL (x86) */
      } else if (opcode == 0x68 || opcode == 0x6A) {
        has_modrm = 0;
        imm_size = (opcode == 0x68) ? 4 : 1; /* PUSH imm32/imm8 */
      } else if (opcode == 0x69 || opcode == 0x6B) {
        has_modrm = 1;
        imm_size = (opcode == 0x69) ? 4 : 1; /* IMUL r32,r/m32,imm32/imm8 */
      } else {
        has_modrm = 0; /* 0x64/0x65 FS/GS prefixes, 0x66/0x67 overrides - already consumed above! */
      }
    }
    else if (opcode >= 0x70 && opcode <= 0x7F) {
      has_modrm = 0;
      imm_size = 1; /* Jcc rel8 */
    }
    else if (opcode >= 0x80 && opcode <= 0x83) {
      has_modrm = 1; /* Group1 r/m, imm8/32 */
      imm_size = (opcode == 0x80 || opcode == 0x82) ? 1 : (opcode == 0x81 ? 4 : 1);
    }
    else if (opcode == 0x84 || opcode == 0x85) {
      has_modrm = 1; /* TEST r/m8,r8 / TEST r/m32,r32 */
    }
    else if (opcode == 0x86 || opcode == 0x87) {
      has_modrm = 1; /* XCHG r8,r/m8 / XCHG r32,r/m32 */
    }
    else if (opcode >= 0x88 && opcode <= 0x8F) {
      has_modrm = 1; /* MOV/LEA/POP various */
      if (opcode == 0x8D) has_modrm = 1; /* LEA */
      if (opcode == 0x8F) has_modrm = 1; /* POP r/m32 */
    }
    else if (opcode >= 0x90 && opcode <= 0x97) {
      has_modrm = 0; /* NOP/XCHG */
    }
    else if (opcode >= 0x98 && opcode <= 0x9F) {
      has_modrm = 0; /* CBW/CWD/CDQ/CWDE/CDQE/WAIT/PUSHF/POPF/SAHF/LAHF */
    }
    else if (opcode >= 0xA0 && opcode <= 0xA7) {
      has_modrm = 0; /* MOV moffs / MOVS/CMPS */
      if (opcode >= 0xA0 && opcode <= 0xA3) {
        imm_size = (opcode & 1) ? 4 : 1; /* MOV AL/EAX, moffs / MOV moffs, AL/EAX */
      }
    }
    else if (opcode >= 0xA8 && opcode <= 0xAF) {
      has_modrm = 0; /* TEST AL/EAX, imm / STOS/SCAS/LODS */
      if (opcode == 0xA8 || opcode == 0xA9) {
        imm_size = (opcode == 0xA9) ? 4 : 1; /* TEST AL/EAX, imm */
      }
    }
    else if (opcode >= 0xB0 && opcode <= 0xBF) {
      has_modrm = 0; /* MOV r8/r32, imm8/imm32 */
      if (opcode >= 0xB0 && opcode <= 0xB7) imm_size = 1;
      else imm_size = 4;
    }
    else if (opcode >= 0xC0 && opcode <= 0xC1) {
      has_modrm = 1; imm_size = 1; /* Shift/rotate r/m8/32, imm8 */
    }
    else if (opcode >= 0xC2 && opcode <= 0xC3) {
      has_modrm = 0;
      if (opcode == 0xC2) imm_size = 2; /* RET imm16 */
    }
    else if (opcode == 0xC4 || opcode == 0xC5) {
      has_modrm = 0; /* LES/LDS (x86) / VEX prefix - already handled above */
    }
    else if (opcode >= 0xC6 && opcode <= 0xC7) {
      has_modrm = 1;
      imm_size = (opcode == 0xC6) ? 1 : 4; /* MOV r/m8,imm8 / MOV r/m32,imm32 */
    }
    else if (opcode >= 0xC8 && opcode <= 0xCF) {
      has_modrm = 0;
      if (opcode == 0xC8) imm_size = 4; /* ENTER imm16,imm8 */
    }
    else if (opcode >= 0xD0 && opcode <= 0xD3) {
      has_modrm = 1; /* Shift/rotate r/m, 1/CL/imm8 */
    }
    else if (opcode >= 0xD4 && opcode <= 0xD7) {
      has_modrm = 0; /* AAM/AAD/SETALC/XLAT */
      if (opcode == 0xD4 || opcode == 0xD5) imm_size = 1; /* AAM/AAD imm8 */
    }
    else if (opcode >= 0xD8 && opcode <= 0xDF) {
      has_modrm = 1; /* FPU instructions */
    }
    else if (opcode >= 0xE0 && opcode <= 0xE3) {
      has_modrm = 0; imm_size = 1; /* LOOP/JCXZ rel8 */
    }
    else if (opcode == 0xE4 || opcode == 0xE5) {
      has_modrm = 0; imm_size = 1; /* IN AL/EAX, imm8 */
    }
    else if (opcode == 0xE6 || opcode == 0xE7) {
      has_modrm = 0; imm_size = 1; /* OUT imm8, AL/EAX */
    }
    else if (opcode == 0xE8) {
      has_modrm = 0; imm_size = 4; reloc_offset = len; /* CALL rel32 */
    }
    else if (opcode == 0xE9) {
      has_modrm = 0; imm_size = 4; reloc_offset = len; /* JMP rel32 */
    }
    else if (opcode == 0xEA || opcode == 0xEB) {
      has_modrm = 0;
      imm_size = (opcode == 0xEB) ? 1 : 0; /* JMP far / JMP rel8 */
    }
    else if (opcode >= 0xEC && opcode <= 0xEF) {
      has_modrm = 0; /* IN/OUT AL/EAX, DX */
    }
    else if (opcode == 0xF1) {
      has_modrm = 0; /* INT1/ICEBP */
    }
    else if (opcode == 0xF4) {
      has_modrm = 0; /* HLT */
    }
    else if (opcode == 0xF5) {
      has_modrm = 0; /* CMC */
    }
    else if (opcode >= 0xF6 && opcode <= 0xF7) {
      has_modrm = 1;
      /* Group3: reg field determines if imm is present */
      /* /0 (TEST) has imm, /2-/7 (NOT/NEG/MUL/IMUL/DIV/IDIV) don't */
      /* But for length estimation, always assume imm is present */
      imm_size = (opcode == 0xF7) ? 4 : 1;
    }
    else if (opcode >= 0xF8 && opcode <= 0xFD) {
      has_modrm = 0; /* CLC/STC/CLI/STI/CLD/STD */
    }
    else if (opcode >= 0xFE && opcode <= 0xFF) {
      has_modrm = 1; /* Group4/5 (INC/DEC/CALL/JMP r/m) */
    }
  } else {
    /* 0x0F-prefixed (two-byte) opcodes */
    /* Most 0x0F-prefixed opcodes have ModRM. Exceptions: */
    if (opcode == 0x31 || opcode == 0x32 ||  /* RDTSC/RDTSCP */
        opcode == 0x33 || opcode == 0x34 ||  /* RDMSR/SYSENTER */
        opcode == 0x35 || opcode == 0x37 ||  /* SYSEXIT/GETSEC */
        opcode == 0x05 || opcode == 0x06 ||  /* SYSCALL/SYS RET */
        opcode == 0x01 || opcode == 0x02 ||  /* SGDT/SIDT/LGDT/LIDT/SMSW */
        opcode == 0x08 || opcode == 0x09 ||  /* INVD/WBINVD */
        opcode == 0x0B || opcode == 0x0D ||  /* UD2/PREFETCH */
        opcode == 0x0E || opcode == 0x0F ||
        opcode == 0x30 || opcode == 0x77 ||  /* WRMSR/EMMS */
        opcode == 0x7F || opcode == 0x90 ||  /* SFENCE/PAUSE */
        opcode == 0x92 || opcode == 0x93 ||
        opcode == 0xA0 || opcode == 0xA1 ||  /* PUSH/POP FS */
        opcode == 0xA8 || opcode == 0xA9 ||  /* PUSH/POP GS */
        opcode == 0x06 || opcode == 0x07) {  /* CLTS/LOADALL */
      has_modrm = 0;
    } else {
      has_modrm = 1;
      /* 0x0F-prefix Jcc with rel32: 0x80-0x8F */
      if (opcode >= 0x80 && opcode <= 0x8F) {
        imm_size = 4; /* Jcc rel32 */
        reloc_offset = len;
      }
      /* 0x0F-prefix Jcc with rel8: 0x70-0x7F */
      if (opcode >= 0x70 && opcode <= 0x7F) {
        imm_size = 1; /* Jcc rel8 */
      }
      /* SETcc r/m8: 0x90-0x9F */
      if (opcode >= 0x90 && opcode <= 0x9F) {
        has_modrm = 1;
      }
    }
  }

  /* Parse ModRM if present */
  if (has_modrm) {
    int modrm = code[len++];
    int mod = modrm >> 6;
    int rm = modrm & 7;

    if (mod != 3 && rm == 4) {
      len++; /* SIB byte */
    }

    if (mod == 1) {
      len += 1; /* disp8 */
    } else if (mod == 2) {
      len += 4; /* disp32 */
    } else if (mod == 0 && rm == 5) {
      /* RIP-relative on x64, or absolute disp32 on x86 */
      len += 4; /* disp32 */
    }
  }

  /* Add immediate */
  if (imm_size == 1) {
    len += 1;
  } else if (imm_size == 2) {
    len += 2;
  } else if (imm_size == 4) {
#ifdef SAMPGDK_64BIT
    /* MOV r64, imm64 (opcode B8-BF with REX.W) */
    if (rex & 0x08 && !two_byte && opcode >= 0xB8 && opcode <= 0xBF) {
      len += 8;
    } else
#endif
    len += 4;
  }

  /* Set relocation offset for relative CALL/JMP */
  if (reloc != NULL && reloc_offset > 0) {
    *reloc = reloc_offset;
  }

  return len;
}

static void _sampgdk_hook_write_jmp(void *src, void *dst, int32_t offset) {
  struct _sampgdk_hook_jmp jmp;

#ifdef SAMPGDK_64BIT
  jmp.opcode = 0xFF;
  jmp.modrm  = 0x25;
  jmp.disp   = 0;
  jmp.target = (uintptr_t)dst;
#else
  jmp.opcode = 0xE9;
  jmp.offset = (int32_t)((uint8_t *)dst - ((uint8_t *)src + sizeof(jmp)));
#endif

  memcpy((uint8_t *)src + offset, &jmp, sizeof(jmp));
}

sampgdk_hook_t sampgdk_hook_new(void *src, void *dst) {
  struct _sampgdk_hook *hook;
  size_t orig_size = 0;
  size_t insn_len;

  if ((hook = (sampgdk_hook_t)malloc(sizeof(*hook))) == NULL) {
    return NULL;
  }

  _sampgdk_hook_unprotect(src, _SAMPGDK_HOOK_JMP_SIZE);
  _sampgdk_hook_unprotect(hook->trampoline, _SAMPGDK_HOOK_TRAMPOLINE_SIZE);

  /* We can't just jump to src + 5 as we could end up in the middle of
   * some instruction. So we need to determine the instruction length.
   */
  while (orig_size < _SAMPGDK_HOOK_JMP_SIZE) {
    uint8_t *insn = (uint8_t *)src + orig_size;
    int reloc = 0;

    if ((insn_len = _sampgdk_hook_disasm(insn, &reloc)) == 0) {
      sampgdk_log_error("Unsupported instr at offset %zu (byte: 0x%02X), func=%p, JMP_SIZE=%d",
                        orig_size, insn[0], src, _SAMPGDK_HOOK_JMP_SIZE);
      break;
    }

    memcpy(hook->trampoline + orig_size, insn, insn_len);

    /* If the original code contains a relative JMP/CALL relocate its
     * destination address.
     */
    if (reloc != 0) {
      int32_t *offset = (int32_t *)(hook->trampoline + orig_size + reloc);
      *offset -= (int32_t)((intptr_t)hook->trampoline - (intptr_t)src);
    }

    orig_size += insn_len;
  }

  if (insn_len == 0) {
    free(hook);
    return NULL;
  }

#ifdef SAMPGDK_64BIT
  /* Absolute JMP (FF 25 + 8B addr): dst is used directly as target. */
  _sampgdk_hook_write_jmp(hook->trampoline, (uint8_t *)src + orig_size, (int32_t)orig_size);
#else
  /* Relative JMP (E9 + rel32): formula is dst - (src_param + 5).
   * To jump to src + orig_size, pass dst = src (not src + orig_size). */
  _sampgdk_hook_write_jmp(hook->trampoline, src, (int32_t)orig_size);
#endif
  _sampgdk_hook_write_jmp(src, dst, 0);

  return hook;
}

void sampgdk_hook_free(sampgdk_hook_t hook) {
  free(hook);
}

void *sampgdk_hook_trampoline(sampgdk_hook_t hook) {
  return hook->trampoline;
}

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "sampgdk.h"

/* #include "logprintf.h" */

/*
 * Gets called instead of the real logprintf when the library has not been
 * initialized yet. See the declaration of sampgdk_logprintf_impl.
 */
static void _sampgdk_logprintf_stub(const char *format, ...) {
  va_list va;

  va_start(va, format);
  vprintf(format, va);
  va_end(va);

  printf("\n");
}

logprintf_t sampgdk_logprintf_impl = &_sampgdk_logprintf_stub;

void sampgdk_do_vlogprintf(const char *format, va_list va) {
  char buffer[SAMPGDK_LOGPRINTF_BUFFER_SIZE];

#ifdef _MSC_VER
  _vsnprintf(buffer, sizeof(buffer), format, va);
  /* _vsnprintf does not insert a terminating NUL if the buffer is too small */
  buffer[sizeof(buffer) - 1] = '\0';
#else
  vsnprintf(buffer, sizeof(buffer), format, va);
#endif

  sampgdk_logprintf_impl("%s", buffer);
}

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <string.h>
#endif

/* #include "array.h" */
/* #include "init.h" */
/* #include "plugin.h" */

static struct sampgdk_array _sampgdk_plugins;

SAMPGDK_MODULE_INIT(plugin) {
  return sampgdk_array_new(&_sampgdk_plugins, 1, sizeof(void *));
}

SAMPGDK_MODULE_CLEANUP(plugin) {
  sampgdk_array_free(&_sampgdk_plugins);
}

static int _sampgdk_plugin_compare_handle(const void *key,
                                          const void *elem) {
  const void *handle2 = *(const void **)elem;

  assert(key != NULL);
  assert(elem != NULL);

  if (key < handle2) return -1;
  if (key > handle2) return +1;

  return 0;
}

int sampgdk_plugin_register(void *plugin) {
  assert(plugin != NULL);
  if (sampgdk_array_find(&_sampgdk_plugins,
                         plugin,
                         _sampgdk_plugin_compare_handle) < 0) {
    return sampgdk_array_append(&_sampgdk_plugins, &plugin);
  }
  return -EINVAL;
}

int sampgdk_plugin_unregister(void *plugin) {
  assert(plugin != NULL);
  return sampgdk_array_find_remove(&_sampgdk_plugins,
                                   plugin,
                                   _sampgdk_plugin_compare_handle);
}

void **sampgdk_plugin_get_plugins(int *number) {
  assert(number != NULL);
  *number = _sampgdk_plugins.count;
  return (void **)_sampgdk_plugins.data;
}

#if SAMPGDK_WINDOWS

void *sampgdk_plugin_get_symbol(void *plugin, const char *name)  {
  assert(plugin != NULL);
  assert(name != NULL);
  return (void *)GetProcAddress((HMODULE)plugin, name);
}

void *sampgdk_plugin_get_handle(void *address) {
  MEMORY_BASIC_INFORMATION mbi;
  assert(address != NULL);
  if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
    return NULL;
  }
  return (void *)mbi.AllocationBase;
}

#else /* SAMPGDK_WINDOWS */

void *sampgdk_plugin_get_symbol(void *plugin, const char *name)  {
  assert(plugin != NULL);
  assert(name != NULL);
  return dlsym(plugin, name);
}

void *sampgdk_plugin_get_handle(void *address) {
  Dl_info info;
  assert(address != NULL);
  if (dladdr(address, &info) != 0) {
    return dlopen(info.dli_fname, RTLD_NOW);
  }
  return NULL;
}

#endif /* !SAMPGDK_WINDOWS */

#ifndef SAMPGDK_INTERNAL_AMX_H
#define SAMPGDK_INTERNAL_AMX_H

#include "sampgdk.h"

#define AMX_EXEC_GDK (-10000)

struct sampgdk_amx_api {
  uint16_t *(AMXAPI *Align16)(uint16_t *v);
  uint32_t *(AMXAPI *Align32)(uint32_t *v);
  uint64_t *(AMXAPI *Align64)(uint64_t *v);
  int (AMXAPI *Allot)(AMX *amx, int cells, cell *amx_addr, cell **phys_addr);
  int (AMXAPI *Callback)(AMX *amx, cell index, cell *result, cell *params);
  int (AMXAPI *Cleanup)(AMX *amx);
  int (AMXAPI *Clone)(AMX *amxClone, AMX *amxSource, void *data);
  int (AMXAPI *Exec)(AMX *amx, cell *retval, int index);
  int (AMXAPI *FindNative)(AMX *amx, const char *name, int *index);
  int (AMXAPI *FindPublic)(AMX *amx, const char *funcname, int *index);
  int (AMXAPI *FindPubVar)(AMX *amx, const char *varname, cell *amx_addr);
  int (AMXAPI *FindTagId)(AMX *amx, cell tag_id, char *tagname);
  int (AMXAPI *Flags)(AMX *amx,uint16_t *flags);
  int (AMXAPI *GetAddr)(AMX *amx,cell amx_addr,cell **phys_addr);
  int (AMXAPI *GetNative)(AMX *amx, int index, char *funcname);
  int (AMXAPI *GetPublic)(AMX *amx, int index, char *funcname);
  int (AMXAPI *GetPubVar)(AMX *amx, int index, char *varname, cell *amx_addr);
  int (AMXAPI *GetString)(char *dest,const cell *source, int use_wchar,
                          size_t size);
  int (AMXAPI *GetTag)(AMX *amx, int index, char *tagname, cell *tag_id);
  int (AMXAPI *GetUserData)(AMX *amx, long tag, void **ptr);
  int (AMXAPI *Init)(AMX *amx, void *program);
  int (AMXAPI *InitJIT)(AMX *amx, void *reloc_table, void *native_code);
  int (AMXAPI *MemInfo)(AMX *amx, long *codesize, long *datasize,
                        long *stackheap);
  int (AMXAPI *NameLength)(AMX *amx, int *length);
  AMX_NATIVE_INFO *(AMXAPI *NativeInfo)(const char *name, AMX_NATIVE func);
  int (AMXAPI *NumNatives)(AMX *amx, int *number);
  int (AMXAPI *NumPublics)(AMX *amx, int *number);
  int (AMXAPI *NumPubVars)(AMX *amx, int *number);
  int (AMXAPI *NumTags)(AMX *amx, int *number);
  int (AMXAPI *Push)(AMX *amx, cell value);
  int (AMXAPI *PushArray)(AMX *amx, cell *amx_addr, cell **phys_addr,
                          const cell array[], int numcells);
  int (AMXAPI *PushString)(AMX *amx, cell *amx_addr, cell **phys_addr,
                           const char *string, int pack, int use_wchar);
  int (AMXAPI *RaiseError)(AMX *amx, int error);
  int (AMXAPI *Register)(AMX *amx, const AMX_NATIVE_INFO *nativelist,
                         int number);
  int (AMXAPI *Release)(AMX *amx, cell amx_addr);
  int (AMXAPI *SetCallback)(AMX *amx, AMX_CALLBACK callback);
  int (AMXAPI *SetDebugHook)(AMX *amx, AMX_DEBUG debug);
  int (AMXAPI *SetString)(cell *dest, const char *source, int pack,
                          int use_wchar, size_t size);
  int (AMXAPI *SetUserData)(AMX *amx, long tag, void *ptr);
  int (AMXAPI *StrLen)(const cell *cstring, int *length);
  int (AMXAPI *UTF8Check)(const char *string, int *length);
  int (AMXAPI *UTF8Get)(const char *string, const char **endptr, cell *value);
  int (AMXAPI *UTF8Len)(const cell *cstr, int *length);
  int (AMXAPI *UTF8Put)(char *string, char **endptr, int maxchars, cell value);
};

extern struct sampgdk_amx_api *sampgdk_amx_api;

#define amx_Align16      sampgdk_amx_api->Align16
#define amx_Align32      sampgdk_amx_api->Align32
#define amx_Align64      sampgdk_amx_api->Align64
#define amx_Allot        sampgdk_amx_api->Allot
#define amx_Callback     sampgdk_amx_api->Callback
#define amx_Cleanup      sampgdk_amx_api->Cleanup
#define amx_Clone        sampgdk_amx_api->Clone
#define amx_Exec         sampgdk_amx_api->Exec
#define amx_FindNative   sampgdk_amx_api->FindNative
#define amx_FindPublic   sampgdk_amx_api->FindPublic
#define amx_FindPubVar   sampgdk_amx_api->FindPubVar
#define amx_FindTagId    sampgdk_amx_api->FindTagId
#define amx_Flags        sampgdk_amx_api->Flags
#define amx_GetAddr      sampgdk_amx_api->GetAddr
#define amx_GetNative    sampgdk_amx_api->GetNative
#define amx_GetPublic    sampgdk_amx_api->GetPublic
#define amx_GetPubVar    sampgdk_amx_api->GetPubVar
#define amx_GetString    sampgdk_amx_api->GetString
#define amx_GetTag       sampgdk_amx_api->GetTag
#define amx_GetUserData  sampgdk_amx_api->GetUserData
#define amx_Init         sampgdk_amx_api->Init
#define amx_InitJIT      sampgdk_amx_api->InitJIT
#define amx_MemInfo      sampgdk_amx_api->MemInfo
#define amx_NameLength   sampgdk_amx_api->NameLength
#define amx_NativeInfo   sampgdk_amx_api->NativeInfo
#define amx_NumNatives   sampgdk_amx_api->NumNatives
#define amx_NumPublics   sampgdk_amx_api->NumPublics
#define amx_NumPubVars   sampgdk_amx_api->NumPubVars
#define amx_NumTags      sampgdk_amx_api->NumTags
#define amx_Push         sampgdk_amx_api->Push
#define amx_PushArray    sampgdk_amx_api->PushArray
#define amx_PushString   sampgdk_amx_api->PushString
#define amx_RaiseError   sampgdk_amx_api->RaiseError
#define amx_Register     sampgdk_amx_api->Register
#define amx_Release      sampgdk_amx_api->Release
#define amx_SetCallback  sampgdk_amx_api->SetCallback
#define amx_SetDebugHook sampgdk_amx_api->SetDebugHook
#define amx_SetString    sampgdk_amx_api->SetString
#define amx_SetUserData  sampgdk_amx_api->SetUserData
#define amx_StrLen       sampgdk_amx_api->StrLen
#define amx_UTF8Check    sampgdk_amx_api->UTF8Check
#define amx_UTF8Get      sampgdk_amx_api->UTF8Get
#define amx_UTF8Len      sampgdk_amx_api->UTF8Len
#define amx_UTF8Put      sampgdk_amx_api->UTF8Put

#endif /* !SAMPGDK_INTERNAL_AMX_H */

#ifndef SAMPGDK_INTERNAL_CALLBACK_H
#define SAMPGDK_INTERNAL_CALLBACK_H

#include "sampgdk.h"

/* Callback handler function. */
typedef bool (*sampgdk_callback)(AMX *amx, void *func, cell *retval);

/* Register and unregister a callback in the global callback table.
 *
 * This is usually done only from generated init and cleanup functions of a_*
 * modules. Currently the only module that defines callbacks is a_samp.
 */
int sampgdk_callback_register(const char *name, sampgdk_callback handler);
void sampgdk_callback_unregister(const char *name);

/* Gets the name of the callback with the specified index,
 * similar to amx_GetPublic().
 */
bool sampgdk_callback_get(int index, char **name);

/* Executes the callback handler registered for the specified callback.
 *
 * The return value indicates whether the callback returned a "bad" value,
 * i.e. whether the gamemode is allowed to execute the associated public
 * function. This value is specified in the IDL files via the callback's
 * "badret" attribute.
 */
bool sampgdk_callback_invoke(AMX *amx, const char *name,
    int paramcount, cell *retval);

#endif /* !SAMPGDK_INTERNAL_CALLBACK_H */

#ifndef SAMPGDK_INTERNAL_FAKEAMX_H
#define SAMPGDK_INTERNAL_FAKEAMX_H

#include "sampgdk.h"

/* Returns the global fake AMX instance. */
AMX *sampgdk_fakeamx_amx(void);

/* Changes the size of the fake AMX heap. Used in the amx_Allot() hook. */
int sampgdk_fakeamx_resize_heap(int cells);

/* Push a value onto the fake AMX heap. */
int sampgdk_fakeamx_push(int cells, cell *address);
int sampgdk_fakeamx_push_cell(cell value, cell *address);
int sampgdk_fakeamx_push_float(float value, cell *address);
int sampgdk_fakeamx_push_array(const cell *src, int size, cell *address);
int sampgdk_fakeamx_push_string(const char *src, int *size, cell *address);
void sampgdk_fakeamx_pop(cell address);

/* Get stuff back from the heap. Usually used for output parameters. */
void sampgdk_fakeamx_get_cell(cell address, cell *value);
void sampgdk_fakeamx_get_bool(cell address, bool *value);
void sampgdk_fakeamx_get_float(cell address, float *value);
void sampgdk_fakeamx_get_array(cell address, cell *dest, int size);
void sampgdk_fakeamx_get_string(cell address, char *dest, int size);

#endif /* !SAMPGDK_INTERNAL_FAKEAMX_H */

#ifndef SAMPGDK_INTERNAL_NATIVE_H
#define SAMPGDK_INTERNAL_NATIVE_H

#include <stdarg.h>

#include "sampgdk.h"

/* Register a native function in the internal natives table. */
int sampgdk_native_register(const char *name, AMX_NATIVE func);

AMX_NATIVE sampgdk_native_find(const char *name);
AMX_NATIVE sampgdk_native_find_warn(const char *name);
AMX_NATIVE sampgdk_native_find_stub(const char *name);
AMX_NATIVE sampgdk_native_find_warn_stub(const char *name);
AMX_NATIVE sampgdk_native_find_flexible(const char *name, AMX_NATIVE current);

/* Returns all currently registered natives. */
const AMX_NATIVE_INFO *sampgdk_native_get_natives(int *number);

cell sampgdk_native_call(AMX_NATIVE native, cell *params);
cell sampgdk_native_invoke(AMX_NATIVE native, const char *format, va_list args);
cell sampgdk_native_invoke_array(
    AMX_NATIVE native, const char *format, void **args);

#endif /* !SAMPGDK_INTERNAL_NATIVE_H */

#ifndef SAMPGDK_INTERNAL_PARAM_H
#define SAMPGDK_INTERNAL_PARAM_H

#include "sampgdk.h"

void sampgdk_param_get_cell(AMX *amx, int index, cell *param);
void sampgdk_param_get_bool(AMX *amx, int index, bool *param);
void sampgdk_param_get_float(AMX *amx, int index, float *param);
void sampgdk_param_get_string(AMX *amx, int index, char **param);

/* Returns pointer to the start of the parameter list. */
cell *sampgdk_param_get_start(AMX *amx);

#endif /* !SAMPGDK_INTERNAL_PARAM_H */

/* #include "amx.h" */

struct sampgdk_amx_api *sampgdk_amx_api;

#ifndef SAMPGDK_INTERNAL_TIMER_H
#define SAMPGDK_INTERNAL_TIMER_H

#include "sampgdk.h"

/* Timer callback function.
 * There's also a public typedef TimerCallback in <sampgdk/types.h>.
 */
typedef void (SAMPGDK_CALL *sampgdk_timer_callback)(int id, void *param);

int sampgdk_timer_set(int interval, bool repeat,
    sampgdk_timer_callback calback, void *param);
int sampgdk_timer_kill(int timerid);

/* Processes timers created by the specified plugin. */
void sampgdk_timer_process_timers(void *plugin);

#endif /* !SAMPGDK_INTERNAL_TIMER_H */

#include <assert.h>
#include <limits.h>
#include <string.h>

/* #include "amx.h" */
/* #include "array.h" */
/* #include "fakeamx.h" */
/* #include "init.h" */

/* Space reserved for the stack. */
#define _SAMPGDK_FAKEAMX_STACK_SIZE 64

/* The initial size of the heap. */
#define _SAMPGDK_FAKEAMX_HEAP_SIZE 1024

static struct {
  AMX                  amx;
  AMX_HEADER           amxhdr;
  struct sampgdk_array heap;
} _sampgdk_fakeamx;

SAMPGDK_MODULE_INIT(fakeamx) {
  int error;

  memset(&_sampgdk_fakeamx, 0, sizeof(_sampgdk_fakeamx));

  error = sampgdk_array_new(&_sampgdk_fakeamx.heap,
                            _SAMPGDK_FAKEAMX_HEAP_SIZE +
                            _SAMPGDK_FAKEAMX_STACK_SIZE,
                            sizeof(cell));
  if (error < 0) {
    return error;
  }

  _sampgdk_fakeamx.amxhdr.magic = AMX_MAGIC;
  _sampgdk_fakeamx.amxhdr.file_version = MIN_FILE_VERSION;
  _sampgdk_fakeamx.amxhdr.amx_version = MIN_AMX_VERSION;
  /* dat is unused for fake AMX; amx->data is set directly below.
   * char* subtraction yields ptrdiff_t (64-bit on x64, 32-bit on x86).
   * Cast to int32_t truncates explicitly; safe because field is unused.
   */
  _sampgdk_fakeamx.amxhdr.dat = (int32_t)(
      (char *)_sampgdk_fakeamx.heap.data
    - (char *)&_sampgdk_fakeamx.amxhdr);
  _sampgdk_fakeamx.amxhdr.defsize = sizeof(AMX_FUNCSTUBNT);

  _sampgdk_fakeamx.amx.base = (unsigned char *)&_sampgdk_fakeamx.amxhdr;
  _sampgdk_fakeamx.amx.data = (unsigned char *)_sampgdk_fakeamx.heap.data;
  _sampgdk_fakeamx.amx.callback = amx_Callback;
  _sampgdk_fakeamx.amx.stp = _sampgdk_fakeamx.heap.size * sizeof(cell);
  _sampgdk_fakeamx.amx.stk = _sampgdk_fakeamx.amx.stp;
  _sampgdk_fakeamx.amx.flags = AMX_FLAG_NTVREG | AMX_FLAG_RELOC;

  sampgdk_array_pad(&_sampgdk_fakeamx.heap);

  return 0;
}

SAMPGDK_MODULE_CLEANUP(fakeamx) {
  sampgdk_array_free(&_sampgdk_fakeamx.heap);
}

AMX *sampgdk_fakeamx_amx(void) {
  return &_sampgdk_fakeamx.amx;
}

int sampgdk_fakeamx_resize_heap(int cells) {
  int error;
  cell old_size;
  cell new_size;
  cell old_stk;
  cell new_stk;
  cell new_stp;

  assert(cells > 0);

  old_size = _sampgdk_fakeamx.heap.size;
  new_size = cells;

  error = sampgdk_array_resize(&_sampgdk_fakeamx.heap, new_size);
  sampgdk_array_pad(&_sampgdk_fakeamx.heap);
  if (error < 0) {
    return error;
  }

  /* Update data pointers to point at the newly allocated heap.
   * dat is unused; amx->data is used instead.
   */
  _sampgdk_fakeamx.amxhdr.dat = (int32_t)(
      (char *)_sampgdk_fakeamx.heap.data
    - (char *)&_sampgdk_fakeamx.amxhdr);
  _sampgdk_fakeamx.amx.data = (unsigned char *)_sampgdk_fakeamx.heap.data;

  old_stk = _sampgdk_fakeamx.amx.stk;
  new_stk = _sampgdk_fakeamx.amx.stk + (new_size - old_size) * sizeof(cell);
  new_stp = _sampgdk_fakeamx.amx.stp + (new_size - old_size) * sizeof(cell);

  /* Shift stack contents. */
  memmove((unsigned char *)_sampgdk_fakeamx.heap.data
                           + new_stk - _SAMPGDK_FAKEAMX_STACK_SIZE,
          (unsigned char *)_sampgdk_fakeamx.heap.data
                           + old_stk - _SAMPGDK_FAKEAMX_STACK_SIZE,
          _SAMPGDK_FAKEAMX_STACK_SIZE);

  _sampgdk_fakeamx.amx.stk = new_stk;
  _sampgdk_fakeamx.amx.stp = new_stp;

  return 0;
}

int sampgdk_fakeamx_push(int cells, cell *address) {
  cell old_hea, new_hea;
  cell old_heap_size, new_heap_size;

  assert(cells > 0);

  old_hea = _sampgdk_fakeamx.amx.hea;
  new_hea = _sampgdk_fakeamx.amx.hea + cells * sizeof(cell);

  old_heap_size = _sampgdk_fakeamx.heap.size;
  new_heap_size = (new_hea + _SAMPGDK_FAKEAMX_STACK_SIZE) / sizeof(cell);

  if (new_hea >= (cell)(old_heap_size * sizeof(cell))) {
    int error;

    error = sampgdk_fakeamx_resize_heap(new_heap_size);
    if (error < 0) {
      return error;
    }
  }

  _sampgdk_fakeamx.amx.hea = new_hea;

  if (address != NULL) {
    *address = old_hea;
  }

  return 0;
}

int sampgdk_fakeamx_push_cell(cell value, cell *address) {
  int error;

  assert(address != NULL);

  if ((error = sampgdk_fakeamx_push(1, address)) < 0) {
    return error;
  }

  ((cell *)(_sampgdk_fakeamx.heap.data))[*address / sizeof(cell)] = value;

  return 0;
}

int sampgdk_fakeamx_push_float(float value, cell *address) {
  return sampgdk_fakeamx_push_cell(amx_ftoc(value), address);
}

int sampgdk_fakeamx_push_array(const cell *src, int size, cell *address) {
  int error;
  cell *dest;

  assert(address != NULL);
  assert(src != NULL);
  assert(size > 0);

  if ((error = sampgdk_fakeamx_push(size, address)) < 0) {
    return error;
  }

  dest = (cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap, *address / sizeof(cell));
  memcpy(dest, src, size * sizeof(cell));

  return 0;
}

int sampgdk_fakeamx_push_string(const char *src, int *size, cell *address) {
  int src_size;
  int error;

  assert(address != NULL);
  assert(src != NULL);

  src_size = (int)strlen(src) + 1;
  if ((error = sampgdk_fakeamx_push(src_size, address)) < 0) {
    return error;
  }

  amx_SetString((cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap,
                                 *address / sizeof(cell)),
                src, 0, 0, src_size);

  if (size != NULL) {
    *size = src_size;
  }

  return 0;
}

void sampgdk_fakeamx_get_cell(cell address, cell *value) {
  assert(address % sizeof(cell) == 0);
  assert(value != NULL);

  *value = *(cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap,
                                      address / sizeof(cell));
}

void sampgdk_fakeamx_get_bool(cell address, bool *value) {
  cell tmp;

  assert(address % sizeof(cell) == 0);
  assert(value != NULL);

  sampgdk_fakeamx_get_cell(address, &tmp);
  *value = !!tmp;
}

void sampgdk_fakeamx_get_float(cell address, float *value) {
  cell tmp;

  assert(address % sizeof(cell) == 0);
  assert(value != NULL);

  sampgdk_fakeamx_get_cell(address, &tmp);
  *value = amx_ctof(tmp);
}

void sampgdk_fakeamx_get_array(cell address, cell *dest, int size) {
  cell *src;

  assert(address % sizeof(cell) == 0);
  assert(dest != NULL);
  assert(size > 0);

  src = (cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap, address / sizeof(cell));
  memcpy(dest, src, size * sizeof(cell));
}

void sampgdk_fakeamx_get_string(cell address, char *dest, int size) {
  assert(address % sizeof(cell) == 0);
  assert(dest != NULL);

  amx_GetString(dest, (cell *)sampgdk_array_get(&_sampgdk_fakeamx.heap,
                                                address / sizeof(cell)),
                                                0, size);
}

void sampgdk_fakeamx_pop(cell address) {
  assert(address % sizeof(cell) == 0);

  if (_sampgdk_fakeamx.amx.hea > address) {
    _sampgdk_fakeamx.amx.hea = address;
  }
}

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* #include "array.h" */
/* #include "fakeamx.h" */
/* #include "init.h" */
/* #include "likely.h" */
/* #include "native.h" */
/* #include "log.h" */

#define _SAMPGDK_NATIVE_MAX_ARGS     32
#define _SAMPGDK_NATIVE_MAX_ARGS_SIZE 8  /* in bytes */

static struct sampgdk_array _sampgdk_natives;

SAMPGDK_MODULE_INIT(native) {
  int error;
  AMX_NATIVE_INFO null = {NULL, NULL};

  error = sampgdk_array_new(&_sampgdk_natives,
                            128,
                            sizeof(AMX_NATIVE_INFO));
  if (error < 0) {
    return error;
  }

  return sampgdk_array_append(&_sampgdk_natives, &null);
}

SAMPGDK_MODULE_CLEANUP(native) {
  sampgdk_array_free(&_sampgdk_natives);
}

int sampgdk_native_register(const char *name, AMX_NATIVE func) {
  AMX_NATIVE old_func;
  AMX_NATIVE_INFO info;
  AMX_NATIVE_INFO *ptr;
  int i;

  info.name = name;
  info.func = func;

  assert(name != 0);

  old_func = sampgdk_native_find(name);
  if (old_func != NULL) {
    return -1;
  }

  /* Keep natives ordered by name.
   * This allows us to use binary search in sampgdk_native_find().
   */
  for (i = 0; i < _sampgdk_natives.count - 1; i++) {
    ptr = (AMX_NATIVE_INFO *)sampgdk_array_get(&_sampgdk_natives, i);
    if (strcmp(name, ptr->name) <= 0) {
      break;
    }
  }

  return sampgdk_array_insert(&_sampgdk_natives, i, 1, &info);
}

static int _sampgdk_native_compare_bsearch(const void *key,
                                           const void *elem) {
  assert(key != NULL);
  assert(elem != NULL);
  return strcmp((const char *)key, ((const AMX_NATIVE_INFO *)elem)->name);
}

AMX_NATIVE sampgdk_native_find(const char *name) {
  AMX_NATIVE_INFO *info;

  assert(name != NULL);

  if (_sampgdk_natives.data == NULL) {
    /* Perhaps they forgot to initialize? */
    return NULL;
  }

  info = (AMX_NATIVE_INFO *)bsearch(name,
                                    _sampgdk_natives.data,
                                    _sampgdk_natives.count - 1,
                                    _sampgdk_natives.elem_size,
                                    _sampgdk_native_compare_bsearch);
  if (info == NULL) {
    return NULL;
  }

  return info->func;
}

AMX_NATIVE sampgdk_native_find_warn(const char *name) {
  AMX_NATIVE func;

  assert(name != NULL);

  func = sampgdk_native_find(name);
  if (func == NULL) {
    sampgdk_log_warning("Native function not found: %s", name);
  }

  return func;
}

static cell AMX_NATIVE_CALL native_stub(AMX *amx, cell *params) {
  return 0;
}

AMX_NATIVE sampgdk_native_find_stub(const char *name) {
  AMX_NATIVE func;

  assert(name != NULL);

  if ((func = sampgdk_native_find(name)) == NULL) {
    return native_stub;
  }

  return func;
}

AMX_NATIVE sampgdk_native_find_warn_stub(const char *name) {
  AMX_NATIVE func;

  assert(name != NULL);

  if ((func = sampgdk_native_find_warn(name)) == NULL) {
    return native_stub;
  }

  return func;
}

AMX_NATIVE sampgdk_native_find_flexible(const char *name, AMX_NATIVE current) {
  char *always_search;

  if (SAMPGDK_LIKELY(current != NULL && current != native_stub)) {
    return current;
  }

  if (current == NULL) {
    /* This is the first time this native is searched for, do it as usual.
     */
    return sampgdk_native_find_warn_stub(name);
  }

  /* current == native_stub */
  if ((always_search = getenv("SAMGDK_NATIVE_SEARCH_ALWAYS")) != NULL
      && atoi(always_search) != 0) {
    /* Previous attempt to find the native failed, but the always search
     * option is set so search again.
     */
    return sampgdk_native_find_warn_stub(name);
  }

  return current;
}

const AMX_NATIVE_INFO *sampgdk_native_get_natives(int *number) {
  if (number != NULL) {
    *number = _sampgdk_natives.count - 1;
  }
  return (AMX_NATIVE_INFO *)_sampgdk_natives.data;
}

cell sampgdk_native_call(AMX_NATIVE native, cell *params) {
  AMX *amx = sampgdk_fakeamx_amx();
  assert(native != NULL);
  return native(amx, params);
}

cell sampgdk_native_invoke(AMX_NATIVE native,
                           const char *format,
                           va_list args) {
  cell i = 0;
  const char *format_ptr = format;
  unsigned char args_copy[_SAMPGDK_NATIVE_MAX_ARGS *
                          _SAMPGDK_NATIVE_MAX_ARGS_SIZE];
  unsigned char *args_ptr = args_copy;
  void *args_array[_SAMPGDK_NATIVE_MAX_ARGS];

  assert(format_ptr != NULL);

  while (*format_ptr != '\0' && i < _SAMPGDK_NATIVE_MAX_ARGS) {
    switch (*format_ptr) {
      case 'i': /* integer */
      case 'd': /* integer */
        *(int *)args_ptr = va_arg(args, int);
        args_array[i++] = args_ptr;
        args_ptr += _SAMPGDK_NATIVE_MAX_ARGS_SIZE;
        break;
      case 'b': /* boolean */
        *(bool *)args_ptr = !!va_arg(args, int);
        args_array[i++] = args_ptr;
        args_ptr += _SAMPGDK_NATIVE_MAX_ARGS_SIZE;
        break;
      case 'f': /* floating-point */
        *(float *)args_ptr = (float)va_arg(args, double);
        args_array[i++] = args_ptr;
        args_ptr += _SAMPGDK_NATIVE_MAX_ARGS_SIZE;
        break;
      case 'r': /* const reference */
      case 'R': /* non-const reference */
      case 's': /* const string */
      case 'S': /* non-const string */
      case 'a': /* const array */
      case 'A': /* non-const array */
        args_array[i++] = va_arg(args, void *);
        break;
    }
    format_ptr++;
  }

  return sampgdk_native_invoke_array(native, format, args_array);
}

cell sampgdk_native_invoke_array(AMX_NATIVE native, const char *format,
                                 void **args) {
  AMX *amx = sampgdk_fakeamx_amx();
  const char *format_ptr = format;
  cell i = 0;
  cell params[_SAMPGDK_NATIVE_MAX_ARGS + 1];
  cell size[_SAMPGDK_NATIVE_MAX_ARGS] = {0};
  char type[_SAMPGDK_NATIVE_MAX_ARGS];
  int needs_size = -1;
  enum {
    ST_READ_SPEC,
    ST_NEED_SIZE,
    ST_READING_SIZE,
    ST_READING_SIZE_ARG,
    ST_READ_SIZE
  } state = ST_READ_SPEC;
  cell retval;

  assert(format_ptr != NULL);
  assert(args != NULL);

  while (*format_ptr != '\0' && i < _SAMPGDK_NATIVE_MAX_ARGS) {
    switch (state) {
      case ST_READ_SPEC:
        switch (*format_ptr) {
          case 'i': /* integer */
          case 'd': /* integer */
            params[i + 1] = *(int *)args[i];
            break;
          case 'b': /* boolean */
            params[i + 1] = *(bool *)args[i];
            break;
          case 'f': /* floating-point */ {
            float value = *(float *)args[i];
            params[i + 1] = amx_ftoc(value);
            break;
          }
          case 'r': /* const reference */
          case 'R': /* non-const reference */ {
            cell *ptr = (cell *)args[i];
            sampgdk_fakeamx_push_cell(*ptr, &params[i + 1]);
            size[i] = sizeof(cell);
            break;
          }
          case 's': /* const string */ {
            char *str = (char *)args[i];
            int str_size;
            sampgdk_fakeamx_push_string(str, &str_size, &params[i + 1]);
            size[i] = str_size;
            break;
          }
          case 'S': /* non-const string */
          case 'a': /* const array */
          case 'A': /* non-const array */
            needs_size = i;
            state = ST_NEED_SIZE;
            break;
          default:
            sampgdk_log_warning("Unrecognized type specifier '%c'",
                                *format_ptr);
        }
        type[i++] = *format_ptr++;
        break;
      case ST_NEED_SIZE:
        if (*format_ptr == '[') {
          state = ST_READING_SIZE;
        } else {
          sampgdk_log_warning("Bad format string: expected '[' but got '%c'",
                              *format_ptr);
        }
        format_ptr++;
        break;
      case ST_READING_SIZE:
        if (*format_ptr == '*') {
          format_ptr++;
          state = ST_READING_SIZE_ARG;
        } else {
          size[needs_size] = (int)strtol(format_ptr, (char **)&format_ptr, 10);
          state = ST_READ_SIZE;
        }
        break;
      case ST_READING_SIZE_ARG: {
        int index = (int)strtol(format_ptr, (char **)&format_ptr, 10);
        size[needs_size] = *(int *)args[index];
        state = ST_READ_SIZE;
        break;
      }
      case ST_READ_SIZE: {
        if (*format_ptr == ']') {
          switch (type[needs_size]) {
            case 'a':
            case 'A':
            case 'S':
              if (size[needs_size] > 0) {
                sampgdk_fakeamx_push_array((const cell *)args[needs_size],
                                           size[needs_size],
                                           &params[needs_size + 1]);
              } else {
                sampgdk_log_warning("Invalid buffer size");
              }
              break;
          }
          needs_size = -1;
          state = ST_READ_SPEC;
        } else {
          sampgdk_log_warning("Bad format string (expected ']' but got '%c')",
                              *format_ptr);
        }
        format_ptr++;
        break;
      }
    }
  }

  if (*format_ptr != '\0') {
    sampgdk_log_warning("Too many native arguments (at most %d allowed)",
                        _SAMPGDK_NATIVE_MAX_ARGS);
  }

  params[0] = i * sizeof(cell);
  assert(native != NULL);
  retval = native(amx, params);

  while (--i >= 0) {
    if (size[i] > 0) {
      /* If this is an output parameter we have to write the updated value
       * back to the argument.
       */
      switch (type[i]) {
        case 'R':
          sampgdk_fakeamx_get_cell(params[i + 1], (cell *)args[i]);
          break;
        case 'S':
          sampgdk_fakeamx_get_string(params[i + 1], (char *)args[i], size[i]);
          break;
        case 'A':
          sampgdk_fakeamx_get_array(params[i + 1], (cell *)args[i], size[i]);
          break;
      }
      sampgdk_fakeamx_pop(params[i + 1]);
    }
  }

  return retval;
}

#include "sampgdk.h"

/* #include "internal/native.h" */

SAMPGDK_API(const AMX_NATIVE_INFO *, sampgdk_GetNatives(int *number)) {
  return sampgdk_native_get_natives(number);
}

SAMPGDK_API(AMX_NATIVE, sampgdk_FindNative(const char *name)) {
  if (name != NULL) {
    return sampgdk_native_find(name);
  }
  return NULL;
}

SAMPGDK_API(cell, sampgdk_CallNative(AMX_NATIVE native, cell *params)) {
  return sampgdk_native_call(native, params);
}

SAMPGDK_API(cell, sampgdk_InvokeNative(AMX_NATIVE native,
                                       const char *format, ...)) {
  cell retval;
  va_list args;

  va_start(args, format);
  retval = sampgdk_native_invoke(native, format, args);
  va_end(args);

  return retval;
}

SAMPGDK_API(cell, sampgdk_InvokeNativeV(AMX_NATIVE native,
                                        const char *format, va_list args)) {
  return sampgdk_native_invoke(native, format, args);
}

SAMPGDK_API(cell, sampgdk_InvokeNativeArray(AMX_NATIVE native,
                                            const char *format, void **args)) {
  return sampgdk_native_invoke_array(native, format, args);
}

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* #include "array.h" */
/* #include "callback.h" */
/* #include "init.h" */
/* #include "log.h" */
/* #include "param.h" */
/* #include "plugin.h" */

#define _SAMPGDK_CALLBACK_MAX_ARGS 32

typedef bool (PLUGIN_CALL *_sampgdk_callback_filter)(
    AMX *amx,
    const char *name,
    cell *params,
    cell *retval);
typedef bool (PLUGIN_CALL *_sampgdk_callback_filter2)(
    AMX *amx,
    const char *name,
    cell *params,
    cell *retval,
    bool *stop);

struct _sampgdk_callback_info {
  char *name;
  char *func_name;
  void *handler;
};

static struct sampgdk_array _sampgdk_callbacks;

static int _sampgdk_callback_compare_name(const void *key,
                                          const void *elem) {
  assert(key != NULL);
  assert(elem != NULL);
  return strcmp((const char *)key,
                ((const struct _sampgdk_callback_info *)elem)->name);
}

static struct _sampgdk_callback_info *_sampgdk_callback_find(const char *name) {
  assert(name != NULL);

  if (_sampgdk_callbacks.count <= 0) {
    return NULL;
  }

  return (struct _sampgdk_callback_info *)bsearch(name,
                 _sampgdk_callbacks.data,
                 _sampgdk_callbacks.count,
                 _sampgdk_callbacks.elem_size,
                 _sampgdk_callback_compare_name);
}

SAMPGDK_MODULE_INIT(callback) {
  int error;

  error = sampgdk_array_new(&_sampgdk_callbacks,
                            32,
                            sizeof(struct _sampgdk_callback_info));
  if (error < 0) {
    return error;
  }

  error = sampgdk_callback_register(":OnPublicCall", NULL);
  if (error < 0) {
    return error;
  }

  error = sampgdk_callback_register(":OnPublicCall2", NULL);
  if (error < 0) {
    return error;
  }

  return 0;
}

SAMPGDK_MODULE_CLEANUP(callback) {
  int i;
  struct _sampgdk_callback_info *callback;

  for (i = 0; i < _sampgdk_callbacks.count; i++) {
    callback = (struct _sampgdk_callback_info *)sampgdk_array_get(&_sampgdk_callbacks, i);
    free(callback->name);
  }

  sampgdk_array_free(&_sampgdk_callbacks);
}

int sampgdk_callback_register(const char *name,
                              sampgdk_callback handler) {
  int error;
  int count;
  int i;
  struct _sampgdk_callback_info callback;
  struct _sampgdk_callback_info *ptr;

  assert(name != NULL);

  ptr = _sampgdk_callback_find(name);
  if (ptr != NULL) {
    return sampgdk_array_get_index(&_sampgdk_callbacks, ptr);
  }

  callback.handler = (void *)handler;

  callback.name = (char *)malloc(strlen(name) + 1);
  if (callback.name == NULL) {
    return -ENOMEM;
  }

  strcpy(callback.name, name);

  if (callback.name[0] == ':') {
    /* Special callbacks have a name that begins with ':'. This is to ensure
     * that their name is not used by SA-MP or user scripts.
     */
    callback.func_name = callback.name + 1;
  } else {
    callback.func_name = callback.name;
  }

  /* Keep callbacks ordered by name.
   * This allows us to use binary search when searching through callbacks.
   */
  count = _sampgdk_callbacks.count;
  for (i = 0; i < count; i++) {
    ptr = (struct _sampgdk_callback_info *)sampgdk_array_get(
        &_sampgdk_callbacks, i);
    if (strcmp(name, ptr->name) <= 0) {
      break;
    }
  }

  error = sampgdk_array_insert(&_sampgdk_callbacks, i, 1, &callback);
  if (error < 0) {
    free(callback.name);
    return error;
  }

  return error; /* index */
}

void sampgdk_callback_unregister(const char *name) {
  struct _sampgdk_callback_info *callback;

  if ((callback = _sampgdk_callback_find(name)) != NULL) {
    callback->handler = NULL;
  }
}

bool sampgdk_callback_get(int index, char **name) {
  struct _sampgdk_callback_info *callback;

  assert(name != NULL);

  if (index < 0 || index >= _sampgdk_callbacks.count) {
    return false;
  }

  callback = (struct _sampgdk_callback_info *)sampgdk_array_get(
      &_sampgdk_callbacks, index);
  *name = callback->name;

  return true;
}

bool sampgdk_callback_invoke(AMX *amx,
                             const char *name,
                             int paramcount,
                             cell *retval)
{
  cell params[_SAMPGDK_CALLBACK_MAX_ARGS + 1];
  void **plugins;
  int num_plugins;
  int i;

  assert(amx != NULL);

  if (paramcount > _SAMPGDK_CALLBACK_MAX_ARGS) {
    sampgdk_log_error("Too many callback arguments (at most %d allowed)",
                      _SAMPGDK_CALLBACK_MAX_ARGS);
    return true;
  }

  params[0] = paramcount * sizeof(cell);
  memcpy(&params[1], sampgdk_param_get_start(amx), params[0]);

  plugins = sampgdk_plugin_get_plugins(&num_plugins);

  for (i = 0; i < num_plugins; i++) {
    void *plugin = plugins[i];
    struct _sampgdk_callback_info *callback;
    struct _sampgdk_callback_info *callback_filter;
    struct _sampgdk_callback_info *callback_filter2;
    void *func;
    bool do_call = true;
    bool stop = false;

    callback_filter = _sampgdk_callback_find(":OnPublicCall");
    assert(callback_filter != NULL);
    
    func = sampgdk_plugin_get_symbol(plugin, callback_filter->func_name);
    if (func != NULL) {
      do_call = ((_sampgdk_callback_filter)func)(amx, name, params, retval);
    }

    /* callback_filter2 is similar to callback_filter except it can stop
     * propagation of public call to other plugins. It was added for backwards
     * compatibility.
     *
     * callback_filter2's return value overrides that of callback_filter.
     */
    callback_filter2 = _sampgdk_callback_find(":OnPublicCall2");
    assert(callback_filter2 != NULL);
    
    func = sampgdk_plugin_get_symbol(plugin, callback_filter2->func_name);
    if (func != NULL) {
      do_call = !((_sampgdk_callback_filter2)func)(amx,
                                                   name,
                                                   params,
                                                   retval,
                                                   &stop);
    }

    if (stop) {
      return false;
    }

    if (!do_call) {
      continue;
    }

    callback = _sampgdk_callback_find(name);
    if (callback == NULL || callback->handler == NULL) {
      continue;
    }
    
    func = sampgdk_plugin_get_symbol(plugin, callback->func_name);
    if (func != NULL
        && !((sampgdk_callback)callback->handler)(amx, func, retval)) {
      return false;
    }
  }

  return true;
}

#include <assert.h>
#include <stdlib.h>

/* #include "amx.h" */
/* #include "param.h" */

void sampgdk_param_get_cell(AMX *amx, int index, cell *param) {
  assert(param != NULL);
  *param = sampgdk_param_get_start(amx)[index];
}

void sampgdk_param_get_bool(AMX *amx, int index, bool *param) {
  assert(param != NULL);
  *param = !!sampgdk_param_get_start(amx)[index];
}

void sampgdk_param_get_float(AMX *amx, int index, float *param) {
  cell p = sampgdk_param_get_start(amx)[index];
  assert(param != NULL);
  *param = amx_ctof(p);
}

void sampgdk_param_get_string(AMX *amx, int index, char **param) {
  cell amx_addr;
  cell *phys_addr;
  int length;
  char *string;

  amx_addr = sampgdk_param_get_start(amx)[index];
  if (amx_GetAddr(amx, amx_addr, &phys_addr) != AMX_ERR_NONE) {
    return;
  }

  amx_StrLen(phys_addr, &length);
  string = (char *)malloc((length + 1) * sizeof(char));

  if (amx_GetString(string, phys_addr, 0, length + 1) != AMX_ERR_NONE) {
    free(string);
    return;
  }

  assert(param != NULL);
  *param = string;
}

cell *sampgdk_param_get_start(AMX *amx) {
  unsigned char *data =  amx->data != NULL
    ? amx->data
    : amx->base + ((AMX_HEADER *)amx->base)->dat;
  return (cell *)(data + amx->stk);
}

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sampgdk.h"

/* #include "amx.h" */
/* #include "array.h" */
/* #include "callback.h" */
/* #include "fakeamx.h" */
/* #include "init.h" */
/* #include "log.h" */
/* #include "native.h" */
/* #include "param.h" */
/* #include "hook.h" */

static AMX *_sampgdk_amxhooks_main_amx;

#define _SAMPGDK_AMXHOOKS_FUNC_LIST(C) \
  C(Register) \
  C(FindPublic) \
  C(Exec) \
  C(Allot)

#define _SAMPGDK_AMXHOOKS_DEFINE_HOOK(name) \
  static sampgdk_hook_t _sampgdk_amxhooks_##name##_hook;
_SAMPGDK_AMXHOOKS_FUNC_LIST(_SAMPGDK_AMXHOOKS_DEFINE_HOOK)
#undef _SAMPGDK_AMXHOOKS_DEFINE_HOOK

/* The "funcidx" native uses amx_FindPublic() to get the public function's
 * index but our FindPublic always returns success regardless of the actual
 * result. So here's a fixed version.
 *
 * Thanks to Incognito for finding this bug!
 */
static cell AMX_NATIVE_CALL _sampgdk_amxhooks_funcidx(AMX *amx, cell *params) {
  char *funcname;
  int index;
  int error;

  amx_StrParam(amx, params[1], funcname);
  if (funcname == NULL) {
    return -1;
  }

  error = amx_FindPublic(amx, funcname, &index);
  if (error != AMX_ERR_NONE || index <= AMX_EXEC_GDK) {
    return -1;
  }

  return index;
}

static int AMXAPI _sampgdk_amxhooks_Register(AMX *amx,
                                             const AMX_NATIVE_INFO *nativelist,
                                             int number) {
  int i;
  int count = 0;

  sampgdk_log_debug("amx_Register(%p, %p, %d)", amx, nativelist, number);

#ifndef SAMPGDK_64BIT
  /* On 32-bit, override funcidx to work around our FindPublic hook
   * which always returns success regardless of the actual result.
   *
   * On 64-bit (open.mp), funcidx is handled by the server itself,
   * and (ucell) would truncate the pointer, so skip it.
   */
  {
    AMX_HEADER *hdr = (AMX_HEADER *)amx->base;
    AMX_FUNCSTUBNT *natives =
        (AMX_FUNCSTUBNT *)(amx->base + hdr->natives);
    if (amx_FindNative(amx, "funcidx", &i) == AMX_ERR_NONE) {
      natives[i].address = (ucell)_sampgdk_amxhooks_funcidx;
    }
  }
#endif

  for (i = 0; (i < number || number == -1) && nativelist[i].name != NULL; i++) {
    if (sampgdk_native_register(nativelist[i].name, nativelist[i].func) >= 0) {
      sampgdk_log_debug("Registered native: %s @ %p",
                        nativelist[i].name, nativelist[i].func);
      count++;
    }
  }

  if (count > 0) {
    sampgdk_log_info("Registered %d natives", count);
  }

  return SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Register_hook, int, AMXAPI,
                              (AMX *, const AMX_NATIVE_INFO *, int),
                              (amx, nativelist, number));
}

static int AMXAPI _sampgdk_amxhooks_FindPublic(AMX *amx,
                                               const char *name,
                                               int *index) {
  int error;
  int index_internal;
  int index_real;

  sampgdk_log_debug("amx_FindPublic(%p, \"%s\", %p)", amx, name, index);

  error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_FindPublic_hook, int, AMXAPI,
                               (AMX *, const char *, int *), (amx, name, index));
  sampgdk_log_debug("amx_FindPublic returned %d", error);

  /* We are interested in intercepting public calls against the following
   * AMX instances:
   *
   * - the main AMX (the gamemode)
   * - the fake AMX (this is needed for HTTP() to work)
   */
  if (amx != _sampgdk_amxhooks_main_amx &&
      amx != sampgdk_fakeamx_amx()) {
    return error;
  }

  /* If the public was really found (and I mean REALLY) there's no need
   * to mess with the index.
   */
  if (error == AMX_ERR_NONE && *index >= 0) {
    return AMX_ERR_NONE;
  }

  /* OK, this public officially doesn't exist. Register it in our internal
   * callback table and return success. The table will allow us to keep track
   * of forged publics in amx_Exec().
   */
  index_internal = sampgdk_callback_register(name, NULL);
  index_real = AMX_EXEC_GDK - index_internal;

  if (index_internal < 0) {
    sampgdk_log_error("Error registering callback: %s",
                      strerror(-index_internal));
  } else if (error == AMX_ERR_NONE && *index < 0) {
    /* If there are other plugins running they better return the same
     * index as we do. Otherwise it would be a total mess and we can't
     * let that happen.
     */
    if (*index != index_real) {
      error = AMX_ERR_NOTFOUND;
      sampgdk_log_warning("Index mismatch for %s (%d != %d)",
                          name, *index, index_real);
    }
  } else if (error != AMX_ERR_NONE) {
    error = AMX_ERR_NONE, *index = index_real;
    sampgdk_log_debug("Registered callback: %s, index = %d", name, *index);
  }

  return error;
}

static int AMXAPI _sampgdk_amxhooks_Exec(AMX *amx, cell *retval, int index) {
  int paramcount;
  int error = AMX_ERR_NONE;
  bool do_exec = true;
  bool do_cleanup = false;

  sampgdk_log_debug("amx_Exec(%p, %p, %d), paramcount = %d, stk = %d",
      amx, retval, index, amx->paramcount, amx->stk);

  /* We have to reset amx->paramcount at this point so if the callback
   * itself calls amx_Exec() it won't pop our arguments off the stack.
   */
  paramcount = amx->paramcount;
  amx->paramcount = 0;

  /* Since filterscripts don't use main() we can assume that the AMX
   * that executes main() is indeed the main AMX i.e. the gamemode.
   */
  if (index == AMX_EXEC_MAIN) {
    /* This extra check is needed in order to stop OnGameModeInit()
     * from being called twice in a row after a gmx.
     */
    if (amx != NULL && _sampgdk_amxhooks_main_amx != amx) {
      _sampgdk_amxhooks_main_amx = amx;

      sampgdk_log_info("Found main AMX, callbacks should work now");
      sampgdk_log_debug("Main AMX instance: %p", amx);

      /* For some odd reason OnGameModeInit() is called before main().
       * Normally callbacks are handled below but this creates an exception.
       */
      sampgdk_callback_invoke(amx, "OnGameModeInit", paramcount, retval);
    }
  } else if (index != AMX_EXEC_CONT && (amx == _sampgdk_amxhooks_main_amx ||
                                        amx == sampgdk_fakeamx_amx())) {
    char *name = NULL;

    if (index <= AMX_EXEC_GDK) {
      sampgdk_callback_get(AMX_EXEC_GDK - index, &name);
    } else {
      AMX *main_amx = _sampgdk_amxhooks_main_amx;
      AMX_FUNCSTUBNT *publics = (AMX_FUNCSTUBNT *)(main_amx->base +
          ((AMX_HEADER *)main_amx->base)->publics);
      name = (char *)(publics[index].nameofs + amx->base);
    }

    if (name != NULL) {
      do_exec = sampgdk_callback_invoke(amx, name, paramcount, retval);
    } else {
      sampgdk_log_warning("Unknown callback, index = %d", index);
    }
  }

  if (do_exec) {
    amx->paramcount = paramcount;
    error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Exec_hook, int, AMXAPI,
                                 (AMX *, cell *, int), (amx, retval, index));
    sampgdk_log_debug("amx_Exec returned %d", error);
  }

  /* Suppress the error and also let the other plugin(s) know that we
   * handle the cleanup (see below).
   */
  if (error == AMX_ERR_INDEX && index <= AMX_EXEC_GDK) {
    error = AMX_ERR_NONE;
    do_cleanup = true;
  }

  /* Someone has to clean things up if amx_Exec() didn't run after all.
   */
  if (!do_exec || do_cleanup) {
    amx->paramcount = 0;
    amx->stk += paramcount * sizeof(cell);
    sampgdk_log_debug("Popped %d parameter(s), stk = %d", paramcount, amx->stk);
  }

  return error;
}

static int AMXAPI _sampgdk_amxhooks_Allot(AMX *amx,
                                          int cells,
                                          cell *amx_addr,
                                          cell **phys_addr) {
  int error;

  sampgdk_log_debug("amx_Allot(%p, %d, %p, %p)", amx, cells, amx_addr,
                                                 phys_addr);

  /* There is a bug in amx_Allot() where it returns success even though
   * there's not enough space on the heap:
   *
   * if (amx->stk - amx->hea - cells*sizeof(cell) < STKMARGIN)
   *   return AMX_ERR_MEMORY;
   *
   * The expression on the left is always positive because of the conversion
   * to size_t, which is unsigned.
   *
   * The code below code should fix this.
   */
  #define STKMARGIN (cell)(16 * sizeof(cell))
  if ((size_t)amx->stk < (size_t)(amx->hea + cells*sizeof(cell) + STKMARGIN)) {
    error =  AMX_ERR_MEMORY;
  } else {
    error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Allot_hook, int, AMXAPI,
                                 (AMX *, int, cell *, cell **), (amx, cells, amx_addr, phys_addr));
    sampgdk_log_debug("amx_Allot returned %d", error);
  }

  /* If called against the fake AMX and failed to allocate the requested
   * amount of space, grow the heap and try again.
   */
  if (error == AMX_ERR_MEMORY && amx == sampgdk_fakeamx_amx()) {
    cell new_size = ((amx->hea + STKMARGIN) / sizeof(cell)) + cells + 2;
    cell resize;

    sampgdk_log_debug("Growing fake AMX heap to %d bytes = %d = %d", new_size);
    resize = sampgdk_fakeamx_resize_heap(new_size);

    if (resize >= 0) {
      error = SAMPGDK_HOOK_CALL_CC(_sampgdk_amxhooks_Allot_hook, int, AMXAPI,
                                   (AMX *, int, cell *, cell **), (amx, cells, amx_addr, phys_addr));
    }
  }

  return error;
}

static int _sampgdk_amxhooks_create(void) {
  #define _SAMPGDK_AMXHOOKS_CREATE_HOOK(name) \
    if ((_sampgdk_amxhooks_##name##_hook = \
        sampgdk_hook_new((void *)sampgdk_amx_api->name, \
                              (void *)_sampgdk_amxhooks_##name)) == NULL) \
      goto no_memory;
  _SAMPGDK_AMXHOOKS_FUNC_LIST(_SAMPGDK_AMXHOOKS_CREATE_HOOK)
  return 0;
no_memory:
  return -ENOMEM;
  #undef _SAMPGDK_AMXHOOKS_CREATE_HOOK
}

static void _sampgdk_amxhooks_destroy(void) {
  #define _SAMPGDK_AMXHOOKS_DESTROY_HOOK(name) \
    sampgdk_hook_free(_sampgdk_amxhooks_##name##_hook);
  _SAMPGDK_AMXHOOKS_FUNC_LIST(_SAMPGDK_AMXHOOKS_DESTROY_HOOK)
  #undef _SAMPGDK_AMXHOOKS_DESTROY_HOOK
}

SAMPGDK_MODULE_INIT(amxhooks) {
  int error;

  error = _sampgdk_amxhooks_create();
  if (error < 0) {
    _sampgdk_amxhooks_destroy();
    return error;
  }

  return 0;
}

SAMPGDK_MODULE_CLEANUP(amxhooks) {
  _sampgdk_amxhooks_destroy();
}

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sampgdk.h"

#if SAMPGDK_WINDOWS
  #include <windows.h>
#endif

/* #include "array.h" */
/* #include "init.h" */
/* #include "log.h" */
/* #include "plugin.h" */
/* #include "timer.h" */
/* #include "types.h" */

struct _sampgdk_timer_info {
  bool    is_set;
  void   *plugin;
  int64_t started;
  int     interval;
  void   *callback;
  void   *param;
  bool    repeat;
};

static struct sampgdk_array _sampgdk_timers;

#if SAMPGDK_WINDOWS

static int64_t _sampgdk_timer_now(void) {
  LARGE_INTEGER freq;
  LARGE_INTEGER counter;

  if (QueryPerformanceFrequency(&freq) == 0) {
    sampgdk_log_error("QueryPerformanceFrequency: error %d", GetLastError());
    return 0;
  }
  if (QueryPerformanceCounter(&counter) == 0) {
    sampgdk_log_error("QueryPerformanceCounter: error %d", GetLastError());
    return 0;
  }

  return (int64_t)(1000.0L / freq.QuadPart * counter.QuadPart);
}

#else /* SAMPGDK_WINDOWS */

static int64_t _sampgdk_timer_now(void) {
  struct timespec ts;
  int64_t msec;
  int64_t msec_fract;

  if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0) {
    sampgdk_log_error("clock_gettime: %s", strerror(errno));
    return 0;
  }

  msec = (int64_t)ts.tv_sec * 1000;
  msec_fract = (int64_t)ts.tv_nsec / 1000000L;

  return msec + msec_fract;
}

#endif /* !SAMPGDK_WINDOWS */

static int _sampgdk_timer_find_slot(void) {
  int i;

  for (i = 0; i < _sampgdk_timers.count; i++) {
    struct _sampgdk_timer_info *timer;

    timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, i);
    if (!timer->is_set) {
      return i;
    }
  }

  return -1;
}

static void _sampgdk_timer_fire(int timerid, int64_t elapsed) {
  struct _sampgdk_timer_info *timer;
  int64_t now = _sampgdk_timer_now();
  int64_t started;

  assert(timerid > 0 && timerid <= _sampgdk_timers.count);
  timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, timerid - 1);

  assert(timer->is_set);
  started = timer->started;

  sampgdk_log_debug("Firing timer %d, now = %" PRId64 ", elapsed = %" PRId64,
      timerid, now, elapsed);
  ((sampgdk_timer_callback)timer->callback)(timerid, timer->param);

  /* We don't want to kill the same timer twice, so make sure it's not
   * been killed inside the timer callback.
   */
  if (timer->is_set && timer->started == started) {
    if (timer->repeat) {
      timer->started = now - (elapsed - timer->interval);
    } else {
      sampgdk_timer_kill(timerid);
    }
  }
}

SAMPGDK_MODULE_INIT(timer) {
  int error;

  error = sampgdk_array_new(&_sampgdk_timers,
                            8,
                            sizeof(struct _sampgdk_timer_info));
  if (error < 0) {
    return error;
  }

  sampgdk_array_zero(&_sampgdk_timers);

  return 0;
}

SAMPGDK_MODULE_CLEANUP(timer) {
  sampgdk_array_free(&_sampgdk_timers);
}

int sampgdk_timer_set(int interval,
                      bool repeat,
                      sampgdk_timer_callback callback,
                      void *param) {
  struct _sampgdk_timer_info timer;
  int slot;
  int error;
  int timerid;

  assert(callback != NULL);

  timer.is_set   = true;
  timer.interval = interval;
  timer.repeat   = repeat;
  timer.callback = (void *)callback;
  timer.param    = param;
  timer.started  = _sampgdk_timer_now();
  timer.plugin   = sampgdk_plugin_get_handle((void *)callback);

  if (timer.started == 0) {
    return 0; /* error already logged */
  }

  slot = _sampgdk_timer_find_slot();
  if (slot >= 0) {
    sampgdk_array_set(&_sampgdk_timers, slot, &timer);
  } else {
    error = sampgdk_array_append(&_sampgdk_timers, &timer);
    if (error < 0) {
      sampgdk_log_error("Error setting timer: %s", strerror(-error));
      return 0;
    }
    slot = _sampgdk_timers.count - 1;
  }

  /* Timer IDs returned by the SA:MP's SetTimer() API begin
   * with 1, and so do they here.
   */
  timerid = slot + 1;

  sampgdk_log_debug("Created timer: ID = %d, interval = %d, repeat = %s",
      timerid, interval, repeat ? "true" : "false");

  return timerid;
}

int sampgdk_timer_kill(int timerid) {
  struct _sampgdk_timer_info *timer;

  if (timerid <= 0 || timerid > _sampgdk_timers.count) {
    return -EINVAL;
  }

  timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, timerid - 1);
  if (!timer->is_set) {
    return -EINVAL;
  }

  timer->is_set = false;

  sampgdk_log_debug("Killed timer %d", timerid);

  return 0;
}

void sampgdk_timer_process_timers(void *plugin) {
  int64_t now;
  int64_t elapsed;
  int i;
  struct _sampgdk_timer_info *timer;

  assert(plugin != NULL);

  now = _sampgdk_timer_now();

  for (i = 0; i < _sampgdk_timers.count; i++) {
    timer = (struct _sampgdk_timer_info *)sampgdk_array_get(&_sampgdk_timers, i);

    if (!timer->is_set
        || (plugin != NULL && timer->plugin != plugin)) {
      continue;
    }

    elapsed = now - timer->started;

    if (elapsed >= timer->interval) {
      _sampgdk_timer_fire(i + 1, elapsed);
    }
  }
}

#include <assert.h>
#include <string.h>

#include "sampgdk.h"

/* #include "internal/amx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/logprintf.h" */
/* #include "internal/plugin.h" */
/* #include "internal/timer.h" */

#undef sampgdk_Load
#undef sampgdk_Unload
#undef sampgdk_ProcessTick

#ifdef _MSC_VER
  #include <intrin.h>
  #define _SAMPGDK_RETURN_ADDRESS() _ReturnAddress()
#else
  #define _SAMPGDK_RETURN_ADDRESS() __builtin_return_address(0)
#endif

#ifdef SAMPGDK_EMBEDDED
  #define _SAMPGDK_CALLER_HANDLE() \
    sampgdk_plugin_get_handle(((void *)_sampgdk_init))
#else
  #define _SAMPGDK_CALLER_HANDLE() \
    sampgdk_plugin_get_handle(_SAMPGDK_RETURN_ADDRESS())
#endif

static void _sampgdk_init(void **plugin_data) {
  int error;

  sampgdk_logprintf_impl = (logprintf_t)plugin_data[PLUGIN_DATA_LOGPRINTF];
  sampgdk_amx_api =
      (struct sampgdk_amx_api *)plugin_data[PLUGIN_DATA_AMX_EXPORTS];

  error = sampgdk_module_init();
  if (error  < 0) {
    sampgdk_log_error("Initialization failed: %s", strerror(-error));
  }

  sampgdk_log_info("GDK version: " SAMPGDK_VERSION_STRING);
}

static int _sampgdk_init_plugin(void *plugin, void **plugin_data) {
  int error;
  int num_plugins;

  assert(plugin != NULL);

  (void)sampgdk_plugin_get_plugins(&num_plugins);
  if (num_plugins == 0) {
    _sampgdk_init(plugin_data);
  }

  error = sampgdk_plugin_register(plugin);
  if (error < 0) {
    sampgdk_log_error("Error registering plugin: %s", strerror(-error));
  }

  return error;
}

static void _sampgdk_cleanup(void) {
  sampgdk_module_cleanup();
}

static void _sampgdk_cleanup_plugin(void *plugin) {
  int error;
  int num_plugins;

  assert(plugin != NULL);

  error = sampgdk_plugin_unregister(plugin);
  if (error < 0) {
    sampgdk_log_error("Error unregistering plugin: %s", strerror(-error));
  }

  (void)sampgdk_plugin_get_plugins(&num_plugins);
  if (num_plugins == 0) {
    _sampgdk_cleanup();
  }
}

SAMPGDK_API(unsigned int, sampgdk_Supports(void)) {
  return SUPPORTS_VERSION;
}

SAMPGDK_API(bool, sampgdk_Load(void **ppData, sampgdk_hidden_t hidden)) {
  return _sampgdk_init_plugin(_SAMPGDK_CALLER_HANDLE(), ppData) >= 0;
}

SAMPGDK_API(void, sampgdk_Unload(sampgdk_hidden_t hidden)) {
  _sampgdk_cleanup_plugin(_SAMPGDK_CALLER_HANDLE());
}

SAMPGDK_API(void, sampgdk_ProcessTick(sampgdk_hidden_t hidden)) {
  sampgdk_timer_process_timers(_SAMPGDK_CALLER_HANDLE());
}

SAMPGDK_API(void, sampgdk_logprintf(const char *format, ...)) {
  va_list args;
  va_start(args, format);
  sampgdk_do_vlogprintf(format, args);
  va_end(args);
}

SAMPGDK_API(void, sampgdk_vlogprintf(const char *format, va_list args)) {
  sampgdk_do_vlogprintf(format, args);
}

#include "sampgdk.h"

SAMPGDK_API(int, sampgdk_GetVersion(void)) {
  return SAMPGDK_VERSION_ID;
}

SAMPGDK_API(const char *, sampgdk_GetVersionString(void)) {
  return SAMPGDK_VERSION_STRING;
}

#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

typedef void (SAMPGDK_CALLBACK_CALL *OnHTTPResponse_callback)(int index, int response_code, const char * data);
static bool _OnHTTPResponse(AMX *amx, void *callback, cell *retval) {
  int index;
  int response_code;
  const char * data;
  sampgdk_param_get_cell(amx, 0, (cell *)&index);
  sampgdk_param_get_cell(amx, 1, (cell *)&response_code);
  sampgdk_param_get_string(amx, 2, (char * *)&data);
  sampgdk_log_debug("OnHTTPResponse(%d, %d, \"%s\")", index, response_code, data);
  ((OnHTTPResponse_callback)callback)(index, response_code, data);
  free((void *)data);
  return true;
}

SAMPGDK_MODULE_INIT(a_http) {
  int error;
  if ((error = sampgdk_callback_register("OnHTTPResponse", _OnHTTPResponse)) < 0) {
    return error;
  }
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_http) {
  sampgdk_callback_unregister("OnHTTPResponse");
}


#include "sampgdk.h"

/* #include "internal/callback.h" */
/* #include "internal/fakeamx.h" */
/* #include "internal/init.h" */
/* #include "internal/log.h" */
/* #include "internal/native.h" */
/* #include "internal/param.h" */

typedef bool (SAMPGDK_CALLBACK_CALL *OnGameModeInit_callback)();
static bool _OnGameModeInit(AMX *amx, void *callback, cell *retval) {
  sampgdk_log_debug("OnGameModeInit()");
  ((OnGameModeInit_callback)callback)();
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnGameModeExit_callback)();
static bool _OnGameModeExit(AMX *amx, void *callback, cell *retval) {
  sampgdk_log_debug("OnGameModeExit()");
  ((OnGameModeExit_callback)callback)();
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerConnect_callback)(int playerid);
static bool _OnPlayerConnect(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerConnect(%d)", playerid);
  retval_ = ((OnPlayerConnect_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerDisconnect_callback)(int playerid, int reason);
static bool _OnPlayerDisconnect(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int reason;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&reason);
  sampgdk_log_debug("OnPlayerDisconnect(%d, %d)", playerid, reason);
  retval_ = ((OnPlayerDisconnect_callback)callback)(playerid, reason);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerSpawn_callback)(int playerid);
static bool _OnPlayerSpawn(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerSpawn(%d)", playerid);
  retval_ = ((OnPlayerSpawn_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerDeath_callback)(int playerid, int killerid, int reason);
static bool _OnPlayerDeath(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int killerid;
  int reason;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&killerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&reason);
  sampgdk_log_debug("OnPlayerDeath(%d, %d, %d)", playerid, killerid, reason);
  retval_ = ((OnPlayerDeath_callback)callback)(playerid, killerid, reason);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleSpawn_callback)(int vehicleid);
static bool _OnVehicleSpawn(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int vehicleid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_log_debug("OnVehicleSpawn(%d)", vehicleid);
  retval_ = ((OnVehicleSpawn_callback)callback)(vehicleid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleDeath_callback)(int vehicleid, int killerid);
static bool _OnVehicleDeath(AMX *amx, void *callback, cell *retval) {
  int vehicleid;
  int killerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&killerid);
  sampgdk_log_debug("OnVehicleDeath(%d, %d)", vehicleid, killerid);
  ((OnVehicleDeath_callback)callback)(vehicleid, killerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerText_callback)(int playerid, const char * text);
static bool _OnPlayerText(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  const char * text;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_string(amx, 1, (char * *)&text);
  sampgdk_log_debug("OnPlayerText(%d, \"%s\")", playerid, text);
  retval_ = ((OnPlayerText_callback)callback)(playerid, text);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)text);
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerCommandText_callback)(int playerid, const char * cmdtext);
static bool _OnPlayerCommandText(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  const char * cmdtext;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_string(amx, 1, (char * *)&cmdtext);
  sampgdk_log_debug("OnPlayerCommandText(%d, \"%s\")", playerid, cmdtext);
  retval_ = ((OnPlayerCommandText_callback)callback)(playerid, cmdtext);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)cmdtext);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerRequestClass_callback)(int playerid, int classid);
static bool _OnPlayerRequestClass(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int classid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&classid);
  sampgdk_log_debug("OnPlayerRequestClass(%d, %d)", playerid, classid);
  ((OnPlayerRequestClass_callback)callback)(playerid, classid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterVehicle_callback)(int playerid, int vehicleid, bool ispassenger);
static bool _OnPlayerEnterVehicle(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int vehicleid;
  bool ispassenger;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_bool(amx, 2, (bool *)&ispassenger);
  sampgdk_log_debug("OnPlayerEnterVehicle(%d, %d, %d)", playerid, vehicleid, ispassenger);
  ((OnPlayerEnterVehicle_callback)callback)(playerid, vehicleid, ispassenger);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerExitVehicle_callback)(int playerid, int vehicleid);
static bool _OnPlayerExitVehicle(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int vehicleid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_log_debug("OnPlayerExitVehicle(%d, %d)", playerid, vehicleid);
  ((OnPlayerExitVehicle_callback)callback)(playerid, vehicleid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerStateChange_callback)(int playerid, int newstate, int oldstate);
static bool _OnPlayerStateChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int newstate;
  int oldstate;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newstate);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldstate);
  sampgdk_log_debug("OnPlayerStateChange(%d, %d, %d)", playerid, newstate, oldstate);
  ((OnPlayerStateChange_callback)callback)(playerid, newstate, oldstate);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterCheckpoint_callback)(int playerid);
static bool _OnPlayerEnterCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerEnterCheckpoint(%d)", playerid);
  ((OnPlayerEnterCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerLeaveCheckpoint_callback)(int playerid);
static bool _OnPlayerLeaveCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerLeaveCheckpoint(%d)", playerid);
  ((OnPlayerLeaveCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEnterRaceCheckpoint_callback)(int playerid);
static bool _OnPlayerEnterRaceCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerEnterRaceCheckpoint(%d)", playerid);
  ((OnPlayerEnterRaceCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerLeaveRaceCheckpoint_callback)(int playerid);
static bool _OnPlayerLeaveRaceCheckpoint(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerLeaveRaceCheckpoint(%d)", playerid);
  ((OnPlayerLeaveRaceCheckpoint_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnRconCommand_callback)(const char * cmd);
static bool _OnRconCommand(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  const char * cmd;
  sampgdk_param_get_string(amx, 0, (char * *)&cmd);
  sampgdk_log_debug("OnRconCommand(\"%s\")", cmd);
  retval_ = ((OnRconCommand_callback)callback)(cmd);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)cmd);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerRequestSpawn_callback)(int playerid);
static bool _OnPlayerRequestSpawn(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerRequestSpawn(%d)", playerid);
  retval_ = ((OnPlayerRequestSpawn_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnObjectMoved_callback)(int objectid);
static bool _OnObjectMoved(AMX *amx, void *callback, cell *retval) {
  int objectid;
  sampgdk_param_get_cell(amx, 0, (cell *)&objectid);
  sampgdk_log_debug("OnObjectMoved(%d)", objectid);
  ((OnObjectMoved_callback)callback)(objectid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerObjectMoved_callback)(int playerid, int objectid);
static bool _OnPlayerObjectMoved(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int objectid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&objectid);
  sampgdk_log_debug("OnPlayerObjectMoved(%d, %d)", playerid, objectid);
  ((OnPlayerObjectMoved_callback)callback)(playerid, objectid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerPickUpPickup_callback)(int playerid, int pickupid);
static bool _OnPlayerPickUpPickup(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int pickupid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&pickupid);
  sampgdk_log_debug("OnPlayerPickUpPickup(%d, %d)", playerid, pickupid);
  ((OnPlayerPickUpPickup_callback)callback)(playerid, pickupid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleMod_callback)(int playerid, int vehicleid, int componentid);
static bool _OnVehicleMod(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  int componentid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&componentid);
  sampgdk_log_debug("OnVehicleMod(%d, %d, %d)", playerid, vehicleid, componentid);
  retval_ = ((OnVehicleMod_callback)callback)(playerid, vehicleid, componentid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnEnterExitModShop_callback)(int playerid, bool enterexit, int interiorid);
static bool _OnEnterExitModShop(AMX *amx, void *callback, cell *retval) {
  int playerid;
  bool enterexit;
  int interiorid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_bool(amx, 1, (bool *)&enterexit);
  sampgdk_param_get_cell(amx, 2, (cell *)&interiorid);
  sampgdk_log_debug("OnEnterExitModShop(%d, %d, %d)", playerid, enterexit, interiorid);
  ((OnEnterExitModShop_callback)callback)(playerid, enterexit, interiorid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehiclePaintjob_callback)(int playerid, int vehicleid, int paintjobid);
static bool _OnVehiclePaintjob(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  int paintjobid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&paintjobid);
  sampgdk_log_debug("OnVehiclePaintjob(%d, %d, %d)", playerid, vehicleid, paintjobid);
  retval_ = ((OnVehiclePaintjob_callback)callback)(playerid, vehicleid, paintjobid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleRespray_callback)(int playerid, int vehicleid, int color1, int color2);
static bool _OnVehicleRespray(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  int color1;
  int color2;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&color1);
  sampgdk_param_get_cell(amx, 3, (cell *)&color2);
  sampgdk_log_debug("OnVehicleRespray(%d, %d, %d, %d)", playerid, vehicleid, color1, color2);
  retval_ = ((OnVehicleRespray_callback)callback)(playerid, vehicleid, color1, color2);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleDamageStatusUpdate_callback)(int vehicleid, int playerid);
static bool _OnVehicleDamageStatusUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int vehicleid;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_log_debug("OnVehicleDamageStatusUpdate(%d, %d)", vehicleid, playerid);
  retval_ = ((OnVehicleDamageStatusUpdate_callback)callback)(vehicleid, playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnUnoccupiedVehicleUpdate_callback)(int vehicleid, int playerid, int passenger_seat, float new_x, float new_y, float new_z, float vel_x, float vel_y, float vel_z);
static bool _OnUnoccupiedVehicleUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int vehicleid;
  int playerid;
  int passenger_seat;
  float new_x;
  float new_y;
  float new_z;
  float vel_x;
  float vel_y;
  float vel_z;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&passenger_seat);
  sampgdk_param_get_float(amx, 3, (float *)&new_x);
  sampgdk_param_get_float(amx, 4, (float *)&new_y);
  sampgdk_param_get_float(amx, 5, (float *)&new_z);
  sampgdk_param_get_float(amx, 6, (float *)&vel_x);
  sampgdk_param_get_float(amx, 7, (float *)&vel_y);
  sampgdk_param_get_float(amx, 8, (float *)&vel_z);
  sampgdk_log_debug("OnUnoccupiedVehicleUpdate(%d, %d, %d, %f, %f, %f, %f, %f, %f)", vehicleid, playerid, passenger_seat, new_x, new_y, new_z, vel_x, vel_y, vel_z);
  retval_ = ((OnUnoccupiedVehicleUpdate_callback)callback)(vehicleid, playerid, passenger_seat, new_x, new_y, new_z, vel_x, vel_y, vel_z);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerSelectedMenuRow_callback)(int playerid, int row);
static bool _OnPlayerSelectedMenuRow(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int row;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&row);
  sampgdk_log_debug("OnPlayerSelectedMenuRow(%d, %d)", playerid, row);
  ((OnPlayerSelectedMenuRow_callback)callback)(playerid, row);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerExitedMenu_callback)(int playerid);
static bool _OnPlayerExitedMenu(AMX *amx, void *callback, cell *retval) {
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerExitedMenu(%d)", playerid);
  ((OnPlayerExitedMenu_callback)callback)(playerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerInteriorChange_callback)(int playerid, int newinteriorid, int oldinteriorid);
static bool _OnPlayerInteriorChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int newinteriorid;
  int oldinteriorid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newinteriorid);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldinteriorid);
  sampgdk_log_debug("OnPlayerInteriorChange(%d, %d, %d)", playerid, newinteriorid, oldinteriorid);
  ((OnPlayerInteriorChange_callback)callback)(playerid, newinteriorid, oldinteriorid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerKeyStateChange_callback)(int playerid, int newkeys, int oldkeys);
static bool _OnPlayerKeyStateChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int newkeys;
  int oldkeys;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&newkeys);
  sampgdk_param_get_cell(amx, 2, (cell *)&oldkeys);
  sampgdk_log_debug("OnPlayerKeyStateChange(%d, %d, %d)", playerid, newkeys, oldkeys);
  ((OnPlayerKeyStateChange_callback)callback)(playerid, newkeys, oldkeys);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnRconLoginAttempt_callback)(const char * ip, const char * password, bool success);
static bool _OnRconLoginAttempt(AMX *amx, void *callback, cell *retval) {
  const char * ip;
  const char * password;
  bool success;
  sampgdk_param_get_string(amx, 0, (char * *)&ip);
  sampgdk_param_get_string(amx, 1, (char * *)&password);
  sampgdk_param_get_bool(amx, 2, (bool *)&success);
  sampgdk_log_debug("OnRconLoginAttempt(\"%s\", \"%s\", %d)", ip, password, success);
  ((OnRconLoginAttempt_callback)callback)(ip, password, success);
  free((void *)ip);
  free((void *)password);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerUpdate_callback)(int playerid);
static bool _OnPlayerUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_log_debug("OnPlayerUpdate(%d)", playerid);
  retval_ = ((OnPlayerUpdate_callback)callback)(playerid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerStreamIn_callback)(int playerid, int forplayerid);
static bool _OnPlayerStreamIn(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnPlayerStreamIn(%d, %d)", playerid, forplayerid);
  ((OnPlayerStreamIn_callback)callback)(playerid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerStreamOut_callback)(int playerid, int forplayerid);
static bool _OnPlayerStreamOut(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnPlayerStreamOut(%d, %d)", playerid, forplayerid);
  ((OnPlayerStreamOut_callback)callback)(playerid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleStreamIn_callback)(int vehicleid, int forplayerid);
static bool _OnVehicleStreamIn(AMX *amx, void *callback, cell *retval) {
  int vehicleid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnVehicleStreamIn(%d, %d)", vehicleid, forplayerid);
  ((OnVehicleStreamIn_callback)callback)(vehicleid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleStreamOut_callback)(int vehicleid, int forplayerid);
static bool _OnVehicleStreamOut(AMX *amx, void *callback, cell *retval) {
  int vehicleid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnVehicleStreamOut(%d, %d)", vehicleid, forplayerid);
  ((OnVehicleStreamOut_callback)callback)(vehicleid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnActorStreamIn_callback)(int actorid, int forplayerid);
static bool _OnActorStreamIn(AMX *amx, void *callback, cell *retval) {
  int actorid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&actorid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnActorStreamIn(%d, %d)", actorid, forplayerid);
  ((OnActorStreamIn_callback)callback)(actorid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnActorStreamOut_callback)(int actorid, int forplayerid);
static bool _OnActorStreamOut(AMX *amx, void *callback, cell *retval) {
  int actorid;
  int forplayerid;
  sampgdk_param_get_cell(amx, 0, (cell *)&actorid);
  sampgdk_param_get_cell(amx, 1, (cell *)&forplayerid);
  sampgdk_log_debug("OnActorStreamOut(%d, %d)", actorid, forplayerid);
  ((OnActorStreamOut_callback)callback)(actorid, forplayerid);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnDialogResponse_callback)(int playerid, int dialogid, int response, int listitem, const char * inputtext);
static bool _OnDialogResponse(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int dialogid;
  int response;
  int listitem;
  const char * inputtext;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&dialogid);
  sampgdk_param_get_cell(amx, 2, (cell *)&response);
  sampgdk_param_get_cell(amx, 3, (cell *)&listitem);
  sampgdk_param_get_string(amx, 4, (char * *)&inputtext);
  sampgdk_log_debug("OnDialogResponse(%d, %d, %d, %d, \"%s\")", playerid, dialogid, response, listitem, inputtext);
  retval_ = ((OnDialogResponse_callback)callback)(playerid, dialogid, response, listitem, inputtext);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)inputtext);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerTakeDamage_callback)(int playerid, int issuerid, float amount, int weaponid, int bodypart);
static bool _OnPlayerTakeDamage(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int issuerid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&issuerid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnPlayerTakeDamage(%d, %d, %f, %d, %d)", playerid, issuerid, amount, weaponid, bodypart);
  retval_ = ((OnPlayerTakeDamage_callback)callback)(playerid, issuerid, amount, weaponid, bodypart);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerGiveDamage_callback)(int playerid, int damagedid, float amount, int weaponid, int bodypart);
static bool _OnPlayerGiveDamage(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int damagedid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&damagedid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnPlayerGiveDamage(%d, %d, %f, %d, %d)", playerid, damagedid, amount, weaponid, bodypart);
  retval_ = ((OnPlayerGiveDamage_callback)callback)(playerid, damagedid, amount, weaponid, bodypart);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerGiveDamageActor_callback)(int playerid, int damaged_actorid, float amount, int weaponid, int bodypart);
static bool _OnPlayerGiveDamageActor(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int damaged_actorid;
  float amount;
  int weaponid;
  int bodypart;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&damaged_actorid);
  sampgdk_param_get_float(amx, 2, (float *)&amount);
  sampgdk_param_get_cell(amx, 3, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 4, (cell *)&bodypart);
  sampgdk_log_debug("OnPlayerGiveDamageActor(%d, %d, %f, %d, %d)", playerid, damaged_actorid, amount, weaponid, bodypart);
  retval_ = ((OnPlayerGiveDamageActor_callback)callback)(playerid, damaged_actorid, amount, weaponid, bodypart);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickMap_callback)(int playerid, float fX, float fY, float fZ);
static bool _OnPlayerClickMap(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_float(amx, 1, (float *)&fX);
  sampgdk_param_get_float(amx, 2, (float *)&fY);
  sampgdk_param_get_float(amx, 3, (float *)&fZ);
  sampgdk_log_debug("OnPlayerClickMap(%d, %f, %f, %f)", playerid, fX, fY, fZ);
  retval_ = ((OnPlayerClickMap_callback)callback)(playerid, fX, fY, fZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickTextDraw_callback)(int playerid, int clickedid);
static bool _OnPlayerClickTextDraw(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int clickedid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&clickedid);
  sampgdk_log_debug("OnPlayerClickTextDraw(%d, %d)", playerid, clickedid);
  retval_ = ((OnPlayerClickTextDraw_callback)callback)(playerid, clickedid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickPlayerTextDraw_callback)(int playerid, int playertextid);
static bool _OnPlayerClickPlayerTextDraw(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int playertextid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&playertextid);
  sampgdk_log_debug("OnPlayerClickPlayerTextDraw(%d, %d)", playerid, playertextid);
  retval_ = ((OnPlayerClickPlayerTextDraw_callback)callback)(playerid, playertextid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnIncomingConnection_callback)(int playerid, const char * ip_address, int port);
static bool _OnIncomingConnection(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  const char * ip_address;
  int port;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_string(amx, 1, (char * *)&ip_address);
  sampgdk_param_get_cell(amx, 2, (cell *)&port);
  sampgdk_log_debug("OnIncomingConnection(%d, \"%s\", %d)", playerid, ip_address, port);
  retval_ = ((OnIncomingConnection_callback)callback)(playerid, ip_address, port);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  free((void *)ip_address);
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnTrailerUpdate_callback)(int playerid, int vehicleid);
static bool _OnTrailerUpdate(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int vehicleid;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_log_debug("OnTrailerUpdate(%d, %d)", playerid, vehicleid);
  retval_ = ((OnTrailerUpdate_callback)callback)(playerid, vehicleid);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnVehicleSirenStateChange_callback)(int playerid, int vehicleid, int newstate);
static bool _OnVehicleSirenStateChange(AMX *amx, void *callback, cell *retval) {
  int playerid;
  int vehicleid;
  int newstate;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&vehicleid);
  sampgdk_param_get_cell(amx, 2, (cell *)&newstate);
  sampgdk_log_debug("OnVehicleSirenStateChange(%d, %d, %d)", playerid, vehicleid, newstate);
  ((OnVehicleSirenStateChange_callback)callback)(playerid, vehicleid, newstate);
  return true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerClickPlayer_callback)(int playerid, int clickedplayerid, int source);
static bool _OnPlayerClickPlayer(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int clickedplayerid;
  int source;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&clickedplayerid);
  sampgdk_param_get_cell(amx, 2, (cell *)&source);
  sampgdk_log_debug("OnPlayerClickPlayer(%d, %d, %d)", playerid, clickedplayerid, source);
  retval_ = ((OnPlayerClickPlayer_callback)callback)(playerid, clickedplayerid, source);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEditObject_callback)(int playerid, bool playerobject, int objectid, int response, float fX, float fY, float fZ, float fRotX, float fRotY, float fRotZ);
static bool _OnPlayerEditObject(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  bool playerobject;
  int objectid;
  int response;
  float fX;
  float fY;
  float fZ;
  float fRotX;
  float fRotY;
  float fRotZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_bool(amx, 1, (bool *)&playerobject);
  sampgdk_param_get_cell(amx, 2, (cell *)&objectid);
  sampgdk_param_get_cell(amx, 3, (cell *)&response);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_param_get_float(amx, 7, (float *)&fRotX);
  sampgdk_param_get_float(amx, 8, (float *)&fRotY);
  sampgdk_param_get_float(amx, 9, (float *)&fRotZ);
  sampgdk_log_debug("OnPlayerEditObject(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f)", playerid, playerobject, objectid, response, fX, fY, fZ, fRotX, fRotY, fRotZ);
  retval_ = ((OnPlayerEditObject_callback)callback)(playerid, playerobject, objectid, response, fX, fY, fZ, fRotX, fRotY, fRotZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerEditAttachedObject_callback)(int playerid, int response, int index, int modelid, int boneid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, float fScaleX, float fScaleY, float fScaleZ);
static bool _OnPlayerEditAttachedObject(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int response;
  int index;
  int modelid;
  int boneid;
  float fOffsetX;
  float fOffsetY;
  float fOffsetZ;
  float fRotX;
  float fRotY;
  float fRotZ;
  float fScaleX;
  float fScaleY;
  float fScaleZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&response);
  sampgdk_param_get_cell(amx, 2, (cell *)&index);
  sampgdk_param_get_cell(amx, 3, (cell *)&modelid);
  sampgdk_param_get_cell(amx, 4, (cell *)&boneid);
  sampgdk_param_get_float(amx, 5, (float *)&fOffsetX);
  sampgdk_param_get_float(amx, 6, (float *)&fOffsetY);
  sampgdk_param_get_float(amx, 7, (float *)&fOffsetZ);
  sampgdk_param_get_float(amx, 8, (float *)&fRotX);
  sampgdk_param_get_float(amx, 9, (float *)&fRotY);
  sampgdk_param_get_float(amx, 10, (float *)&fRotZ);
  sampgdk_param_get_float(amx, 11, (float *)&fScaleX);
  sampgdk_param_get_float(amx, 12, (float *)&fScaleY);
  sampgdk_param_get_float(amx, 13, (float *)&fScaleZ);
  sampgdk_log_debug("OnPlayerEditAttachedObject(%d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f)", playerid, response, index, modelid, boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
  retval_ = ((OnPlayerEditAttachedObject_callback)callback)(playerid, response, index, modelid, boneid, fOffsetX, fOffsetY, fOffsetZ, fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerSelectObject_callback)(int playerid, int type, int objectid, int modelid, float fX, float fY, float fZ);
static bool _OnPlayerSelectObject(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int type;
  int objectid;
  int modelid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&type);
  sampgdk_param_get_cell(amx, 2, (cell *)&objectid);
  sampgdk_param_get_cell(amx, 3, (cell *)&modelid);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_log_debug("OnPlayerSelectObject(%d, %d, %d, %d, %f, %f, %f)", playerid, type, objectid, modelid, fX, fY, fZ);
  retval_ = ((OnPlayerSelectObject_callback)callback)(playerid, type, objectid, modelid, fX, fY, fZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerWeaponShot_callback)(int playerid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ);
static bool _OnPlayerWeaponShot(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int weaponid;
  int hittype;
  int hitid;
  float fX;
  float fY;
  float fZ;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&weaponid);
  sampgdk_param_get_cell(amx, 2, (cell *)&hittype);
  sampgdk_param_get_cell(amx, 3, (cell *)&hitid);
  sampgdk_param_get_float(amx, 4, (float *)&fX);
  sampgdk_param_get_float(amx, 5, (float *)&fY);
  sampgdk_param_get_float(amx, 6, (float *)&fZ);
  sampgdk_log_debug("OnPlayerWeaponShot(%d, %d, %d, %d, %f, %f, %f)", playerid, weaponid, hittype, hitid, fX, fY, fZ);
  retval_ = ((OnPlayerWeaponShot_callback)callback)(playerid, weaponid, hittype, hitid, fX, fY, fZ);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != false;
}

typedef bool (SAMPGDK_CALLBACK_CALL *OnPlayerRequestDownload_callback)(int playerid, int type, int crc);
static bool _OnPlayerRequestDownload(AMX *amx, void *callback, cell *retval) {
  bool retval_;
  int playerid;
  int type;
  int crc;
  sampgdk_param_get_cell(amx, 0, (cell *)&playerid);
  sampgdk_param_get_cell(amx, 1, (cell *)&type);
  sampgdk_param_get_cell(amx, 2, (cell *)&crc);
  sampgdk_log_debug("OnPlayerRequestDownload(%d, %d, %d)", playerid, type, crc);
  retval_ = ((OnPlayerRequestDownload_callback)callback)(playerid, type, crc);
  if (retval != NULL) {
    *retval = (cell)retval_;
  }
  return !!retval_ != true;
}

SAMPGDK_MODULE_INIT(a_samp) {
  int error;
  if ((error = sampgdk_callback_register("OnVehicleStreamOut", _OnVehicleStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleStreamIn", _OnVehicleStreamIn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleSpawn", _OnVehicleSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleSirenStateChange", _OnVehicleSirenStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleRespray", _OnVehicleRespray)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehiclePaintjob", _OnVehiclePaintjob)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleMod", _OnVehicleMod)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleDeath", _OnVehicleDeath)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnVehicleDamageStatusUpdate", _OnVehicleDamageStatusUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnUnoccupiedVehicleUpdate", _OnUnoccupiedVehicleUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnTrailerUpdate", _OnTrailerUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnRconLoginAttempt", _OnRconLoginAttempt)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnRconCommand", _OnRconCommand)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerWeaponShot", _OnPlayerWeaponShot)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerUpdate", _OnPlayerUpdate)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerText", _OnPlayerText)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerTakeDamage", _OnPlayerTakeDamage)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerStreamOut", _OnPlayerStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerStreamIn", _OnPlayerStreamIn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerStateChange", _OnPlayerStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerSpawn", _OnPlayerSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerSelectedMenuRow", _OnPlayerSelectedMenuRow)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerSelectObject", _OnPlayerSelectObject)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerRequestSpawn", _OnPlayerRequestSpawn)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerRequestDownload", _OnPlayerRequestDownload)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerRequestClass", _OnPlayerRequestClass)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerPickUpPickup", _OnPlayerPickUpPickup)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerObjectMoved", _OnPlayerObjectMoved)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerLeaveRaceCheckpoint", _OnPlayerLeaveRaceCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerLeaveCheckpoint", _OnPlayerLeaveCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerKeyStateChange", _OnPlayerKeyStateChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerInteriorChange", _OnPlayerInteriorChange)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerGiveDamageActor", _OnPlayerGiveDamageActor)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerGiveDamage", _OnPlayerGiveDamage)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerExitedMenu", _OnPlayerExitedMenu)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerExitVehicle", _OnPlayerExitVehicle)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterVehicle", _OnPlayerEnterVehicle)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterRaceCheckpoint", _OnPlayerEnterRaceCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEnterCheckpoint", _OnPlayerEnterCheckpoint)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEditObject", _OnPlayerEditObject)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerEditAttachedObject", _OnPlayerEditAttachedObject)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerDisconnect", _OnPlayerDisconnect)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerDeath", _OnPlayerDeath)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerConnect", _OnPlayerConnect)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerCommandText", _OnPlayerCommandText)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickTextDraw", _OnPlayerClickTextDraw)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickPlayerTextDraw", _OnPlayerClickPlayerTextDraw)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickPlayer", _OnPlayerClickPlayer)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnPlayerClickMap", _OnPlayerClickMap)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnObjectMoved", _OnObjectMoved)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnIncomingConnection", _OnIncomingConnection)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnGameModeInit", _OnGameModeInit)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnGameModeExit", _OnGameModeExit)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnEnterExitModShop", _OnEnterExitModShop)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnDialogResponse", _OnDialogResponse)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnActorStreamOut", _OnActorStreamOut)) < 0) {
    return error;
  }
  if ((error = sampgdk_callback_register("OnActorStreamIn", _OnActorStreamIn)) < 0) {
    return error;
  }
  return 0;
}

SAMPGDK_MODULE_CLEANUP(a_samp) {
  sampgdk_callback_unregister("OnVehicleStreamOut");
  sampgdk_callback_unregister("OnVehicleStreamIn");
  sampgdk_callback_unregister("OnVehicleSpawn");
  sampgdk_callback_unregister("OnVehicleSirenStateChange");
  sampgdk_callback_unregister("OnVehicleRespray");
  sampgdk_callback_unregister("OnVehiclePaintjob");
  sampgdk_callback_unregister("OnVehicleMod");
  sampgdk_callback_unregister("OnVehicleDeath");
  sampgdk_callback_unregister("OnVehicleDamageStatusUpdate");
  sampgdk_callback_unregister("OnUnoccupiedVehicleUpdate");
  sampgdk_callback_unregister("OnTrailerUpdate");
  sampgdk_callback_unregister("OnRconLoginAttempt");
  sampgdk_callback_unregister("OnRconCommand");
  sampgdk_callback_unregister("OnPlayerWeaponShot");
  sampgdk_callback_unregister("OnPlayerUpdate");
  sampgdk_callback_unregister("OnPlayerText");
  sampgdk_callback_unregister("OnPlayerTakeDamage");
  sampgdk_callback_unregister("OnPlayerStreamOut");
  sampgdk_callback_unregister("OnPlayerStreamIn");
  sampgdk_callback_unregister("OnPlayerStateChange");
  sampgdk_callback_unregister("OnPlayerSpawn");
  sampgdk_callback_unregister("OnPlayerSelectedMenuRow");
  sampgdk_callback_unregister("OnPlayerSelectObject");
  sampgdk_callback_unregister("OnPlayerRequestSpawn");
  sampgdk_callback_unregister("OnPlayerRequestDownload");
  sampgdk_callback_unregister("OnPlayerRequestClass");
  sampgdk_callback_unregister("OnPlayerPickUpPickup");
  sampgdk_callback_unregister("OnPlayerObjectMoved");
  sampgdk_callback_unregister("OnPlayerLeaveRaceCheckpoint");
  sampgdk_callback_unregister("OnPlayerLeaveCheckpoint");
  sampgdk_callback_unregister("OnPlayerKeyStateChange");
  sampgdk_callback_unregister("OnPlayerInteriorChange");
  sampgdk_callback_unregister("OnPlayerGiveDamageActor");
  sampgdk_callback_unregister("OnPlayerGiveDamage");
  sampgdk_callback_unregister("OnPlayerExitedMenu");
  sampgdk_callback_unregister("OnPlayerExitVehicle");
  sampgdk_callback_unregister("OnPlayerEnterVehicle");
  sampgdk_callback_unregister("OnPlayerEnterRaceCheckpoint");
  sampgdk_callback_unregister("OnPlayerEnterCheckpoint");
  sampgdk_callback_unregister("OnPlayerEditObject");
  sampgdk_callback_unregister("OnPlayerEditAttachedObject");
  sampgdk_callback_unregister("OnPlayerDisconnect");
  sampgdk_callback_unregister("OnPlayerDeath");
  sampgdk_callback_unregister("OnPlayerConnect");
  sampgdk_callback_unregister("OnPlayerCommandText");
  sampgdk_callback_unregister("OnPlayerClickTextDraw");
  sampgdk_callback_unregister("OnPlayerClickPlayerTextDraw");
  sampgdk_callback_unregister("OnPlayerClickPlayer");
  sampgdk_callback_unregister("OnPlayerClickMap");
  sampgdk_callback_unregister("OnObjectMoved");
  sampgdk_callback_unregister("OnIncomingConnection");
  sampgdk_callback_unregister("OnGameModeInit");
  sampgdk_callback_unregister("OnGameModeExit");
  sampgdk_callback_unregister("OnEnterExitModShop");
  sampgdk_callback_unregister("OnDialogResponse");
  sampgdk_callback_unregister("OnActorStreamOut");
  sampgdk_callback_unregister("OnActorStreamIn");
}


