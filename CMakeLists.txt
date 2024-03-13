cmake_minimum_required(VERSION 3.11.0)

# set project name
set(NAME VulkanEngine)
project(${NAME} VERSION 1.0.0)

# make sure vulkan and glfw are installed
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_compile_options(
  "$<$<COMPILE_LANGUAGE:CXX>:-g>"
)

# find source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# add source files
add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES})

############## Build SHADERS #######################
 
# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
# find_program(GLSL_VALIDATOR glslangValidator HINTS 
#   ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
#   /usr/bin 
#   /usr/local/bin 
#   ${VULKAN_SDK_PATH}/Bin
#   ${VULKAN_SDK_PATH}/Bin32
#   $ENV{VULKAN_SDK}/Bin/ 
#   $ENV{VULKAN_SDK}/Bin32/
# )
 
# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)
 
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND glslc ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)
 
add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
