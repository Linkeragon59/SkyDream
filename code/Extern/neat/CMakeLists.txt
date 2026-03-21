cmake_minimum_required(VERSION 4.0)

add_library(NEAT)
target_sources(NEAT
	PRIVATE
		EvolutionParams.h
		EvolutionParams.cpp
		Genome.h
		Genome.cpp
		Link.h
		Link.cpp
		Node.h
		Node.cpp
		Population.h
		Population.cpp
		Specie.h
		Specie.cpp
)

target_include_directories(NEAT PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
