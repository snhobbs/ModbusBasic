if(NOT DEFINED CMAKE_CXX_CPPLINT)
set(CMAKE_CXX_CPPLINT "cpplint;--verbose=1;--linelength=100")
endif()

if(NOT DEFINED CMAKE_CXX_CLANG_TIDY)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*;-format-style=google;-export-fixes=${CMAKE_CURRENT_BINARY_DIR}/clangtidy.results")
endif()
#------------------------------------------------
#  CppClean
#------------------------------------------------
function(cppclean directory files command)
    set(target_name cppclean)
    set(output ${directory}/${target_name}.results)
    add_custom_target(
        ${target_name} 
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET ${target_name} 
        BYPRODUCTS ${output}
        COMMENT "Running ${target_name}, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND ${command}
        ${files} 2>&1 | tee ${output}
    )
endfunction()
#------------------------------------------------
#  CppCheck
#------------------------------------------------
function(Cppcheck directory files command)
    set(output ${directory}/cppcheck.results)
    add_custom_target(
        cppcheck
        ALL
        WORKING_DIRECTORY ${directory}
    )
add_custom_command(
        TARGET cppcheck
        COMMENT "Running CppCheck, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        BYPRODUCTS ${output}
        COMMAND "${command}"
        ARGS
        #--enable=warning,performance,portability,information,missingInclude
        --enable=all
        #--check-config
        --output-file=${output}
        --force
        --language=c++
        --std=c++14
        #--library=qt.cfg
        #--template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --verbose
        --quiet
        ${files}
    )
endfunction()
#------------------------------------------------
#  Clangtidy
#------------------------------------------------
function(clangtidy directory files command)
    set(target_name clangtidy)
    set(output ${directory}/${target_name}.results)
    add_custom_target(
        ${target_name} 
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET ${target_name} 
        BYPRODUCTS ${output}
        COMMENT "Running ${target_name}, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND ${command}
        ARGS "-checks=*"
        ARGS "-format-style=google"
        ARGS "-export-fixes="
        ARGS "-checks=*"
        ARGS "-checks=*"
        ${files} 2>&1 | tee ${output}
    )
endfunction()
#------------------------------------------------
#  Flawfinder
#------------------------------------------------
function(flawfinder directory files command)
    set(target_name flawfinder)
    set(output ${directory}/${target_name}.results)
    add_custom_target(
        ${target_name} 
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET ${target_name} 
        BYPRODUCTS ${output}
        COMMENT "Running ${target_name}, on ${files} exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND ${command}
        ARGS ${files} 2>&1 | tee ${output}
    )
endfunction()
#------------------------------------------------
#  oclint
#------------------------------------------------
function(oclint directory files command)
    set(TargetName oclint)
    set(output ${directory}/${TargetName}.results)
    add_custom_target(
        ${TargetName}
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET ${TargetName}
        PRE_BUILD
        BYPRODUCTS ${output}
        COMMENT "Running ${TargetName}, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND "${command}"
        ARGS
        --linelength=250
        --verbose=5
        ${files} 2>&1 | tee ${output}
    )
endfunction()
#------------------------------------------------
#  Flint
#------------------------------------------------
function(flint directory files command)
    set(target_name flint)
    set(output ${directory}/${target_name}.results)
    add_custom_target(
        ${target_name} 
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET ${target_name} 
        BYPRODUCTS ${output}
        COMMENT "Running ${target_name}, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND "${command}"
        ARGS
        ${files} 2>&1 | tee ${output}
    )
endfunction()
#------------------------------------------------
#  ClangCheck
#------------------------------------------------
function(clangcheck directory files command)
    set(TargetName clangcheck)
    set(output ${directory}/${TargetName}.results)
    add_custom_target(
        ${TargetName}
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET ${TargetName}
        PRE_BUILD
        BYPRODUCTS ${output}
        COMMENT "Running ${TargetName}, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND "${command}"
        ${files} 2>&1 | tee ${output}
    )
endfunction()
#------------------------------------------------
#  CppLint
#------------------------------------------------
function(Cpplint directory files command)
    set(output ${directory}/cpplint.results)
    add_custom_target(
        cpplint
        ALL
        WORKING_DIRECTORY ${directory}
    )

    add_custom_command(
        TARGET cpplint
        PRE_BUILD
        BYPRODUCTS ${output}
        COMMENT "Running CppLint, exporting to ${output}"
        WORKING_DIRECTORY ${directory}
        COMMAND "${command}"
        ARGS
        --linelength=250
        --verbose=5
        ${files} 2>&1 | tee ${output}
    )
endfunction()

function(StaticAnalysis AnalyseFiles)
message(STATUS "Analyze ${AnalyseFiles}")
Cpplint("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" cpplint)

Cppcheck("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" cppcheck)

#cccc("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" cccc)

flint("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" flint++)

message("Static analysis files ${AnalyseFiles}")
flawfinder("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" flawfinder)


cppclean("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" cppclean)

#oclint("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" oclint)

clangcheck("${CMAKE_CURRENT_SOURCE_DIR}" "${AnalyseFiles}" clang-check-9)
endfunction()

