# SPDX-License-Identifer: MIT

tests_to_pass = [
    'whitespace',
    'empty_array',
    'empty_object',
    'empty_string',
    'multiple_top',
    'nested_array',
    'nested_object',
    'literal_array',
    'number_array',
    'escape_string',
    'long_string',
    'populated_object',
    'complex',
    'bytewise',
    'whitespace2',
]

tests_to_fail = [
    'array_missing_comma',
    'object_missing_comma',
    'object_missing_colon',
]

foreach name : tests_to_pass
    exe = executable(name, f'pass_@name@_test.c',
        dependencies: [sjp_dep],
    )
    test(f'pass_@name@', exe, should_fail: false)
endforeach

foreach name : tests_to_fail
    exe = executable(name, f'fail_@name@_test.c',
        dependencies: [sjp_dep],
    )
    test(f'fail_@name@', exe, should_fail: true)
endforeach
