#pragma once

#include "automaton.h"
#include "alphabet_retriever.h"

struct _AlphabetRetriever {
	void *derived;
	
	GList *(*get_alphabet) (AlphabetRetriever*, Automaton *);
	void (*destroy) (AlphabetRetriever *);
};

// This class is abstract
AlphabetRetriever *alphabet_retriever_new_private();
void alphabet_retriever_destroy_private(AlphabetRetriever *self);