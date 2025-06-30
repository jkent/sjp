# About SJP

SJP (Streaming JSON Parser) is a small, lightweight JSON parser that is
designed for embedded use. It was designed to be paired with
[ACEWS](https://github.com/jkent/acews) for use with websockets. It can process
chunks of data in any size, from bytes at a time to complete JSON documents or
even multiple documents.

A callback system is employed -- the start, continuation, or end of a data
element is detected, the callback is triggered with information about the
event. A stack is also used to keep track of array and object nesting.

## Functional description

SJP uses a single data structure to keep track of its state. The library
itself only consists of 4 functions:

  * `sjp_init(cb) → sjp`: create sjp
  * `sjp_free(sjp) → void`: free sjp
  * `sjp_reset(sjp) → void`: reset sjp (for error recovery)
  * `sjp_parse(sjp, buf, len) → int`: parse a chunk, -1 on error

The `sjp` structure:

  * `user`: user-defined pointer
  * `cb`: callback function
  * `d`: stack depth
  * `stk[]`: stack
    * `i`: index
    * `t`: token
  * `str`: string value
  * `str_bytes`: string byte length
  * `str_pos`: string bytes so far
  * `num`: number value

Tokens are split up into two parts, the data element type, and flags. Data
types, values 0 thru 7:

  * `SJP_NONE_T`: Not a valid type
  * `SJP_OBJ_T`: Object type
  * `SJP_ARRAY_T`: Array type
  * `SJP_STR_T`: String type
  * `SJP_NUM_T`: Number type
  * `SJP_TRUE_T`: literal `true` type
  * `SJP_FALSE_T`: literal `false` type
  * `SJP_NULL_T`: literal `null` type

There is a mask, `SJP_TYPE` to remove the flags so you can test the data
element type separately. These are the user flags:

  * `SJP_KEY`: String is an object key
  * `SJP_START`: Begining of type
  * `SJP_END`: End of type

There are a couple of other flags, but they are used internally for parsing
state. There are also a couple of masks, one for data element type, `SJP_TYPE`
and the other for the user-facing flags, `SJP_FLAGS`.

The callback function signature is `callback(sjp) → void` and you're expected
to check `sjp->d`, `sjp->stk[sjp->d].i`, and `sjp->stk[sjp->d].t` to figure out
what you should do within the callback.

## Example

Take the following document for example:

```json
{
    "name": "John",
    "age": 30,
    "car": null
}
```

Here's an example of how you could process it:

```c
static void callback(sjp_t *sjp)
{
    user_t *user = sjp->user;

    /* do something with sjp state */
}

static void process_document(const char *document, user_t *user)
{
    sjp_t *sjp = sjp_init(callback);
    sjp->user = user;

    if (sjp_parse(sjp, document, strlen(document)) < 0) {
        fprintf(stderr, "error parsing document");
        /* perform any necessary cleanup */
    }

    sjp_free(sjp);
}
```

If you ran `process_document` with the above document, 8 calls to callback with
the following values would occur:

```
sjp->d == 0
sjp->stk[0].i == 0
sjp->stk[0].t & SJP_TYPE == SJP_OBJ_T
sjp->stk[0].t & SJP_FLAGS == SJP_START
```

```
sjp->d == 1
sjp->stk[1].i == 0
(sjp->stk[1].t & SJP_TYPE) == SJP_STR_T
(sjp->stk[1].t & SJP_FLAGS) == SJP_KEY | SJP_START | SJP_END
sjp->str_len == 4
sjp->str == "name"
```

```
sjp->d == 1
sjp->stk[1].i == 0
(sjp->stk[1].t & SJP_TYPE) == SJP_STR_T
(sjp->stk[1].t & SJP_FLAGS) == SJP_START | SJP_END
sjp->str_len == 4
sjp->str == "John"
```

```
sjp->d == 1
sjp->stk[1].i == 1
(sjp->stk[1].t & SJP_TYPE) == SJP_STR_T
(sjp->stk[1].t & SJP_FLAGS) == SJP_KEY | SJP_START | SJP_END
sjp->str_len == 3
sjp->str == "age"
```

```
sjp->d == 1
sjp->stk[1].i == 1
(sjp->stk[1].t & SJP_TYPE) == SJP_NUM_T
(sjp->stk[1].t & SJP_FLAGS) == SJP_START | SJP_END
sjp->num == 30
```

```
sjp->d == 1
sjp->stk[1].i == 2
(sjp->stk[1].t & SJP_TYPE) == SJP_STR_T
(sjp->stk[1].t & SJP_FLAGS) == SJP_KEY | SJP_START | SJP_END
sjp->str_len == 3
sjp->str == "car"
```

```
sjp->d == 1
sjp->stk[1].i == 2
(sjp->stk[1].t & SJP_TYPE) == SJP_NULL_T
(sjp->stk[1].t & SJP_FLAGS) == SJP_START | SJP_END
```

```
sjp->d == 0
sjp->stk[0].i == 0
sjp->stk[0].t & SJP_TYPE == SJP_OBJ_T
sjp->stk[0].t & SJP_FLAGS == SJP_END
```

Note that strings can and will be split on a UTF-8 character boundary if a
document is split up in chunks when being parsed. You can use `sjp->str_pos` to
get the current byte position in the string.
