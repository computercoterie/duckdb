add_library_unity(test_parallel_csv OBJECT test_parallel_csv.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_parallel_csv>
    PARENT_SCOPE)
