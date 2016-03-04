cmake_minimum_required (VERSION 3.0)
project (ArbitraryThrash)

find_package(Boost 1.59.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  include_directories("../../boost_1_59_0")
endif()

set(CommonCompilerFlags "")

if (MSVC)
	list(APPEND CommonCompilerFlags "/fp:fast") # optimize floating-point code for speed at the expense of accuracy and correctness
	list(APPEND CommonCompilerFlags "/EHs") 	# use proper C++ exception handling
	list(APPEND CommonCompilerFlags "/Oi") 		# use intrinsics to speed up code
	list(APPEND CommonCompilerFlags "/WX") 		# treat warnings as errors
	#list(APPEND CommonCompilerFlags "/W4") 		# turn on most of warnings
	list(APPEND CommonCompilerFlags "-wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4456 -wd4457 -wd4702") # disable some warnings
	list(APPEND CommonCompilerFlags "-D_CRT_SECURE_NO_WARNINGS") # get rid of useless CRT warings
	list(APPEND CommonCompilerFlags "-D_SCL_SECURE_NO_WARNINGS") # get rid of useless CRT warings
else()
#if (CMAKE_COMPILER_IS_GNUCXX)
	list(APPEND CommonCompilerFlags "-std=c++11")
	list(APPEND CommonCompilerFlags "-Wno-shadow")
	list(APPEND CommonCompilerFlags "-fpermissive")
endif()

set(CommonLibraries "")
#list(APPEND CommonLibraries user32.lib gdi32.lib winmm.lib opengl32.lib)

add_definitions(${CommonCompilerFlags})

file(GLOB_RECURSE ALL_INCLUDES "../arbitrary_format/*.h")

add_library(ArbitraryHeaders STATIC ${ALL_INCLUDES})
set_target_properties(ArbitraryHeaders PROPERTIES LINKER_LANGUAGE CXX)


add_executable(BinarySerializer BinarySerializer.cpp Playground.cpp)
include_directories("../")
#target_compile_options(BinarySerializer ${CommonCompilerFlags})
target_link_libraries(BinarySerializer ${CommonLibraries})

add_executable(XmlSerializer XmlSerializer.cpp)
include_directories("../")
#target_compile_options(XmlSerializer ${CommonCompilerFlags})
target_link_libraries(XmlSerializer ${CommonLibraries})

add_executable(Example-Person Example-Person.cpp)
include_directories("../")
#target_compile_options(Example-Person ${CommonCompilerFlags})
target_link_libraries(Example-Person ${CommonLibraries})
