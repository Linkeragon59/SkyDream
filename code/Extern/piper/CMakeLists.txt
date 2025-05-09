cmake_minimum_required(VERSION 3.16)

add_library(eSpeakNGLib INTERFACE)

set(eSpeakNGDebugDLLPath ${CMAKE_CURRENT_SOURCE_DIR}/espeak-ng/bin/win64/Debug/espeak-ng.dll CACHE INTERNAL "")
set(eSpeakNGReleaseDLLPath ${CMAKE_CURRENT_SOURCE_DIR}/espeak-ng/bin/win64/Release/espeak-ng.dll CACHE INTERNAL "")
set(eSpeakNGDataPath ${CMAKE_CURRENT_SOURCE_DIR}/espeak-ng/share/espeak-ng-data CACHE INTERNAL "")
target_link_directories(eSpeakNGLib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/espeak-ng/lib/win64)
target_link_libraries(eSpeakNGLib INTERFACE espeak-ng)

add_library(Piper)
target_sources(Piper
	PRIVATE
		piper-phonemize-master/src/phonemize.cpp
		piper-phonemize-master/src/phoneme_ids.cpp
		piper-phonemize-master/src/tashkeel.cpp
		piper-phonemize-master/src/shared.cpp
		piper-master/src/cpp/piper.cpp
)

target_compile_features(Piper PRIVATE cxx_std_23)
target_compile_options(Piper PRIVATE
	/wd"4100"
	/wd"4244"
	/wd"4267"
	/wd"4456"
	/wd"4459"
)

target_include_directories(Piper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/espeak-ng/include)
target_link_libraries(Piper PRIVATE spdlogInclude)

target_include_directories(Piper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/piper-phonemize-master/src)
target_include_directories(Piper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/piper-master/src/cpp)
target_link_libraries(Piper PUBLIC OnnxRuntimeInclude)
target_link_libraries(Piper PUBLIC eSpeakNGLib)
