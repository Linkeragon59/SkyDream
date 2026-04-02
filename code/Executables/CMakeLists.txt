cmake_minimum_required(VERSION 4.0)

add_subdirectory(Asteorhythm)
set_target_properties(Asteorhythm PROPERTIES FOLDER "Executables")

#add_subdirectory(LocalLLMTest)
#set_target_properties(LocalLLMTest PROPERTIES FOLDER "Executables")

add_subdirectory(NeatPoleBalancing)
set_target_properties(NeatPoleBalancing PROPERTIES FOLDER "Executables")

add_subdirectory(NeatPoleBalancing2)
set_target_properties(NeatPoleBalancing2 PROPERTIES FOLDER "Executables")

add_subdirectory(NeatDoublePoleBalancing)
set_target_properties(NeatDoublePoleBalancing PROPERTIES FOLDER "Executables")

add_subdirectory(NeatXOR)
set_target_properties(NeatXOR PROPERTIES FOLDER "Executables")

#add_subdirectory(TextToSpeechTest)
#set_target_properties(TextToSpeechTest PROPERTIES FOLDER "Executables")

add_subdirectory(UnitTestsSample)
set_target_properties(UnitTestsSample PROPERTIES FOLDER "Executables")

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_subdirectory(TouchInputLinuxTest)
	set_target_properties(TouchInputLinuxTest PROPERTIES FOLDER "Executables")
endif ()
