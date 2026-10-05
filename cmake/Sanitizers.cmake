function(aican_enable_sanitizers target)
    if(NOT AICAN_ENABLE_ASAN)
        return()
    endif()
    if(MSVC)
        target_compile_options(${target} PUBLIC /fsanitize=address)
    else()
        target_compile_options(${target} PUBLIC
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PUBLIC
            -fsanitize=address,undefined)
    endif()
endfunction()
