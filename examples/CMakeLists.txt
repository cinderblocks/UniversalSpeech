# -*- cmake -*-

project(UniversalSpeechExample)

add_executable(UniversalSpeechExample test.c)

target_link_libraries(UniversalSpeechExample
					  UniversalSpeech)

set_target_properties(UniversalSpeechExample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	)