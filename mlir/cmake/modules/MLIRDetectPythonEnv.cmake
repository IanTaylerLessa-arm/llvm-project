# Macros and functions related to detecting details of the Python environment.

# Finds and configures python packages needed to build MLIR Python bindings.
macro(mlir_configure_python_dev_packages)
  if(NOT MLIR_DISABLE_CONFIGURE_PYTHON_DEV_PACKAGES)
    if(MLIR_DETECT_PYTHON_ENV_PRIME_SEARCH)
      # Prime the search for python to see if there is a full development
      # package. This seems to work around cmake bugs searching only for
      # Development.Module in some environments. However, in other environments
      # it may interfere with the subsequent search for Development.Module.
      find_package(Python3 ${LLVM_MINIMUM_PYTHON_VERSION}
        COMPONENTS Interpreter Development)
    endif()

    # After CMake 3.18, we are able to limit the scope of the search to just
    # Development.Module. Searching for Development will fail in situations where
    # the Python libraries are not available. When possible, limit to just
    # Development.Module.
    # See https://pybind11.readthedocs.io/en/stable/compiling.html#findpython-mode
    set(_python_development_component Development.Module)

    find_package(Python3 ${LLVM_MINIMUM_PYTHON_VERSION}
      COMPONENTS Interpreter ${_python_development_component} REQUIRED)

    # We look for both Python3 and Python, the search algorithm should be
    # consistent, otherwise disastrous result is almost guaranteed.
    # Warn if the policies for treating virtual environment are not defined
    # consistently.
    # For more details check issue #126162.
    if(((DEFINED Python_FIND_VIRTUALENV) AND (NOT DEFINED Python3_FIND_VIRTUALENV)) OR
       ((NOT DEFINED Python_FIND_VIRTUALENV) AND (DEFINED Python3_FIND_VIRTUALENV)))
      message(WARNING "Only one of Python3_FIND_VIRTUALENV and Python_FIND_VIRTUALENV variables is defined. "
                      "Make sure that both variables are defined and have the same value.")
    elseif((DEFINED Python_FIND_VIRTUALENV) AND (DEFINED Python3_FIND_VIRTUALENV) AND
           (NOT Python_FIND_VIRTUALENV STREQUAL Python3_FIND_VIRTUALENV))
      message(WARNING "Python3_FIND_VIRTUALENV and Python_FIND_VIRTUALENV are defined differently. "
                      "Make sure that the variables have the same values.")
    endif()

    # It's a little silly to detect Python a second time, but nanobind's cmake
    # code looks for Python_ not Python3_.
    find_package(Python ${LLVM_MINIMUM_PYTHON_VERSION}
      COMPONENTS Interpreter ${_python_development_component} REQUIRED)

    unset(_python_development_component)
    message(STATUS "Found python include dirs: ${Python3_INCLUDE_DIRS}")
    message(STATUS "Found python libraries: ${Python3_LIBRARIES}")
    message(STATUS "Found numpy v${Python3_NumPy_VERSION}: ${Python3_NumPy_INCLUDE_DIRS}")
    mlir_detect_pybind11_install()
    find_package(pybind11 2.10 CONFIG REQUIRED)
    message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIR}")
    message(STATUS "Python prefix = '${PYTHON_MODULE_PREFIX}', "
                  "suffix = '${PYTHON_MODULE_SUFFIX}', "
                  "extension = '${PYTHON_MODULE_EXTENSION}")

    mlir_detect_nanobind_install()
    find_package(nanobind 2.4 CONFIG REQUIRED)
    message(STATUS "Found nanobind v${nanobind_VERSION}: ${nanobind_INCLUDE_DIR}")
    message(STATUS "Python prefix = '${PYTHON_MODULE_PREFIX}', "
                  "suffix = '${PYTHON_MODULE_SUFFIX}', "
                  "extension = '${PYTHON_MODULE_EXTENSION}")
  endif()
endmacro()

# Detects a pybind11 package installed in the current python environment
# and sets variables to allow it to be found. This allows pybind11 to be
# installed via pip, which typically yields a much more recent version than
# the OS install, which will be available otherwise.
function(mlir_detect_pybind11_install)
  if(pybind11_DIR)
    message(STATUS "Using explicit pybind11 cmake directory: ${pybind11_DIR} (-Dpybind11_DIR to change)")
  else()
    message(STATUS "Checking for pybind11 in python path...")
    execute_process(
      COMMAND "${Python3_EXECUTABLE}"
      -c "import pybind11;print(pybind11.get_cmake_dir(), end='')"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE STATUS
      OUTPUT_VARIABLE PACKAGE_DIR
      ERROR_QUIET)
    if(NOT STATUS EQUAL "0")
      message(STATUS "not found (install via 'pip install pybind11' or set pybind11_DIR)")
      return()
    endif()
    message(STATUS "found (${PACKAGE_DIR})")
    set(pybind11_DIR "${PACKAGE_DIR}" PARENT_SCOPE)
  endif()
endfunction()


# Detects a nanobind package installed in the current python environment
# and sets variables to allow it to be found. This allows nanobind to be
# installed via pip, which typically yields a much more recent version than
# the OS install, which will be available otherwise.
function(mlir_detect_nanobind_install)
  if(nanobind_DIR)
    message(STATUS "Using explicit nanobind cmake directory: ${nanobind_DIR} (-Dnanobind_DIR to change)")
  else()
    message(STATUS "Checking for nanobind in python path...")
    execute_process(
      COMMAND "${Python3_EXECUTABLE}"
      -c "import nanobind;print(nanobind.cmake_dir(), end='')"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE STATUS
      OUTPUT_VARIABLE PACKAGE_DIR
      ERROR_QUIET)
    if(NOT STATUS EQUAL "0")
      message(STATUS "not found (install via 'pip install nanobind' or set nanobind_DIR)")
      return()
    endif()
    message(STATUS "found (${PACKAGE_DIR})")
    set(nanobind_DIR "${PACKAGE_DIR}" PARENT_SCOPE)
    execute_process(
      COMMAND "${Python3_EXECUTABLE}"
      -c "import nanobind;print(nanobind.include_dir(), end='')"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE STATUS
      OUTPUT_VARIABLE PACKAGE_DIR
      ERROR_QUIET)
    if(NOT STATUS EQUAL "0")
      message(STATUS "not found (install via 'pip install nanobind' or set nanobind_DIR)")
      return()
    endif()
    set(nanobind_INCLUDE_DIR "${PACKAGE_DIR}" PARENT_SCOPE)
  endif()
endfunction()
