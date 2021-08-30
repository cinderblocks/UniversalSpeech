# -*- cmake -*-

project(UniversalSpeechExample)

add_executable(UniversalSpeechExample test.c)

target_link_libraries(UniversalSpeechExample
					  UniversalSpeech)
