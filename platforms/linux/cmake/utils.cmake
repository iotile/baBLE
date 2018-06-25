#### Utils functions ####

# Function used to add a cached flag if it does not already exist
function(add_cached_flag var_name flag)
    # Find for the flag
    string(FIND " ${${var_name}} " " ${flag} " flag_index)
    if(NOT flag_index EQUAL -1)
        # If it exists, return
        return()
    endif()

    # If the variable was empty, just add the new value it without space, else separate it from old value with a space
    string(COMPARE EQUAL "" "${${var_name}}" is_empty)
    if(is_empty)
        set("${var_name}" "${flag}" CACHE STRING "" FORCE)
    else()
        set("${var_name}" "${flag} ${${var_name}}" CACHE STRING "" FORCE)
    endif()
endfunction()
