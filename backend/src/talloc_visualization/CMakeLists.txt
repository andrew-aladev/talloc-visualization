add_library(
    talloc_visualization SHARED
    main.c
)
target_link_libraries(talloc_visualization talloc-debug)