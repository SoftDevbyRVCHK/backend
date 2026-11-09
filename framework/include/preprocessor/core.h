#ifndef ODYSSEY_FRAMEWORK_PREPROCESSOR_H
#define ODYSSEY_FRAMEWORK_PREPROCESSOR_H

/** preprocessor instruments **/

#define FM_EMPTY()
#define FM_DEFER(id) id FM_EMPTY()
#define FM_MKCALL(id, ...) \
    DEFER(id##_INDIRECT)()(__VA_ARGS__)

#define FM_EXPAND(...) E6(E6(__VA_ARGS__))
#define FM_E6(...) E5(E5(__VA_ARGS__))
#define FM_E5(...) E4(E4(__VA_ARGS__))
#define FM_E4(...) E3(E3(__VA_ARGS__))
#define FM_E3(...) E2(E2(__VA_ARGS__))
#define FM_E2(...) E1(E1(__VA_ARGS__))
#define FM_E1(...) __VA_ARGS__

#define FM_TUPLE_TO_VA_BODY(...) __VA_ARGS__
#define FM_TUPLE_TO_VA(tuple) FM_TUPLE_TO_VA_BODY tuple

/** preprocessor instruments **/

#endif /* ODYSSEY_FRAMEWORK_PREPROCESSOR_H */