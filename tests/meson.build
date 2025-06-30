# SPDX-License-Identifer: MIT

tests_to_pass = [
    'whitespace',
    'whitespace2',
    'object_empty',
    'object_nested',
    'object_populated',
    'object_complex',
    'object_complex_byte',
    'array_empty',
    'array_nested',
    'array_numbers',
    'array_literals',
    'string_empty',
    'string_escape',
    'string_long',
    'literal_true',
    'literal_false',
    'literal_null',
    'multiple_documents',
]

tests_to_fail = [
    'object_trailing_comma',
    'object_missing_comma',
    'object_missing_colon',
    'object_missing_value',
    'array_trailing_comma',
    'array_missing_comma',
]

foreach name : tests_to_pass
    exe = executable(name, f'pass_@name@_test.c',
        build_by_default: false,
        dependencies: [sjp_dep],
    )
    test(f'pass_@name@', exe, should_fail: false)
endforeach

foreach name : tests_to_fail
    exe = executable(name, f'fail_@name@_test.c',
        build_by_default: false,
        dependencies: [sjp_dep],
    )
    test(f'fail_@name@', exe, should_fail: true)
endforeach
